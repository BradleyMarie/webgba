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
                            &context->bases[registers->vcount][2u * i + 0u],
                            &row_dirty);
    OpenGlBgAffineLoadFixed(context, registers->internal.affine[i].current[1u],
                            &context->bases[registers->vcount][2u * i + 1u],
                            &row_dirty);
    OpenGlBgAffineLoadFixed(context, registers->affine[i].pa,
                            &context->scales[registers->vcount][2u * i + 0u],
                            &row_dirty);
    OpenGlBgAffineLoadFixed(context, registers->affine[i].pc,
                            &context->scales[registers->vcount][2u * i + 1u],
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
        context->bases[registers->vcount][2u * i + 0u] +
            (context->bases[registers->vcount][2u * i + 0u] -
             context->bases[registers->vcount - 1u][2u * i + 0u]),
        &context->bases[registers->vcount + 1u][2u * i + 0u], &row_dirty);
    OpenGlBgAffineLoadFloat(
        context,
        context->bases[registers->vcount][2u * i + 1u] +
            (context->bases[registers->vcount][2u * i + 1u] -
             context->bases[registers->vcount - 1u][2u * i + 1u]),
        &context->bases[registers->vcount + 1u][2u * i + 1u], &row_dirty);
    OpenGlBgAffineLoadFloat(
        context,
        context->scales[registers->vcount][2u * i + 0u] +
            (context->scales[registers->vcount][2u * i + 0u] -
             context->scales[registers->vcount - 1u][2u * i + 0u]),
        &context->scales[registers->vcount + 1u][2u * i + 0u], &row_dirty);
    OpenGlBgAffineLoadFloat(
        context,
        context->scales[registers->vcount][2u * i + 1u] +
            (context->scales[registers->vcount][2u * i + 1u] -
             context->scales[registers->vcount - 1u][2u * i + 1u]),
        &context->scales[registers->vcount + 1u][2u * i + 1u], &row_dirty);
  }

  if (row_dirty) {
    context->dirty_end = registers->vcount + 1u;
    context->dirty = true;
  }

  return context->dirty;
}

void OpenGlBgAffineBind(OpenGlBgAffine* context, GLuint program) {
  GLint affine_bases = glGetUniformBlockIndex(program, "AffineBases");
  if (affine_bases < 0) {
    return;
  }

  glUniformBlockBinding(program, affine_bases, AFFINE_BASES_BUFFER);
  glBindBufferBase(GL_UNIFORM_BUFFER, AFFINE_BASES_BUFFER,
                   context->buffers[0u]);

  GLint affine_scales = glGetUniformBlockIndex(program, "AffineScales");

  glUniformBlockBinding(program, affine_scales, AFFINE_SCALES_BUFFER);
  glBindBufferBase(GL_UNIFORM_BUFFER, AFFINE_SCALES_BUFFER,
                   context->buffers[1u]);

  if (context->dirty) {
    glBindBuffer(GL_UNIFORM_BUFFER, context->buffers[0u]);
    glBufferSubData(GL_UNIFORM_BUFFER,
                    /*offset=*/4u * sizeof(GLfloat) * context->dirty_start,
                    /*size=*/4u * sizeof(GLfloat) *
                        (context->dirty_end - context->dirty_start + 1u),
                    /*data=*/&context->bases[context->dirty_start]);
    glBindBuffer(GL_UNIFORM_BUFFER, context->buffers[1u]);
    glBufferSubData(GL_UNIFORM_BUFFER,
                    /*offset=*/4u * sizeof(GLfloat) * context->dirty_start,
                    /*size=*/4u * sizeof(GLfloat) *
                        (context->dirty_end - context->dirty_start + 1u),
                    /*data=*/&context->scales[context->dirty_start]);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    context->dirty = false;
  }
}

void OpenGlBgAffineReloadContext(OpenGlBgAffine* context) {
  glGenBuffers(2, context->buffers);
  glBindBuffer(GL_UNIFORM_BUFFER, context->buffers[0u]);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(context->bases), context->bases,
               GL_DYNAMIC_DRAW);
  glBindBuffer(GL_UNIFORM_BUFFER, context->buffers[1u]);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(context->scales), context->scales,
               GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OpenGlBgAffineDestroy(OpenGlBgAffine* context) {
  glDeleteBuffers(2, context->buffers);
}