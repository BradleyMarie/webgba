#ifndef _WEBGBA_EMULATOR_PPU_GBA_PPU_WINDOW_
#define _WEBGBA_EMULATOR_PPU_GBA_PPU_WINDOW_

#include "emulator/ppu/gba/registers.h"

void GbaPpuWindowCheck(const GbaPpuRegisters *registers, uint_fast8_t x,
                       uint_fast8_t y, bool *draw_obj, bool *draw_bg0,
                       bool *draw_bg1, bool *draw_bg2, bool *draw_bg3,
                       bool *enable_blending);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_PPU_WINDOW_