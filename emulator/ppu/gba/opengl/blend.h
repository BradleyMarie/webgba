#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BLEND_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BLEND_

#include <GLES3/gl3.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/registers.h"

typedef struct {
  struct {
    GLuint blend_mode;
    GLfloat blend_eva;
    GLfloat blend_evb;
    GLfloat blend_evy;
    GLuint bg_top[4u][4u];
    GLuint bg_bottom[4u][4u];
    GLuint bd_top;
    GLuint bd_bottom;
    GLuint obj_top;
    GLuint obj_bottom;
  } staging;
  GLuint buffer;
} OpenGlBlend;

void OpenGlBlendReload(OpenGlBlend* context, const GbaPpuRegisters* registers,
                       GbaPpuDirtyBits* dirty_bits);

void OpenGlBlendBind(const OpenGlBlend* context, GLuint program);

void OpenGlBlendReloadContext(OpenGlBlend* context);

void OpenGlBlendDestroy(OpenGlBlend* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BLEND_