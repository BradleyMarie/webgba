#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_BITMAP_MODE4_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_BITMAP_MODE4_

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/memory.h"
#include "emulator/ppu/gba/opengl/uniform_locations.h"

typedef struct {
  GLuint current_textures[2u];
  GLuint texture_pool[GBA_SCREEN_HEIGHT];
  uint8_t texture_pool_index;
  uint8_t page;
  bool enabled;
  bool dirty;
} OpenGlBgBitmapMode4;

bool OpenGlBgBitmapMode4Stage(OpenGlBgBitmapMode4* context,
                              const GbaPpuMemory* memory,
                              const GbaPpuRegisters* registers,
                              GbaPpuDirtyBits* dirty_bits);

void OpenGlBgBitmapMode4Bind(const OpenGlBgBitmapMode4* context,
                             const UniformLocations* locations);

void OpenGlBgBitmapMode4Reload(OpenGlBgBitmapMode4* context,
                               const GbaPpuMemory* memory);

void OpenGlBgBitmapMode4ReloadContext(OpenGlBgBitmapMode4* context);

void OpenGlBgBitmapMode4Destroy(OpenGlBgBitmapMode4* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_BITMAP_MODE4_