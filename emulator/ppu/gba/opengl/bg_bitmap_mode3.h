#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_BITMAP_MODE3_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_BITMAP_MODE3_

#include <GLES3/gl3.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/memory.h"

typedef struct {
  uint16_t staging[GBA_SCREEN_HEIGHT * GBA_SCREEN_WIDTH];
  GLuint texture;
  bool enabled;
  bool dirty;
} OpenGlBgBitmapMode3;

bool OpenGlBgBitmapMode3Stage(OpenGlBgBitmapMode3* context,
                              const GbaPpuMemory* memory,
                              const GbaPpuRegisters* registers,
                              GbaPpuDirtyBits* dirty_bits);

void OpenGlBgBitmapMode3Bind(const OpenGlBgBitmapMode3* context,
                             GLuint program);

void OpenGlBgBitmapMode3Reload(OpenGlBgBitmapMode3* context);

void OpenGlBgBitmapMode3ReloadContext(OpenGlBgBitmapMode3* context);

void OpenGlBgBitmapMode3Destroy(OpenGlBgBitmapMode3* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_BITMAP_MODE3_