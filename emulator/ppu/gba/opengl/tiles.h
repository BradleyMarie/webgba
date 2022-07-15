#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_TILES_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_TILES_

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/memory.h"
#include "emulator/ppu/gba/opengl/uniform_locations.h"
#include "emulator/ppu/gba/registers.h"

typedef struct {
  GLuint bg_textures[GBA_SCREEN_HEIGHT];
  GLuint obj_textures[GBA_SCREEN_HEIGHT];
  uint8_t bg_texture_index;
  uint8_t obj_texture_index;
  bool bg_dirty;
  bool obj_dirty;
} OpenGlTiles;

bool OpenGlTilesStage(OpenGlTiles* context, const GbaPpuMemory* memory,
                      const GbaPpuRegisters* registers,
                      GbaPpuDirtyBits* dirty_bits);

void OpenGlTilesBind(const OpenGlTiles* context,
                     const UniformLocations* locations);

void OpenGlTilesReload(OpenGlTiles* context, const GbaPpuMemory* memory);

void OpenGlTilesReloadContext(OpenGlTiles* context);

void OpenGlTilesDestroy(OpenGlTiles* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_TILES_