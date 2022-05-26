#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_TILES_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_TILES_

#include <GLES2/gl2.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/memory.h"
#include "emulator/ppu/gba/registers.h"

typedef struct {
  GLuint bg_tiles;
  GLuint obj_tiles;
  uint8_t staging[GBA_TILE_MODE_TILE_BLOCK_NUM_S_TILES][GBA_TILE_1D_SIZE]
                 [GBA_TILE_1D_SIZE][2u];
} OpenGlTiles;

void OpenGlTilesReload(OpenGlTiles* context, const GbaPpuMemory* memory,
                       const GbaPpuRegisters* registers,
                       GbaPpuDirtyBits* dirty_bits);

void OpenGlTilesBind(const OpenGlTiles* context, GLuint program);

void OpenGlTilesReloadContext(OpenGlTiles* context);

void OpenGlTilesDestroy(OpenGlTiles* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_TILES_