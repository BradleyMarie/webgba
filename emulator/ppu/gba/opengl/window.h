#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_WINDOW_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_WINDOW_

#include <GLES3/gl3.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/registers.h"

typedef struct {
  struct {
    struct {
      GLuint obj;
      GLuint bg0;
      GLuint bg1;
      GLuint bg2;
      GLuint bg3;
      GLuint bld;
      GLuint padding[2u];
    } windows[4u];
    GLuint shift[2u][4u];
    GLuint bounds[2u][4u];
    GLuint winobj_enabled;
  } staging;
  GLuint buffer;
} OpenGlWindow;

void OpenGlWindowReload(OpenGlWindow* context, const GbaPpuRegisters* registers,
                        GbaPpuDirtyBits* dirty_bits);

void OpenGlWindowBind(const OpenGlWindow* context, GLuint program);

void OpenGlWindowReloadContext(OpenGlWindow* context);

void OpenGlWindowDestroy(OpenGlWindow* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_WINDOW_