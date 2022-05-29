#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_MOSAIC_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_MOSAIC_

#include <GLES3/gl3.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/memory.h"
#include "emulator/ppu/gba/registers.h"

typedef struct {
  GLfloat mosaic[4][2];
} OpenGlBgMosaic;

void OpenGlBgMosaicReload(OpenGlBgMosaic* context,
                          const GbaPpuRegisters* registers,
                          GbaPpuDirtyBits* dirty_bits);

void OpenGlBgMosaicBind(const OpenGlBgMosaic* context, GLuint program);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_MOSAIC_