#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_WINDOW_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_WINDOW_

#include <GLES3/gl3.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/registers.h"

typedef struct {
  GLuint window[4u];
  GLuint staging[4u];
  bool dirty;
} OpenGlWindow;

bool OpenGlWindowStage(OpenGlWindow* context, const GbaPpuRegisters* registers,
                       GbaPpuDirtyBits* dirty_bits);

void OpenGlWindowBind(const OpenGlWindow* context, GLuint program);

void OpenGlWindowReload(OpenGlWindow* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_WINDOW_