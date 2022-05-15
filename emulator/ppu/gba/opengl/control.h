#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_CONTROL_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_CONTROL_

#include <GLES2/gl2.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/registers.h"

typedef struct {
  GLint blank;
  GLint mode;
  GLint bg_enabled[4u];
  GLint win0_enabled;
  GLint win1_enabled;
  GLint winobj_enabled;
} OpenGlControl;

void OpenGlControlReload(OpenGlControl* context,
                         const GbaPpuRegisters* registers,
                         GbaPpuDirtyBits* dirty_bits);

void OpenGlControlBind(const OpenGlControl* context, GLuint program);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_CONTROL_