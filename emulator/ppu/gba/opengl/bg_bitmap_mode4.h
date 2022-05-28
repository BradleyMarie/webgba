#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_BITMAP_MODE4_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_BITMAP_MODE4_

#include <GLES3/gl3.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/memory.h"

typedef struct {
  bool enabled;
  bool page;
  GLuint textures[2u];
  uint8_t staging[GBA_SCREEN_HEIGHT * GBA_SCREEN_WIDTH];
} OpenGlBgBitmapMode4;

void OpenGlBgBitmapMode4Reload(OpenGlBgBitmapMode4* context,
                               const GbaPpuMemory* memory,
                               const GbaPpuRegisters* registers,
                               GbaPpuDirtyBits* dirty_bits);

void OpenGlBgBitmapMode4Bind(const OpenGlBgBitmapMode4* context,
                             GLuint program);

void OpenGlBgBitmapMode4ReloadContext(OpenGlBgBitmapMode4* context);

void OpenGlBgBitmapMode4Destroy(OpenGlBgBitmapMode4* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_BITMAP_MODE4_