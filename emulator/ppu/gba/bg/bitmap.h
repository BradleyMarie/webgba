#ifndef _WEBGBA_EMULATOR_PPU_GBA_BG_BITMAP_
#define _WEBGBA_EMULATOR_PPU_GBA_BG_BITMAP_

#include "emulator/ppu/gba/memory.h"
#include "emulator/ppu/gba/registers.h"

bool GbaPpuBitmapMode3Pixel(const GbaPpuMemory* memory,
                            const GbaPpuRegisters* registers,
                            const GbaPpuInternalRegisters* internal_registers,
                            uint_fast8_t x, uint_fast8_t y, uint16_t* color);

bool GbaPpuBitmapMode4Pixel(const GbaPpuMemory* memory,
                            const GbaPpuRegisters* registers,
                            const GbaPpuInternalRegisters* internal_registers,
                            uint_fast8_t x, uint_fast8_t y, uint16_t* color);

bool GbaPpuBitmapMode5Pixel(const GbaPpuMemory* memory,
                            const GbaPpuRegisters* registers,
                            const GbaPpuInternalRegisters* internal_registers,
                            uint_fast8_t x, uint_fast8_t y, uint16_t* color);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_BG_BITMAP_