#ifndef _WEBGBA_EMULATOR_PPU_GBA_VRAM_VRAM_
#define _WEBGBA_EMULATOR_PPU_GBA_VRAM_VRAM_

#include "emulator/memory/memory.h"
#include "emulator/ppu/gba/memory.h"

Memory* VRamAllocate(GbaPpuVideoMemory* video_memory, void* free_address,
                     uint16_t* reference_count);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_VRAM_VRAM_