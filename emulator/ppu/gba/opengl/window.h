#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_WINDOW_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_WINDOW_

#include <GLES2/gl2.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/registers.h"

typedef struct {
  GLint win0[6u];
  GLint win1[6u];
  GLint winobj[6u];
  GLint winout[6u];
  GLfloat win0_start[2u];
  GLfloat win0_end[2u];
  GLfloat win1_start[2u];
  GLfloat win1_end[2u];
} OpenGlWindow;

void OpenGlWindowReload(OpenGlWindow* context, const GbaPpuRegisters* registers,
                        GbaPpuDirtyBits* dirty_bits);

void OpenGlWindowBind(const OpenGlWindow* context, GLuint program);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_WINDOW_