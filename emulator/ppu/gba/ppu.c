#include "emulator/ppu/gba/ppu.h"

#include <assert.h>
#include <stdlib.h>

#include "emulator/ppu/gba/bg/affine.h"
#include "emulator/ppu/gba/bg/bitmap.h"
#include "emulator/ppu/gba/bg/linear.h"
#include "emulator/ppu/gba/io/io.h"
#include "emulator/ppu/gba/memory.h"
#include "emulator/ppu/gba/oam/oam.h"
#include "emulator/ppu/gba/palette/palette.h"
#include "emulator/ppu/gba/registers.h"
#include "emulator/ppu/gba/screen.h"
#include "emulator/ppu/gba/vram/vram.h"

#define GBA_PPU_CYCLES_PER_PIXEL 4u
#define GBA_PPU_PIXELS_PER_SCANLINE 308u
#define GBA_PPU_ROWS_PER_REFRESH 228u
#define GBA_PPU_CYCLES_PER_VBLANK                           \
  (GBA_PPU_ROWS_PER_REFRESH * GBA_PPU_PIXELS_PER_SCANLINE * \
   GBA_PPU_CYCLES_PER_PIXEL)

typedef void (*GbaPpuStepRoutine)(GbaPpu *ppu);

struct _GbaPpu {
  GbaPlatform *platform;
  GbaPpuMemory memory;
  GbaPpuScreen screen;
  GbaPpuRegisters registers;
  GbaPpuInternalRegisters internal_registers;
  GLuint fbo;
  bool hardware_render;
  PpuRenderDoneFunction frame_done;
  uint_fast8_t x;
  uint_fast8_t y;
  uint32_t next_wake;
  uint16_t reference_count;
};

static void GbaPpuStepMode0(GbaPpu *ppu) {
  GbaPpuBackground0Mode0Pixel(&ppu->memory, &ppu->registers,
                              &ppu->internal_registers, ppu->x, ppu->y,
                              &ppu->screen);
  GbaPpuBackground1Mode0Pixel(&ppu->memory, &ppu->registers,
                              &ppu->internal_registers, ppu->x, ppu->y,
                              &ppu->screen);
  GbaPpuBackground2Mode0Pixel(&ppu->memory, &ppu->registers,
                              &ppu->internal_registers, ppu->x, ppu->y,
                              &ppu->screen);
  GbaPpuBackground3Mode0Pixel(&ppu->memory, &ppu->registers,
                              &ppu->internal_registers, ppu->x, ppu->y,
                              &ppu->screen);
}

static void GbaPpuStepMode1(GbaPpu *ppu) {
  GbaPpuBackground0Mode1Pixel(&ppu->memory, &ppu->registers,
                              &ppu->internal_registers, ppu->x, ppu->y,
                              &ppu->screen);
  GbaPpuBackground1Mode1Pixel(&ppu->memory, &ppu->registers,
                              &ppu->internal_registers, ppu->x, ppu->y,
                              &ppu->screen);
  GbaPpuBackground2Mode1Pixel(&ppu->memory, &ppu->registers,
                              &ppu->internal_registers, ppu->x, ppu->y,
                              &ppu->screen);
}

static void GbaPpuStepMode2(GbaPpu *ppu) {
  GbaPpuBackground2Mode2Pixel(&ppu->memory, &ppu->registers,
                              &ppu->internal_registers, ppu->x, ppu->y,
                              &ppu->screen);
  GbaPpuBackground3Mode2Pixel(&ppu->memory, &ppu->registers,
                              &ppu->internal_registers, ppu->x, ppu->y,
                              &ppu->screen);
}

static void GbaPpuStepMode3(GbaPpu *ppu) {
  GbaPpuBackground2Mode3Pixel(&ppu->memory, &ppu->registers,
                              &ppu->internal_registers, ppu->x, ppu->y,
                              &ppu->screen);
}

static void GbaPpuStepMode4(GbaPpu *ppu) {
  GbaPpuBackground2Mode4Pixel(&ppu->memory, &ppu->registers,
                              &ppu->internal_registers, ppu->x, ppu->y,
                              &ppu->screen);
}

static void GbaPpuStepMode5(GbaPpu *ppu) {
  GbaPpuBackground2Mode5Pixel(&ppu->memory, &ppu->registers,
                              &ppu->internal_registers, ppu->x, ppu->y,
                              &ppu->screen);
}

static void GbaPpuStepNoOp(GbaPpu *ppu) {
  // Do Nothing
}

