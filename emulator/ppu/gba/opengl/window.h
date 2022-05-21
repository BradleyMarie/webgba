#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_WINDOW_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_WINDOW_

#include <GLES2/gl2.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/registers.h"

typedef struct {
  GLint win0_bg0;
  GLint win0_bg1;
  GLint win0_bg2;
  GLint win0_bg3;
  GLint win0_obj;
  GLint win0_bld;
  GLint win1_bg0;
  GLint win1_bg1;
  GLint win1_bg2;
  GLint win1_bg3;
  GLint win1_obj;
  GLint win1_bld;
  GLint winobj_bg0;
  GLint winobj_bg1;
  GLint winobj_bg2;
  GLint winobj_bg3;
  GLint winobj_obj;
  GLint winobj_bld;
  GLint winout_bg0;
  GLint winout_bg1;
  GLint winout_bg2;
  GLint winout_bg3;
  GLint winout_obj;
  GLint winout_bld;
  GLfloat win0_start[2u];
  GLfloat win0_end[2u];
  GLfloat win1_start[2u];
  GLfloat win1_end[2u];
} OpenGlWindow;

void OpenGlWindowReload(OpenGlWindow* context, const GbaPpuRegisters* registers,
                        GbaPpuDirtyBits* dirty_bits);

void OpenGlWindowBind(const OpenGlWindow* context, GLuint program);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_WINDOW_