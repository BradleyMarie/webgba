#include "emulator/ppu/gba/opengl/bg_scrolling.h"

#include <assert.h>
#include <string.h>

#include "emulator/ppu/gba/opengl/texture_bindings.h"

bool OpenGlBgScrollingLoad(OpenGlBgScrolling* context,
                           const GbaPpuRegisters* registers,
                           GbaPpuDirtyBits* dirty_bits) {
  assert(registers->vcount < GBA_SCREEN_HEIGHT);

  if (registers->dispcnt.mode > 1u) {
    return false;
  }

  for (uint8_t i = 0; i < GBA_PPU_NUM_BACKGROUNDS; i++) {
    if (i == 0u && !registers->dispcnt.bg0_enable) {
      continue;
    }

    if (i == 1u && !registers->dispcnt.bg1_enable) {
      continue;
    }

    if (i == 2u &&
        (!registers->dispcnt.bg2_enable || registers->dispcnt.mode != 0u)) {
      continue;
    }

    if (i == 3u &&
        (!registers->dispcnt.bg3_enable || registers->dispcnt.mode != 0u)) {
      continue;
    }

    if (context->staging[registers->vcount].origins[i][0u] !=
        registers->bg_offsets[i].x) {
      context->staging[registers->vcount].origins[i][0u] =
          registers->bg_offsets[i].x;
      context->dirty = true;
    }

    if (context->staging[registers->vcount].origins[i][1u] !=
        registers->bg_offsets[i].y) {
      context->staging[registers->vcount].origins[i][1u] =
          registers->bg_offsets[i].y;
      context->dirty = true;
    }
  }

  return context->dirty;
}

void OpenGlBgScrollingBind(OpenGlBgScrolling* context, GLint start, GLint end,
                           GLuint program) {
  assert(0 <= start);
  assert(start != end);
  assert(end <= GBA_SCREEN_HEIGHT);

  GLint scrolling_backgrounds =
      glGetUniformBlockIndex(program, "ScrollingBackgrounds");
  glUniformBlockBinding(program, scrolling_backgrounds, SCROLLING_BUFFER);

  glBindBuffer(GL_UNIFORM_BUFFER, context->buffer);
  glBindBufferBase(GL_UNIFORM_BUFFER, SCROLLING_BUFFER, context->buffer);

  if (context->dirty) {
    glBufferSubData(GL_UNIFORM_BUFFER,
                    /*offset=*/sizeof(OpenGlScrollingRow) * start,
                    /*size=*/sizeof(OpenGlScrollingRow) * (end - start),
                    /*data=*/&context->staging[start]);
    context->dirty = false;
  }

  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void OpenGlBgScrollingReloadContext(OpenGlBgScrolling* context) {
  glGenBuffers(1, &context->buffer);
  glBindBuffer(GL_UNIFORM_BUFFER, context->buffer);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(context->staging), &context->staging,
               GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OpenGlBgScrollingDestroy(OpenGlBgScrolling* context) {
  glDeleteBuffers(1, &context->buffer);
}