#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_AFFINE_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_AFFINE_

#include <GLES3/gl3.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/registers.h"

typedef struct {
  GLuint buffers[GBA_PPU_NUM_AFFINE_BACKGROUNDS];
  BackgroundAffineRegister affine[GBA_PPU_NUM_AFFINE_BACKGROUNDS];
  uint16_t reload_scanlines[GBA_PPU_NUM_AFFINE_BACKGROUNDS];
} OpenGlBgAffine;

void OpenGlBgAffineInitialize(OpenGlBgAffine* context);

void OpenGlBgAffineReload(OpenGlBgAffine* context,
                          const GbaPpuRegisters* registers,
                          GbaPpuDirtyBits* dirty_bits);

void OpenGlBgAffineBind(const OpenGlBgAffine* context, GLuint program);

void OpenGlBgAffineReloadContext(OpenGlBgAffine* context);

void OpenGlBgAffineDestroy(OpenGlBgAffine* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_AFFINE_