#include "emulator/ppu/gba/opengl/bg_bitmap_mode4.h"

#include <assert.h>
#include <string.h>

#include "emulator/ppu/gba/opengl/texture_bindings.h"

void OpenGlBgBitmapMode4Reload(OpenGlBgBitmapMode4* context,
                               const GbaPpuMemory* memory,
                               const GbaPpuRegisters* registers,
                               GbaPpuDirtyBits* dirty_bits) {
  if (!registers->dispcnt.bg2_enable) {
    context->enabled = false;
    return;
  }

  context->enabled = true;
  context->page = registers->dispcnt.page_select;

  if (!dirty_bits->vram.mode_4.pages[registers->dispcnt.page_select]) {
    return;
  }

  for (uint_fast8_t y = 0; y < GBA_SCREEN_HEIGHT; y++) {
    for (uint_fast8_t x = 0; x < GBA_SCREEN_WIDTH; x++) {
      context->staging[y * GBA_SCREEN_WIDTH + x] =
          memory->vram.mode_4.bg.pages[registers->dispcnt.page_select]
              .pixels[y][x];
    }
  }

  glBindTexture(GL_TEXTURE_2D,
                context->textures[registers->dispcnt.page_select]);
  glTexSubImage2D(GL_TEXTURE_2D, /*level=*/0, /*xoffset=*/0, /*yoffset=*/0,
                  /*width=*/GBA_SCREEN_WIDTH, /*height=*/GBA_SCREEN_HEIGHT,
                  /*format=*/GL_RED_INTEGER, /*type=*/GL_UNSIGNED_BYTE,
                  /*pixels=*/context->staging);
  glBindTexture(GL_TEXTURE_2D, 0);

  dirty_bits->vram.mode_4.pages[registers->dispcnt.page_select] = false;
}

void OpenGlBgBitmapMode4Bind(const OpenGlBgBitmapMode4* context,
                             GLuint program) {
  GLint bg_mode4 = glGetUniformLocation(program, "mode4_bitmap");
  glUniform1i(bg_mode4, BG2_MODE4_TEXTURE);

  glActiveTexture(GL_TEXTURE0 + BG2_MODE4_TEXTURE);
  glBindTexture(GL_TEXTURE_2D, context->textures[context->page]);
}

void OpenGlBgBitmapMode4ReloadContext(OpenGlBgBitmapMode4* context) {
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
                 /*pixels=*/NULL);
  }
  glBindTexture(GL_TEXTURE_2D, 0);
}

void OpenGlBgBitmapMode4Destroy(OpenGlBgBitmapMode4* context) {
  glDeleteTextures(2u, context->textures);
}