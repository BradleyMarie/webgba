#include "emulator/ppu/gba/ppu.h"

#include <assert.h>
#include <stdlib.h>

#include "emulator/ppu/gba/bg/affine.h"
#include "emulator/ppu/gba/bg/bitmap.h"
#include "emulator/ppu/gba/bg/scrolling.h"
#include "emulator/ppu/gba/io/io.h"
#include "emulator/ppu/gba/memory.h"
#include "emulator/ppu/gba/oam/oam.h"
#include "emulator/ppu/gba/obj/draw.h"
#include "emulator/ppu/gba/palette/palette.h"
#include "emulator/ppu/gba/registers.h"
#include "emulator/ppu/gba/screen.h"
#include "emulator/ppu/gba/vram/vram.h"

#define GBA_PPU_CYCLES_PER_PIXEL 4u
#define GBA_PPU_PIXELS_PER_SCANLINE 308u
#define GBA_PPU_SCANLINES_PER_REFRESH 228u

#define GBA_PPU_DRAW_LENGTH_CYCLES \
  (GBA_PPU_SCANLINES_PER_REFRESH * GBA_PPU_CYCLES_PER_PIXEL)

#define GBA_PPU_HBLANK_LENGTH_PIXELS \
  (GBA_PPU_PIXELS_PER_SCANLINE - GBA_SCREEN_WIDTH)
#define GBA_PPU_HBLANK_LENGTH_CYCLES \
  (GBA_PPU_HBLANK_LENGTH_PIXELS * GBA_PPU_CYCLES_PER_PIXEL)

#define GBA_PPU_FIRST_PIXEL_WAKE_CYCLE (GBA_PPU_CYCLES_PER_PIXEL - 1u)

typedef void (*GbaPpuStepRoutine)(GbaPpu *ppu);

