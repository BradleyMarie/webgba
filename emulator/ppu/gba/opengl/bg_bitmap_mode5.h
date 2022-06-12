#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_BITMAP_MODE5_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_BITMAP_MODE5_

#include <GLES3/gl3.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/memory.h"

typedef struct {
  uint16_t staging[GBA_REDUCED_FRAME_HEIGHT * GBA_REDUCED_FRAME_WIDTH];
  GLuint textures[2u];
  bool page;
  bool enabled;
  bool dirty;
} OpenGlBgBitmapMode5;

bool OpenGlBgBitmapMode5Stage(OpenGlBgBitmapMode5* context,
                               const GbaPpuMemory* memory,
                               const GbaPpuRegisters* registers,
                               GbaPpuDirtyBits* dirty_bits);

void OpenGlBgBitmapMode5Bind(const OpenGlBgBitmapMode5* context, GLuint program);

void OpenGlBgBitmapMode5Reload(OpenGlBgBitmapMode5* context);

void OpenGlBgBitmapMode5ReloadContext(OpenGlBgBitmapMode5* context);

void OpenGlBgBitmapMode5Destroy(OpenGlBgBitmapMode5* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_BITMAP_MODE5_