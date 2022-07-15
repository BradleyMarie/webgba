#include "emulator/ppu/gba/opengl/bg_bitmap_mode5.h"

#include "emulator/ppu/gba/opengl/texture_bindings.h"

bool OpenGlBgBitmapMode5Stage(OpenGlBgBitmapMode5* context,
                              const GbaPpuMemory* memory,
                              const GbaPpuRegisters* registers,
                              GbaPpuDirtyBits* dirty_bits) {
  if (!registers->dispcnt.bg2_enable || registers->dispcnt.mode != 5) {
    context->enabled = false;
    return false;
  }

  context->enabled = true;
  context->page = registers->dispcnt.page_select ? 1u : 0u;

  if (!dirty_bits->vram.bitmap_mode_5[context->page]) {
    return false;
  }

  for (uint_fast8_t y = 0; y < GBA_REDUCED_FRAME_HEIGHT; y++) {
    for (uint_fast8_t x = 0; x < GBA_REDUCED_FRAME_WIDTH; x++) {
      uint16_t color = memory->vram.mode_5.bg.pages[context->page].pixels[y][x];
      context->staging[y * GBA_REDUCED_FRAME_WIDTH + x] = (color << 1u) | 1u;
    }
  }

  dirty_bits->vram.bitmap_mode_5[context->page] = false;
  context->dirty = true;

  return true;
}

void OpenGlBgBitmapMode5Bind(const OpenGlBgBitmapMode5* context,
                             const UniformLocations* locations) {
  if (!context->enabled) {
    return;
  }

  glUniform1i(locations->bitmap, BITMAP_TEXTURE);
  glActiveTexture(GL_TEXTURE0 + BITMAP_TEXTURE);
  glBindTexture(GL_TEXTURE_2D, context->current_textures[context->page]);
}

void OpenGlBgBitmapMode5Reload(OpenGlBgBitmapMode5* context) {
  if (context->dirty) {
    context->texture_pool_index += 1u;
    if (context->texture_pool_index == GBA_SCREEN_HEIGHT) {
      context->texture_pool_index = 0u;
    }

    glBindTexture(GL_TEXTURE_2D,
                  context->texture_pool[context->texture_pool_index]);
    glTexSubImage2D(GL_TEXTURE_2D, /*level=*/0, /*xoffset=*/0, /*yoffset=*/0,
                    /*width=*/GBA_REDUCED_FRAME_WIDTH,
                    /*height=*/GBA_REDUCED_FRAME_HEIGHT,
                    /*format=*/GL_RGBA, /*type=*/GL_UNSIGNED_SHORT_5_5_5_1,
                    /*pixels=*/context->staging);
    glBindTexture(GL_TEXTURE_2D, 0);

    context->current_textures[context->page] =
        context->texture_pool[context->texture_pool_index];

    context->dirty = false;
  }
}

void OpenGlBgBitmapMode5ReloadContext(OpenGlBgBitmapMode5* context) {
  glGenTextures(GBA_SCREEN_HEIGHT, context->texture_pool);
  for (uint8_t i = 0u; i < 2u; i++) {
    glBindTexture(GL_TEXTURE_2D, context->texture_pool[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, /*level=*/0, /*internal_format=*/GL_RGBA,
                 /*width=*/GBA_REDUCED_FRAME_WIDTH,
                 /*height=*/GBA_REDUCED_FRAME_HEIGHT,
                 /*border=*/0, /*format=*/GL_RGBA,
                 /*type=*/GL_UNSIGNED_SHORT_5_5_5_1,
                 /*pixels=*/context->staging);
  }
  glBindTexture(GL_TEXTURE_2D, 0);

  context->current_textures[0u] =
      context->texture_pool[context->texture_pool_index];
  context->current_textures[1u] =
      context->texture_pool[context->texture_pool_index];
}

void OpenGlBgBitmapMode5Destroy(OpenGlBgBitmapMode5* context) {
  glDeleteTextures(GBA_SCREEN_HEIGHT, context->texture_pool);
}