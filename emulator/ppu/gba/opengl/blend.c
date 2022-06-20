#include "emulator/ppu/gba/opengl/blend.h"

#include <assert.h>
#include <math.h>
#include <string.h>

#include "emulator/ppu/gba/opengl/texture_bindings.h"

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

  OpenGlBlendRow old_row = context->staging[registers->vcount];

  context->staging[registers->vcount].mode = registers->bldcnt.mode;
  context->staging[registers->vcount].top = registers->bldcnt.value & 0x3Fu;
  context->staging[registers->vcount].bottom =
      (registers->bldcnt.value >> 8u) & 0x3Fu;
  context->staging[registers->vcount].eva =
      (registers->bldalpha.eva > 16u) ? 16u : registers->bldalpha.eva;
  context->staging[registers->vcount].evb =
      (registers->bldalpha.evb > 16u) ? 16u : registers->bldalpha.evb;
  context->staging[registers->vcount].evy =
      (registers->bldy.evy > 16u) ? 16u : registers->bldy.evy;

  if (memcmp(&old_row, &context->staging[registers->vcount],
             sizeof(OpenGlBlendRow)) != 0) {
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