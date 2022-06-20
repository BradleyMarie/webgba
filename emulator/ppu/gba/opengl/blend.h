#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BLEND_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BLEND_

#include <GLES3/gl3.h>

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/registers.h"

typedef struct {
  uint8_t mode;
  uint8_t eva;
  uint8_t padding0[2u];
  uint8_t top;
  uint8_t evb;
  uint8_t padding1[2u];
  uint8_t bottom;
  uint8_t evy;
  uint8_t padding2[6u];
} OpenGlBlendRow;

typedef struct {
  OpenGlBlendRow staging[GBA_SCREEN_HEIGHT];
  GLuint buffer;
  uint8_t dirty_start;
  uint8_t dirty_end;
  bool dirty;
} OpenGlBlend;

bool OpenGlBlendLoad(OpenGlBlend* context, const GbaPpuRegisters* registers,
                     GbaPpuDirtyBits* dirty_bits);

void OpenGlBlendBind(OpenGlBlend* context, GLuint program);

void OpenGlBlendReloadContext(OpenGlBlend* context);

void OpenGlBlendDestroy(OpenGlBlend* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BLEND_