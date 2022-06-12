#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_AFFINE_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_AFFINE_

#include <GLES3/gl3.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/registers.h"

typedef struct {
  GLfloat staging[GBA_PPU_NUM_AFFINE_BACKGROUNDS][6u];
  GLuint buffers[GBA_PPU_NUM_AFFINE_BACKGROUNDS];
  bool dirty[GBA_PPU_NUM_AFFINE_BACKGROUNDS];
} OpenGlBgAffine;

bool OpenGlBgAffineStage(OpenGlBgAffine* context,
                          const GbaPpuRegisters* registers,
                          GbaPpuDirtyBits* dirty_bits);

void OpenGlBgAffineBind(const OpenGlBgAffine* context, GLuint program);

void OpenGlBgAffineReload(OpenGlBgAffine* context);

void OpenGlBgAffineReloadContext(OpenGlBgAffine* context);

void OpenGlBgAffineDestroy(OpenGlBgAffine* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_AFFINE_