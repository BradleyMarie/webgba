#ifndef _WEBGBA_EMULATOR_PPU_GBA_BITMAP_
#define _WEBGBA_EMULATOR_PPU_GBA_BITMAP_

#include "emulator/ppu/gba/memory.h"
#include "emulator/ppu/gba/registers.h"
#include "emulator/ppu/gba/screen.h"

void GbaPpuRenderMode3Pixel(const GbaPpuMemory* memory,
                            const GbaPpuRegisters* registers,
                            GbaPpuInternalRegisters* internal_registers,
                            uint_fast8_t x, uint_fast8_t y,
                            GbaPpuScreen* screen);

void GbaPpuRenderMode4Pixel(const GbaPpuMemory* memory,
                            const GbaPpuRegisters* registers,
                            GbaPpuInternalRegisters* internal_registers,
                            uint_fast8_t x, uint_fast8_t y,
                            GbaPpuScreen* screen);

void GbaPpuRenderMode5Pixel(const GbaPpuMemory* memory,
                            const GbaPpuRegisters* registers,
                            GbaPpuInternalRegisters* internal_registers,
                            uint_fast8_t x, uint_fast8_t y,
                            GbaPpuScreen* screen);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_BITMAP_