#include "emulator/ppu/gba/opengl/bg_affine.h"

#include <assert.h>

#include "emulator/ppu/gba/opengl/texture_bindings.h"

static void OpenGlBgAffineLoadFloat(OpenGlBgAffine* context, GLfloat new_value,
                                    GLfloat* result, bool* dirty) {
  if (new_value != *result) {
    *result = new_value;
    *dirty = true;
  }
}

static void OpenGlBgAffineLoadFixed(OpenGlBgAffine* context, int32_t value,
                                    GLfloat* output, bool* dirty) {
  return OpenGlBgAffineLoadFloat(context, (double)value / 256.0, output, dirty);
}

bool OpenGlBgAffineLoad(OpenGlBgAffine* context,
                        const GbaPpuRegisters* registers,
                        GbaPpuDirtyBits* dirty_bits) {
  assert(registers->vcount < GBA_SCREEN_HEIGHT);

  if (registers->dispcnt.mode == 0u) {
    return false;
  }

  if (!context->dirty) {
    context->dirty_start = registers->vcount;
  }

  bool row_dirty = false;
  for (uint8_t i = 0; i < GBA_PPU_NUM_AFFINE_BACKGROUNDS; i++) {
    OpenGlBgAffineLoadFixed(context, registers->internal.affine[i].current[0u],
                            &context->base_scale[registers->vcount][i][0u],
                            &row_dirty);
    OpenGlBgAffineLoadFixed(context, registers->internal.affine[i].current[1u],
                            &context->base_scale[registers->vcount][i][1u],
                            &row_dirty);
    OpenGlBgAffineLoadFixed(context, registers->affine[i].pa,
                            &context->base_scale[registers->vcount][i][2u],
                            &row_dirty);
    OpenGlBgAffineLoadFixed(context, registers->affine[i].pc,
                            &context->base_scale[registers->vcount][i][3u],
                            &row_dirty);
  }

  if (row_dirty) {
    context->dirty_end = registers->vcount;
    context->dirty = true;
  }

  if (registers->vcount != GBA_SCREEN_HEIGHT - 1) {
    return context->dirty;
  }

  if (!context->dirty) {
    context->dirty_start = registers->vcount;
  }

  row_dirty = false;
  for (uint8_t i = 0; i < GBA_PPU_NUM_AFFINE_BACKGROUNDS; i++) {
    OpenGlBgAffineLoadFloat(
        context,
        context->base_scale[registers->vcount][i][0u] +
            (context->base_scale[registers->vcount][i][0u] -
             context->base_scale[registers->vcount - 1u][i][0u]),
        &context->base_scale[registers->vcount + 1u][i][0u], &row_dirty);
    OpenGlBgAffineLoadFloat(
        context,
        context->base_scale[registers->vcount][i][1u] +
            (context->base_scale[registers->vcount][i][1u] -
             context->base_scale[registers->vcount - 1u][i][1u]),
        &context->base_scale[registers->vcount + 1u][i][1u], &row_dirty);
    OpenGlBgAffineLoadFloat(
        context,
        context->base_scale[registers->vcount][i][2u] +
            (context->base_scale[registers->vcount][i][2u] -
             context->base_scale[registers->vcount - 1u][i][2u]),
        &context->base_scale[registers->vcount + 1u][i][2u], &row_dirty);
    OpenGlBgAffineLoadFloat(
        context,
        context->base_scale[registers->vcount][i][3u] +
            (context->base_scale[registers->vcount][i][3u] -
             context->base_scale[registers->vcount - 1u][i][3u]),
        &context->base_scale[registers->vcount + 1u][i][3u], &row_dirty);
  }

  if (row_dirty) {
    context->dirty_end = registers->vcount + 1u;
    context->dirty = true;
  }

  return context->dirty;
}

void OpenGlBgAffineBind(OpenGlBgAffine* context, GLuint program) {
  GLint affine_coordinates =
      glGetUniformLocation(program, "affine_coordinates");
  if (affine_coordinates < 0) {
    return;
  }

  glUniform1i(affine_coordinates, BG_AFFINE_COORDINATES_TEXTURE);
  glActiveTexture(GL_TEXTURE0 + BG_AFFINE_COORDINATES_TEXTURE);
  glBindTexture(GL_TEXTURE_2D, context->texture);

  if (context->dirty) {
    glTexSubImage2D(GL_TEXTURE_2D, /*level=*/0, /*xoffset=*/0,
                    /*yoffset=*/context->dirty_start,
                    /*width=*/GBA_PPU_NUM_AFFINE_BACKGROUNDS,
                    /*height=*/context->dirty_end - context->dirty_start + 1u,
                    /*format=*/GL_RGBA, /*type=*/GL_FLOAT,
                    /*pixels=*/context->base_scale[context->dirty_start]);
    context->dirty = false;
  }
}

void OpenGlBgAffineReloadContext(OpenGlBgAffine* context) {
  glGenTextures(1, &context->texture);
  glBindTexture(GL_TEXTURE_2D, context->texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, /*level=*/0, /*internal_format=*/GL_RGBA32F,
               /*width=*/GBA_PPU_NUM_AFFINE_BACKGROUNDS,
               /*height=*/GBA_SCREEN_HEIGHT + 1u, /*border=*/0,
               /*format=*/GL_RGBA, /*type=*/GL_FLOAT,
               /*pixels=*/context->base_scale);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void OpenGlBgAffineDestroy(OpenGlBgAffine* context) {
  glDeleteTextures(1u, &context->texture);
}