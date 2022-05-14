#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_PALETTE_LARGE_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_PALETTE_LARGE_

#include <GLES2/gl2.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/memory.h"

typedef struct {
  GLuint palette;
  uint16_t colors[GBA_LARGE_PALETTE_SIZE];
} OpenGlBgLargePalette;

void OpenGlBgLargePaletteReload(OpenGlBgLargePalette* context,
                                const GbaPpuMemory* memory,
                                GbaPpuDirtyBits* dirty_bits);

GLuint OpenGlBgLargePaletteGet(const OpenGlBgLargePalette* context);

void OpenGlBgLargePaletteReloadContext(OpenGlBgLargePalette* context);

void OpenGlBgLargePaletteDestroy(OpenGlBgLargePalette* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_PALETTE_LARGE_