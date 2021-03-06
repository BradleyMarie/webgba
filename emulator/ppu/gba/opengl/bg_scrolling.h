#ifndef _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_SCROLLING_
#define _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_SCROLLING_

#include "emulator/ppu/gba/dirty.h"
#include "emulator/ppu/gba/opengl/uniform_locations.h"
#include "emulator/ppu/gba/registers.h"

typedef struct {
  GLuint staging[GBA_PPU_NUM_BACKGROUNDS];
  GLuint scrolling[GBA_PPU_NUM_BACKGROUNDS];
  bool dirty;
} OpenGlBgScrolling;

bool OpenGlBgScrollingStage(OpenGlBgScrolling* context,
                            const GbaPpuRegisters* registers,
                            GbaPpuDirtyBits* dirty_bits);

void OpenGlBgScrollingBind(const OpenGlBgScrolling* context,
                           const UniformLocations* locations);

void OpenGlBgScrollingReload(OpenGlBgScrolling* context);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_OPENGL_BG_SCROLLING_