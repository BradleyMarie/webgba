#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_MOSAIC_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_MOSAIC_

#include <GLES2/gl2.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/memory.h"
#include "emulator/ppu/gba/registers.h"

typedef struct {
  GLfloat bg0[2];
  GLfloat bg1[2];
  GLfloat bg2[2];
  GLfloat bg3[2];
} OpenGlBgMosaic;

void OpenGlBgMosaicReload(OpenGlBgMosaic* context,
                          const GbaPpuRegisters* registers,
                          GbaPpuDirtyBits* dirty_bits);

void OpenGlBgMosaicBind(OpenGlBgMosaic* context, GLuint program);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_MOSAIC_