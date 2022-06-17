#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_TILEMAP_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_TILEMAP_

#include <GLES3/gl3.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/memory.h"
#include "emulator/ppu/gba/registers.h"

#define AFFINE_TILEMAP_TEXTURE_Y_SIZE 32u
#define AFFINE_TILEMAP_TEXTURE_X_SIZE 64u

typedef struct {
  GLuint affine;
  GLuint scrolling;
  bool affine_dirty[GBA_TILE_MODE_NUM_BACKGROUND_TILE_MAP_BLOCKS];
  bool scrolling_dirty[GBA_TILE_MODE_NUM_BACKGROUND_TILE_MAP_BLOCKS];
} OpenGlTilemap;

bool OpenGlTilemapStage(OpenGlTilemap* context, const GbaPpuMemory* memory,
                        const GbaPpuRegisters* registers,
                        GbaPpuDirtyBits* dirty_bits);

void OpenGlTilemapBind(const OpenGlTilemap* context, GLuint program);

void OpenGlTilemapReload(OpenGlTilemap* context, const GbaPpuMemory* memory);

void OpenGlTilemapReloadContext(OpenGlTilemap* context);

void OpenGlTilemapDestroy(OpenGlTilemap* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_TILEMAP_