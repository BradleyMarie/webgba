#include "emulator/ppu/gba/opengl/bg_affine.h"

#include "emulator/ppu/gba/opengl/texture_bindings.h"

static void OpenGlBgAffineLoad(OpenGlBgAffine* context, int32_t value,
                               GLfloat* output) {
  GLfloat new_value = (double)value / 256.0;
  if (new_value != *output) {
    context->dirty = true;
  }
  *output = new_value;
}

bool OpenGlBgAffineStage(OpenGlBgAffine* context,
                         const GbaPpuRegisters* registers,
                         GbaPpuDirtyBits* dirty_bits) {
  if (registers->dispcnt.mode == 0u) {
    return false;
  }

  for (uint8_t i = 0; i < GBA_PPU_NUM_AFFINE_BACKGROUNDS; i++) {
    if (i == 0 && !registers->dispcnt.bg2_enable) {
      continue;
    }

    if (i == 1 &&
        (!registers->dispcnt.bg3_enable || registers->dispcnt.mode != 2u)) {
      continue;
    }

    OpenGlBgAffineLoad(context, registers->internal.affine[i].current[0u],
                       &context->staging.rows[registers->vcount].bases[i][0u]);

    OpenGlBgAffineLoad(context, registers->internal.affine[i].current[1u],
                       &context->staging.rows[registers->vcount].bases[i][1u]);

    OpenGlBgAffineLoad(context, registers->affine[i].pa,
                       &context->staging.rows[registers->vcount].scale[i][0u]);

    OpenGlBgAffineLoad(context, registers->affine[i].pc,
                       &context->staging.rows[registers->vcount].scale[i][1u]);

    if (registers->vcount == GBA_SCREEN_HEIGHT - 1) {
      OpenGlBgAffineLoad(
          context,
          registers->internal.affine[i].current[0u] +
              (registers->internal.affine[i].current[0u] -
               context->staging.rows[registers->vcount - 1u].bases[i][0u]),
          &context->staging.rows[registers->vcount + 1u].bases[i][0u]);

      OpenGlBgAffineLoad(
          context,
          registers->internal.affine[i].current[1u] +
              (registers->internal.affine[i].current[1u] -
               context->staging.rows[registers->vcount - 1u].bases[i][1u]),
          &context->staging.rows[registers->vcount + 1u].bases[i][1u]);

      OpenGlBgAffineLoad(
          context,
          registers->affine[i].pa +
              (registers->affine[i].pa -
               context->staging.rows[registers->vcount - 1u].scale[i][0u]),
          &context->staging.rows[registers->vcount + 1u].scale[i][0u]);

      OpenGlBgAffineLoad(
          context,
          registers->affine[i].pc +
              (registers->affine[i].pc -
               context->staging.rows[registers->vcount - 1u].scale[i][1u]),
          &context->staging.rows[registers->vcount + 1u].scale[i][1u]);
    }
  }

  return false;
}

void OpenGlBgAffineBind(OpenGlBgAffine* context, GLint start, GLint end,
                        GLuint program) {
  GLint affine_backgrounds =
      glGetUniformBlockIndex(program, "AffineBackgrounds");
  glUniformBlockBinding(program, affine_backgrounds, AFFINE_BUFFER);

  glBindBuffer(GL_UNIFORM_BUFFER, context->buffer);
  glBindBufferBase(GL_UNIFORM_BUFFER, AFFINE_BUFFER, context->buffer);

  if (context->dirty) {
    glBindBuffer(GL_UNIFORM_BUFFER, context->buffer);
    glBufferSubData(
        GL_UNIFORM_BUFFER, /*offset=*/0,
        /*size=*/(sizeof(context->staging) / 161u) * (end - start + 1u),
        /*data=*/&context->staging.rows[start]);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    context->dirty = false;
  }
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