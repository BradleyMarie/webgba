#include "emulator/ppu/gba/opengl/tilemap.h"

#include <stdlib.h>

#include "emulator/ppu/gba/opengl/texture_bindings.h"

bool OpenGlTilemapStage(OpenGlTilemap* context, const GbaPpuMemory* memory,
                        const GbaPpuRegisters* registers,
                        GbaPpuDirtyBits* dirty_bits) {
  bool result = false;
  for (uint32_t i = 0u; i < GBA_TILE_MODE_NUM_BACKGROUND_TILE_MAP_BLOCKS; i++) {
    if (dirty_bits->vram.affine_tilemap[i] &&
        (registers->dispcnt.mode == 1u || registers->dispcnt.mode == 2u)) {
      dirty_bits->vram.affine_tilemap[i] = false;
      context->affine_dirty[i] = true;
      result = true;
    }

    if (dirty_bits->vram.scrolling_tilemap[i] &&
        registers->dispcnt.mode <= 1u) {
      dirty_bits->vram.scrolling_tilemap[i] = false;
      context->scrolling_dirty[i] = true;
      result = true;
    }
  }

  return result;
}

void OpenGlTilemapBind(const OpenGlTilemap* context, GLuint program) {
  GLint bg_affine_tilemap = glGetUniformLocation(program, "affine_tilemap");
  glUniform1i(bg_affine_tilemap, BG_AFFINE_TILEMAP_TEXTURE);

  glActiveTexture(GL_TEXTURE0 + BG_AFFINE_TILEMAP_TEXTURE);
  glBindTexture(GL_TEXTURE_2D_ARRAY, context->affine);

  GLint bg_scrolling_tilemap =
      glGetUniformLocation(program, "scrolling_tilemap");
  glUniform1i(bg_scrolling_tilemap, BG_SCROLLING_TILEMAP_TEXTURE);

  glActiveTexture(GL_TEXTURE0 + BG_SCROLLING_TILEMAP_TEXTURE);
  glBindTexture(GL_TEXTURE_2D_ARRAY, context->scrolling);
}

void OpenGlTilemapReload(OpenGlTilemap* context, const GbaPpuMemory* memory) {
  for (uint8_t i = 0; i < GBA_TILE_MODE_NUM_BACKGROUND_TILE_MAP_BLOCKS; i++) {
    if (context->affine_dirty[i]) {
      glBindTexture(GL_TEXTURE_2D_ARRAY, context->affine);
      glTexSubImage3D(
          GL_TEXTURE_2D_ARRAY, /*level=*/0, /*xoffset=*/0,
          /*yoffset=*/0, /*zoffset=*/i,
          /*width=*/AFFINE_TILEMAP_TEXTURE_X_SIZE,
          /*height=*/AFFINE_TILEMAP_TEXTURE_Y_SIZE,
          /*depth=*/1, /*format=*/GL_RED_INTEGER,
          /*type=*/GL_UNSIGNED_BYTE,
          /*pixels=*/memory->vram.mode_012.bg.tile_map.blocks[i].indices);
      glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
      context->affine_dirty[i] = false;
    }

    if (context->scrolling_dirty[i]) {
      glBindTexture(GL_TEXTURE_2D_ARRAY, context->scrolling);
      glTexSubImage3D(
          GL_TEXTURE_2D_ARRAY, /*level=*/0, /*xoffset=*/0,
          /*yoffset=*/0, /*zoffset=*/i,
          /*width=*/GBA_TILE_MAP_BLOCK_1D_SIZE,
          /*height=*/GBA_TILE_MAP_BLOCK_1D_SIZE,
          /*depth=*/1, /*format=*/GL_RED_INTEGER,
          /*type=*/GL_UNSIGNED_SHORT,
          /*pixels=*/memory->vram.mode_012.bg.tile_map.blocks[i].entries);
      glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
      context->scrolling_dirty[i] = false;
    }
  }
}

void OpenGlTilemapReloadContext(OpenGlTilemap* context) {
  void* zeroes = calloc(1u, GBA_TILE_MODE_NUM_BACKGROUND_TILE_MAP_BLOCKS *
                                GBA_AFFINE_TILE_MAP_INDICES_PER_BLOCK);

  glGenTextures(1u, &context->affine);
  glBindTexture(GL_TEXTURE_2D_ARRAY, context->affine);
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

  glGenTextures(1u, &context->scrolling);
  glBindTexture(GL_TEXTURE_2D_ARRAY, context->scrolling);
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

  glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

  free(zeroes);
}

void OpenGlTilemapDestroy(OpenGlTilemap* context) {
  glDeleteTextures(1u, &context->affine);
  glDeleteTextures(1u, &context->scrolling);
}