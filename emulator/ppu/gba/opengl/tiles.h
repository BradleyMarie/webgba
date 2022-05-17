#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_TILES_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_TILES_

#include <GLES2/gl2.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/memory.h"

typedef struct {
  GLuint s_tiles;
  GLuint d_tiles;
  uint8_t staging[GBA_TILE_MODE_TILE_BLOCK_NUM_S_TILES * GBA_TILE_1D_SIZE *
                  GBA_TILE_1D_SIZE];
} OpenGlBgTiles;

void OpenGlBgTilesReload(OpenGlBgTiles* context, const GbaPpuMemory* memory,
                         GbaPpuDirtyBits* dirty_bits);

void OpenGlBgTilesBind(const OpenGlBgTiles* context, GLuint program);

void OpenGlBgTilesReloadContext(OpenGlBgTiles* context);

void OpenGlBgTilesDestroy(OpenGlBgTiles* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_TILES_