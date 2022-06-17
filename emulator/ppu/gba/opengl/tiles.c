#include "emulator/ppu/gba/opengl/tiles.h"

#include <stdlib.h>

#include "emulator/ppu/gba/opengl/texture_bindings.h"

bool OpenGlTilesStage(OpenGlTiles* context, const GbaPpuMemory* memory,
                      const GbaPpuRegisters* registers,
                      GbaPpuDirtyBits* dirty_bits) {
  // Backgrounds
  bool result = false;
  for (uint32_t i = 0u; i < GBA_TILE_MODE_NUM_BACKGROUND_TILE_BLOCKS; i++) {
    if (!dirty_bits->vram.tiles[i] || registers->dispcnt.mode >= 3) {
      continue;
    }

    dirty_bits->vram.tiles[i] = false;
    context->bg_dirty[i] = true;
    result = true;
  }

  // Objects
  uint8_t starting_tile = (registers->dispcnt.mode < 3) ? 0u : 1u;
  for (uint32_t i = starting_tile; i < 2; i++) {
    if (!dirty_bits->vram.tiles[GBA_TILE_MODE_NUM_BACKGROUND_TILE_BLOCKS + i]) {
      continue;
    }

    dirty_bits->vram.tiles[GBA_TILE_MODE_NUM_BACKGROUND_TILE_BLOCKS + i] =
        false;
    context->obj_dirty[i] = true;
    result = true;
  }

  return result;
}

void OpenGlTilesBind(const OpenGlTiles* context, GLuint program) {
  GLint background_tiles = glGetUniformLocation(program, "background_tiles");
  glUniform1i(background_tiles, BG_TILES_TEXTURE);

  glActiveTexture(GL_TEXTURE0 + BG_TILES_TEXTURE);
  glBindTexture(GL_TEXTURE_2D_ARRAY, context->bg_tiles);

  GLint obj_tiles_s = glGetUniformLocation(program, "object_tiles");
  glUniform1i(obj_tiles_s, OBJ_TILES_TEXTURE);

  glActiveTexture(GL_TEXTURE0 + OBJ_TILES_TEXTURE);
  glBindTexture(GL_TEXTURE_2D, context->obj_tiles);
}

void OpenGlTilesReload(OpenGlTiles* context, const GbaPpuMemory* memory) {
  for (uint8_t i = 0u; i < GBA_TILE_MODE_NUM_BACKGROUND_TILE_BLOCKS; i++) {
    if (context->bg_dirty[i]) {
      glBindTexture(GL_TEXTURE_2D_ARRAY, context->bg_tiles);
      glTexSubImage3D(
          GL_TEXTURE_2D_ARRAY, /*level=*/0, /*xoffset=*/0, /*yoffset=*/0,
          /*zoffset=*/i, /*width=*/GBA_TILE_1D_SIZE * GBA_TILE_1D_SIZE / 2u,
          /*height=*/GBA_TILE_MODE_TILE_BLOCK_NUM_S_TILES, /*depth=*/1,
          /*format=*/GL_RED_INTEGER, /*type=*/GL_UNSIGNED_BYTE,
          /*pixels=*/memory->vram.mode_012.bg.tiles.blocks[i].s_tiles);

      glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
      context->bg_dirty[i] = false;
    }
  }

  for (uint8_t i = 0u; i < GBA_TILE_MODE_NUM_OBJECT_S_TILES /
                               GBA_TILE_MODE_TILE_BLOCK_NUM_S_TILES;
       i++) {
    if (context->obj_dirty[i]) {
      glBindTexture(GL_TEXTURE_2D, context->obj_tiles);
      glTexSubImage2D(GL_TEXTURE_2D, /*level=*/0, /*xoffset=*/0,
                      /*yoffset=*/GBA_TILE_MODE_TILE_BLOCK_NUM_S_TILES * i,
                      /*width=*/GBA_TILE_1D_SIZE * GBA_TILE_1D_SIZE / 2u,
                      /*height=*/GBA_TILE_MODE_TILE_BLOCK_NUM_S_TILES,
                      /*format=*/GL_RED_INTEGER, /*type=*/GL_UNSIGNED_BYTE,
                      /*pixels=*/memory->vram.mode_012.obj.s_tiles +
                          (i * GBA_TILE_MODE_TILE_BLOCK_NUM_S_TILES));
      glBindTexture(GL_TEXTURE_2D, 0);
      context->obj_dirty[i] = false;
    }
  }
}

void OpenGlTilesReloadContext(OpenGlTiles* context) {
  void* zeroes = calloc(1u, GBA_TILE_MODE_NUM_BACKGROUND_TILE_BLOCKS *
                                GBA_TILE_MODE_TILE_BLOCK_NUM_D_TILES *
                                GBA_TILE_1D_SIZE * GBA_TILE_1D_SIZE);

  glGenTextures(1u, &context->bg_tiles);
  glBindTexture(GL_TEXTURE_2D_ARRAY, context->bg_tiles);
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
  glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

  glGenTextures(1u, &context->obj_tiles);
  glBindTexture(GL_TEXTURE_2D, context->obj_tiles);
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
  glBindTexture(GL_TEXTURE_2D, 0);

  free(zeroes);
}

void OpenGlTilesDestroy(OpenGlTiles* context) {
  glDeleteTextures(1u, &context->bg_tiles);
  glDeleteTextures(1u, &context->obj_tiles);
}