struct _GbaPpu {
  GbaDmaUnit *dma_unit;
  GbaPlatform *platform;
  GbaPpuMemory memory;
  GbaPpuScreen screen;
  GbaPpuRegisters registers;
  GbaPpuInternalRegisters internal_registers;
  GbaPpuObjectVisibility object_visibility;
  GLuint fbo;
  bool hardware_render;
  PpuRenderDoneFunction frame_done;
  GbaPpuStepRoutine next_wake_routine;
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
// Rendering Routines
//

static void GbaPpuStepMode0(GbaPpu *ppu) {
  if (ppu->registers.dispcnt.bg0_enable) {
    GbaPpuScrollingBackgroundPixel(&ppu->memory, &ppu->registers,
                                   &ppu->internal_registers,
                                   GBA_PPU_SCROLLING_BACKGROUND_0, ppu->x,
                                   ppu->registers.vcount, &ppu->screen);
  }

  if (ppu->registers.dispcnt.bg1_enable) {
    GbaPpuScrollingBackgroundPixel(&ppu->memory, &ppu->registers,
                                   &ppu->internal_registers,
                                   GBA_PPU_SCROLLING_BACKGROUND_1, ppu->x,
                                   ppu->registers.vcount, &ppu->screen);
  }

  if (ppu->registers.dispcnt.bg2_enable) {
    GbaPpuScrollingBackgroundPixel(&ppu->memory, &ppu->registers,
                                   &ppu->internal_registers,
                                   GBA_PPU_SCROLLING_BACKGROUND_2, ppu->x,
                                   ppu->registers.vcount, &ppu->screen);
  }

  if (ppu->registers.dispcnt.bg3_enable) {
    GbaPpuScrollingBackgroundPixel(&ppu->memory, &ppu->registers,
                                   &ppu->internal_registers,
                                   GBA_PPU_SCROLLING_BACKGROUND_3, ppu->x,
                                   ppu->registers.vcount, &ppu->screen);
  }

  if (ppu->registers.dispcnt.object_enable) {
    GbaPpuObjectPixel(&ppu->memory, &ppu->registers, &ppu->internal_registers,
                      &ppu->object_visibility, ppu->x, ppu->registers.vcount,
                      &ppu->screen);
  }
}

static void GbaPpuStepMode1(GbaPpu *ppu) {
  if (ppu->registers.dispcnt.bg0_enable) {
    GbaPpuScrollingBackgroundPixel(&ppu->memory, &ppu->registers,
                                   &ppu->internal_registers,
                                   GBA_PPU_SCROLLING_BACKGROUND_0, ppu->x,
                                   ppu->registers.vcount, &ppu->screen);
  }

  if (ppu->registers.dispcnt.bg1_enable) {
    GbaPpuScrollingBackgroundPixel(&ppu->memory, &ppu->registers,
                                   &ppu->internal_registers,
                                   GBA_PPU_SCROLLING_BACKGROUND_1, ppu->x,
                                   ppu->registers.vcount, &ppu->screen);
  }

  if (ppu->registers.dispcnt.bg2_enable) {
    GbaPpuAffineBackgroundPixel(&ppu->memory, &ppu->registers,
                                &ppu->internal_registers,
                                GBA_PPU_AFFINE_BACKGROUND_2, ppu->x,
                                ppu->registers.vcount, &ppu->screen);
  }

  if (ppu->registers.dispcnt.object_enable) {
    GbaPpuObjectPixel(&ppu->memory, &ppu->registers, &ppu->internal_registers,
                      &ppu->object_visibility, ppu->x, ppu->registers.vcount,
                      &ppu->screen);
  }
}

static void GbaPpuStepMode2(GbaPpu *ppu) {
  if (ppu->registers.dispcnt.bg2_enable) {
    GbaPpuAffineBackgroundPixel(&ppu->memory, &ppu->registers,
                                &ppu->internal_registers,
                                GBA_PPU_AFFINE_BACKGROUND_2, ppu->x,
                                ppu->registers.vcount, &ppu->screen);
  }

  if (ppu->registers.dispcnt.bg3_enable) {
    GbaPpuAffineBackgroundPixel(&ppu->memory, &ppu->registers,
                                &ppu->internal_registers,
                                GBA_PPU_AFFINE_BACKGROUND_3, ppu->x,
                                ppu->registers.vcount, &ppu->screen);
  }

  if (ppu->registers.dispcnt.object_enable) {
    GbaPpuObjectPixel(&ppu->memory, &ppu->registers, &ppu->internal_registers,
                      &ppu->object_visibility, ppu->x, ppu->registers.vcount,
                      &ppu->screen);
  }
}

static void GbaPpuStepMode3(GbaPpu *ppu) {
  if (ppu->registers.dispcnt.bg2_enable) {
    GbaPpuBitmapMode3Pixel(&ppu->memory, &ppu->registers,
                           &ppu->internal_registers, ppu->x,
                           ppu->registers.vcount, &ppu->screen);
  }

  if (ppu->registers.dispcnt.object_enable) {
    GbaPpuObjectPixel(&ppu->memory, &ppu->registers, &ppu->internal_registers,
                      &ppu->object_visibility, ppu->x, ppu->registers.vcount,
                      &ppu->screen);
  }
}

static void GbaPpuStepMode4(GbaPpu *ppu) {
  if (ppu->registers.dispcnt.bg2_enable) {
    GbaPpuBitmapMode4Pixel(&ppu->memory, &ppu->registers,
                           &ppu->internal_registers, ppu->x,
                           ppu->registers.vcount, &ppu->screen);
  }

  if (ppu->registers.dispcnt.object_enable) {
    GbaPpuObjectPixel(&ppu->memory, &ppu->registers, &ppu->internal_registers,
                      &ppu->object_visibility, ppu->x, ppu->registers.vcount,
                      &ppu->screen);
  }
}

static void GbaPpuStepMode5(GbaPpu *ppu) {
  if (ppu->registers.dispcnt.bg2_enable) {
    GbaPpuBitmapMode5Pixel(&ppu->memory, &ppu->registers,
                           &ppu->internal_registers, ppu->x,
                           ppu->registers.vcount, &ppu->screen);
  }

  if (ppu->registers.dispcnt.object_enable) {
    GbaPpuObjectPixel(&ppu->memory, &ppu->registers, &ppu->internal_registers,
                      &ppu->object_visibility, ppu->x, ppu->registers.vcount,
                      &ppu->screen);
  }
}

static void GbaPpuStepNoOp(GbaPpu *ppu) {
  // Do Nothing
}

//
// PPU State Machine
//

static void GbaPpuStartHBlank(GbaPpu *ppu) {
  ppu->registers.dispstat.hblank_status = true;
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

static void GbaPpuPreVBlank(GbaPpu *ppu);
static void GbaPpuPostVBlank(GbaPpu *ppu);
static void GbaPpuPostDrawnHBlank(GbaPpu *ppu);
static void GbaPpuPreOffScreenHBlank(GbaPpu *ppu);
static void GbaPpuPostOffScreenHBlank(GbaPpu *ppu);

static void GbaPpuDrawPixel(GbaPpu *ppu) {
  if (ppu->registers.dispcnt.forced_blank) {
    GbaPpuScreenDrawTransparentPixel(&ppu->screen, ppu->x,
                                     ppu->registers.vcount, 0u);
  } else {
    GbaPpuScreenDrawTransparentPixel(&ppu->screen, ppu->x,
                                     ppu->registers.vcount,
                                     ppu->memory.palette.bg.large_palette[0u]);

    static const GbaPpuStepRoutine mode_step_routines[8u] = {
        GbaPpuStepMode0, GbaPpuStepMode1, GbaPpuStepMode2, GbaPpuStepMode3,
        GbaPpuStepMode4, GbaPpuStepMode5, GbaPpuStepNoOp,  GbaPpuStepNoOp};
    mode_step_routines[ppu->registers.dispcnt.mode](ppu);
  }

  if (ppu->x == GBA_SCREEN_WIDTH - 1u) {
    GbaPpuStartHBlank(ppu);

    GbaDmaUnitSignalHBlank(ppu->dma_unit, ppu->registers.vcount);
    if (ppu->registers.dispstat.hblank_irq_enable) {
      GbaPlatformRaiseHBlankInterrupt(ppu->platform);
    }

    if (ppu->registers.vcount == GBA_SCREEN_HEIGHT - 1) {
      ppu->internal_registers.affine[0u].x = ppu->registers.affine[0u].x;
      ppu->internal_registers.affine[0u].y = ppu->registers.affine[0u].y;
      ppu->internal_registers.affine[1u].x = ppu->registers.affine[1u].x;
      ppu->internal_registers.affine[1u].y = ppu->registers.affine[1u].y;
      ppu->next_wake_routine = GbaPpuPreVBlank;
    } else {
      ppu->internal_registers.affine[0u].x += ppu->registers.affine[0u].pb;
      ppu->internal_registers.affine[0u].y += ppu->registers.affine[0u].pd;
      ppu->internal_registers.affine[1u].x += ppu->registers.affine[1u].pb;
      ppu->internal_registers.affine[1u].y += ppu->registers.affine[1u].pd;
      ppu->next_wake_routine = GbaPpuPostDrawnHBlank;
    }

    ppu->next_wake += GBA_PPU_HBLANK_LENGTH_CYCLES;
    ppu->x = 0u;
  } else {
    ppu->next_wake_routine = GbaPpuDrawPixel;
    ppu->next_wake += GBA_PPU_CYCLES_PER_PIXEL;
    ppu->x += 1u;
  }

  ppu->cycle_count += 1;
}

static void GbaPpuPostDrawnHBlank(GbaPpu *ppu) {
  GbaPpuEndHBlank(ppu, ppu->registers.vcount + 1u);

  ppu->next_wake_routine = GbaPpuDrawPixel;
  ppu->next_wake += GBA_PPU_CYCLES_PER_PIXEL;

  ppu->cycle_count += 1;
}

static void GbaPpuPreVBlank(GbaPpu *ppu) {
  GbaPpuEndHBlank(ppu, ppu->registers.vcount + 1u);

  ppu->registers.dispstat.vblank_status = true;

  if (ppu->registers.dispstat.vblank_irq_enable) {
    GbaPlatformRaiseVBlankInterrupt(ppu->platform);
  }

  GbaDmaUnitSignalVBlank(ppu->dma_unit);
  GbaPpuScreenClear(&ppu->screen);

  if (!ppu->hardware_render) {
    GbaPpuScreenRenderToFbo(&ppu->screen, ppu->fbo);
  }

  if (ppu->frame_done != NULL) {
    ppu->frame_done(GBA_SCREEN_WIDTH, GBA_SCREEN_HEIGHT);
  }

  ppu->next_wake_routine = GbaPpuPreOffScreenHBlank;
  ppu->next_wake += GBA_PPU_DRAW_LENGTH_CYCLES;

  ppu->cycle_count += 1;
}

static void GbaPpuPreOffScreenHBlank(GbaPpu *ppu) {
  GbaPpuStartHBlank(ppu);

  if (ppu->registers.vcount == GBA_PPU_SCANLINES_PER_REFRESH - 1) {
    ppu->next_wake_routine = GbaPpuPostVBlank;
  } else {
    ppu->next_wake_routine = GbaPpuPostOffScreenHBlank;
  }

  ppu->next_wake += GBA_PPU_HBLANK_LENGTH_CYCLES;

  ppu->cycle_count += 1;
}

static void GbaPpuPostOffScreenHBlank(GbaPpu *ppu) {
  GbaPpuEndHBlank(ppu, ppu->registers.vcount + 1u);

  ppu->next_wake_routine = GbaPpuPreOffScreenHBlank;
  ppu->next_wake += GBA_PPU_DRAW_LENGTH_CYCLES;

  ppu->cycle_count += 1;
}

static void GbaPpuPostVBlank(GbaPpu *ppu) {
  GbaPpuEndHBlank(ppu, 0u);

  ppu->registers.dispstat.vblank_status = false;

  ppu->next_wake_routine = GbaPpuDrawPixel;
  ppu->next_wake = GBA_PPU_FIRST_PIXEL_WAKE_CYCLE;

  ppu->cycle_count = 0u;
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

  *palette = PaletteAllocate(&(*ppu)->memory.palette, GbaPpuRelease, *ppu);
  if (*palette == NULL) {
    free(*ppu);
    return false;
  }

  (*ppu)->reference_count += 1u;

  *vram = VRamAllocate(&(*ppu)->memory.vram, GbaPpuRelease, *ppu);
  if (*vram == NULL) {
    MemoryFree(*palette);
    free(*ppu);
    return false;
  }

  (*ppu)->reference_count += 1u;

  *oam = OamAllocate(&(*ppu)->memory.oam, &(*ppu)->internal_registers,
                     &(*ppu)->object_visibility, GbaPpuRelease, *ppu);
  if (*oam == NULL) {
    MemoryFree(*vram);
    MemoryFree(*palette);
    free(*ppu);
    return false;
  }

  (*ppu)->reference_count += 1u;

  *registers = GbaPpuIoAllocate(&(*ppu)->registers, &(*ppu)->internal_registers,
                                GbaPpuRelease, *ppu);
  if (*registers == NULL) {
    MemoryFree(*oam);
    MemoryFree(*vram);
    MemoryFree(*palette);
    free(*ppu);
    return false;
  }

  (*ppu)->reference_count += 1u;

  (*ppu)->dma_unit = dma_unit;
  (*ppu)->platform = platform;
  (*ppu)->registers.dispcnt.forced_blank = true;
  (*ppu)->registers.affine[0u].pa = 0x100;
  (*ppu)->registers.affine[0u].pd = 0x100;
  (*ppu)->registers.affine[1u].pa = 0x100;
  (*ppu)->registers.affine[1u].pd = 0x100;
  (*ppu)->registers.dispstat.vcount_status = true;
  (*ppu)->next_wake_routine = GbaPpuDrawPixel;
  (*ppu)->next_wake = GBA_PPU_FIRST_PIXEL_WAKE_CYCLE;

  for (uint_fast8_t object = 0; object < OAM_NUM_OBJECTS; object++) {
    GbaPpuObjectVisibilityDrawn(&(*ppu)->memory.oam, object,
                                &(*ppu)->internal_registers,
                                &(*ppu)->object_visibility);
  }

  GbaDmaUnitRetain(dma_unit);
  GbaPlatformRetain(platform);

  return true;
}

void GbaPpuStep(GbaPpu *ppu) {
  if (ppu->cycle_count != ppu->next_wake) {
    ppu->cycle_count += 1u;
  } else {
    ppu->next_wake_routine(ppu);
  }
}

void GbaPpuFree(GbaPpu *ppu) {
  assert(ppu->reference_count != 0u);
  ppu->reference_count -= 1u;
  if (ppu->reference_count == 0u) {
    GbaPlatformRelease(ppu->platform);
    GbaPpuScreenDestroy(&ppu->screen);
    free(ppu);
  }
}

void GbaPpuSetRenderOutput(GbaPpu *ppu, GLuint framebuffer) {
  ppu->fbo = framebuffer;
}

void GbaPpuSetRenderScale(GbaPpu *ppu, uint8_t scale_factor) {
  assert(scale_factor != 0);
}

void GbaPpuSetRenderDoneCallback(GbaPpu *ppu,
                                 PpuRenderDoneFunction frame_done) {
  ppu->frame_done = frame_done;
}

void GbaPpuReloadContext(GbaPpu *ppu) {
  GbaPpuScreenReloadContext(&ppu->screen);
}