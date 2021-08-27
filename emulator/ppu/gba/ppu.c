#include "emulator/ppu/gba/ppu.h"

#include <assert.h>
#include <stdlib.h>

#include "emulator/ppu/gba/bitmap.h"
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

void GbaPpuRelease(void *context) {
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

void GbaPpuFree(GbaPpu *ppu) {
  assert(ppu->reference_count != 0u);
  ppu->reference_count -= 1u;
  if (ppu->reference_count == 0u) {
    GbaPlatformRelease(ppu->platform);
    GbaPpuScreenDestroy(&ppu->screen);
    free(ppu);
  }
}

void GbaPpuStep(GbaPpu *ppu) {
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
      if (!ppu->hardware_render) {
        GbaPpuScreenCopyToFbo(&ppu->screen, ppu->fbo);
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

  switch (ppu->registers.dispcnt.mode) {
    case 0u:
      break;
    case 1u:
      break;
    case 2u:
      break;
    case 3u:
      GbaPpuRenderMode3Pixel(&ppu->memory, &ppu->registers,
                             &ppu->internal_registers, x, y, &ppu->screen);
      break;
    case 4u:
      GbaPpuRenderMode4Pixel(&ppu->memory, &ppu->registers,
                             &ppu->internal_registers, x, y, &ppu->screen);
      break;
    case 5u:
      GbaPpuRenderMode5Pixel(&ppu->memory, &ppu->registers,
                             &ppu->internal_registers, x, y, &ppu->screen);
      break;
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