#include "emulator/ppu/gba/opengl/bg_bitmap_mode3.h"

#include "emulator/ppu/gba/opengl/texture_bindings.h"

bool OpenGlBgBitmapMode3Stage(OpenGlBgBitmapMode3* context,
                              const GbaPpuMemory* memory,
                              const GbaPpuRegisters* registers,
                              GbaPpuDirtyBits* dirty_bits) {
  if (!registers->dispcnt.bg2_enable || registers->dispcnt.mode != 3) {
    context->enabled = false;
    return false;
  }

  context->enabled = true;

  if (!dirty_bits->vram.bitmap_mode_3) {
    return false;
  }

  for (uint_fast8_t y = 0; y < GBA_SCREEN_HEIGHT; y++) {
    for (uint_fast8_t x = 0; x < GBA_SCREEN_WIDTH; x++) {
      context->staging[y * GBA_SCREEN_WIDTH + x] =
          (memory->vram.mode_3.bg.pixels[y][x] << 1u) | 1u;
    }
  }

  dirty_bits->vram.bitmap_mode_3 = false;
  context->dirty = true;

  return true;
}

void OpenGlBgBitmapMode3Bind(const OpenGlBgBitmapMode3* context,
                             const UniformLocations* locations) {
  if (!context->enabled) {
    return;
  }

  glUniform1i(locations->bitmap, BITMAP_TEXTURE);
  glActiveTexture(GL_TEXTURE0 + BITMAP_TEXTURE);
  glBindTexture(GL_TEXTURE_2D, context->textures[context->texture_index]);
}

void OpenGlBgBitmapMode3Reload(OpenGlBgBitmapMode3* context) {
  if (context->dirty) {
    context->texture_index += 1u;
    if (context->texture_index == GBA_SCREEN_HEIGHT) {
      context->texture_index = 0u;
    }

    glBindTexture(GL_TEXTURE_2D, context->textures[context->texture_index]);
    glTexSubImage2D(GL_TEXTURE_2D, /*level=*/0, /*xoffset=*/0, /*yoffset=*/0,
                    /*width=*/GBA_SCREEN_WIDTH, /*height=*/GBA_SCREEN_HEIGHT,
                    /*format=*/GL_RGBA, /*type=*/GL_UNSIGNED_SHORT_5_5_5_1,
                    /*pixels=*/context->staging);
    glBindTexture(GL_TEXTURE_2D, 0);
    context->dirty = false;
  }
}

void OpenGlBgBitmapMode3ReloadContext(OpenGlBgBitmapMode3* context) {
  glGenTextures(GBA_SCREEN_HEIGHT, context->textures);
  for (uint8_t i = 0u; i < GBA_SCREEN_HEIGHT; i++) {
    glBindTexture(GL_TEXTURE_2D, context->textures[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, /*level=*/0, /*internal_format=*/GL_RGBA,
                 /*width=*/GBA_SCREEN_WIDTH, /*height=*/GBA_SCREEN_HEIGHT,
                 /*border=*/0, /*format=*/GL_RGBA,
                 /*type=*/GL_UNSIGNED_SHORT_5_5_5_1,
                 /*pixels=*/context->staging);
  }
  glBindTexture(GL_TEXTURE_2D, 0);
}

void OpenGlBgBitmapMode3Destroy(OpenGlBgBitmapMode3* context) {
  glDeleteTextures(GBA_SCREEN_HEIGHT, context->textures);
}