#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_AFFINE_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_AFFINE_

#include <GLES2/gl2.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/registers.h"

typedef struct {
  GLuint buffers[GBA_PPU_NUM_AFFINE_BACKGROUNDS];
  uint16_t reload_scanlines[2u];
} OpenGlBgAffine;

void OpenGlBgAffineInitialize(OpenGlBgAffine* context);

void OpenGlBgAffineReload(OpenGlBgAffine* context,
                          const GbaPpuRegisters* registers,
                          GbaPpuDirtyBits* dirty_bits, uint8_t index);

void OpenGlBgAffineBind(OpenGlBgAffine* context,
                        const GbaPpuRegisters* registers, GLuint program);

void OpenGlBgAffineReloadContext(OpenGlBgAffine* context);

void OpenGlBgAffineDestroy(OpenGlBgAffine* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_AFFINE_