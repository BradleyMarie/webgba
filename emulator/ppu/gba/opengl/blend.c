#include "emulator/ppu/gba/opengl/blend.h"

#include <assert.h>
#include <math.h>

#include "emulator/ppu/gba/opengl/texture_bindings.h"

static void OpenGlBlendSetGLuint(OpenGlBlend* context, GLuint value,
                                 GLuint* result, bool* dirty) {
  if (*result != value) {
    *result = value;
    *dirty = true;
  }
}

static void OpenGlBlendSetGLfloat(OpenGlBlend* context, unsigned char value,
                                  GLfloat* result, bool* dirty) {
  GLfloat as_float = fmin((double)value / 16.0, 1.0);
  if (*result != as_float) {
    *result = as_float;
    *dirty = true;
  }
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

  if (!context->dirty) {
    context->dirty_start = registers->vcount;
  }

  bool row_dirty = false;
  OpenGlBlendSetGLuint(context, registers->bldcnt.mode,
                       &context->staging[registers->vcount].bldcnt[0u],
                       &row_dirty);
  OpenGlBlendSetGLuint(context, registers->bldcnt.value & 0x3Fu,
                       &context->staging[registers->vcount].bldcnt[1u],
                       &row_dirty);
  OpenGlBlendSetGLuint(context, (registers->bldcnt.value >> 8u) & 0x3F,
                       &context->staging[registers->vcount].bldcnt[2u],
                       &row_dirty);
  OpenGlBlendSetGLfloat(context, registers->bldalpha.eva,
                        &context->staging[registers->vcount].ev[0u],
                        &row_dirty);
  OpenGlBlendSetGLfloat(context, registers->bldalpha.evb,
                        &context->staging[registers->vcount].ev[1u],
                        &row_dirty);
  OpenGlBlendSetGLfloat(context, registers->bldy.evy,
                        &context->staging[registers->vcount].ev[2u],
                        &row_dirty);

  if (row_dirty) {
    context->dirty_end = registers->vcount;
    context->dirty = true;
  }

  return context->dirty;
}

void OpenGlBlendBind(OpenGlBlend* context, GLuint program) {
  GLint blend = glGetUniformBlockIndex(program, "Blend");
  glUniformBlockBinding(program, blend, BLEND_BUFFER);

  glBindBuffer(GL_UNIFORM_BUFFER, context->buffer);
  glBindBufferBase(GL_UNIFORM_BUFFER, BLEND_BUFFER, context->buffer);

  if (context->dirty) {
    glBufferSubData(GL_UNIFORM_BUFFER,
                    /*offset=*/sizeof(OpenGlBlendRow) * context->dirty_start,
                    /*size=*/sizeof(OpenGlBlendRow) *
                        (context->dirty_end - context->dirty_start + 1u),
                    /*data=*/&context->staging[context->dirty_start]);
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