#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_BITMAP_MODE5_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_BITMAP_MODE5_

#include <GLES2/gl2.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/memory.h"

typedef struct {
  bool enabled;
  bool page;
  GLuint textures[2u];
  uint16_t staging[GBA_REDUCED_FRAME_HEIGHT * GBA_REDUCED_FRAME_WIDTH];
} OpenGlBgBitmapMode5;

void OpenGlBgBitmapMode5Reload(OpenGlBgBitmapMode5* context,
                               const GbaPpuMemory* memory,
                               const GbaPpuRegisters* registers,
                               GbaPpuDirtyBits* dirty_bits);

bool OpenGlBgBitmapMode5Texture(const OpenGlBgBitmapMode5* context,
                                GLuint* texture);

void OpenGlBgBitmapMode5ReloadContext(OpenGlBgBitmapMode5* context);

void OpenGlBgBitmapMode5Destroy(OpenGlBgBitmapMode5* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_BITMAP_MODE5_