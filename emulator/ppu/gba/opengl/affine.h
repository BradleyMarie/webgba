#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_AFFINE_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_AFFINE_

#include <GLES2/gl2.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/registers.h"

typedef struct {
  GLuint bg_affine[GBA_PPU_NUM_AFFINE_BACKGROUNDS];
} GbaPpuOpenGlAffineContext;

GLuint GbaPpuOpenGlAffineBG(GbaPpuOpenGlAffineContext* context,
                            const GbaPpuRegisters* registers,
                            GbaPpuDirtyBits* dirty_bits, uint8_t index);

void GbaPpuOpenGlAffineReloadContext(GbaPpuOpenGlAffineContext* context);

void GbaPpuOpenGlAffineDestroy(GbaPpuOpenGlAffineContext* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_AFFINE_