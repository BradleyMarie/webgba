#ifndef _WEBGBA_EMULATOR_PPU_GBA_PPU_
#define _WEBGBA_EMULATOR_PPU_GBA_PPU_

#include <GLES2/gl2.h>

#include "emulator/dma/gba/dma.h"
#include "emulator/memory/memory.h"
#include "emulator/platform/gba/platform.h"

typedef struct _GbaPpu GbaPpu;

// General PPU routines
bool GbaPpuAllocate(GbaDmaUnit *dma_unit, GbaPlatform *platform, GbaPpu **ppu,
                    Memory **pram, Memory **vram, Memory **oam,
                    Memory **registers);

bool GbaPpuStep(GbaPpu *ppu, GLuint fbo, uint8_t scale_factor);

// Context Loss Recovery
void GbaPpuReloadContext(GbaPpu *ppu);

void GbaPpuFree(GbaPpu *ppu);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_PPU_