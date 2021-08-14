#ifndef _WEBGBA_EMULATOR_MEMORY_GBA_IO_
#define _WEBGBA_EMULATOR_MEMORY_GBA_IO_

#include "emulator/memory/memory.h"

Memory *IoMemoryAllocate(Memory *ppu, Memory *sound, Memory *dma, Memory *timer,
                         Memory *peripherals, Memory *platform);

#endif  // _WEBGBA_EMULATOR_MEMORY_GBA_IO_