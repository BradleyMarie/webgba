#include "emulator/ppu/gba/opengl/bg_scrolling.h"

bool OpenGlBgScrollingStage(OpenGlBgScrolling* context,
                            const GbaPpuRegisters* registers,
                            GbaPpuDirtyBits* dirty_bits) {
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

    GLuint value = registers->bg_offsets[i].y;
    value <<= 16u;
    value |= registers->bg_offsets[i].x;

    if (context->staging[i] != value) {
      context->staging[i] = value;
      context->dirty = true;
    }
  }

  return context->dirty;
}

void OpenGlBgScrollingBind(const OpenGlBgScrolling* context, GLuint program) {
  GLint scrolling_coordinates =
      glGetUniformLocation(program, "scrolling_coordinates");
  if (scrolling_coordinates < 0) {
    return;
  }

  glUniform4ui(scrolling_coordinates, context->scrolling[0u],
               context->scrolling[1u], context->scrolling[2u],
               context->scrolling[3u]);
}

void OpenGlBgScrollingReload(OpenGlBgScrolling* context) {
  for (uint8_t i = 0u; i < GBA_PPU_NUM_BACKGROUNDS; i++) {
    context->scrolling[i] = context->staging[i];
  }
}