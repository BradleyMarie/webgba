#ifndef _WEBGBA_EMULATOR_PPU_GBA_OAM_OAM_
#define _WEBGBA_EMULATOR_PPU_GBA_OAM_OAM_

#include "emulator/memory/memory.h"
#include "emulator/ppu/gba/memory.h"

Memory* OamAllocate(GbaPpuObjectAttributeMemory* oam_memory, void* free_address,
                    uint16_t* reference_count);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OAM_OAM_