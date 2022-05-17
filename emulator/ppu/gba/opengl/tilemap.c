#include "emulator/ppu/gba/opengl/tilemap.h"

#include <string.h>

#include "emulator/ppu/gba/opengl/texture_bindings.h"

void OpenGlBgTilemapReload(OpenGlBgTilemap* context, const GbaPpuMemory* memory,
                           GbaPpuDirtyBits* dirty_bits) {
  for (uint8_t i = 0u; i < GBA_TILE_MODE_NUM_BACKGROUND_TILE_MAP_BLOCKS; i++) {
    if (!dirty_bits->vram.tile_mode.scrolling_tilemap[i]) {
      continue;
    }

    uint16_t indices[GBA_TILE_MAP_BLOCK_1D_SIZE][GBA_TILE_MAP_BLOCK_1D_SIZE];
    uint16_t params[GBA_TILE_MAP_BLOCK_1D_SIZE][GBA_TILE_MAP_BLOCK_1D_SIZE];
    for (uint8_t y = 0u; y < GBA_TILE_MAP_BLOCK_1D_SIZE; y++) {
      for (uint8_t x = 0u; x < GBA_TILE_MAP_BLOCK_1D_SIZE; x++) {
        indices[y][x] =
            memory->vram.mode_012.bg.tile_map.blocks[i].entries[y][x].index
            << 6u;

        uint16_t r = 0u;
        if (memory->vram.mode_012.bg.tile_map.blocks[i].entries[y][x].h_flip) {
          r = 0xF800u;
        }

        uint16_t g = 0u;
        if (memory->vram.mode_012.bg.tile_map.blocks[i].entries[y][x].v_flip) {
          g = 0x07E0;
        }

        uint16_t b = (uint16_t)memory->vram.mode_012.bg.tile_map.blocks[i]
                         .entries[y][x]
                         .palette
                     << 1u;

        params[y][x] = r | g | b;
      }
    }

    glBindTexture(GL_TEXTURE_2D, context->indices);
    glTexSubImage2D(GL_TEXTURE_2D, /*level=*/0, /*xoffset=*/0,
                    /*yoffset=*/GBA_TILE_MAP_BLOCK_1D_SIZE * i,
                    /*width=*/GBA_TILE_MAP_BLOCK_1D_SIZE,
                    /*height=*/GBA_TILE_MAP_BLOCK_1D_SIZE,
                    /*format=*/GL_LUMINANCE_ALPHA, /*type=*/GL_UNSIGNED_BYTE,
                    /*pixels=*/indices);
    glBindTexture(GL_TEXTURE_2D, context->params);
    glTexSubImage2D(GL_TEXTURE_2D, /*level=*/0, /*xoffset=*/0,
                    /*yoffset=*/GBA_TILE_MAP_BLOCK_1D_SIZE * i,
                    /*width=*/GBA_TILE_MAP_BLOCK_1D_SIZE,
                    /*height=*/GBA_TILE_MAP_BLOCK_1D_SIZE,
                    /*format=*/GL_RGB, /*type=*/GL_UNSIGNED_SHORT_5_6_5,
                    /*pixels=*/params);
    glBindTexture(GL_TEXTURE_2D, 0);

    dirty_bits->vram.tile_mode.scrolling_tilemap[i] = false;
  }
}

void OpenGlBgTilemapBind(const OpenGlBgTilemap* context, GLuint program) {
  GLint bg_scrolling_tilemap_indices =
      glGetUniformLocation(program, "bg_scrolling_tilemap_indices");
  glUniform1i(bg_scrolling_tilemap_indices,
              BG_SCROLLING_TILEMAP_INDICES_TEXTURE);

  glActiveTexture(GL_TEXTURE0 + BG_SCROLLING_TILEMAP_INDICES_TEXTURE);
  glBindTexture(GL_TEXTURE_2D, context->indices);

  GLint bg_scrolling_tilemap_params =
      glGetUniformLocation(program, "bg_scrolling_tilemap_params");
  glUniform1i(bg_scrolling_tilemap_params, BG_SCROLLING_TILEMAP_PARAMS_TEXTURE);

  glActiveTexture(GL_TEXTURE0 + BG_SCROLLING_TILEMAP_PARAMS_TEXTURE);
  glBindTexture(GL_TEXTURE_2D, context->params);
}

void OpenGlBgTilemapReloadContext(OpenGlBgTilemap* context) {
  glGenTextures(1u, &context->indices);
  glBindTexture(GL_TEXTURE_2D, context->indices);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, /*level=*/0,
               /*internal_format=*/GL_LUMINANCE_ALPHA,
               /*width=*/GBA_TILE_MAP_BLOCK_1D_SIZE,
               /*height=*/GBA_TILE_MAP_BLOCK_1D_SIZE *
                   GBA_TILE_MODE_NUM_BACKGROUND_TILE_MAP_BLOCKS,
               /*border=*/0, /*format=*/GL_LUMINANCE_ALPHA,
               /*type=*/GL_UNSIGNED_BYTE,
               /*pixels=*/NULL);

  glGenTextures(1u, &context->params);
  glBindTexture(GL_TEXTURE_2D, context->params);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, /*level=*/0, /*internal_format=*/GL_RGB,
               /*width=*/GBA_TILE_MAP_BLOCK_1D_SIZE,
               /*height=*/GBA_TILE_MAP_BLOCK_1D_SIZE *
                   GBA_TILE_MODE_NUM_BACKGROUND_TILE_MAP_BLOCKS,
               /*border=*/0, /*format=*/GL_RGB,
               /*type=*/GL_UNSIGNED_SHORT_5_6_5,
               /*pixels=*/NULL);

  glBindTexture(GL_TEXTURE_2D, 0);
}

void OpenGlBgTilemapDestroy(OpenGlBgTilemap* context) {
  glDeleteTextures(1u, &context->indices);
  glDeleteTextures(1u, &context->params);
}