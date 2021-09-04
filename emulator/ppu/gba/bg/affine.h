#ifndef _WEBGBA_EMULATOR_PPU_GBA_BG_AFFINE_
#define _WEBGBA_EMULATOR_PPU_GBA_BG_AFFINE_

#include "emulator/ppu/gba/memory.h"
#include "emulator/ppu/gba/registers.h"
#include "emulator/ppu/gba/screen.h"

typedef enum {
  GBA_PPU_AFFINE_BACKGROUND_2 = 0,
  GBA_PPU_AFFINE_BACKGROUND_3 = 1,
} GbaPpuAffineBackground;

void GbaPpuAffineBackgroundPixel(
    const GbaPpuMemory* memory, const GbaPpuRegisters* registers,
    const GbaPpuInternalRegisters* internal_registers,
    GbaPpuAffineBackground background, uint_fast8_t x, uint_fast8_t y,
    GbaPpuScreen* screen);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_BG_AFFINE_