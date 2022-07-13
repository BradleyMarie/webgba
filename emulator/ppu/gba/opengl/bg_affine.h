#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_AFFINE_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_AFFINE_

#include <GLES3/gl3.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/registers.h"

typedef struct {
  GLfloat start_base_scale[GBA_PPU_NUM_AFFINE_BACKGROUNDS][4u];
  GLfloat end_base_scale[GBA_PPU_NUM_AFFINE_BACKGROUNDS][4u];
  GLfloat final_base_scale[GBA_PPU_NUM_AFFINE_BACKGROUNDS][4u];
} OpenGlBgAffine;

bool OpenGlBgAffineStage(OpenGlBgAffine* context,
                         const GbaPpuRegisters* registers,
                         GbaPpuDirtyBits* dirty_bits);

void OpenGlBgAffineBind(const OpenGlBgAffine* context, GLuint program);

void OpenGlBgAffineReload(OpenGlBgAffine* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_AFFINE_