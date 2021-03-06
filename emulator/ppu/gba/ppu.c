#include "emulator/ppu/gba/ppu.h"

#include <assert.h>
#include <stdlib.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/io/io.h"
#include "emulator/ppu/gba/memory.h"
#include "emulator/ppu/gba/oam/oam.h"
#include "emulator/ppu/gba/opengl/render.h"
#include "emulator/ppu/gba/palette/palette.h"
#include "emulator/ppu/gba/registers.h"
#include "emulator/ppu/gba/software/render.h"
#include "emulator/ppu/gba/vram/vram.h"

#define GBA_PPU_CYCLES_PER_PIXEL 4u
#define GBA_PPU_PIXELS_PER_SCANLINE 308u
#define GBA_PPU_SCANLINES_PER_REFRESH 228u

#define GBA_PPU_DRAW_LENGTH_CYCLES (GBA_SCREEN_WIDTH * GBA_PPU_CYCLES_PER_PIXEL)

#define GBA_PPU_HBLANK_LENGTH_PIXELS \
  (GBA_PPU_PIXELS_PER_SCANLINE - GBA_SCREEN_WIDTH)
#define GBA_PPU_HBLANK_LENGTH_CYCLES \
  (GBA_PPU_HBLANK_LENGTH_PIXELS * GBA_PPU_CYCLES_PER_PIXEL)

typedef enum {
  GBA_PPU_DRAW_PIXEL,
  GBA_PPU_DRAW_ROW,
  GBA_PPU_POST_DRAWN_HBLANK,
  GBA_PPU_PRE_OFFSCREEN_HBLANK,
  GBA_PPU_POST_OFFSCREEN_HBLANK,
} GbaPpuState;

struct _GbaPpu {
  GbaDmaUnit *dma_unit;
  GbaPlatform *platform;
  GbaPpuSoftwareRenderer *software_renderer;
  GbaPpuOpenGlRenderer *opengl_renderer;
  GbaPpuMemory memory;
  GbaPpuRegisters registers;
  GbaPpuRenderMode next_render_mode;
  uint8_t next_render_scale;
  GbaPpuState next_wake_state;
  GbaPpuState draw_state;
  GbaPpuDirtyBits dirty;
  bool use_hardware_renderer;
  bool render_mode_changed;
  uint32_t cycles_from_hblank_to_draw;
  uint_fast8_t x;
  uint32_t cycle_count;
  uint32_t next_wake;
  uint16_t reference_count;
};

//
// Reference Counting
//

static void GbaPpuRelease(void *context) {
  GbaPpu *ppu = (GbaPpu *)context;
  GbaPpuFree(ppu);
}

//
// HBlank
//

static void GbaPpuStartHBlank(GbaPpu *ppu) {
  ppu->registers.dispstat.hblank_status = true;

  if (ppu->registers.dispstat.hblank_irq_enable) {
    GbaPlatformRaiseHBlankInterrupt(ppu->platform);
  }
}

static void GbaPpuEndHBlank(GbaPpu *ppu, uint16_t next_row) {
  ppu->registers.dispstat.hblank_status = false;

  ppu->registers.vcount = next_row;
  if (ppu->registers.dispstat.vcount_trigger == ppu->registers.vcount) {
    ppu->registers.dispstat.vcount_status = true;
    if (ppu->registers.dispstat.vcount_irq_enable) {
      GbaPlatformRaiseVBlankCountInterrupt(ppu->platform);
    }
  } else {
    ppu->registers.dispstat.vcount_status = false;
  }
}

