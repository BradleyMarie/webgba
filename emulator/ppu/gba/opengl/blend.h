#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BLEND_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BLEND_

#include <GLES3/gl3.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/registers.h"

typedef struct {
  GLint blend_mode;
  GLfloat blend_eva;
  GLfloat blend_evb;
  GLfloat blend_evy;
  GLint obj_top;
  GLint obj_bottom;
  GLint bg0_top;
  GLint bg0_bottom;
  GLint bg1_top;
  GLint bg1_bottom;
  GLint bg2_top;
  GLint bg2_bottom;
  GLint bg3_top;
  GLint bg3_bottom;
  GLint bd_top;
  GLint bd_bottom;
} OpenGlBlend;

void OpenGlBlendReload(OpenGlBlend* context, const GbaPpuRegisters* registers,
                       GbaPpuDirtyBits* dirty_bits);

void OpenGlBlendBind(const OpenGlBlend* context, GLuint program);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BLEND_