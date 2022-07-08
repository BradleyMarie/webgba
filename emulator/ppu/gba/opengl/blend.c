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

  bool dirty = false;

  // First Entry - (mode, eva)
  uint16_t new_value = registers->bldcnt.mode;
  new_value |= ((registers->bldalpha.eva > 16u) ? 16u : registers->bldalpha.eva)
               << 8u;
  if (new_value != context->staging[registers->vcount][0u]) {
    context->staging[registers->vcount][0u] = new_value;
    dirty = true;
  }

  // Second Entry - (top, evb)
  new_value = registers->bldcnt.value & 0x3Fu;
  new_value |= ((registers->bldalpha.evb > 16u) ? 16u : registers->bldalpha.evb)
               << 8u;
  if (new_value != context->staging[registers->vcount][1u]) {
    context->staging[registers->vcount][1u] = new_value;
    dirty = true;
  }

  // Third Entry - (bot, evy)
  new_value = (registers->bldcnt.value >> 8u) & 0x3Fu;
  new_value |= ((registers->bldy.evy > 16u) ? 16u : registers->bldy.evy) << 8u;
  if (new_value != context->staging[registers->vcount][2u]) {
    context->staging[registers->vcount][2u] = new_value;
    dirty = true;
  }

  if (dirty) {
    context->dirty_end = registers->vcount;
    context->dirty = true;
  }

  return context->dirty;
}

void OpenGlBlendBind(OpenGlBlend* context, GLuint program) {
  GLint blend_rows = glGetUniformLocation(program, "blend_rows");
  glUniform1i(blend_rows, BLEND_TEXTURE);

  glActiveTexture(GL_TEXTURE0 + BLEND_TEXTURE);
  glBindTexture(GL_TEXTURE_2D, context->texture);

  if (context->dirty) {
    glTexSubImage2D(GL_TEXTURE_2D, /*level=*/0, /*xoffset=*/0,
                    /*yoffset=*/context->dirty_start, /*width=*/1u,
                    /*height=*/context->dirty_end - context->dirty_start + 1u,
                    /*format=*/GL_RGBA_INTEGER, /*type=*/GL_UNSIGNED_INT,
                    /*pixels=*/context->staging[context->dirty_start]);
    context->dirty = false;
  }
}

void OpenGlBlendReloadContext(OpenGlBlend* context) {
  glGenTextures(1, &context->texture);
  glBindTexture(GL_TEXTURE_2D, context->texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, /*level=*/0, /*internal_format=*/GL_RGBA32UI,
               /*width=*/1u, /*height=*/GBA_SCREEN_HEIGHT, /*border=*/0,
               /*format=*/GL_RGBA_INTEGER, /*type=*/GL_UNSIGNED_INT,
               /*pixels=*/context->staging);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void OpenGlBlendDestroy(OpenGlBlend* context) {
  glDeleteTextures(1u, &context->texture);
}