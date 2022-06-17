#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_BITMAP_MODE4_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_BITMAP_MODE4_

#include <GLES3/gl3.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/memory.h"

typedef struct {
  GLuint textures[2u];
  uint8_t page;
  bool dirty;
} OpenGlBgBitmapMode4;

bool OpenGlBgBitmapMode4Stage(OpenGlBgBitmapMode4* context,
                              const GbaPpuMemory* memory,
                              const GbaPpuRegisters* registers,
                              GbaPpuDirtyBits* dirty_bits);

void OpenGlBgBitmapMode4Bind(const OpenGlBgBitmapMode4* context,
                             GLuint program);

void OpenGlBgBitmapMode4Reload(OpenGlBgBitmapMode4* context,
                               const GbaPpuMemory* memory);

void OpenGlBgBitmapMode4ReloadContext(OpenGlBgBitmapMode4* context);

void OpenGlBgBitmapMode4Destroy(OpenGlBgBitmapMode4* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_BITMAP_MODE4_