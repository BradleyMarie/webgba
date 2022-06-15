#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_AFFINE_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_AFFINE_

#include <GLES3/gl3.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/registers.h"

typedef struct {
  GLfloat bases[2u][4u];
  GLfloat scale[2u][4u];
} OpenGlBgAffineRow;

typedef struct {
  struct {
    OpenGlBgAffineRow rows[161u];
  } staging;
  GLuint buffer;
  bool dirty;
} OpenGlBgAffine;

bool OpenGlBgAffineLoad(OpenGlBgAffine* context,
                        const GbaPpuRegisters* registers,
                        GbaPpuDirtyBits* dirty_bits);

void OpenGlBgAffineBind(OpenGlBgAffine* context, GLint start, GLint end,
                        GLuint program);

void OpenGlBgAffineReloadContext(OpenGlBgAffine* context);

void OpenGlBgAffineDestroy(OpenGlBgAffine* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_AFFINE_