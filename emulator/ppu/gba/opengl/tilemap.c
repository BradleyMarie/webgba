#include "emulator/ppu/gba/opengl/tilemap.h"

#include "emulator/ppu/gba/opengl/texture_bindings.h"

bool OpenGlBgTilemapStage(OpenGlBgTilemap* context, const GbaPpuMemory* memory,
                          const GbaPpuRegisters* registers,
                          GbaPpuDirtyBits* dirty_bits) {
  bool result = false;
  for (uint32_t i = 0u; i < GBA_TILE_MODE_NUM_BACKGROUND_TILE_MAP_BLOCKS; i++) {
    if (dirty_bits->vram.affine_tilemap[i] &&
        (registers->dispcnt.mode == 1u || registers->dispcnt.mode == 2u)) {
      for (uint16_t j = 0u; j < GBA_AFFINE_TILE_MAP_INDICES_PER_BLOCK; j++) {
        context->staging_affine[i][j] =
            memory->vram.mode_012.bg.tile_map.blocks[i].indices[j];
      }

      dirty_bits->vram.affine_tilemap[i] = false;
      context->affine_dirty[i] = true;
      result = true;
    }

    if (dirty_bits->vram.scrolling_tilemap[i] &&
        registers->dispcnt.mode <= 1u) {
      for (uint8_t y = 0u; y < GBA_TILE_MAP_BLOCK_1D_SIZE; y++) {
        for (uint8_t x = 0u; x < GBA_TILE_MAP_BLOCK_1D_SIZE; x++) {
          context->staging_scrolling[i][y][x][0u] =
              memory->vram.mode_012.bg.tile_map.blocks[i].entries[y][x].index;

          uint16_t h_flip = 0u;
          if (memory->vram.mode_012.bg.tile_map.blocks[i]
                  .entries[y][x]
                  .h_flip) {
            h_flip = 7u;
          }

          context->staging_scrolling[i][y][x][1u] = h_flip;

          uint16_t v_flip = 0u;
          if (memory->vram.mode_012.bg.tile_map.blocks[i]
                  .entries[y][x]
                  .v_flip) {
            v_flip = 7u;
          }

          context->staging_scrolling[i][y][x][2u] = v_flip;

          context->staging_scrolling[i][y][x][3u] =
              memory->vram.mode_012.bg.tile_map.blocks[i]
                  .entries[y][x]
                  .palette *
              16;
        }
      }

      dirty_bits->vram.scrolling_tilemap[i] = false;
      context->scrolling_dirty[i] = true;
      result = true;
    }
  }

  return result;
}

void OpenGlBgTilemapBind(const OpenGlBgTilemap* context, GLuint program) {
  GLint bg_affine_tilemap = glGetUniformLocation(program, "affine_tilemap");
  glUniform1i(bg_affine_tilemap, BG_AFFINE_TILEMAP_TEXTURE);

  glActiveTexture(GL_TEXTURE0 + BG_AFFINE_TILEMAP_TEXTURE);
  glBindTexture(GL_TEXTURE_2D, context->affine);

  GLint bg_scrolling_tilemap =
      glGetUniformLocation(program, "scrolling_tilemap");
  glUniform1i(bg_scrolling_tilemap, BG_SCROLLING_TILEMAP_TEXTURE);

  glActiveTexture(GL_TEXTURE0 + BG_SCROLLING_TILEMAP_TEXTURE);
  glBindTexture(GL_TEXTURE_2D, context->scrolling);
}

void OpenGlBgTilemapReload(OpenGlBgTilemap* context) {
  for (uint8_t i = 0; i < GBA_TILE_MODE_NUM_BACKGROUND_TILE_MAP_BLOCKS; i++) {
    if (context->affine_dirty[i]) {
      glBindTexture(GL_TEXTURE_2D, context->affine);
      glTexSubImage2D(GL_TEXTURE_2D, /*level=*/0, /*xoffset=*/0,
                      /*yoffset=*/8 * i,
                      /*width=*/256, /*height=*/8, /*format=*/GL_RED_INTEGER,
                      /*type=*/GL_UNSIGNED_BYTE,
                      /*pixels=*/context->staging_affine[i]);
      glBindTexture(GL_TEXTURE_2D, 0);
      context->affine_dirty[i] = false;
    }

    if (context->scrolling_dirty[i]) {
      glBindTexture(GL_TEXTURE_2D, context->scrolling);
      glTexSubImage2D(GL_TEXTURE_2D, /*level=*/0, /*xoffset=*/0,
                      /*yoffset=*/GBA_TILE_MAP_BLOCK_1D_SIZE * i,
                      /*width=*/GBA_TILE_MAP_BLOCK_1D_SIZE,
                      /*height=*/GBA_TILE_MAP_BLOCK_1D_SIZE,
                      /*format=*/GL_RGBA_INTEGER, /*type=*/GL_UNSIGNED_SHORT,
                      /*pixels=*/context->staging_scrolling[i]);
      glBindTexture(GL_TEXTURE_2D, 0);
      context->scrolling_dirty[i] = false;
    }
  }
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
               /*pixels=*/context->staging_affine);

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
               /*type=*/GL_UNSIGNED_SHORT,
               /*pixels=*/context->staging_scrolling);

  glBindTexture(GL_TEXTURE_2D, 0);
}

void OpenGlBgTilemapDestroy(OpenGlBgTilemap* context) {
  glDeleteTextures(1u, &context->affine);
  glDeleteTextures(1u, &context->scrolling);
}