#ifndef _WEBGBA_EMULATOR_PPU_GBA_BG_BITMAP_
#define _WEBGBA_EMULATOR_PPU_GBA_BG_BITMAP_

#include "emulator/ppu/gba/memory.h"
#include "emulator/ppu/gba/registers.h"
#include "emulator/ppu/gba/screen.h"

void GbaPpuBitmapMode3Pixel(const GbaPpuMemory* memory,
                            const GbaPpuRegisters* registers,
                            const GbaPpuInternalRegisters* internal_registers,
                            uint_fast8_t x, uint_fast8_t y,
                            GbaPpuScreen* screen);

void GbaPpuBitmapMode4Pixel(const GbaPpuMemory* memory,
                            const GbaPpuRegisters* registers,
                            const GbaPpuInternalRegisters* internal_registers,
                            uint_fast8_t x, uint_fast8_t y,
                            GbaPpuScreen* screen);

void GbaPpuBitmapMode5Pixel(const GbaPpuMemory* memory,
                            const GbaPpuRegisters* registers,
                            const GbaPpuInternalRegisters* internal_registers,
                            uint_fast8_t x, uint_fast8_t y,
                            GbaPpuScreen* screen);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_BG_BITMAP_