static void GbaPpuRelease(void *context) {
  GbaPpu *ppu = (GbaPpu *)context;
  GbaPpuFree(ppu);
}

bool GbaPpuAllocate(GbaPlatform *platform, GbaPpu **ppu, Memory **palette,
                    Memory **vram, Memory **oam, Memory **registers) {
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

  *oam = OamAllocate(&(*ppu)->memory.oam, GbaPpuRelease, *ppu);
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

  (*ppu)->platform = platform;
  (*ppu)->registers.dispcnt.forced_blank = true;
  (*ppu)->registers.affine[0u].pa = 0x100;
  (*ppu)->registers.affine[0u].pd = 0x100;
  (*ppu)->registers.affine[1u].pa = 0x100;
  (*ppu)->registers.affine[1u].pd = 0x100;
  (*ppu)->next_wake = GBA_PPU_CYCLES_PER_PIXEL - 1u;

  GbaPlatformRetain(platform);

  return true;
}

void GbaPpuStep(GbaPpu *ppu) {
  if (ppu->internal_registers.cycle_count != ppu->next_wake) {
    ppu->internal_registers.cycle_count += 1u;
    return;
  }

  if (ppu->internal_registers.cycle_count == GBA_PPU_CYCLES_PER_VBLANK - 1u) {
    ppu->internal_registers.cycle_count = 0u;
    ppu->next_wake = GBA_PPU_CYCLES_PER_PIXEL - 1u;
    return;
  }

  if (ppu->y == GBA_SCREEN_HEIGHT) {
    GbaPlatformRaiseVBlankCountInterrupt(ppu->platform);
    GbaPpuScreenClear(&ppu->screen);
    if (!ppu->hardware_render) {
      GbaPpuScreenRenderToFbo(&ppu->screen, ppu->fbo);
    }
    if (ppu->frame_done != NULL) {
      ppu->frame_done(GBA_SCREEN_WIDTH, GBA_SCREEN_HEIGHT);
    }
    ppu->internal_registers.affine[0u].x = ppu->registers.affine[0u].x;
    ppu->internal_registers.affine[0u].y = ppu->registers.affine[0u].y;
    ppu->internal_registers.affine[1u].x = ppu->registers.affine[1u].x;
    ppu->internal_registers.affine[1u].y = ppu->registers.affine[1u].y;
    ppu->x = 0u;
    ppu->y = 0u;
    ppu->next_wake = GBA_PPU_CYCLES_PER_VBLANK - 1u;
    return;
  }

  GbaPpuScreenDrawPixel(&ppu->screen, ppu->x, ppu->y,
                        ppu->memory.palette.bg.large_palette[0u],
                        GBA_PPU_SCREEN_TRANSPARENT_PRIORITY);

  static const GbaPpuStepRoutine mode_step_routines[8u] = {
      GbaPpuStepMode0, GbaPpuStepMode1, GbaPpuStepMode2, GbaPpuStepMode3,
      GbaPpuStepMode4, GbaPpuStepMode5, GbaPpuStepNoOp,  GbaPpuStepNoOp};
  mode_step_routines[ppu->registers.dispcnt.mode](ppu);

  ppu->internal_registers.cycle_count += 1u;

  if (ppu->x == GBA_SCREEN_WIDTH - 1u) {
    if (ppu->y < GBA_SCREEN_HEIGHT - 1u) {
      GbaPlatformRaiseHBlankInterrupt(ppu->platform);
      ppu->next_wake += (GBA_PPU_PIXELS_PER_SCANLINE - GBA_SCREEN_WIDTH) *
                        GBA_PPU_CYCLES_PER_PIXEL;
    } else {
      // Schedule VBlank
      ppu->next_wake = GBA_PPU_PIXELS_PER_SCANLINE * GBA_SCREEN_HEIGHT *
                       GBA_PPU_CYCLES_PER_PIXEL;
    }
    ppu->internal_registers.affine[0u].x += ppu->registers.affine[0u].pb;
    ppu->internal_registers.affine[0u].y += ppu->registers.affine[0u].pd;
    ppu->internal_registers.affine[1u].x += ppu->registers.affine[1u].pb;
    ppu->internal_registers.affine[1u].y += ppu->registers.affine[1u].pd;
    ppu->x = 0u;
    ppu->y += 1u;
  } else {
    ppu->next_wake += GBA_PPU_CYCLES_PER_PIXEL;
    ppu->x += 1u;
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