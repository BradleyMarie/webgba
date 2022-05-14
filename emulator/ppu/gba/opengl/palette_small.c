#include "emulator/ppu/gba/opengl/palette_small.h"

#include <assert.h>
#include <string.h>

void OpenGlBgSmallPaletteReload(OpenGlBgSmallPalette* context,
                                  const GbaPpuMemory* memory,
                                  GbaPpuDirtyBits* dirty_bits, uint8_t index) {
  assert(index < GBA_NUM_SMALL_PALETTES);

  if (!dirty_bits->palette.bg_small_palettes[index]) {
    return;
  }

  uint16_t colors[GBA_SMALL_PALETTE_SIZE];

  colors[0] = 0u;
  for (uint16_t i = 1; i < GBA_SMALL_PALETTE_SIZE; i++) {
    colors[i] = memory->palette.bg.small_palettes[index][i] << 1u;
  }

  glBindTexture(GL_TEXTURE_2D, context->palettes[index]);
  glTexSubImage2D(GL_TEXTURE_2D, /*level=*/0, /*xoffset=*/0, /*yoffset=*/0,
                  /*width=*/GBA_SMALL_PALETTE_SIZE, /*height=*/1u,
                  /*format=*/GL_RGBA, /*type=*/GL_UNSIGNED_SHORT_5_5_5_1,
                  /*pixels=*/colors);
  glBindTexture(GL_TEXTURE_2D, 0);

  dirty_bits->palette.bg_small_palettes[index] = false;
}

GLuint OpenGlBgSmallPaletteGet(const OpenGlBgSmallPalette* context,
                               uint8_t index) {
  assert(index < GBA_NUM_SMALL_PALETTES);
  return context->palettes[index];
}

void OpenGlBgSmallPaletteReloadContext(OpenGlBgSmallPalette* context) {
  glGenTextures(GBA_NUM_SMALL_PALETTES, context->palettes);
  for (uint8_t i = 0; i < GBA_NUM_SMALL_PALETTES; i++) {
    glBindTexture(GL_TEXTURE_2D, context->palettes[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, /*level=*/0, /*internal_format=*/GL_RGBA,
                 /*width=*/GBA_SMALL_PALETTE_SIZE,
                 /*height=*/1u,
                 /*border=*/0, /*format=*/GL_RGBA,
                 /*type=*/GL_UNSIGNED_SHORT_5_5_5_1,
                 /*pixels=*/NULL);
  }
  glBindTexture(GL_TEXTURE_2D, 0);
}

void OpenGlBgSmallPaletteDestroy(OpenGlBgSmallPalette* context) {
  glDeleteTextures(GBA_NUM_SMALL_PALETTES, context->palettes);
}