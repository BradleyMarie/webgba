#include "emulator/ppu/gba/opengl/bg_scrolling.h"

#include <string.h>

#include "emulator/ppu/gba/opengl/texture_bindings.h"

bool OpenGlBgScrollingStage(OpenGlBgScrolling* context,
                            const GbaPpuRegisters* registers,
                            GbaPpuDirtyBits* dirty_bits) {
  if (registers->dispcnt.mode > 1u) {
    return false;
  }

  bool result = false;
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

    if (!dirty_bits->io.bg_offset[i]) {
      continue;
    }

    context->staging.origins[i][0u] = registers->bg_offsets[i].x;
    context->staging.origins[i][1u] = registers->bg_offsets[i].y;

    dirty_bits->io.bg_offset[i] = false;
    result = true;
  }

  context->dirty = result;

  return result;
}

void OpenGlBgScrollingBind(const OpenGlBgScrolling* context, GLuint program) {
  GLint scrolling_backgrounds =
      glGetUniformBlockIndex(program, "ScrollingBackgrounds");
  glUniformBlockBinding(program, scrolling_backgrounds, SCROLLING_BUFFER);

  glBindBuffer(GL_UNIFORM_BUFFER, context->buffer);
  glBindBufferBase(GL_UNIFORM_BUFFER, SCROLLING_BUFFER, context->buffer);
}

void OpenGlBgScrollingReload(OpenGlBgScrolling* context) {
  if (context->dirty) {
    glBindBuffer(GL_UNIFORM_BUFFER, context->buffer);
    glBufferSubData(GL_UNIFORM_BUFFER, /*offset=*/0,
                    /*size=*/sizeof(context->staging),
                    /*data=*/&context->staging);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    context->dirty = false;
  }
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