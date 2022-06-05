#include "emulator/ppu/gba/opengl/bg_bitmap_mode3.h"

#include <assert.h>
#include <string.h>

#include "emulator/ppu/gba/opengl/texture_bindings.h"

void OpenGlBgBitmapMode3Reload(OpenGlBgBitmapMode3* context,
                               const GbaPpuMemory* memory,
                               const GbaPpuRegisters* registers,
                               GbaPpuDirtyBits* dirty_bits) {
  if (!registers->dispcnt.bg2_enable || registers->dispcnt.mode != 3) {
    context->enabled = false;
    return;
  }

  context->enabled = true;

  if (!dirty_bits->vram.mode_3.overall) {
    return;
  }

  for (uint_fast8_t y = 0; y < GBA_SCREEN_HEIGHT; y++) {
    for (uint_fast8_t x = 0; x < GBA_SCREEN_WIDTH; x++) {
      context->staging[y * GBA_SCREEN_WIDTH + x] =
          (memory->vram.mode_3.bg.pixels[y][x] << 1u) | 1u;
    }
  }

  glBindTexture(GL_TEXTURE_2D, context->texture);
  glTexSubImage2D(GL_TEXTURE_2D, /*level=*/0, /*xoffset=*/0, /*yoffset=*/0,
                  /*width=*/GBA_SCREEN_WIDTH, /*height=*/GBA_SCREEN_HEIGHT,
                  /*format=*/GL_RGBA, /*type=*/GL_UNSIGNED_SHORT_5_5_5_1,
                  /*pixels=*/context->staging);
  glBindTexture(GL_TEXTURE_2D, 0);

  dirty_bits->vram.mode_3.overall = false;
}

void OpenGlBgBitmapMode3Bind(const OpenGlBgBitmapMode3* context,
                             GLuint program) {
  if (!context->enabled) {
    return;
  }

  GLint bg_mode3 = glGetUniformLocation(program, "bitmap");
  glUniform1i(bg_mode3, BITMAP_TEXTURE);

  glActiveTexture(GL_TEXTURE0 + BITMAP_TEXTURE);
  glBindTexture(GL_TEXTURE_2D, context->texture);
}

void OpenGlBgBitmapMode3ReloadContext(OpenGlBgBitmapMode3* context) {
  glGenTextures(1u, &context->texture);
  glBindTexture(GL_TEXTURE_2D, context->texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, /*level=*/0, /*internal_format=*/GL_RGBA,
               /*width=*/GBA_SCREEN_WIDTH, /*height=*/GBA_SCREEN_HEIGHT,
               /*border=*/0, /*format=*/GL_RGBA,
               /*type=*/GL_UNSIGNED_SHORT_5_5_5_1,
               /*pixels=*/context->staging);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void OpenGlBgBitmapMode3Destroy(OpenGlBgBitmapMode3* context) {
  glDeleteTextures(1u, &context->texture);
}