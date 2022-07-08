#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_CONTROL_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_CONTROL_

#include <GLES3/gl3.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/registers.h"

typedef struct {
  GLuint staging[GBA_SCREEN_HEIGHT][GBA_PPU_NUM_BACKGROUNDS];
  GLuint texture;
  uint8_t dirty_start;
  uint8_t dirty_end;
  bool dirty;
} OpenGlBgControl;

bool OpenGlBgControlLoad(OpenGlBgControl* context,
                         const GbaPpuRegisters* registers,
                         GbaPpuDirtyBits* dirty_bits);

void OpenGlBgControlBind(OpenGlBgControl* context, GLuint program);

void OpenGlBgControlReloadContext(OpenGlBgControl* context);

void OpenGlBgControlDestroy(OpenGlBgControl* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_CONTROL_