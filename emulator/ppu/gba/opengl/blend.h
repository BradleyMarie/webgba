#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BLEND_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BLEND_

#include <GLES3/gl3.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/registers.h"

typedef struct {
  GLuint staging[3u];
  GLuint bldcnt_ev[3u];
  bool dirty;
} OpenGlBlend;

bool OpenGlBlendStage(OpenGlBlend* context, const GbaPpuRegisters* registers,
                      GbaPpuDirtyBits* dirty_bits);

void OpenGlBlendBind(const OpenGlBlend* context, GLuint program);

void OpenGlBlendReload(OpenGlBlend* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BLEND_