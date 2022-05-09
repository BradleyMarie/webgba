#include "emulator/ppu/gba/opengl/palette_large.h"

#include <string.h>

static void GbaPpuReloadLargePalette(GbaPpuOpenGlLargePaletteContext* context,
                                     const GbaPpuPaletteSegment* palette) {
  context->colors[0] = 0u;
  for (uint16_t i = 1; i < GBA_LARGE_PALETTE_SIZE; i++) {
    context->colors[i] = palette->large_palette[i] << 1u;
  }

  glBindTexture(GL_TEXTURE_2D, context->palette);
  glTexSubImage2D(GL_TEXTURE_2D, /*level=*/0, /*xoffset=*/0, /*yoffset=*/0,
                  /*width=*/GBA_LARGE_PALETTE_SIZE, /*height=*/1u,
                  /*format=*/GL_RGBA, /*type=*/GL_UNSIGNED_SHORT_5_5_5_1,
                  /*pixels=*/context->colors);
  glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint GbaPpuOpenGlLargePaletteBG(GbaPpuOpenGlLargePaletteContext* context,
                                  const GbaPpuMemory* memory,
                                  GbaPpuDirtyBits* dirty_bits) {
  if (dirty_bits->palette.bg_large_palette) {
    GbaPpuReloadLargePalette(context, &memory->palette.bg);
    dirty_bits->palette.bg_large_palette = false;
  }
  return context->palette;
}

GLuint GbaPpuOpenGlLargePaletteOBJ(GbaPpuOpenGlLargePaletteContext* context,
                                   const GbaPpuMemory* memory,
                                   GbaPpuDirtyBits* dirty_bits) {
  if (dirty_bits->palette.obj_large_palette) {
    GbaPpuReloadLargePalette(context, &memory->palette.obj);
    dirty_bits->palette.obj_large_palette = false;
  }
  return context->palette;
}

void GbaPpuOpenGlLargePaletteReloadContext(
    GbaPpuOpenGlLargePaletteContext* context) {
  glGenTextures(1u, &context->palette);
  glBindTexture(GL_TEXTURE_2D, context->palette);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, /*level=*/0, /*internal_format=*/GL_RGBA,
               /*width=*/GBA_LARGE_PALETTE_SIZE,
               /*height=*/1u,
               /*border=*/0, /*format=*/GL_RGBA,
               /*type=*/GL_UNSIGNED_SHORT_5_5_5_1,
               /*pixels=*/NULL);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void GbaPpuOpenGlLargePaletteDestroy(GbaPpuOpenGlLargePaletteContext* context) {
  glDeleteTextures(1u, &context->palette);
}