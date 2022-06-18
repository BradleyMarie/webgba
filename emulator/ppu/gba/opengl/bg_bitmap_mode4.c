#include "emulator/ppu/gba/opengl/bg_bitmap_mode4.h"

#include <stdlib.h>

#include "emulator/ppu/gba/opengl/texture_bindings.h"

bool OpenGlBgBitmapMode4Stage(OpenGlBgBitmapMode4* context,
                              const GbaPpuMemory* memory,
                              const GbaPpuRegisters* registers,
                              GbaPpuDirtyBits* dirty_bits) {
  if (!registers->dispcnt.bg2_enable || registers->dispcnt.mode != 4) {
    context->enabled = false;
    return false;
  }

  context->enabled = true;

  context->page = registers->dispcnt.page_select ? 1u : 0u;

  if (!dirty_bits->vram.bitmap_mode_4[context->page]) {
    return false;
  }

  dirty_bits->vram.bitmap_mode_4[context->page] = false;
  context->dirty = true;

  return true;
}

void OpenGlBgBitmapMode4Bind(const OpenGlBgBitmapMode4* context,
                             GLuint program) {
  if (!context->enabled) {
    return;
  }

  GLint bg_mode4 = glGetUniformLocation(program, "palette_bitmap");
  glUniform1i(bg_mode4, PALETTE_BITMAP_TEXTURE);

  glActiveTexture(GL_TEXTURE0 + PALETTE_BITMAP_TEXTURE);
  glBindTexture(GL_TEXTURE_2D, context->textures[context->page]);
}

void OpenGlBgBitmapMode4Reload(OpenGlBgBitmapMode4* context,
                               const GbaPpuMemory* memory) {
  if (context->dirty) {
    glBindTexture(GL_TEXTURE_2D, context->textures[context->page]);
    glTexSubImage2D(
        GL_TEXTURE_2D, /*level=*/0, /*xoffset=*/0, /*yoffset=*/0,
        /*width=*/GBA_SCREEN_WIDTH, /*height=*/GBA_SCREEN_HEIGHT,
        /*format=*/GL_RED_INTEGER, /*type=*/GL_UNSIGNED_BYTE,
        /*pixels=*/memory->vram.mode_4.bg.pages[context->page].pixels);
    glBindTexture(GL_TEXTURE_2D, 0);
    context->dirty = false;
  }
}

void OpenGlBgBitmapMode4ReloadContext(OpenGlBgBitmapMode4* context) {
  void* zeroes = calloc(1u, GBA_SCREEN_HEIGHT * GBA_SCREEN_WIDTH);

  glGenTextures(2u, context->textures);
  for (uint8_t i = 0u; i < 2u; i++) {
    glBindTexture(GL_TEXTURE_2D, context->textures[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, /*level=*/0, /*internal_format=*/GL_R8UI,
                 /*width=*/GBA_SCREEN_WIDTH, /*height=*/GBA_SCREEN_HEIGHT,
                 /*border=*/0, /*format=*/GL_RED_INTEGER,
                 /*type=*/GL_UNSIGNED_BYTE,
                 /*pixels=*/zeroes);
  }

  glBindTexture(GL_TEXTURE_2D, 0);

  free(zeroes);
}

void OpenGlBgBitmapMode4Destroy(OpenGlBgBitmapMode4* context) {
  glDeleteTextures(2u, context->textures);
}