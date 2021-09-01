#ifndef _WEBGBA_EMULATOR_PPU_GBA_PPU_SCREEN_
#define _WEBGBA_EMULATOR_PPU_GBA_PPU_SCREEN_

#include <GLES2/gl2.h>
#include <assert.h>

#include "emulator/ppu/gba/memory.h"
#include "emulator/ppu/gba/registers.h"

#define GBA_SCREEN_HEIGHT GBA_FULL_FRAME_HEIGHT
#define GBA_SCREEN_WIDTH GBA_FULL_FRAME_WIDTH

#define GBA_PPU_SCREEN_TRANSPARENT_PRIORITY 5u

typedef struct {
  uint16_t pixels[GBA_SCREEN_HEIGHT][GBA_SCREEN_WIDTH];
  uint8_t priorities[GBA_SCREEN_HEIGHT][GBA_SCREEN_WIDTH];
  GLuint program;
  GLuint texture;
} GbaPpuScreen;

static inline void GbaPpuScreenDrawPixel(GbaPpuScreen* screen, uint_fast8_t x,
                                         uint_fast8_t y, uint16_t value,
                                         uint8_t priority) {
  assert(x < GBA_SCREEN_WIDTH);
  assert(y < GBA_SCREEN_HEIGHT);
  priority = UINT8_MAX - priority;
  if (priority > screen->priorities[y][x]) {
    screen->pixels[y][x] = value;
    screen->priorities[y][x] = priority;
  }
}

void GbaPpuScreenClear(GbaPpuScreen* screen);

void GbaPpuScreenRenderToFbo(GbaPpuScreen* screen, GLuint fbo);

void GbaPpuScreenReloadContext(GbaPpuScreen* screen);

void GbaPpuScreenDestroy(GbaPpuScreen* screen);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_PPU_SCREEN_