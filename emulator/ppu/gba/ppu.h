#ifndef _WEBGBA_EMULATOR_PPU_GBA_PPU_
#define _WEBGBA_EMULATOR_PPU_GBA_PPU_

#include "emulator/interrupts/gba/interrupt_controller.h"
#include "emulator/memory/memory.h"

typedef struct _GbaPpu GbaPpu;

bool GbaPpuAllocate(GbaInterruptController *interrupt_controller, GbaPpu **ppu,
                    Memory **pram, Memory **vram, Memory **oam,
                    Memory **registers);

bool GbaPpuStep(GbaPpu *ppu, void *output);

void GbaPpuFree(GbaPpu *ppu);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_PPU_