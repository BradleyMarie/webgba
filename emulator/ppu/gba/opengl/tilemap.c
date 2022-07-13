#include "emulator/ppu/gba/opengl/tilemap.h"

#include <stdlib.h>

#include "emulator/ppu/gba/opengl/texture_bindings.h"

bool OpenGlTilemapStage(OpenGlTilemap* context, const GbaPpuMemory* memory,
                        const GbaPpuRegisters* registers,
                        GbaPpuDirtyBits* dirty_bits) {
  if (dirty_bits->vram.affine_tilemap &&
      (registers->dispcnt.mode == 1u || registers->dispcnt.mode == 2u)) {
    dirty_bits->vram.affine_tilemap = false;
    context->affine_dirty = true;
  }

  if (dirty_bits->vram.scrolling_tilemap && registers->dispcnt.mode <= 1u) {
    dirty_bits->vram.scrolling_tilemap = false;
    context->scrolling_dirty = true;
  }

  return context->affine_dirty || context->scrolling_dirty;
}

void OpenGlTilemapBind(const OpenGlTilemap* context, GLuint program) {
  GLint affine_tilemap = glGetUniformLocation(program, "affine_tilemap");
  if (affine_tilemap >= 0) {
    glUniform1i(affine_tilemap, BG_AFFINE_TILEMAP_TEXTURE);

    glActiveTexture(GL_TEXTURE0 + BG_AFFINE_TILEMAP_TEXTURE);
    glBindTexture(GL_TEXTURE_2D_ARRAY,
                  context->affine_textures[context->affine_texture_index]);
  }

  GLint scrolling_tilemap = glGetUniformLocation(program, "scrolling_tilemap");
  if (scrolling_tilemap >= 0) {
    glUniform1i(scrolling_tilemap, BG_SCROLLING_TILEMAP_TEXTURE);

    glActiveTexture(GL_TEXTURE0 + BG_SCROLLING_TILEMAP_TEXTURE);
    glBindTexture(
        GL_TEXTURE_2D_ARRAY,
        context->scrolling_textures[context->scrolling_texture_index]);
  }
}

void OpenGlTilemapReload(OpenGlTilemap* context, const GbaPpuMemory* memory) {
  if (context->affine_dirty) {
    context->affine_texture_index += 1u;
    if (context->affine_texture_index == GBA_SCREEN_HEIGHT) {
      context->affine_texture_index = 0u;
    }

    glBindTexture(GL_TEXTURE_2D_ARRAY,
                  context->affine_textures[context->affine_texture_index]);
    glTexSubImage3D(
        GL_TEXTURE_2D_ARRAY, /*level=*/0, /*xoffset=*/0, /*yoffset=*/0,
        /*zoffset=*/0u, /*width=*/AFFINE_TILEMAP_TEXTURE_X_SIZE,
        /*height=*/AFFINE_TILEMAP_TEXTURE_Y_SIZE,
        /*depth=*/GBA_TILE_MODE_NUM_BACKGROUND_TILE_MAP_BLOCKS,
        /*format=*/GL_RED_INTEGER, /*type=*/GL_UNSIGNED_BYTE,
        /*pixels=*/memory->vram.mode_012.bg.tile_map.blocks[0u].indices);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    context->affine_dirty = false;
  }

  if (context->scrolling_dirty) {
    context->scrolling_texture_index += 1u;
    if (context->scrolling_texture_index == GBA_SCREEN_HEIGHT) {
      context->scrolling_texture_index = 0u;
    }

    glBindTexture(
        GL_TEXTURE_2D_ARRAY,
        context->scrolling_textures[context->scrolling_texture_index]);
    glTexSubImage3D(
        GL_TEXTURE_2D_ARRAY, /*level=*/0, /*xoffset=*/0, /*yoffset=*/0,
        /*zoffset=*/0u, /*width=*/GBA_TILE_MAP_BLOCK_1D_SIZE,
        /*height=*/GBA_TILE_MAP_BLOCK_1D_SIZE,
        /*depth=*/GBA_TILE_MODE_NUM_BACKGROUND_TILE_MAP_BLOCKS,
        /*format=*/GL_RED_INTEGER, /*type=*/GL_UNSIGNED_SHORT,
        /*pixels=*/memory->vram.mode_012.bg.tile_map.blocks[0u].entries);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    context->scrolling_dirty = false;
  }
}

void OpenGlTilemapReloadContext(OpenGlTilemap* context) {
  void* zeroes = calloc(1u, GBA_TILE_MODE_NUM_BACKGROUND_TILE_MAP_BLOCKS *
                                GBA_AFFINE_TILE_MAP_INDICES_PER_BLOCK);

  glGenTextures(GBA_SCREEN_HEIGHT, context->affine_textures);
  for (uint8_t i = 0u; i < GBA_SCREEN_HEIGHT; i++) {
    glBindTexture(GL_TEXTURE_2D_ARRAY, context->affine_textures[i]);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, /*level=*/0, /*internal_format=*/GL_R8UI,
                 /*width=*/AFFINE_TILEMAP_TEXTURE_X_SIZE,
                 /*height=*/AFFINE_TILEMAP_TEXTURE_Y_SIZE,
                 /*depth=*/GBA_TILE_MODE_NUM_BACKGROUND_TILE_MAP_BLOCKS,
                 /*border=*/0, /*format=*/GL_RED_INTEGER,
                 /*type=*/GL_UNSIGNED_BYTE,
                 /*pixels=*/zeroes);
  }

  glGenTextures(GBA_SCREEN_HEIGHT, context->scrolling_textures);
  for (uint8_t i = 0u; i < GBA_SCREEN_HEIGHT; i++) {
    glBindTexture(GL_TEXTURE_2D_ARRAY, context->scrolling_textures[i]);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, /*level=*/0, /*internal_format=*/GL_R16UI,
                 /*width=*/GBA_TILE_MAP_BLOCK_1D_SIZE,
                 /*height=*/GBA_TILE_MAP_BLOCK_1D_SIZE,
                 /*depth=*/GBA_TILE_MODE_NUM_BACKGROUND_TILE_MAP_BLOCKS,
                 /*border=*/0, /*format=*/GL_RED_INTEGER,
                 /*type=*/GL_UNSIGNED_SHORT,
                 /*pixels=*/zeroes);
  }

  glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

  free(zeroes);
}

void OpenGlTilemapDestroy(OpenGlTilemap* context) {
  glDeleteTextures(GBA_SCREEN_HEIGHT, context->affine_textures);
  glDeleteTextures(GBA_SCREEN_HEIGHT, context->scrolling_textures);
}