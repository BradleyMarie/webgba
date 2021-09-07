#ifndef _WEBGBA_EMULATOR_PPU_GBA_OBJ_OBJECTS_
#define _WEBGBA_EMULATOR_PPU_GBA_OBJ_OBJECTS_

#include "emulator/ppu/gba/memory.h"
#include "emulator/ppu/gba/object_state.h"
#include "emulator/ppu/gba/registers.h"
#include "emulator/ppu/gba/screen.h"

void GbaPpuObjectsPixel(const GbaPpuMemory* memory,
                        const GbaPpuRegisters* registers,
                        const GbaPpuObjectState* object_state,
                        const uint_fast8_t x, uint_fast8_t y,
                        GbaPpuScreen* screen);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OBJ_OBJECTS_