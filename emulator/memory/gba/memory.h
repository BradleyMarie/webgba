#ifndef _WEBGBA_EMULATOR_MEMORY_GBA_MEMORY_
#define _WEBGBA_EMULATOR_MEMORY_GBA_MEMORY_

#include "emulator/memory/memory.h"

Memory* GbaMemoryAllocate(Memory* io, Memory* palette, Memory* vram,
                          Memory* oam, Memory* game, Memory* sram);

#endif  // _WEBGBA_EMULATOR_MEMORY_GBA_MEMORY_