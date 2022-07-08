#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_WINDOW_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_WINDOW_

#include <GLES3/gl3.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/registers.h"

typedef struct {
  uint16_t win0;
  uint16_t win0_shift;
  uint16_t win1;
  uint16_t win0_bound;
  uint16_t winobj;
  uint16_t win1_shift;
  uint16_t winout;
  uint16_t win1_bound;
} OpenGlWindowRow;

typedef struct {
  OpenGlWindowRow staging[GBA_SCREEN_HEIGHT];
  GLuint texture;
  uint8_t dirty_start;
  uint8_t dirty_end;
  bool dirty;
} OpenGlWindow;

bool OpenGlWindowLoad(OpenGlWindow* context, const GbaPpuRegisters* registers,
                      GbaPpuDirtyBits* dirty_bits);

void OpenGlWindowBind(OpenGlWindow* context, GLuint program);

void OpenGlWindowReloadContext(OpenGlWindow* context);

void OpenGlWindowDestroy(OpenGlWindow* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_WINDOW_