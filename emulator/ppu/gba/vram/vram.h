#ifndef _WEBGBA_EMULATOR_PPU_GBA_VRAM_VRAM_
#define _WEBGBA_EMULATOR_PPU_GBA_VRAM_VRAM_

#include "emulator/memory/memory.h"
#include "emulator/ppu/gba/types.h"

Memory* VRamAllocate(GbaPpuMemory* ppu_memory);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_VRAM_VRAM_