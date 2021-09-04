#ifndef _WEBGBA_EMULATOR_PPU_GBA_BG_AFFINE_
#define _WEBGBA_EMULATOR_PPU_GBA_BG_AFFINE_

#include "emulator/ppu/gba/memory.h"
#include "emulator/ppu/gba/registers.h"
#include "emulator/ppu/gba/screen.h"

void GbaPpuBackground2Mode1Pixel(const GbaPpuMemory* memory,
                                 const GbaPpuRegisters* registers,
                                 GbaPpuInternalRegisters* internal_registers,
                                 uint_fast8_t x, uint_fast8_t y,
                                 GbaPpuScreen* screen);

void GbaPpuBackground2Mode2Pixel(const GbaPpuMemory* memory,
                                 const GbaPpuRegisters* registers,
                                 GbaPpuInternalRegisters* internal_registers,
                                 uint_fast8_t x, uint_fast8_t y,
                                 GbaPpuScreen* screen);

void GbaPpuBackground3Mode2Pixel(const GbaPpuMemory* memory,
                                 const GbaPpuRegisters* registers,
                                 GbaPpuInternalRegisters* internal_registers,
                                 uint_fast8_t x, uint_fast8_t y,
                                 GbaPpuScreen* screen);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_BG_AFFINE_