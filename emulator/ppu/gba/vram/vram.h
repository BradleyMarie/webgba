#ifndef _WEBGBA_EMULATOR_PPU_GBA_VRAM_VRAM_
#define _WEBGBA_EMULATOR_PPU_GBA_VRAM_VRAM_

#include "emulator/memory/memory.h"
#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/memory.h"

Memory* VRamAllocate(GbaPpuVideoMemory* video_memory,
                     GbaPpuVramDirtyBits* dirty, MemoryContextFree free_routine,
                     void* free_address);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_VRAM_VRAM_