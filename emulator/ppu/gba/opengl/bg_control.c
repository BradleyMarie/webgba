#include "emulator/ppu/gba/opengl/bg_control.h"

bool OpenGlBgControlStage(OpenGlBgControl* context,
                          const GbaPpuRegisters* registers,
                          GbaPpuDirtyBits* dirty_bits) {
  for (uint8_t i = 0u; i < GBA_PPU_NUM_BACKGROUNDS; i++) {
    if (i == 0u &&
        (!registers->dispcnt.bg0_enable || registers->dispcnt.mode > 1u)) {
      continue;
    }

    if (i == 1u &&
        (!registers->dispcnt.bg1_enable || registers->dispcnt.mode > 1u)) {
      continue;
    }

    if (i == 2u && !registers->dispcnt.bg2_enable) {
      continue;
    }

    if (i == 3u &&
        (!registers->dispcnt.bg3_enable ||
         (registers->dispcnt.mode != 0u && registers->dispcnt.mode != 2u))) {
      continue;
    }

    GLuint new_value = registers->bgcnt[i].value;

    if (registers->bgcnt[i].mosaic) {
      new_value |= (registers->mosaic.bg_horiz + 1u) << 16u;
      new_value |= (registers->mosaic.bg_vert + 1u) << 24u;
    } else {
      new_value |= 1u << 16u;
      new_value |= 1u << 24u;
    }

    if (new_value != context->staging[i]) {
      context->staging[i] = new_value;
      context->dirty = true;
    }
  }

  return context->dirty;
}

void OpenGlBgControlBind(const OpenGlBgControl* context,
                         const UniformLocations* locations) {
  glUniform4ui(locations->background_control, context->bgcnt[0u],
               context->bgcnt[1u], context->bgcnt[2u], context->bgcnt[3u]);
}

void OpenGlBgControlReload(OpenGlBgControl* context) {
  if (context->dirty) {
    for (uint8_t i = 0; i < GBA_PPU_NUM_BACKGROUNDS; i++) {
      context->bgcnt[i] = context->staging[i];
    }

    context->dirty = false;
  }
}