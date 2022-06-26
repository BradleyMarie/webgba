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

  if (!context->dirty) {
    context->dirty_start = registers->vcount;
  }

  bool row_dirty = false;
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

    GLuint value = registers->bg_offsets[i].y;
    value <<= 16u;
    value |= registers->bg_offsets[i].x;

    if (context->staging[registers->vcount].origins[i] != value) {
      context->staging[registers->vcount].origins[i] = value;
      row_dirty = true;
    }
  }

  if (row_dirty) {
    context->dirty_end = registers->vcount;
    context->dirty = true;
  }

  return context->dirty;
}

void OpenGlBgScrollingBind(OpenGlBgScrolling* context, GLuint program) {
  GLint scrolling_backgrounds =
      glGetUniformBlockIndex(program, "ScrollingBackgrounds");
  if (scrolling_backgrounds < 0) {
    return;
  }

  glUniformBlockBinding(program, scrolling_backgrounds, SCROLLING_BUFFER);

  glBindBuffer(GL_UNIFORM_BUFFER, context->buffer);
  glBindBufferBase(GL_UNIFORM_BUFFER, SCROLLING_BUFFER, context->buffer);

  if (context->dirty) {
    glBufferSubData(
        GL_UNIFORM_BUFFER,
        /*offset=*/sizeof(OpenGlScrollingRow) * context->dirty_start,
        /*size=*/sizeof(OpenGlScrollingRow) *
            (context->dirty_end - context->dirty_start + 1u),
        /*data=*/&context->staging[context->dirty_start]);
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