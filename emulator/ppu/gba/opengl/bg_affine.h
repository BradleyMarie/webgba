#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_AFFINE_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_AFFINE_

#include <GLES3/gl3.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/registers.h"

typedef struct {
  GLfloat base_scale[GBA_SCREEN_HEIGHT + 1u][GBA_PPU_NUM_AFFINE_BACKGROUNDS]
                    [4u];
  GLuint texture;
  uint8_t dirty_start;
  uint8_t dirty_end;
  bool dirty;
} OpenGlBgAffine;

bool OpenGlBgAffineLoad(OpenGlBgAffine* context,
                        const GbaPpuRegisters* registers,
                        GbaPpuDirtyBits* dirty_bits);

void OpenGlBgAffineBind(OpenGlBgAffine* context, GLuint program);

void OpenGlBgAffineReloadContext(OpenGlBgAffine* context);

void OpenGlBgAffineDestroy(OpenGlBgAffine* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_AFFINE_