static void GbaPpuDrawnHBlank(GbaPpu *ppu) {
  GbaPpuStartHBlank(ppu);

  GbaDmaUnitSignalHBlank(ppu->dma_unit, ppu->registers.vcount);

  if (ppu->registers.vcount == GBA_SCREEN_HEIGHT - 1) {
    ppu->registers.internal.affine[0u].row_start[0u] =
        ppu->registers.affine[0u].x;
    ppu->registers.internal.affine[0u].row_start[1u] =
        ppu->registers.affine[0u].y;
    ppu->registers.internal.affine[1u].row_start[0u] =
        ppu->registers.affine[1u].x;
    ppu->registers.internal.affine[1u].row_start[1u] =
        ppu->registers.affine[1u].y;
  } else {
    ppu->registers.internal.affine[0u].row_start[0u] +=
        ppu->registers.affine[0u].pb;
    ppu->registers.internal.affine[0u].row_start[1u] +=
        ppu->registers.affine[0u].pd;
    ppu->registers.internal.affine[1u].row_start[0u] +=
        ppu->registers.affine[1u].pb;
    ppu->registers.internal.affine[1u].row_start[1u] +=
        ppu->registers.affine[1u].pd;
  }

  ppu->registers.internal.affine[0u].current[0u] =
      ppu->registers.internal.affine[0u].row_start[0u];
  ppu->registers.internal.affine[0u].current[1u] =
      ppu->registers.internal.affine[0u].row_start[1u];
  ppu->registers.internal.affine[1u].current[0u] =
      ppu->registers.internal.affine[1u].row_start[0u];
  ppu->registers.internal.affine[1u].current[1u] =
      ppu->registers.internal.affine[1u].row_start[1u];

  ppu->next_wake_state = GBA_PPU_POST_DRAWN_HBLANK;
  ppu->next_wake += GBA_PPU_HBLANK_LENGTH_CYCLES;
  ppu->x = 0u;
}

//
// Public Functions
//

bool GbaPpuAllocate(GbaDmaUnit *dma_unit, GbaPlatform *platform, GbaPpu **ppu,
                    Memory **palette, Memory **vram, Memory **oam,
                    Memory **registers) {
  *ppu = (GbaPpu *)calloc(1, sizeof(GbaPpu));
  if (*ppu == NULL) {
    return false;
  }

  (*ppu)->reference_count = 1u;

  *palette = PaletteAllocate(&(*ppu)->memory.palette, &(*ppu)->dirty.palette,
                             GbaPpuRelease, *ppu);
  if (*palette == NULL) {
    free(*ppu);
    return false;
  }

  (*ppu)->reference_count += 1u;

  *vram = VRamAllocate(&(*ppu)->memory.vram, &(*ppu)->dirty.vram, GbaPpuRelease,
                       *ppu);
  if (*vram == NULL) {
    MemoryFree(*palette);
    free(*ppu);
    return false;
  }

  (*ppu)->reference_count += 1u;

  *oam =
      OamAllocate(&(*ppu)->memory.oam, &(*ppu)->dirty.oam, GbaPpuRelease, *ppu);
  if (*oam == NULL) {
    MemoryFree(*vram);
    MemoryFree(*palette);
    free(*ppu);
    return false;
  }

  (*ppu)->reference_count += 1u;

  *registers = GbaPpuIoAllocate(&(*ppu)->registers, &(*ppu)->dirty.io,
                                GbaPpuRelease, *ppu);
  if (*registers == NULL) {
    MemoryFree(*oam);
    MemoryFree(*vram);
    MemoryFree(*palette);
    free(*ppu);
    return false;
  }

  (*ppu)->reference_count += 1u;

  (*ppu)->software_renderer = GbaPpuSoftwareRendererAllocate();
  if ((*ppu)->software_renderer == NULL) {
    MemoryFree(*oam);
    MemoryFree(*vram);
    MemoryFree(*palette);
    MemoryFree(*registers);
    free(*ppu);
    return false;
  }

  (*ppu)->opengl_renderer = GbaPpuOpenGlRendererAllocate();
  if ((*ppu)->opengl_renderer == NULL) {
    MemoryFree(*oam);
    MemoryFree(*vram);
    MemoryFree(*palette);
    MemoryFree(*registers);
    GbaPpuSoftwareRendererFree((*ppu)->software_renderer);
    free(*ppu);
    return false;
  }

  for (uint8_t i = 0; i < OAM_NUM_OBJECTS; i++) {
    GbaPpuObjectVisibilityHidden(&(*ppu)->memory.oam, i);
    GbaPpuObjectVisibilityDrawn(&(*ppu)->memory.oam, i);
  }

  (*ppu)->dma_unit = dma_unit;
  (*ppu)->platform = platform;
  (*ppu)->registers.dispcnt.forced_blank = true;
  (*ppu)->registers.affine[0u].pa = 0x100;
  (*ppu)->registers.affine[0u].pd = 0x100;
  (*ppu)->registers.affine[1u].pa = 0x100;
  (*ppu)->registers.affine[1u].pd = 0x100;
  (*ppu)->registers.dispstat.vcount_status = true;
  GbaPpuDirtyBitsAllDirty(&(*ppu)->dirty);

  GbaPpuSetRenderMode(*ppu, RENDER_MODE_SOFTWARE_ROWS, 1u);

  GbaDmaUnitRetain(dma_unit);
  GbaPlatformRetain(platform);

  return true;
}

