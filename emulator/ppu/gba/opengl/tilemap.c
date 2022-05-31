#include "emulator/ppu/gba/opengl/tilemap.h"

#include <string.h>

#include "emulator/ppu/gba/opengl/texture_bindings.h"

void OpenGlBgTilemapReload(OpenGlBgTilemap* context, const GbaPpuMemory* memory,
                           GbaPpuDirtyBits* dirty_bits) {
  for (uint32_t i = 0u; i < GBA_TILE_MODE_NUM_BACKGROUND_TILE_MAP_BLOCKS; i++) {
    if (dirty_bits->vram.tile_mode.affine_tilemap[i]) {
      glBindTexture(GL_TEXTURE_2D, context->affine);
      glTexSubImage2D(
          GL_TEXTURE_2D, /*level=*/0, /*xoffset=*/0, /*yoffset=*/8 * i,
          /*width=*/256, /*height=*/8, /*format=*/GL_RED_INTEGER,
          /*type=*/GL_UNSIGNED_BYTE,
          /*pixels=*/memory->vram.mode_012.bg.tile_map.blocks[i].indices);

      dirty_bits->vram.tile_mode.affine_tilemap[i] = false;
    }

    if (dirty_bits->vram.tile_mode.scrolling_tilemap[i]) {
      uint16_t staging[GBA_TILE_MAP_BLOCK_1D_SIZE][GBA_TILE_MAP_BLOCK_1D_SIZE]
                      [4u];
      for (uint8_t y = 0u; y < GBA_TILE_MAP_BLOCK_1D_SIZE; y++) {
        for (uint8_t x = 0u; x < GBA_TILE_MAP_BLOCK_1D_SIZE; x++) {
          staging[y][x][0u] =
              memory->vram.mode_012.bg.tile_map.blocks[i].entries[y][x].index;

          uint16_t h_flip = 0u;
          if (memory->vram.mode_012.bg.tile_map.blocks[i]
                  .entries[y][x]
                  .h_flip) {
            h_flip = 7u;
          }

          staging[y][x][1u] = h_flip;

          uint16_t v_flip = 0u;
          if (memory->vram.mode_012.bg.tile_map.blocks[i]
                  .entries[y][x]
                  .v_flip) {
            v_flip = 7u;
          }

          staging[y][x][2u] = v_flip;

          staging[y][x][3u] = memory->vram.mode_012.bg.tile_map.blocks[i]
                                  .entries[y][x]
                                  .palette *
                              16;
        }
      }

      glBindTexture(GL_TEXTURE_2D, context->scrolling);
      glTexSubImage2D(GL_TEXTURE_2D, /*level=*/0, /*xoffset=*/0,
                      /*yoffset=*/GBA_TILE_MAP_BLOCK_1D_SIZE * i,
                      /*width=*/GBA_TILE_MAP_BLOCK_1D_SIZE,
                      /*height=*/GBA_TILE_MAP_BLOCK_1D_SIZE,
                      /*format=*/GL_RGBA_INTEGER, /*type=*/GL_UNSIGNED_SHORT,
                      /*pixels=*/staging);

      dirty_bits->vram.tile_mode.scrolling_tilemap[i] = false;
    }
  }

  glBindTexture(GL_TEXTURE_2D, 0);
}

void OpenGlBgTilemapBind(const OpenGlBgTilemap* context, GLuint program) {
  GLint bg_affine_tilemap = glGetUniformLocation(program, "bg_affine_tilemap");
  glUniform1i(bg_affine_tilemap, BG_AFFINE_TILEMAP_TEXTURE);

  glActiveTexture(GL_TEXTURE0 + BG_AFFINE_TILEMAP_TEXTURE);
  glBindTexture(GL_TEXTURE_2D, context->affine);

  GLint bg_scrolling_tilemap =
      glGetUniformLocation(program, "bg_scrolling_tilemap");
  glUniform1i(bg_scrolling_tilemap, BG_SCROLLING_TILEMAP_TEXTURE);

  glActiveTexture(GL_TEXTURE0 + BG_SCROLLING_TILEMAP_TEXTURE);
  glBindTexture(GL_TEXTURE_2D, context->scrolling);
}

void OpenGlBgTilemapReloadContext(OpenGlBgTilemap* context) {
  glGenTextures(1u, &context->affine);
  glBindTexture(GL_TEXTURE_2D, context->affine);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, /*level=*/0, /*internal_format=*/GL_R8UI,
               /*width=*/256, /*height=*/256, /*border=*/0,
               /*format=*/GL_RED_INTEGER, /*type=*/GL_UNSIGNED_BYTE,
               /*pixels=*/NULL);

  glGenTextures(1u, &context->scrolling);
  glBindTexture(GL_TEXTURE_2D, context->scrolling);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, /*level=*/0,
               /*internal_format=*/GL_RGBA16UI,
               /*width=*/GBA_TILE_MAP_BLOCK_1D_SIZE,
               /*height=*/GBA_TILE_MAP_BLOCK_1D_SIZE *
                   GBA_TILE_MODE_NUM_BACKGROUND_TILE_MAP_BLOCKS,
               /*border=*/0, /*format=*/GL_RGBA_INTEGER,
               /*type=*/GL_UNSIGNED_SHORT, /*pixels=*/NULL);

  glBindTexture(GL_TEXTURE_2D, 0);
}

void OpenGlBgTilemapDestroy(OpenGlBgTilemap* context) {
  glDeleteTextures(1u, &context->affine);
  glDeleteTextures(1u, &context->scrolling);
}