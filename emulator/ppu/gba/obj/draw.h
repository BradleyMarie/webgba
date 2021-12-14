#ifndef _WEBGBA_EMULATOR_PPU_GBA_OBJ_DRAW_
#define _WEBGBA_EMULATOR_PPU_GBA_OBJ_DRAW_

#include "emulator/ppu/gba/memory.h"
#include "emulator/ppu/gba/obj/visibility.h"
#include "emulator/ppu/gba/registers.h"

bool GbaPpuObjectPixel(const GbaPpuMemory* memory,
                       const GbaPpuRegisters* registers,
                       const GbaPpuInternalRegisters* internal_registers,
                       const GbaPpuObjectVisibility* visibility,
                       const uint_fast8_t x, uint_fast8_t y, uint16_t* color,
                       uint8_t* priority, bool* semi_transparent, bool *on_obj_mask);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OBJ_DRAW_