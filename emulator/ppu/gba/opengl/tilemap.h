#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_TILEMAP_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_TILEMAP_

#include <GLES3/gl3.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/memory.h"
#include "emulator/ppu/gba/registers.h"

#define AFFINE_TILEMAP_TEXTURE_Y_SIZE 32u
#define AFFINE_TILEMAP_TEXTURE_X_SIZE 64u

typedef struct {
  uint16_t staging_scrolling[GBA_TILE_MODE_NUM_BACKGROUND_TILE_MAP_BLOCKS]
                            [GBA_TILE_MAP_BLOCK_1D_SIZE]
                            [GBA_TILE_MAP_BLOCK_1D_SIZE][4u];
  GLuint affine;
  GLuint scrolling;
  bool affine_dirty[GBA_TILE_MODE_NUM_BACKGROUND_TILE_MAP_BLOCKS];
  bool scrolling_dirty[GBA_TILE_MODE_NUM_BACKGROUND_TILE_MAP_BLOCKS];
} OpenGlBgTilemap;

bool OpenGlBgTilemapStage(OpenGlBgTilemap* context, const GbaPpuMemory* memory,
                          const GbaPpuRegisters* registers,
                          GbaPpuDirtyBits* dirty_bits);

void OpenGlBgTilemapBind(const OpenGlBgTilemap* context, GLuint program);

void OpenGlBgTilemapReload(OpenGlBgTilemap* context,
                           const GbaPpuMemory* memory);

void OpenGlBgTilemapReloadContext(OpenGlBgTilemap* context);

void OpenGlBgTilemapDestroy(OpenGlBgTilemap* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_TILEMAP_