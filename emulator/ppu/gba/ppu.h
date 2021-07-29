#ifndef _WEBGBA_EMULATOR_PPU_GBA_PPU_
#define _WEBGBA_EMULATOR_PPU_GBA_PPU_

#include <GLES2/gl2.h>

#include "emulator/memory/memory.h"
#include "emulator/platform/gba/platform.h"

typedef struct _GbaPpu GbaPpu;

bool GbaPpuAllocate(GbaPlatform *platform, GbaPpu **ppu, Memory **pram,
                    Memory **vram, Memory **oam, Memory **registers);

typedef void (*PpuFrameDoneFunction)(unsigned width, unsigned height);
void GbaPpuStep(GbaPpu *ppu, GLuint framebuffer,
                PpuFrameDoneFunction done_function);

void GbaPpuReloadContext(GbaPpu *ppu);

void GbaPpuFree(GbaPpu *ppu);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_PPU_