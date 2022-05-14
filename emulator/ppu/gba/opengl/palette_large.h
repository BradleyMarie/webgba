#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_PALETTE_LARGE_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_PALETTE_LARGE_

#include <GLES2/gl2.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/memory.h"

typedef struct {
  GLuint palette;
  uint16_t colors[GBA_LARGE_PALETTE_SIZE];
} OpenGlLargePalette;

GLuint OpenGlLargePaletteBG(OpenGlLargePalette* context,
                            const GbaPpuMemory* memory,
                            GbaPpuDirtyBits* dirty_bits);

GLuint OpenGlLargePaletteOBJ(OpenGlLargePalette* context,
                             const GbaPpuMemory* memory,
                             GbaPpuDirtyBits* dirty_bits);

void OpenGlLargePaletteReloadContext(OpenGlLargePalette* context);

void OpenGlLargePaletteDestroy(OpenGlLargePalette* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_PALETTE_LARGE_