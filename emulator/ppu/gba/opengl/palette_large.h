#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_PALETTE_LARGE_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_PALETTE_LARGE_

#include <GLES2/gl2.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/memory.h"

typedef struct {
  GLuint palette;
  uint16_t colors[GBA_LARGE_PALETTE_SIZE];
} GbaPpuOpenGlLargePalette;

GLuint GbaPpuOpenGlLargePaletteBG(GbaPpuOpenGlLargePalette* context,
                                  const GbaPpuMemory* memory,
                                  GbaPpuDirtyBits* dirty_bits);

GLuint GbaPpuOpenGlLargePaletteOBJ(GbaPpuOpenGlLargePalette* context,
                                   const GbaPpuMemory* memory,
                                   GbaPpuDirtyBits* dirty_bits);

void GbaPpuOpenGlLargePaletteReloadContext(GbaPpuOpenGlLargePalette* context);

void GbaPpuOpenGlLargePaletteDestroy(GbaPpuOpenGlLargePalette* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_PALETTE_LARGE_