#include "emulator/ppu/gba/opengl/blend.h"

#include <math.h>
#include <string.h>

#include "emulator/ppu/gba/opengl/texture_bindings.h"

static GLfloat FixedToFloat(uint8_t fixed) {
  return fmin((double)fixed / 16.0, 1.0);
}

void OpenGlBlendReload(OpenGlBlend* context, const GbaPpuRegisters* registers,
                       GbaPpuDirtyBits* dirty_bits) {
  if (!dirty_bits->io.blend) {
    return;
  }

  context->staging.blend_mode = registers->bldcnt.mode;
  context->staging.blend_eva = FixedToFloat(registers->bldalpha.eva);
  context->staging.blend_evb = FixedToFloat(registers->bldalpha.evb);
  context->staging.blend_evy = FixedToFloat(registers->bldy.evy);
  context->staging.obj_top = registers->bldcnt.a_obj;
  context->staging.obj_bottom = registers->bldcnt.b_obj;
  context->staging.bg_top[0u][0u] = registers->bldcnt.a_bg0;
  context->staging.bg_bottom[0u][0u] = registers->bldcnt.b_bg0;
  context->staging.bg_top[1u][0u] = registers->bldcnt.a_bg1;
  context->staging.bg_bottom[1u][0u] = registers->bldcnt.b_bg1;
  context->staging.bg_top[2u][0u] = registers->bldcnt.a_bg2;
  context->staging.bg_bottom[2u][0u] = registers->bldcnt.b_bg2;
  context->staging.bg_top[3u][0u] = registers->bldcnt.a_bg3;
  context->staging.bg_bottom[3u][0u] = registers->bldcnt.b_bg3;
  context->staging.bd_top = registers->bldcnt.a_bd;
  context->staging.bd_bottom = registers->bldcnt.b_bd;

  glBindBuffer(GL_UNIFORM_BUFFER, context->buffer);
  glBufferSubData(GL_UNIFORM_BUFFER, /*offset=*/0,
                  /*size=*/sizeof(context->staging),
                  /*data=*/&context->staging);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  dirty_bits->io.blend = false;
}

void OpenGlBlendBind(const OpenGlBlend* context, GLuint program) {
  GLint blend = glGetUniformBlockIndex(program, "Blend");
  glUniformBlockBinding(program, blend, BLEND_BUFFER);

  glBindBuffer(GL_UNIFORM_BUFFER, context->buffer);
  glBindBufferBase(GL_UNIFORM_BUFFER, BLEND_BUFFER, context->buffer);
}

void OpenGlBlendReloadContext(OpenGlBlend* context) {
  glGenBuffers(1, &context->buffer);
  glBindBuffer(GL_UNIFORM_BUFFER, context->buffer);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(context->staging), NULL,
               GL_DYNAMIC_DRAW);
}

void OpenGlBlendDestroy(OpenGlBlend* context) {
  glDeleteTextures(1u, &context->buffer);
}