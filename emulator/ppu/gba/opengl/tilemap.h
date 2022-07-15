#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_TILEMAP_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_TILEMAP_

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/memory.h"
#include "emulator/ppu/gba/opengl/uniform_locations.h"
#include "emulator/ppu/gba/registers.h"

#define AFFINE_TILEMAP_TEXTURE_Y_SIZE 32u
#define AFFINE_TILEMAP_TEXTURE_X_SIZE 64u

typedef struct {
  GLuint affine_textures[GBA_SCREEN_HEIGHT];
  GLuint scrolling_textures[GBA_SCREEN_HEIGHT];
  uint8_t affine_texture_index;
  uint8_t scrolling_texture_index;
  bool affine_dirty;
  bool scrolling_dirty;
} OpenGlTilemap;

bool OpenGlTilemapStage(OpenGlTilemap* context, const GbaPpuMemory* memory,
                        const GbaPpuRegisters* registers,
                        GbaPpuDirtyBits* dirty_bits);

void OpenGlTilemapBind(const OpenGlTilemap* context,
                       const UniformLocations* locations);

void OpenGlTilemapReload(OpenGlTilemap* context, const GbaPpuMemory* memory);

void OpenGlTilemapReloadContext(OpenGlTilemap* context);

void OpenGlTilemapDestroy(OpenGlTilemap* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_TILEMAP_