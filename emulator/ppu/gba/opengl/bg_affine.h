#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_AFFINE_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_AFFINE_

#include <GLES3/gl3.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/registers.h"

typedef struct {
  struct {
    GLfloat origins[2u][4u];
    GLfloat values[2u][4u];
    GLfloat transformations[2u][2u][4u];
  } staging;
  GLuint buffer;
  bool dirty;
} OpenGlBgAffine;

bool OpenGlBgAffineStage(OpenGlBgAffine* context,
                         const GbaPpuRegisters* registers,
                         GbaPpuDirtyBits* dirty_bits);

void OpenGlBgAffineBind(const OpenGlBgAffine* context, uint8_t render_scale,
                        GLuint program);

void OpenGlBgAffineReload(OpenGlBgAffine* context);

void OpenGlBgAffineReloadContext(OpenGlBgAffine* context);

void OpenGlBgAffineDestroy(OpenGlBgAffine* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_AFFINE_