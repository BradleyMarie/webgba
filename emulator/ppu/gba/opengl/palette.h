#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_PALETTE_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_PALETTE_

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/memory.h"
#include "emulator/ppu/gba/opengl/uniform_locations.h"

typedef struct {
  uint16_t zeroes[GBA_LARGE_PALETTE_SIZE];
  GLuint bg_palette;
  GLuint obj_palette;
  bool bg_dirty;
  bool obj_dirty;
} OpenGlPalette;

bool OpenGlPaletteStage(OpenGlPalette* context, GbaPpuDirtyBits* dirty_bits);

void OpenGlPaletteBind(const OpenGlPalette* context,
                       const UniformLocations* locations);

void OpenGlPaletteReload(OpenGlPalette* context, const GbaPpuMemory* memory);

void OpenGlPaletteReloadContext(OpenGlPalette* context);

void OpenGlPaletteDestroy(OpenGlPalette* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_PALETTE_