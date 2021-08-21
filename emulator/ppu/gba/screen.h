#ifndef _WEBGBA_EMULATOR_PPU_GBA_PPU_SCREEN_
#define _WEBGBA_EMULATOR_PPU_GBA_PPU_SCREEN_

#include <GLES2/gl2.h>
#include <assert.h>

#include "emulator/ppu/gba/types.h"

typedef struct {
  uint16_t pixels[GBA_SCREEN_HEIGHT][GBA_SCREEN_WIDTH];
  GLuint program;
  GLuint texture;
} GbaPpuScreen;

static inline void GbaPpuScreenSetPixel(GbaPpuScreen* screen, uint_fast8_t x,
                                        uint_fast8_t y, uint16_t value) {
  assert(x < GBA_SCREEN_WIDTH);
  assert(y < GBA_SCREEN_HEIGHT);
  screen->pixels[y][x] = value << 1u;
}

static inline void GbaPpuScreenCopyPixel(GbaPpuScreen* screen,
                                         uint_fast8_t src_x, uint_fast8_t src_y,
                                         uint_fast8_t dest_x,
                                         uint_fast8_t dest_y) {
  assert(src_x < GBA_SCREEN_WIDTH);
  assert(src_y < GBA_SCREEN_HEIGHT);
  assert(dest_x < GBA_SCREEN_WIDTH);
  assert(dest_y < GBA_SCREEN_HEIGHT);
  screen->pixels[dest_y][dest_x] = screen->pixels[src_y][src_x];
}

void GbaPpuScreenCopyToFbo(const GbaPpuScreen* screen, GLuint fbo);

void GbaPpuScreenReloadContext(GbaPpuScreen* screen);

void GbaPpuScreenDestroy(GbaPpuScreen* screen);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_PPU_SCREEN_