#ifndef _WEBGBA_EMULATOR_PPU_GBA_SOFTWARE_BG_AFFINE_
#define _WEBGBA_EMULATOR_PPU_GBA_SOFTWARE_BG_AFFINE_

#include "emulator/ppu/gba/memory.h"
#include "emulator/ppu/gba/registers.h"

typedef enum {
  GBA_PPU_AFFINE_BACKGROUND_2 = 0,
  GBA_PPU_AFFINE_BACKGROUND_3 = 1,
} GbaPpuAffineBackground;

bool GbaPpuAffineBackgroundPixel(const GbaPpuMemory* memory,
                                 const GbaPpuRegisters* registers,
                                 GbaPpuAffineBackground background, int32_t x,
                                 int32_t y, uint16_t* color);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_SOFTWARE_BG_AFFINE_