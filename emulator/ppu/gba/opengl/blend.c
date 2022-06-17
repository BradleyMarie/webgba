#include "emulator/ppu/gba/opengl/blend.h"

#include <assert.h>
#include <math.h>

#include "emulator/ppu/gba/opengl/texture_bindings.h"

void OpenGlBlendSetGLuint(OpenGlBlend* context, GLuint value, GLuint* result) {
  if (*result != value) {
    context->dirty = true;
  }
  *result = value;
}

void OpenGlBlendSetGLfloat(OpenGlBlend* context, unsigned char value,
                           GLfloat* result) {
  GLfloat as_float = fmin((double)value / 16.0, 1.0);
  if (*result != as_float) {
    context->dirty = true;
  }
  *result = as_float;
}

bool OpenGlBlendLoad(OpenGlBlend* context, const GbaPpuRegisters* registers,
                     GbaPpuDirtyBits* dirty_bits) {
  assert(registers->vcount < GBA_SCREEN_HEIGHT);

  if ((!registers->dispcnt.win0_enable || !registers->winin.win0.bld) &&
      (!registers->dispcnt.win1_enable || !registers->winin.win1.bld) &&
      (!registers->dispcnt.winobj_enable || !registers->winout.winobj.bld) &&
      ((registers->dispcnt.win0_enable || registers->dispcnt.win1_enable ||
        registers->dispcnt.winobj_enable) &&
       !registers->winout.winout.bld)) {
    return false;
  }

  OpenGlBlendSetGLuint(context, registers->bldcnt.mode,
                       &context->staging[registers->vcount].bldcnt[0u]);
  OpenGlBlendSetGLuint(context, registers->bldcnt.value & 0x3Fu,
                       &context->staging[registers->vcount].bldcnt[1u]);
  OpenGlBlendSetGLuint(context, (registers->bldcnt.value >> 8u) & 0x3F,
                       &context->staging[registers->vcount].bldcnt[2u]);
  OpenGlBlendSetGLfloat(context, registers->bldalpha.eva,
                        &context->staging[registers->vcount].ev[0u]);
  OpenGlBlendSetGLfloat(context, registers->bldalpha.evb,
                        &context->staging[registers->vcount].ev[1u]);
  OpenGlBlendSetGLfloat(context, registers->bldy.evy,
                        &context->staging[registers->vcount].ev[2u]);

  return true;
}

void OpenGlBlendBind(OpenGlBlend* context, GLint start, GLint end,
                     GLuint program) {
  assert(0 <= start);
  assert(start != end);
  assert(end <= GBA_SCREEN_HEIGHT);

  GLint blend = glGetUniformBlockIndex(program, "Blend");
  glUniformBlockBinding(program, blend, BLEND_BUFFER);

  glBindBuffer(GL_UNIFORM_BUFFER, context->buffer);
  glBindBufferBase(GL_UNIFORM_BUFFER, BLEND_BUFFER, context->buffer);

  if (context->dirty) {
    glBufferSubData(GL_UNIFORM_BUFFER,
                    /*offset=*/sizeof(OpenGlBlendRow) * start,
                    /*size=*/sizeof(OpenGlBlendRow) * (end - start),
                    /*data=*/&context->staging[start]);
    context->dirty = false;
  }

  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void OpenGlBlendReloadContext(OpenGlBlend* context) {
  glGenBuffers(1, &context->buffer);
  glBindBuffer(GL_UNIFORM_BUFFER, context->buffer);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(context->staging), &context->staging,
               GL_DYNAMIC_DRAW);
}

void OpenGlBlendDestroy(OpenGlBlend* context) {
  glDeleteTextures(1u, &context->buffer);
}