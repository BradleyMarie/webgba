#ifndef _WEBGBA_EMULATOR_PPU_GBA_PALETTE_PALETTE_
#define _WEBGBA_EMULATOR_PPU_GBA_PALETTE_PALETTE_

#include "emulator/memory/memory.h"
#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/memory.h"

Memory* PaletteAllocate(GbaPpuPaletteMemory* palette_memory,
                        GbaPpuPaletteDirtyBits* dirty,
                        MemoryContextFree free_routine, void* free_address);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_PALETTE_PALETTE_