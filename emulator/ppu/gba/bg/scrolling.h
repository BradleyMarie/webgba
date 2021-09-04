#ifndef _WEBGBA_EMULATOR_PPU_GBA_BG_SCROLLING_
#define _WEBGBA_EMULATOR_PPU_GBA_BG_SCROLLING_

#include "emulator/ppu/gba/memory.h"
#include "emulator/ppu/gba/registers.h"
#include "emulator/ppu/gba/screen.h"

typedef enum {
  GBA_PPU_SCROLLING_BACKGROUND_0 = 0,
  GBA_PPU_SCROLLING_BACKGROUND_1 = 1,
  GBA_PPU_SCROLLING_BACKGROUND_2 = 2,
  GBA_PPU_SCROLLING_BACKGROUND_3 = 3,
} GbaPpuScrollingBackground;

void GbaPpuScrollingBackgroundPixel(
    const GbaPpuMemory* memory, const GbaPpuRegisters* registers,
    const GbaPpuInternalRegisters* internal_registers,
    GbaPpuScrollingBackground background, uint_fast8_t x, uint_fast8_t y,
    GbaPpuScreen* screen);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_BG_SCROLLING_