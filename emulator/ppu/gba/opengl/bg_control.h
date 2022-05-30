#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_CONTROL_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_CONTROL_

#include <GLES3/gl3.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/registers.h"

typedef struct {
  GLint size[4u][2u];
  GLint tilemap_base[4u];
  GLint tile_base[4u];
  GLuint priority[4u];
  GLint large_palette[4u];
  GLint wraparound[4u];
} OpenGlBgControl;

void OpenGlBgControlReload(OpenGlBgControl* context,
                           const GbaPpuRegisters* registers,
                           GbaPpuDirtyBits* dirty_bits);

void OpenGlBgControlBind(const OpenGlBgControl* context, GLuint program);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_CONTROL_