#ifndef _WEBGBA_EMULATOR_PPU_GBA_OBJ_DRAW_
#define _WEBGBA_EMULATOR_PPU_GBA_OBJ_DRAW_

#include "emulator/ppu/gba/memory.h"
#include "emulator/ppu/gba/obj/visibility.h"
#include "emulator/ppu/gba/registers.h"
#include "emulator/ppu/gba/screen.h"

void GbaPpuObjectPixel(const GbaPpuMemory* memory,
                       const GbaPpuRegisters* registers,
                       const GbaPpuInternalRegisters* internal_registers,
                       const GbaPpuObjectVisibility* visibility,
                       const uint_fast8_t x, uint_fast8_t y,
                       GbaPpuScreen* screen);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OBJ_DRAW_