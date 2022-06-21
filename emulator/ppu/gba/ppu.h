#ifndef _WEBGBA_EMULATOR_PPU_GBA_PPU_
#define _WEBGBA_EMULATOR_PPU_GBA_PPU_

#include <GLES3/gl3.h>

#include "emulator/dma/gba/dma.h"
#include "emulator/memory/memory.h"
#include "emulator/platform/gba/platform.h"
#include "emulator/screen.h"

typedef enum {
  RENDER_MODE_OPENGL_ROWS = 0u,
  RENDER_MODE_SOFTWARE_ROWS = 1u,
  RENDER_MODE_SOFTWARE_PIXELS = 2u,
} GbaPpuRenderMode;

typedef struct _GbaPpu GbaPpu;

// General PPU routines
bool GbaPpuAllocate(GbaDmaUnit *dma_unit, GbaPlatform *platform, GbaPpu **ppu,
                    Memory **pram, Memory **vram, Memory **oam,
                    Memory **registers);

uint32_t GbaPpuCyclesUntilNextWake(const GbaPpu *ppu);

bool GbaPpuStep(GbaPpu *ppu, Screen *screen, uint32_t num_cycles);

// Rendering mode changes take effect at the beginning of each frame
void GbaPpuSetRenderMode(GbaPpu *ppu, GbaPpuRenderMode render_mode,
                         uint8_t opengl_render_scale);

// Context Loss Recovery
void GbaPpuReloadContext(GbaPpu *ppu);

void GbaPpuFree(GbaPpu *ppu);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_PPU_