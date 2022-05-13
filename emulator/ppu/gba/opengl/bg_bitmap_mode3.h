#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_BITMAP_MODE3_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_BITMAP_MODE3_

#include <GLES2/gl2.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/memory.h"

typedef struct {
  bool enabled;
  GLuint texture;
  uint16_t staging[GBA_SCREEN_HEIGHT * GBA_SCREEN_WIDTH];
} OpenGlBgBitmapMode3;

void OpenGlBgBitmapMode3Reload(OpenGlBgBitmapMode3* context,
                               const GbaPpuMemory* memory,
                               const GbaPpuRegisters* registers,
                               GbaPpuDirtyBits* dirty_bits);

bool OpenGlBgBitmapMode3Texture(const OpenGlBgBitmapMode3* context,
                                GLuint* texture);

void OpenGlBgBitmapMode3ReloadContext(OpenGlBgBitmapMode3* context);

void OpenGlBgBitmapMode3Destroy(OpenGlBgBitmapMode3* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_BITMAP_MODE3_