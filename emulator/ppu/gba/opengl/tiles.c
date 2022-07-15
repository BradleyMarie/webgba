#include "emulator/ppu/gba/opengl/tiles.h"

#include <stdlib.h>

#include "emulator/ppu/gba/opengl/texture_bindings.h"

bool OpenGlTilesStage(OpenGlTiles* context, const GbaPpuMemory* memory,
                      const GbaPpuRegisters* registers,
                      GbaPpuDirtyBits* dirty_bits) {
  // Backgrounds
  if (dirty_bits->vram.bg_tiles && registers->dispcnt.mode <= 2) {
    dirty_bits->vram.bg_tiles = false;
    context->bg_dirty = true;
  }

  // Objects
  if (dirty_bits->vram.obj_tiles && registers->dispcnt.object_enable) {
    dirty_bits->vram.obj_tiles = false;
    context->obj_dirty = true;
  }

  return context->bg_dirty || context->obj_dirty;
}

void OpenGlTilesBind(const OpenGlTiles* context,
                     const UniformLocations* locations) {
  if (locations->background_tiles >= 0) {
    glUniform1i(locations->background_tiles, BG_TILES_TEXTURE);
    glActiveTexture(GL_TEXTURE0 + BG_TILES_TEXTURE);
    glBindTexture(GL_TEXTURE_2D_ARRAY,
                  context->bg_textures[context->bg_texture_index]);
  }

  if (locations->object_tiles >= 0) {
    glUniform1i(locations->object_tiles, OBJ_TILES_TEXTURE);
    glActiveTexture(GL_TEXTURE0 + OBJ_TILES_TEXTURE);
    glBindTexture(GL_TEXTURE_2D,
                  context->obj_textures[context->obj_texture_index]);
  }
}

void OpenGlTilesReload(OpenGlTiles* context, const GbaPpuMemory* memory) {
  if (context->bg_dirty) {
    context->bg_texture_index += 1u;
    if (context->bg_texture_index == GBA_SCREEN_HEIGHT) {
      context->bg_texture_index = 0u;
    }

    glBindTexture(GL_TEXTURE_2D_ARRAY,
                  context->bg_textures[context->bg_texture_index]);
    glTexSubImage3D(
        GL_TEXTURE_2D_ARRAY, /*level=*/0, /*xoffset=*/0, /*yoffset=*/0,
        /*zoffset=*/0u, /*width=*/GBA_TILE_1D_SIZE * GBA_TILE_1D_SIZE / 2u,
        /*height=*/GBA_TILE_MODE_TILE_BLOCK_NUM_S_TILES,
        /*depth=*/GBA_TILE_MODE_NUM_BACKGROUND_TILE_BLOCKS,
        /*format=*/GL_RED_INTEGER, /*type=*/GL_UNSIGNED_BYTE,
        /*pixels=*/memory->vram.mode_012.bg.tiles.blocks[0u].s_tiles);

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    context->bg_dirty = false;
  }

  if (context->obj_dirty) {
    context->obj_texture_index += 1u;
    if (context->obj_texture_index == GBA_SCREEN_HEIGHT) {
      context->obj_texture_index = 0u;
    }

    glBindTexture(GL_TEXTURE_2D,
                  context->obj_textures[context->obj_texture_index]);
    glTexSubImage2D(GL_TEXTURE_2D, /*level=*/0, /*xoffset=*/0, /*yoffset=*/0u,
                    /*width=*/GBA_TILE_1D_SIZE * GBA_TILE_1D_SIZE / 2u,
                    /*height=*/GBA_TILE_MODE_NUM_OBJECT_S_TILES,
                    /*format=*/GL_RED_INTEGER, /*type=*/GL_UNSIGNED_BYTE,
                    /*pixels=*/memory->vram.mode_012.obj.s_tiles);
    glBindTexture(GL_TEXTURE_2D, 0);

    context->obj_dirty = false;
  }
}

void OpenGlTilesReloadContext(OpenGlTiles* context) {
  void* zeroes = calloc(1u, GBA_TILE_MODE_NUM_BACKGROUND_TILE_BLOCKS *
                                GBA_TILE_MODE_TILE_BLOCK_NUM_D_TILES *
                                GBA_TILE_1D_SIZE * GBA_TILE_1D_SIZE);

  glGenTextures(GBA_SCREEN_HEIGHT, context->bg_textures);
  for (uint8_t i = 0u; i < GBA_SCREEN_HEIGHT; i++) {
    glBindTexture(GL_TEXTURE_2D_ARRAY, context->bg_textures[i]);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, /*level=*/0, /*internal_format=*/GL_R8UI,
                 /*width=*/GBA_TILE_1D_SIZE * GBA_TILE_1D_SIZE / 2u,
                 /*height=*/GBA_TILE_MODE_TILE_BLOCK_NUM_S_TILES,
                 /*depth=*/GBA_TILE_MODE_NUM_BACKGROUND_TILE_BLOCKS,
                 /*border=*/0, /*format=*/GL_RED_INTEGER,
                 /*type=*/GL_UNSIGNED_BYTE,
                 /*pixels=*/zeroes);
  }

  glGenTextures(GBA_SCREEN_HEIGHT, context->obj_textures);
  for (uint8_t i = 0u; i < GBA_SCREEN_HEIGHT; i++) {
    glBindTexture(GL_TEXTURE_2D, context->obj_textures[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, /*level=*/0, /*internal_format=*/GL_R8UI,
                 /*width=*/GBA_TILE_1D_SIZE * GBA_TILE_1D_SIZE / 2u,
                 /*height=*/GBA_TILE_MODE_NUM_OBJECT_S_TILES,
                 /*border=*/0, /*format=*/GL_RED_INTEGER,
                 /*type=*/GL_UNSIGNED_BYTE,
                 /*pixels=*/zeroes);
  }

  glBindTexture(GL_TEXTURE_2D, 0);

  free(zeroes);
}

void OpenGlTilesDestroy(OpenGlTiles* context) {
  glDeleteTextures(GBA_SCREEN_HEIGHT, context->bg_textures);
  glDeleteTextures(GBA_SCREEN_HEIGHT, context->obj_textures);
}