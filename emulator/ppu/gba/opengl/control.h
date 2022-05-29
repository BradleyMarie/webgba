#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_CONTROL_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_CONTROL_

#include <GLES3/gl3.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/registers.h"

typedef struct {
  GLint blank;
  GLint mode;
  GLint obj_mode;
  GLint obj_enabled;
  GLint bg_enabled[GBA_PPU_NUM_BACKGROUNDS];
  GLint win0_enabled;
  GLint win1_enabled;
  GLint winobj_enabled;
} OpenGlControl;

void OpenGlControlReload(OpenGlControl* context,
                         const GbaPpuRegisters* registers,
                         GbaPpuDirtyBits* dirty_bits);

void OpenGlControlBind(const OpenGlControl* context, GLuint program);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_CONTROL_