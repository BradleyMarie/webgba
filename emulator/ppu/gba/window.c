#include "emulator/ppu/gba/window.h"

void GbaPpuWindowCheck(const GbaPpuRegisters *registers, uint_fast8_t x,
                       uint_fast8_t y, bool object_on_pixel, bool *draw_obj,
                       bool *draw_bg0, bool *draw_bg1, bool *draw_bg2,
                       bool *draw_bg3, bool *enable_blending) {
  *draw_obj = true;
  *draw_bg0 = true;
  *draw_bg1 = true;
  *draw_bg2 = true;
  *draw_bg3 = true;
  *enable_blending = true;
}