#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_AFFINE_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_AFFINE_

#include <GLES2/gl2.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/registers.h"

typedef struct {
  GLuint buffers[GBA_PPU_NUM_AFFINE_BACKGROUNDS];
  uint16_t reload_scanlines[2u];
} GbaPpuOpenGlBgAffine;

void GbaPpuOpenGlBgAffineInitialize(GbaPpuOpenGlBgAffine* context);

void GbaPpuOpenGlBgAffineReload(GbaPpuOpenGlBgAffine* context,
                                const GbaPpuRegisters* registers,
                                GbaPpuDirtyBits* dirty_bits, uint8_t index);

bool GbaPpuOpenGlBgAffineGet(GbaPpuOpenGlBgAffine* context,
                             const GbaPpuRegisters* registers, uint8_t index,
                             GLuint* buffer);

void GbaPpuOpenGlBgAffineReloadContext(GbaPpuOpenGlBgAffine* context);

void GbaPpuOpenGlBgAffineDestroy(GbaPpuOpenGlBgAffine* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_AFFINE_