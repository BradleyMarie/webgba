#include "emulator/ppu/gba/ppu.h"

#include <assert.h>
#include <stdlib.h>

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

typedef void (*GbaPpuStepRoutine)(GbaPpu *ppu, uint32_t x, uint32_t y);

struct _GbaPpu {
  GbaPlatform *platform;
  GbaPpuMemory memory;
  GbaPpuScreen screen;
  GbaPpuRegisters registers;
  GbaPpuInternalRegisters internal_registers;
  GLuint fbo;
  bool hardware_render;
  PpuRenderDoneFunction frame_done;
  uint32_t cycle_count;
  uint16_t reference_count;
};

static void GbaPpuStepMode0(GbaPpu *ppu, uint32_t x, uint32_t y) {
  GbaPpuBackground0Mode0Pixel(&ppu->memory, &ppu->registers,
                              &ppu->internal_registers, x, y, &ppu->screen);
  GbaPpuBackground1Mode0Pixel(&ppu->memory, &ppu->registers,
                              &ppu->internal_registers, x, y, &ppu->screen);
  GbaPpuBackground2Mode0Pixel(&ppu->memory, &ppu->registers,
                              &ppu->internal_registers, x, y, &ppu->screen);
  GbaPpuBackground3Mode0Pixel(&ppu->memory, &ppu->registers,
                              &ppu->internal_registers, x, y, &ppu->screen);
}

static void GbaPpuStepMode1(GbaPpu *ppu, uint32_t x, uint32_t y) {
  GbaPpuBackground0Mode1Pixel(&ppu->memory, &ppu->registers,
                              &ppu->internal_registers, x, y, &ppu->screen);
  GbaPpuBackground1Mode1Pixel(&ppu->memory, &ppu->registers,
                              &ppu->internal_registers, x, y, &ppu->screen);
}

static void GbaPpuStepMode2(GbaPpu *ppu, uint32_t x, uint32_t y) {
  // TODO: Implement
}

static void GbaPpuStepMode3(GbaPpu *ppu, uint32_t x, uint32_t y) {
  GbaPpuBackground2Mode3Pixel(&ppu->memory, &ppu->registers,
                              &ppu->internal_registers, x, y, &ppu->screen);
}

static void GbaPpuStepMode4(GbaPpu *ppu, uint32_t x, uint32_t y) {
  GbaPpuBackground2Mode4Pixel(&ppu->memory, &ppu->registers,
                              &ppu->internal_registers, x, y, &ppu->screen);
}

static void GbaPpuStepMode5(GbaPpu *ppu, uint32_t x, uint32_t y) {
  GbaPpuBackground2Mode5Pixel(&ppu->memory, &ppu->registers,
                              &ppu->internal_registers, x, y, &ppu->screen);
}

static void GbaPpuStepNoOp(GbaPpu *ppu, uint32_t x, uint32_t y) {
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
  (*ppu)->registers.bg2pa = 0x100u;
  (*ppu)->registers.bg2pd = 0x100u;

  GbaPlatformRetain(platform);

  return true;
}

void GbaPpuStep(GbaPpu *ppu) {
  static const GbaPpuStepRoutine mode_step_routines[8u] = {
      GbaPpuStepMode0, GbaPpuStepMode1, GbaPpuStepMode2, GbaPpuStepMode3,
      GbaPpuStepMode4, GbaPpuStepMode5, GbaPpuStepNoOp,  GbaPpuStepNoOp};

  uint32_t cycle_position = ppu->cycle_count % GBA_PPU_CYCLES_PER_PIXEL;
  uint32_t pixel = ppu->cycle_count / GBA_PPU_CYCLES_PER_PIXEL;
  uint32_t x = pixel % GBA_PPU_PIXELS_PER_SCANLINE;
  uint32_t y = pixel / GBA_PPU_PIXELS_PER_SCANLINE;

  if (x != GBA_PPU_PIXELS_PER_SCANLINE - 1 ||
      y != GBA_PPU_ROWS_PER_REFRESH - 1) {
    ppu->cycle_count += 1u;
  } else {
    ppu->cycle_count = 0u;
  }

  ppu->registers.vcount = y;

  if (cycle_position == 0u) {
    if (x == GBA_SCREEN_WIDTH && y < GBA_SCREEN_HEIGHT) {
      GbaPlatformRaiseHBlankInterrupt(ppu->platform);
    } else if (x == 0 && y == GBA_SCREEN_HEIGHT) {
      GbaPlatformRaiseVBlankCountInterrupt(ppu->platform);
      GbaPpuScreenClear(&ppu->screen);
      if (!ppu->hardware_render) {
        GbaPpuScreenRenderToFbo(&ppu->screen, ppu->fbo);
      }
      if (ppu->frame_done != NULL) {
        ppu->frame_done(GBA_SCREEN_WIDTH, GBA_SCREEN_HEIGHT);
      }
    }
    return;
  }

  if (cycle_position != GBA_PPU_CYCLES_PER_PIXEL - 1u ||
      x >= GBA_SCREEN_WIDTH || y >= GBA_SCREEN_HEIGHT) {
    return;
  }

  mode_step_routines[ppu->registers.dispcnt.mode](ppu, x, y);
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