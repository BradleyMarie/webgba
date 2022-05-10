#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_AFFINE_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_AFFINE_

#include <GLES2/gl2.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/registers.h"

typedef struct {
  GLuint bg_affine[GBA_PPU_NUM_AFFINE_BACKGROUNDS];
} GbaPpuOpenGlAffine;

GLuint GbaPpuOpenGlAffineBG(GbaPpuOpenGlAffine* context,
                            const GbaPpuRegisters* registers,
                            GbaPpuDirtyBits* dirty_bits, uint8_t index);

void GbaPpuOpenGlAffineReloadContext(GbaPpuOpenGlAffine* context);

void GbaPpuOpenGlAffineDestroy(GbaPpuOpenGlAffine* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_AFFINE_