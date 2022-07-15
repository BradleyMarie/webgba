#include "emulator/ppu/gba/opengl/palette.h"

#include "emulator/ppu/gba/opengl/texture_bindings.h"

bool OpenGlPaletteStage(OpenGlPalette* context, GbaPpuDirtyBits* dirty_bits) {
  if (dirty_bits->palette.palette[0u]) {
    dirty_bits->palette.palette[0u] = false;
    context->bg_dirty = true;
  }

  if (dirty_bits->palette.palette[1u]) {
    dirty_bits->palette.palette[1u] = false;
    context->obj_dirty = true;
  }

  return context->bg_dirty || context->obj_dirty;
}

void OpenGlPaletteBind(const OpenGlPalette* context,
                       const UniformLocations* locations) {
  if (locations->background_palette >= 0) {
    glUniform1i(locations->background_palette, BG_PALETTE_TEXTURE);
    glActiveTexture(GL_TEXTURE0 + BG_PALETTE_TEXTURE);
    glBindTexture(GL_TEXTURE_2D, context->bg_palette);
  }

  if (locations->object_palette >= 0) {
    glUniform1i(locations->object_palette, OBJ_PALETTE_TEXTURE);
    glActiveTexture(GL_TEXTURE0 + OBJ_PALETTE_TEXTURE);
    glBindTexture(GL_TEXTURE_2D, context->obj_palette);
  }
}

void OpenGlPaletteReload(OpenGlPalette* context, const GbaPpuMemory* memory) {
  if (context->bg_dirty) {
    glBindTexture(GL_TEXTURE_2D, context->bg_palette);
    glTexSubImage2D(GL_TEXTURE_2D, /*level=*/0, /*xoffset=*/0, /*yoffset=*/0,
                    /*width=*/GBA_LARGE_PALETTE_SIZE, /*height=*/1,
                    /*format=*/GL_RGBA, /*type=*/GL_UNSIGNED_SHORT_5_5_5_1,
                    /*pixels=*/memory->palette.bg.large_palette);
    glBindTexture(GL_TEXTURE_2D, 0);
    context->bg_dirty = false;
  }

  if (context->obj_dirty) {
    glBindTexture(GL_TEXTURE_2D, context->obj_palette);
    glTexSubImage2D(GL_TEXTURE_2D, /*level=*/0, /*xoffset=*/0, /*yoffset=*/0,
                    /*width=*/GBA_LARGE_PALETTE_SIZE, /*height=*/1,
                    /*format=*/GL_RGBA, /*type=*/GL_UNSIGNED_SHORT_5_5_5_1,
                    /*pixels=*/memory->palette.obj.large_palette);
    glBindTexture(GL_TEXTURE_2D, 0);
    context->obj_dirty = false;
  }
}

void OpenGlPaletteReloadContext(OpenGlPalette* context) {
  glGenTextures(1u, &context->bg_palette);
  glBindTexture(GL_TEXTURE_2D, context->bg_palette);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, /*level=*/0, /*internal_format=*/GL_RGBA,
               /*width=*/GBA_LARGE_PALETTE_SIZE,
               /*height=*/1, /*border=*/0, /*format=*/GL_RGBA,
               /*type=*/GL_UNSIGNED_SHORT_5_5_5_1,
               /*pixels=*/context->zeroes);

  glGenTextures(1u, &context->obj_palette);
  glBindTexture(GL_TEXTURE_2D, context->obj_palette);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, /*level=*/0, /*internal_format=*/GL_RGBA,
               /*width=*/GBA_LARGE_PALETTE_SIZE,
               /*height=*/1, /*border=*/0, /*format=*/GL_RGBA,
               /*type=*/GL_UNSIGNED_SHORT_5_5_5_1,
               /*pixels=*/context->zeroes);

  glBindTexture(GL_TEXTURE_2D, 0);
}

void OpenGlPaletteDestroy(OpenGlPalette* context) {
  glDeleteTextures(1u, &context->bg_palette);
  glDeleteTextures(1u, &context->obj_palette);
}