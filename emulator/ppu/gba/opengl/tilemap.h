#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_TILEMAP_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_TILEMAP_

#include <GLES2/gl2.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/memory.h"

typedef struct {
  GLuint affine;
  GLuint indices;
  GLuint params;
} OpenGlBgTilemap;

void OpenGlBgTilemapReload(OpenGlBgTilemap* context, const GbaPpuMemory* memory,
                           GbaPpuDirtyBits* dirty_bits);

void OpenGlBgTilemapBind(const OpenGlBgTilemap* context, GLuint program);

void OpenGlBgTilemapReloadContext(OpenGlBgTilemap* context);

void OpenGlBgTilemapDestroy(OpenGlBgTilemap* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_TILEMAP_