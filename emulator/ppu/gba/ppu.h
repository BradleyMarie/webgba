#ifndef _WEBGBA_EMULATOR_PPU_GBA_PPU_
#define _WEBGBA_EMULATOR_PPU_GBA_PPU_

#include "emulator/interrupts/gba/interrupt_controller.h"
#include "emulator/memory/memory.h"

#include <GLES2/gl2.h>

typedef struct _GbaPpu GbaPpu;

bool GbaPpuAllocate(GbaInterruptController *interrupt_controller, GbaPpu **ppu,
                    Memory **pram, Memory **vram, Memory **oam,
                    Memory **registers);

typedef void (*PpuFrameDoneFunction)(unsigned width, unsigned height);
void GbaPpuStep(GbaPpu *ppu, GLuint framebuffer,
                PpuFrameDoneFunction done_function);

void GbaPpuReloadContext(GbaPpu *ppu);

void GbaPpuFree(GbaPpu *ppu);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_PPU_