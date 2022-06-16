#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_WINDOW_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_WINDOW_

#include <GLES3/gl3.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/registers.h"

typedef struct {
  GLuint windows[4u];
  GLuint shift_bounds[4u];
} OpenGlWindowRow;

typedef struct {
  OpenGlWindowRow staging[GBA_SCREEN_HEIGHT];
  GLuint buffer;
  bool dirty;
} OpenGlWindow;

bool OpenGlWindowLoad(OpenGlWindow* context, const GbaPpuRegisters* registers,
                      GbaPpuDirtyBits* dirty_bits);

void OpenGlWindowBind(OpenGlWindow* context, GLint start, GLint end,
                      GLuint program);

void OpenGlWindowReloadContext(OpenGlWindow* context);

void OpenGlWindowDestroy(OpenGlWindow* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_WINDOW_