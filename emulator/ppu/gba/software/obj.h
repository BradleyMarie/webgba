#ifndef _WEBGBA_EMULATOR_PPU_GBA_SOFTWARE_OBJ_
#define _WEBGBA_EMULATOR_PPU_GBA_SOFTWARE_OBJ_

#include "emulator/ppu/gba/memory.h"
#include "emulator/ppu/gba/registers.h"
#include "emulator/ppu/gba/software/obj_visibility.h"

bool GbaPpuObjectPixel(const GbaPpuMemory* memory,
                       const GbaPpuRegisters* registers,
                       const GbaPpuObjectVisibility* visibility,
                       const uint_fast8_t x, uint_fast8_t y, uint16_t* color,
                       uint8_t* priority, bool* semi_transparent,
                       bool* on_obj_mask);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_SOFTWARE_OBJ_