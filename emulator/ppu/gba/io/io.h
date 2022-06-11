#ifndef _WEBGBA_EMULATOR_PPU_GBA_IO_IO_
#define _WEBGBA_EMULATOR_PPU_GBA_IO_IO_

#include "emulator/memory/memory.h"
#include "emulator/ppu/gba/registers.h"

Memory* GbaPpuIoAllocate(GbaPpuRegisters* registers,
                         MemoryContextFree free_routine, void* free_address);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_IO_IO_