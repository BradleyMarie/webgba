#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_PALETTE_LARGE_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_PALETTE_LARGE_

#include <GLES3/gl3.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/memory.h"

typedef struct {
  GLuint bg_palette;
  GLuint obj_palette;
  GLfloat bg_staging[GBA_LARGE_PALETTE_SIZE][4u];
  GLfloat obj_staging[GBA_LARGE_PALETTE_SIZE][4u];
  bool bg_dirty;
  bool obj_dirty;
} OpenGlBgPalette;

bool OpenGlBgPaletteStage(OpenGlBgPalette* context, const GbaPpuMemory* memory,
                          GbaPpuDirtyBits* dirty_bits);

void OpenGlBgPaletteBind(const OpenGlBgPalette* context, GLuint program);

void OpenGlBgPaletteReload(OpenGlBgPalette* context);

void OpenGlBgPaletteReloadContext(OpenGlBgPalette* context);

void OpenGlBgPaletteDestroy(OpenGlBgPalette* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_PALETTE_LARGE_