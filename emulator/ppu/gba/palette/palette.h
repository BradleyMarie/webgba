#ifndef _WEBGBA_EMULATOR_PPU_GBA_PALETTE_PALETTE_
#define _WEBGBA_EMULATOR_PPU_GBA_PALETTE_PALETTE_

#include "emulator/memory/memory.h"
#include "emulator/ppu/gba/memory.h"

Memory* PaletteAllocate(GbaPpuPaletteMemory* palette_memory, void* free_address,
                        uint16_t* reference_count);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_PALETTE_PALETTE_