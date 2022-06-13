#include "emulator/ppu/gba/opengl/bg_affine.h"

#include "emulator/ppu/gba/opengl/texture_bindings.h"

static GLfloat FixedToFloat(int32_t value) { return (double)value / 256.0; }

bool OpenGlBgAffineStage(OpenGlBgAffine* context,
                         const GbaPpuRegisters* registers,
                         GbaPpuDirtyBits* dirty_bits) {
  if (registers->dispcnt.mode == 0u) {
    return false;
  }

  bool result = false;
  for (uint8_t i = 0; i < GBA_PPU_NUM_AFFINE_BACKGROUNDS; i++) {
    if (i == 0 && !registers->dispcnt.bg2_enable) {
      continue;
    }

    if (i == 1 &&
        (!registers->dispcnt.bg3_enable || registers->dispcnt.mode != 2u)) {
      continue;
    }

    if (!dirty_bits->io.bg_affine[i]) {
      continue;
    }

    context->staging.origins[i][0u] = 0.0;
    context->staging.origins[i][1u] = registers->vcount;
    context->staging.values[i][0u] =
        FixedToFloat(registers->internal.affine[i].current[0u]);
    context->staging.values[i][1u] =
        FixedToFloat(registers->internal.affine[i].current[1u]);
    context->staging.transformations[i][0u][0u] =
        FixedToFloat(registers->affine[i].pa);
    context->staging.transformations[i][0u][1u] =
        FixedToFloat(registers->affine[i].pc);
    context->staging.transformations[i][1u][0u] =
        FixedToFloat(registers->affine[i].pb);
    context->staging.transformations[i][1u][1u] =
        FixedToFloat(registers->affine[i].pd);

    dirty_bits->io.bg_affine[i] = false;
    context->dirty = true;
    result = true;
  }

  return result;
}

void OpenGlBgAffineBind(const OpenGlBgAffine* context, GLuint program) {
  GLint affine_backgrounds =
      glGetUniformBlockIndex(program, "AffineBackgrounds");
  glUniformBlockBinding(program, affine_backgrounds, AFFINE_BUFFER);

  glBindBuffer(GL_UNIFORM_BUFFER, context->buffer);
  glBindBufferBase(GL_UNIFORM_BUFFER, AFFINE_BUFFER, context->buffer);
}

void OpenGlBgAffineReload(OpenGlBgAffine* context) {
  if (context->dirty) {
    glBindBuffer(GL_UNIFORM_BUFFER, context->buffer);
    glBufferSubData(GL_UNIFORM_BUFFER, /*offset=*/0,
                    /*size=*/sizeof(context->staging),
                    /*data=*/&context->staging);
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