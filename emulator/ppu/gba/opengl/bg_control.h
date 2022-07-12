#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_CONTROL_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_CONTROL_

#include <GLES3/gl3.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/registers.h"

typedef struct {
  GLuint staging[GBA_PPU_NUM_BACKGROUNDS];
  GLuint bgcnt[GBA_PPU_NUM_BACKGROUNDS];
  bool dirty;
} OpenGlBgControl;

bool OpenGlBgControlStage(OpenGlBgControl* context,
                          const GbaPpuRegisters* registers,
                          GbaPpuDirtyBits* dirty_bits);

void OpenGlBgControlBind(const OpenGlBgControl* context, GLuint program);

void OpenGlBgControlReload(OpenGlBgControl* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_CONTROL_