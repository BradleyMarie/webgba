#ifndef _WEBGBA_EMULATOR_PPU_GBA_OAM_OAM_
#define _WEBGBA_EMULATOR_PPU_GBA_OAM_OAM_

#include "emulator/memory/memory.h"
#include "emulator/ppu/gba/memory.h"

Memory* OamAllocate(GbaPpuObjectAttributeMemory* oam_memory,
                    MemoryContextFree free_routine, void* free_address);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OAM_OAM_