#ifndef _WEBGBA_EMULATOR_PPU_GBA_PPU_
#define _WEBGBA_EMULATOR_PPU_GBA_PPU_

#include <GLES2/gl2.h>

#include "emulator/dma/gba/dma.h"
#include "emulator/memory/memory.h"
#include "emulator/platform/gba/platform.h"

typedef enum {
  RENDER_MODE_HARDWARE_ROWS = 0u,
  RENDER_MODE_SOFTWARE_ROWS = 1u,
  RENDER_MODE_SOFTWARE_PIXELS = 2u,
} GbaPpuRenderMode;

typedef struct _GbaPpu GbaPpu;

// General PPU routines
bool GbaPpuAllocate(GbaDmaUnit *dma_unit, GbaPlatform *platform, GbaPpu **ppu,
                    Memory **pram, Memory **vram, Memory **oam,
                    Memory **registers);

uint32_t GbaPpuCyclesUntilNextWake(const GbaPpu *ppu);

bool GbaPpuStep(GbaPpu *ppu, uint32_t num_cycles, GLuint fbo,
                uint8_t scale_factor);

// Rendering mode changes take effect at the beginning of each frame
void GbaPpuSetRenderMode(GbaPpu *ppu, GbaPpuRenderMode render_mode);

// Context Loss Recovery
void GbaPpuReloadContext(GbaPpu *ppu);

void GbaPpuFree(GbaPpu *ppu);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_PPU_