uint32_t GbaPpuCyclesUntilNextWake(const GbaPpu *ppu) {
  return ppu->next_wake - ppu->cycle_count;
}

bool GbaPpuStep(GbaPpu *ppu, Screen *screen, uint32_t num_cycles) {
  ppu->cycle_count += num_cycles;
  assert(ppu->cycle_count <= ppu->next_wake);

  if (ppu->cycle_count != ppu->next_wake) {
    return false;
  }

  switch (ppu->next_wake_state) {
    case GBA_PPU_DRAW_ROW:
      if (ppu->use_hardware_renderer) {
        if (ppu->registers.vcount == 0u) {
          GbaPpuOpenGlRendererSetScale(ppu->opengl_renderer,
                                       ppu->next_render_scale);
          GbaPpuOpenGlRendererSetScreen(ppu->opengl_renderer, screen);
        }

        GbaPpuOpenGlRendererDrawRow(ppu->opengl_renderer, &ppu->memory,
                                    &ppu->registers, &ppu->dirty);
      } else {
        if (ppu->registers.vcount == 0u) {
          // TODO: Handle allocation failure
          GbaPpuSoftwareRendererSetScreen(ppu->software_renderer, screen);
        }

        GbaPpuSoftwareRendererDrawRow(ppu->software_renderer, &ppu->memory,
                                      &ppu->registers, &ppu->dirty);
      }

      GbaPpuDrawnHBlank(ppu);
      break;
    case GBA_PPU_DRAW_PIXEL:
      if (ppu->use_hardware_renderer) {
        // Not Implemented
      } else {
        if (ppu->registers.vcount == 0u && ppu->x == 0u) {
          // TODO: Handle allocation failure
          GbaPpuSoftwareRendererSetScreen(ppu->software_renderer, screen);
        }

        GbaPpuSoftwareRendererDrawPixel(ppu->software_renderer, &ppu->memory,
                                        &ppu->registers, &ppu->dirty, ppu->x);
      }

      if (ppu->x == GBA_SCREEN_WIDTH - 1u) {
        GbaPpuDrawnHBlank(ppu);
      } else {
        ppu->registers.internal.affine[0u].current[0u] +=
            ppu->registers.affine[0u].pa;
        ppu->registers.internal.affine[0u].current[1u] +=
            ppu->registers.affine[0u].pc;
        ppu->registers.internal.affine[1u].current[0u] +=
            ppu->registers.affine[1u].pa;
        ppu->registers.internal.affine[1u].current[1u] +=
            ppu->registers.affine[1u].pc;
        ppu->x += 1u;

        ppu->next_wake_state = GBA_PPU_DRAW_PIXEL;
        ppu->next_wake += GBA_PPU_CYCLES_PER_PIXEL;
      }
      break;
    case GBA_PPU_POST_DRAWN_HBLANK:
      GbaPpuEndHBlank(ppu, ppu->registers.vcount + 1u);
      if (ppu->registers.vcount == GBA_SCREEN_HEIGHT) {
        ppu->registers.dispstat.vblank_status = true;

        if (ppu->registers.dispstat.vblank_irq_enable) {
          GbaPlatformRaiseVBlankInterrupt(ppu->platform);
        }

        GbaDmaUnitSignalVBlank(ppu->dma_unit);

        ppu->next_wake_state = GBA_PPU_PRE_OFFSCREEN_HBLANK;
        ppu->next_wake += GBA_PPU_DRAW_LENGTH_CYCLES;
        return true;
      }

      ppu->next_wake_state = ppu->draw_state;
      ppu->next_wake += ppu->cycles_from_hblank_to_draw;
      break;
    case GBA_PPU_PRE_OFFSCREEN_HBLANK:
      GbaPpuStartHBlank(ppu);
      ppu->next_wake_state = GBA_PPU_POST_OFFSCREEN_HBLANK;
      ppu->next_wake += GBA_PPU_HBLANK_LENGTH_CYCLES;
      break;
    case GBA_PPU_POST_OFFSCREEN_HBLANK:
      if (ppu->registers.vcount < GBA_PPU_SCANLINES_PER_REFRESH - 1) {
        GbaPpuEndHBlank(ppu, ppu->registers.vcount + 1u);
        ppu->next_wake_state = GBA_PPU_PRE_OFFSCREEN_HBLANK;
        ppu->next_wake += GBA_PPU_DRAW_LENGTH_CYCLES;
      } else {
        GbaPpuEndHBlank(ppu, 0u);
        ppu->registers.dispstat.vblank_status = false;
        ppu->cycle_count = 0u;
        GbaPpuSetRenderMode(ppu, ppu->next_render_mode, ppu->next_render_scale);
      }
      break;
  }

  return false;
}

