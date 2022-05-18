#include "emulator/ppu/gba/opengl/tiles.h"

#include <string.h>

#include "emulator/ppu/gba/opengl/texture_bindings.h"

void OpenGlBgTilesReload(OpenGlBgTiles* context, const GbaPpuMemory* memory,
                         GbaPpuDirtyBits* dirty_bits) {
  for (uint8_t i = 0u; i < GBA_TILE_MODE_NUM_BACKGROUND_TILE_BLOCKS; i++) {
    if (!dirty_bits->vram.tile_mode.tiles[i]) {
      continue;
    }

    uint32_t insert_index = 0u;
    for (uint16_t t = 0u; t < GBA_TILE_MODE_TILE_BLOCK_NUM_S_TILES; t++) {
      for (uint8_t y = 0u; y < GBA_TILE_1D_SIZE; y++) {
        for (uint8_t x = 0u; x < GBA_TILE_1D_SIZE; x++) {
          STilePixelPair value =
              memory->vram.mode_012.bg.tiles.blocks[i].s_tiles->pixels[y][x];

          if (x % 2 == 0u) {
            context->staging[insert_index++] = value.first << 4u;
          } else {
            context->staging[insert_index++] = value.second << 4u;
          }
        }
      }
    }

    glBindTexture(GL_TEXTURE_2D, context->s_tiles);
    glTexSubImage2D(
        GL_TEXTURE_2D, /*level=*/0, /*xoffset=*/0,
        /*yoffset=*/GBA_TILE_1D_SIZE * GBA_TILE_MODE_TILE_BLOCK_NUM_S_TILES * i,
        /*width=*/GBA_TILE_1D_SIZE,
        /*height=*/GBA_TILE_1D_SIZE * GBA_TILE_MODE_TILE_BLOCK_NUM_S_TILES,
        /*format=*/GL_LUMINANCE, /*type=*/GL_UNSIGNED_BYTE,
        /*pixels=*/context->staging);

    glBindTexture(GL_TEXTURE_2D, context->d_tiles);
    glTexSubImage2D(
        GL_TEXTURE_2D, /*level=*/0, /*xoffset=*/0,
        /*yoffset=*/GBA_TILE_1D_SIZE * GBA_TILE_MODE_TILE_BLOCK_NUM_D_TILES * i,
        /*width=*/GBA_TILE_1D_SIZE,
        /*height=*/GBA_TILE_1D_SIZE * GBA_TILE_MODE_TILE_BLOCK_NUM_D_TILES,
        /*format=*/GL_LUMINANCE, /*type=*/GL_UNSIGNED_BYTE,
        /*pixels=*/memory->vram.mode_012.bg.tiles.blocks[i].d_tiles);
    glBindTexture(GL_TEXTURE_2D, 0);

    dirty_bits->vram.tile_mode.tiles[i] = false;
  }
}

void OpenGlBgTilesBind(const OpenGlBgTiles* context, GLuint program) {
  GLint bg_tiles_s = glGetUniformLocation(program, "bg_tiles_s");
  glUniform1i(bg_tiles_s, BG_TILES_S_TEXTURE);

  glActiveTexture(GL_TEXTURE0 + BG_TILES_S_TEXTURE);
  glBindTexture(GL_TEXTURE_2D, context->s_tiles);

  GLint bg_tiles_d = glGetUniformLocation(program, "bg_tiles_d");
  glUniform1i(bg_tiles_d, BG_TILES_D_TEXTURE);

  glActiveTexture(GL_TEXTURE0 + BG_TILES_D_TEXTURE);
  glBindTexture(GL_TEXTURE_2D, context->d_tiles);
}

void OpenGlBgTilesReloadContext(OpenGlBgTiles* context) {
  glGenTextures(1u, &context->s_tiles);
  glBindTexture(GL_TEXTURE_2D, context->s_tiles);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, /*level=*/0, /*internal_format=*/GL_LUMINANCE,
               /*width=*/GBA_TILE_1D_SIZE,
               /*height=*/GBA_TILE_1D_SIZE *
                   GBA_TILE_MODE_TILE_BLOCK_NUM_S_TILES *
                   GBA_TILE_MODE_NUM_BACKGROUND_TILE_BLOCKS,
               /*border=*/0, /*format=*/GL_LUMINANCE, /*type=*/GL_UNSIGNED_BYTE,
               /*pixels=*/NULL);

  glGenTextures(1u, &context->d_tiles);
  glBindTexture(GL_TEXTURE_2D, context->d_tiles);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, /*level=*/0, /*internal_format=*/GL_LUMINANCE,
               /*width=*/GBA_TILE_1D_SIZE,
               /*height=*/GBA_TILE_1D_SIZE *
                   GBA_TILE_MODE_TILE_BLOCK_NUM_D_TILES *
                   GBA_TILE_MODE_NUM_BACKGROUND_TILE_BLOCKS,
               /*border=*/0, /*format=*/GL_LUMINANCE,
               /*type=*/GL_UNSIGNED_BYTE,
               /*pixels=*/NULL);

  glBindTexture(GL_TEXTURE_2D, 0);
}

void OpenGlBgTilesDestroy(OpenGlBgTiles* context) {
  glDeleteTextures(1u, &context->s_tiles);
  glDeleteTextures(1u, &context->d_tiles);
}