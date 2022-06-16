#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_SCROLLING_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_SCROLLING_

#include <GLES3/gl3.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/registers.h"

typedef struct {
  GLfloat origins[GBA_PPU_NUM_BACKGROUNDS][4u];
} OpenGlScrollingRow;

typedef struct {
  OpenGlScrollingRow staging[GBA_SCREEN_HEIGHT];
  GLuint buffer;
  bool dirty;
} OpenGlBgScrolling;

bool OpenGlBgScrollingLoad(OpenGlBgScrolling* context,
                           const GbaPpuRegisters* registers,
                           GbaPpuDirtyBits* dirty_bits);

void OpenGlBgScrollingBind(OpenGlBgScrolling* context, GLint start, GLint end,
                           GLuint program);

void OpenGlBgScrollingReloadContext(OpenGlBgScrolling* context);

void OpenGlBgScrollingDestroy(OpenGlBgScrolling* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_SCROLLING_