void GbaPpuSetRenderMode(GbaPpu *ppu, GbaPpuRenderMode render_mode,
                         uint8_t opengl_render_scale) {
  if (ppu->next_render_mode != render_mode ||
      ppu->next_render_scale != opengl_render_scale) {
    ppu->render_mode_changed = true;
  }

  ppu->next_render_mode = render_mode;
  ppu->next_render_scale = opengl_render_scale;

  if (ppu->cycle_count != 0) {
    return;
  }

  if (ppu->render_mode_changed) {
    GbaPpuDirtyBitsAllDirty(&ppu->dirty);
    ppu->render_mode_changed = false;
  }

  switch (render_mode) {
    case RENDER_MODE_OPENGL_ROWS:
      ppu->use_hardware_renderer = true;
      ppu->next_wake_state = GBA_PPU_DRAW_ROW;
      ppu->next_wake = GBA_PPU_DRAW_LENGTH_CYCLES;
      ppu->draw_state = GBA_PPU_DRAW_ROW;
      ppu->cycles_from_hblank_to_draw = GBA_PPU_DRAW_LENGTH_CYCLES;
      break;
    case RENDER_MODE_SOFTWARE_ROWS:
      ppu->use_hardware_renderer = false;
      ppu->next_wake_state = GBA_PPU_DRAW_ROW;
      ppu->next_wake = GBA_PPU_DRAW_LENGTH_CYCLES;
      ppu->draw_state = GBA_PPU_DRAW_ROW;
      ppu->cycles_from_hblank_to_draw = GBA_PPU_DRAW_LENGTH_CYCLES;
      break;
    case RENDER_MODE_SOFTWARE_PIXELS:
      ppu->use_hardware_renderer = false;
      ppu->next_wake_state = GBA_PPU_DRAW_PIXEL;
      ppu->next_wake = GBA_PPU_CYCLES_PER_PIXEL;
      ppu->draw_state = GBA_PPU_DRAW_PIXEL;
      ppu->cycles_from_hblank_to_draw = GBA_PPU_CYCLES_PER_PIXEL;
      break;
  }
}

void GbaPpuFree(GbaPpu *ppu) {
  assert(ppu->reference_count != 0u);
  ppu->reference_count -= 1u;
  if (ppu->reference_count == 0u) {
    GbaPlatformRelease(ppu->platform);
    GbaPpuSoftwareRendererFree(ppu->software_renderer);
    GbaPpuOpenGlRendererFree(ppu->opengl_renderer);
    free(ppu);
  }
}

void GbaPpuReloadContext(GbaPpu *ppu) {
  GbaPpuOpenGlRendererReloadContext(ppu->opengl_renderer);
  GbaPpuDirtyBitsAllDirty(&ppu->dirty);
}