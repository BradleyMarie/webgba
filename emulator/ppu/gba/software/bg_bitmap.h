#ifndef _WEBGBA_EMULATOR_PPU_GBA_SOFTWARE_BG_BITMAP_
#define _WEBGBA_EMULATOR_PPU_GBA_SOFTWARE_BG_BITMAP_

#include "emulator/ppu/gba/memory.h"
#include "emulator/ppu/gba/registers.h"

bool GbaPpuBitmapMode3Pixel(const GbaPpuMemory* memory,
                            const GbaPpuRegisters* registers, int32_t x,
                            int32_t y, uint16_t* color);

bool GbaPpuBitmapMode4Pixel(const GbaPpuMemory* memory,
                            const GbaPpuRegisters* registers, int32_t x,
                            int32_t y, uint16_t* color);

bool GbaPpuBitmapMode5Pixel(const GbaPpuMemory* memory,
                            const GbaPpuRegisters* registers, int32_t x,
                            int32_t y, uint16_t* color);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_SOFTWARE_BG_BITMAP_