#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BLEND_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BLEND_

#include <GLES3/gl3.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/registers.h"

typedef struct {
  GLuint bldcnt[4u];
  GLfloat ev[4u];
} OpenGlBlendRow;

typedef struct {
  OpenGlBlendRow staging[GBA_SCREEN_HEIGHT];
  GLuint buffer;
  bool dirty;
} OpenGlBlend;

bool OpenGlBlendLoad(OpenGlBlend* context, const GbaPpuRegisters* registers,
                     GbaPpuDirtyBits* dirty_bits);

void OpenGlBlendBind(OpenGlBlend* context, GLint start, GLint end,
                     GLuint program);

void OpenGlBlendReloadContext(OpenGlBlend* context);

void OpenGlBlendDestroy(OpenGlBlend* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BLEND_