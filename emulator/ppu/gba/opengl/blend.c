#include "emulator/ppu/gba/opengl/blend.h"

#include <math.h>

#include "emulator/ppu/gba/opengl/texture_bindings.h"

static void OpenGlBlendSetGLfloat(OpenGlBlend* context, unsigned char fixed,
                                  GLfloat* result) {
  GLfloat value = fmin((double)fixed / 16.0, 1.0);
  if (*result != value) {
    context->dirty = true;
  }
  *result = value;
}

static void OpenGlBlendSetGLuint(OpenGlBlend* context, unsigned char value,
                                 GLuint* result) {
  if (*result != value) {
    context->dirty = true;
  }
  *result = value;
}

bool OpenGlBlendStage(OpenGlBlend* context, const GbaPpuRegisters* registers,
                      GbaPpuDirtyBits* dirty_bits) {
  if ((!registers->dispcnt.win0_enable || !registers->winin.win0.bld) &&
      (!registers->dispcnt.win1_enable || !registers->winin.win1.bld) &&
      (!registers->dispcnt.winobj_enable || !registers->winout.winobj.bld) &&
      ((registers->dispcnt.win0_enable || registers->dispcnt.win1_enable ||
        registers->dispcnt.winobj_enable) &&
       !registers->winout.winout.bld)) {
    return false;
  }

  if (!dirty_bits->io.blend) {
    return false;
  }

  OpenGlBlendSetGLuint(context, registers->bldcnt.mode,
                       &context->staging.blend_mode);
  OpenGlBlendSetGLfloat(context, registers->bldalpha.eva,
                        &context->staging.blend_eva);
  OpenGlBlendSetGLfloat(context, registers->bldalpha.evb,
                        &context->staging.blend_evb);
  OpenGlBlendSetGLfloat(context, registers->bldy.evy,
                        &context->staging.blend_evy);
  OpenGlBlendSetGLuint(context, registers->bldcnt.a_obj,
                       &context->staging.obj_top);
  OpenGlBlendSetGLuint(context, registers->bldcnt.b_obj,
                       &context->staging.obj_bottom);
  OpenGlBlendSetGLuint(context, registers->bldcnt.a_bg0,
                       &context->staging.bg_top[0u][0u]);
  OpenGlBlendSetGLuint(context, registers->bldcnt.b_bg0,
                       &context->staging.bg_bottom[0u][0u]);
  OpenGlBlendSetGLuint(context, registers->bldcnt.a_bg1,
                       &context->staging.bg_top[1u][0u]);
  OpenGlBlendSetGLuint(context, registers->bldcnt.b_bg1,
                       &context->staging.bg_bottom[1u][0u]);
  OpenGlBlendSetGLuint(context, registers->bldcnt.a_bg2,
                       &context->staging.bg_top[2u][0u]);
  OpenGlBlendSetGLuint(context, registers->bldcnt.b_bg2,
                       &context->staging.bg_bottom[2u][0u]);
  OpenGlBlendSetGLuint(context, registers->bldcnt.a_bg3,
                       &context->staging.bg_top[3u][0u]);
  OpenGlBlendSetGLuint(context, registers->bldcnt.b_bg3,
                       &context->staging.bg_bottom[3u][0u]);
  OpenGlBlendSetGLuint(context, registers->bldcnt.a_bd,
                       &context->staging.bd_top);
  OpenGlBlendSetGLuint(context, registers->bldcnt.b_bd,
                       &context->staging.bd_bottom);

  dirty_bits->io.blend = false;

  return context->dirty;
}

void OpenGlBlendBind(const OpenGlBlend* context, GLuint program) {
  GLint blend = glGetUniformBlockIndex(program, "Blend");
  glUniformBlockBinding(program, blend, BLEND_BUFFER);

  glBindBuffer(GL_UNIFORM_BUFFER, context->buffer);
  glBindBufferBase(GL_UNIFORM_BUFFER, BLEND_BUFFER, context->buffer);
}

void OpenGlBlendReload(OpenGlBlend* context) {
  if (context->dirty) {
    glBindBuffer(GL_UNIFORM_BUFFER, context->buffer);
    glBufferSubData(GL_UNIFORM_BUFFER, /*offset=*/0,
                    /*size=*/sizeof(context->staging),
                    /*data=*/&context->staging);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    context->dirty = false;
  }
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