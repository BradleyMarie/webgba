#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_MOSAIC_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_MOSAIC_

#include <GLES2/gl2.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/memory.h"
#include "emulator/ppu/gba/registers.h"

typedef struct {
  GLfloat bg[2];
  GLfloat obj[2];
} GbaPpuOpenGlMosaic;

void GbaPpuOpenGlMosaicBG(GbaPpuOpenGlMosaic* context,
                          const GbaPpuRegisters* registers,
                          GbaPpuDirtyBits* dirty_bits, uint8_t index,
                          GLfloat mosaic[2]);

void GbaPpuOpenGlMosaicOBJ(GbaPpuOpenGlMosaic* context,
                           const GbaPpuMemory* memory,
                           const GbaPpuRegisters* registers,
                           GbaPpuDirtyBits* dirty_bits, uint8_t index,
                           GLfloat mosaic[2]);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_MOSAIC_