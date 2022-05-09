#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_PALETTE_SMALL_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_PALETTE_SMALL_

#include <GLES2/gl2.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/memory.h"

typedef struct {
  GLuint palettes[GBA_NUM_SMALL_PALETTES];
} GbaPpuOpenGlSmallPaletteContext;

GLuint GbaPpuOpenGlSmallPaletteBG(GbaPpuOpenGlSmallPaletteContext* context,
                                  const GbaPpuMemory* memory,
                                  GbaPpuDirtyBits* dirty_bits, uint8_t index);

GLuint GbaPpuOpenGlSmallPaletteOBJ(GbaPpuOpenGlSmallPaletteContext* context,
                                   const GbaPpuMemory* memory,
                                   GbaPpuDirtyBits* dirty_bits, uint8_t index);

void GbaPpuOpenGlSmallPaletteReloadContext(
    GbaPpuOpenGlSmallPaletteContext* context);

void GbaPpuOpenGlSmallPaletteDestroy(GbaPpuOpenGlSmallPaletteContext* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_PALETTE_SMALL_