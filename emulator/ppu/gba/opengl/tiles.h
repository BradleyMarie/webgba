#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_TILES_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_TILES_

#include <GLES3/gl3.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/memory.h"
#include "emulator/ppu/gba/registers.h"

typedef struct {
  GLuint bg_tiles;
  GLuint obj_tiles;
  uint8_t obj_staging[GBA_TILE_MODE_NUM_OBJECT_S_TILES /
                      GBA_TILE_MODE_TILE_BLOCK_NUM_S_TILES]
                     [GBA_TILE_MODE_TILE_BLOCK_NUM_S_TILES]
                     [GBA_TILE_1D_SIZE * GBA_TILE_1D_SIZE][2u];
  bool bg_dirty[GBA_TILE_MODE_NUM_BACKGROUND_TILE_BLOCKS];
  bool obj_dirty[GBA_TILE_MODE_NUM_OBJECT_S_TILES /
                 GBA_TILE_MODE_TILE_BLOCK_NUM_S_TILES];
} OpenGlTiles;

bool OpenGlTilesStage(OpenGlTiles* context, const GbaPpuMemory* memory,
                      const GbaPpuRegisters* registers,
                      GbaPpuDirtyBits* dirty_bits);

void OpenGlTilesBind(const OpenGlTiles* context, GLuint program);

void OpenGlTilesReload(OpenGlTiles* context, const GbaPpuMemory* memory);

void OpenGlTilesReloadContext(OpenGlTiles* context);

void OpenGlTilesDestroy(OpenGlTiles* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_TILES_