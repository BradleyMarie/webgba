#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_CONTROL_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_CONTROL_

#include <GLES3/gl3.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/registers.h"

typedef struct {
  struct {
    GLint size[2];
    GLint mosaic[2];
    GLint tilemap_base;
    GLint tile_base;
    GLuint priority;
    GLuint large_palette;
    GLuint wraparound;
    GLint padding[3];
  } staging[4];
  GLuint buffer;
} OpenGlBgControl;

void OpenGlBgControlReload(OpenGlBgControl* context,
                           const GbaPpuRegisters* registers,
                           GbaPpuDirtyBits* dirty_bits);

void OpenGlBgControlBind(const OpenGlBgControl* context, GLuint program);

void OpenGlBgControlReloadContext(OpenGlBgControl* context);

void OpenGlBgControlDestroy(OpenGlBgControl* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_CONTROL_