#ifndef _WEBGBA_EMULATOR_PPU_GBA_PPU_SOFTWARE_SCREEN_
#define _WEBGBA_EMULATOR_PPU_GBA_PPU_SOFTWARE_SCREEN_

#include <GLES2/gl2.h>
#include <assert.h>

#include "emulator/ppu/gba/memory.h"
#include "emulator/ppu/gba/registers.h"

typedef struct {
  uint16_t pixels[GBA_SCREEN_HEIGHT][GBA_SCREEN_WIDTH];
  GLuint program;
  GLuint vertices;
  GLuint texture;
  bool initialized;
} GbaPpuScreen;

static inline void GbaPpuScreenSet(GbaPpuScreen* screen, uint_fast8_t x,
                                   uint_fast8_t y, uint16_t value) {
  assert(x < GBA_SCREEN_WIDTH);
  assert(y < GBA_SCREEN_HEIGHT);
  screen->pixels[y][x] = value;
}

void GbaPpuScreenRenderToFbo(GbaPpuScreen* screen, GLuint fbo, GLsizei width,
                             GLsizei height);

void GbaPpuScreenReloadContext(GbaPpuScreen* screen);

void GbaPpuScreenDestroy(GbaPpuScreen* screen);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_PPU_SOFTWARE_SCREEN_