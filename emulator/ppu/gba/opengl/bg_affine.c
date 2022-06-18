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
    OpenGlBgAffineLoadFixed(
        context, registers->internal.affine[i].current[0u],
        &context->staging.rows[registers->vcount].bases[i][0u], &row_dirty);
    OpenGlBgAffineLoadFixed(
        context, registers->internal.affine[i].current[1u],
        &context->staging.rows[registers->vcount].bases[i][1u], &row_dirty);
    OpenGlBgAffineLoadFixed(
        context, registers->affine[i].pa,
        &context->staging.rows[registers->vcount].scale[i][0u], &row_dirty);
    OpenGlBgAffineLoadFixed(
        context, registers->affine[i].pc,
        &context->staging.rows[registers->vcount].scale[i][1u], &row_dirty);
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
        context->staging.rows[registers->vcount].bases[i][0u] +
            (context->staging.rows[registers->vcount].bases[i][0u] -
             context->staging.rows[registers->vcount - 1u].bases[i][0u]),
        &context->staging.rows[registers->vcount + 1u].bases[i][0u],
        &row_dirty);
    OpenGlBgAffineLoadFloat(
        context,
        context->staging.rows[registers->vcount].bases[i][1u] +
            (context->staging.rows[registers->vcount].bases[i][1u] -
             context->staging.rows[registers->vcount - 1u].bases[i][1u]),
        &context->staging.rows[registers->vcount + 1u].bases[i][1u],
        &row_dirty);
    OpenGlBgAffineLoadFloat(
        context,
        context->staging.rows[registers->vcount].scale[i][0u] +
            (context->staging.rows[registers->vcount].scale[i][0u] -
             context->staging.rows[registers->vcount - 1u].scale[i][0u]),
        &context->staging.rows[registers->vcount + 1u].scale[i][0u],
        &row_dirty);
    OpenGlBgAffineLoadFloat(
        context,
        context->staging.rows[registers->vcount].scale[i][1u] +
            (context->staging.rows[registers->vcount].scale[i][1u] -
             context->staging.rows[registers->vcount - 1u].scale[i][1u]),
        &context->staging.rows[registers->vcount + 1u].scale[i][1u],
        &row_dirty);
  }

  if (row_dirty) {
    context->dirty_end = registers->vcount + 1u;
    context->dirty = true;
  }

  return context->dirty;
}

void OpenGlBgAffineBind(OpenGlBgAffine* context, GLuint program) {
  GLint affine_backgrounds =
      glGetUniformBlockIndex(program, "AffineBackgrounds");
  if (affine_backgrounds < 0) {
    return;
  }

  glUniformBlockBinding(program, affine_backgrounds, AFFINE_BUFFER);

  glBindBuffer(GL_UNIFORM_BUFFER, context->buffer);
  glBindBufferBase(GL_UNIFORM_BUFFER, AFFINE_BUFFER, context->buffer);

  if (context->dirty) {
    glBufferSubData(GL_UNIFORM_BUFFER,
                    /*offset=*/sizeof(OpenGlBgAffineRow) * context->dirty_start,
                    /*size=*/sizeof(OpenGlBgAffineRow) *
                        (context->dirty_end - context->dirty_start + 1u),
                    /*data=*/&context->staging.rows[context->dirty_start]);
    context->dirty = false;
  }

  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void OpenGlBgAffineReloadContext(OpenGlBgAffine* context) {
  glGenBuffers(1, &context->buffer);
  glBindBuffer(GL_UNIFORM_BUFFER, context->buffer);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(context->staging), &context->staging,
               GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OpenGlBgAffineDestroy(OpenGlBgAffine* context) {
  glDeleteBuffers(1, &context->buffer);
}