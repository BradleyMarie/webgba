#include "emulator/ppu/gba/opengl/tiles.h"

#include <string.h>

#include "emulator/ppu/gba/opengl/texture_bindings.h"

void OpenGlTilesReload(OpenGlTiles* context, const GbaPpuMemory* memory,
                       const GbaPpuRegisters* registers,
                       GbaPpuDirtyBits* dirty_bits) {
  for (uint8_t i = 0u; i < GBA_TILE_MODE_NUM_BACKGROUND_TILE_BLOCKS; i++) {
    if (!dirty_bits->vram.tile_mode.tiles[i] || registers->dispcnt.mode >= 3) {
      continue;
    }

    uint32_t insert_index = 0u;
    for (uint16_t t = 0u; t < GBA_TILE_MODE_TILE_BLOCK_NUM_S_TILES; t++) {
      for (uint8_t y = 0u; y < GBA_TILE_1D_SIZE; y++) {
        for (uint8_t x = 0u; x < GBA_TILE_1D_SIZE / 2u; x++) {
          STilePixelPair value =
              memory->vram.mode_012.bg.tiles.blocks[i].s_tiles[t].pixels[y][x];
          context->staging[insert_index++] = value.first << 4u;
          context->staging[insert_index++] = value.second << 4u;
        }
      }
    }

    glBindTexture(GL_TEXTURE_2D, context->bg_s_tiles);
    glTexSubImage2D(
        GL_TEXTURE_2D, /*level=*/0, /*xoffset=*/0,
        /*yoffset=*/GBA_TILE_1D_SIZE * GBA_TILE_MODE_TILE_BLOCK_NUM_S_TILES * i,
        /*width=*/GBA_TILE_1D_SIZE,
        /*height=*/GBA_TILE_1D_SIZE * GBA_TILE_MODE_TILE_BLOCK_NUM_S_TILES,
        /*format=*/GL_LUMINANCE, /*type=*/GL_UNSIGNED_BYTE,
        /*pixels=*/context->staging);

    glBindTexture(GL_TEXTURE_2D, context->bg_d_tiles);
    glTexSubImage2D(
        GL_TEXTURE_2D, /*level=*/0, /*xoffset=*/0,
        /*yoffset=*/GBA_TILE_1D_SIZE * GBA_TILE_MODE_TILE_BLOCK_NUM_D_TILES * i,
        /*width=*/GBA_TILE_1D_SIZE,
        /*height=*/GBA_TILE_1D_SIZE * GBA_TILE_MODE_TILE_BLOCK_NUM_D_TILES,
        /*format=*/GL_LUMINANCE, /*type=*/GL_UNSIGNED_BYTE,
        /*pixels=*/memory->vram.mode_012.bg.tiles.blocks[i].d_tiles);

    dirty_bits->vram.tile_mode.tiles[i] = false;
  }

  uint8_t starting_tile = (registers->dispcnt.mode < 3) ? 0u : 1u;
  for (uint8_t i = starting_tile; i < 2; i++) {
    if (!dirty_bits->vram.obj_tilemode_tiles[i]) {
      continue;
    }

    uint32_t insert_index = 0u;
    uint16_t s_tile_base = GBA_BITMAP_MODE_NUM_OBJECT_S_TILES * i;
    uint16_t d_tile_base = GBA_BITMAP_MODE_NUM_OBJECT_D_TILES * i;
    for (uint16_t t = 0u; t < GBA_BITMAP_MODE_NUM_OBJECT_S_TILES; t++) {
      for (uint8_t y = 0u; y < GBA_TILE_1D_SIZE; y++) {
        for (uint8_t x = 0u; x < GBA_TILE_1D_SIZE / 2u; x++) {
          STilePixelPair value =
              memory->vram.mode_012.obj.s_tiles[s_tile_base + t].pixels[y][x];
          context->staging[insert_index++] = value.first << 4u;
          context->staging[insert_index++] = value.second << 4u;
        }
      }
    }

    glBindTexture(GL_TEXTURE_2D, context->obj_s_tiles);
    glTexSubImage2D(
        GL_TEXTURE_2D, /*level=*/0, /*xoffset=*/0,
        /*yoffset=*/GBA_TILE_1D_SIZE * s_tile_base,
        /*width=*/GBA_TILE_1D_SIZE,
        /*height=*/GBA_TILE_1D_SIZE * GBA_BITMAP_MODE_NUM_OBJECT_S_TILES,
        /*format=*/GL_LUMINANCE, /*type=*/GL_UNSIGNED_BYTE,
        /*pixels=*/context->staging);

    glBindTexture(GL_TEXTURE_2D, context->obj_d_tiles);
    glTexSubImage2D(
        GL_TEXTURE_2D, /*level=*/0, /*xoffset=*/0,
        /*yoffset=*/GBA_TILE_1D_SIZE * GBA_BITMAP_MODE_NUM_OBJECT_D_TILES * i,
        /*width=*/GBA_TILE_1D_SIZE,
        /*height=*/GBA_TILE_1D_SIZE * GBA_BITMAP_MODE_NUM_OBJECT_S_TILES,
        /*format=*/GL_LUMINANCE, /*type=*/GL_UNSIGNED_BYTE,
        /*pixels=*/&memory->vram.mode_012.obj.d_tiles[d_tile_base]);
    glBindTexture(GL_TEXTURE_2D, 0);

    dirty_bits->vram.obj_tilemode_tiles[i] = false;
  }
}

void OpenGlTilesBind(const OpenGlTiles* context, GLuint program) {
  GLint bg_tiles_s = glGetUniformLocation(program, "bg_tiles_s");
  glUniform1i(bg_tiles_s, BG_TILES_S_TEXTURE);

  glActiveTexture(GL_TEXTURE0 + BG_TILES_S_TEXTURE);
  glBindTexture(GL_TEXTURE_2D, context->bg_s_tiles);

  GLint bg_tiles_d = glGetUniformLocation(program, "bg_tiles_d");
  glUniform1i(bg_tiles_d, BG_TILES_D_TEXTURE);

  glActiveTexture(GL_TEXTURE0 + BG_TILES_D_TEXTURE);
  glBindTexture(GL_TEXTURE_2D, context->bg_d_tiles);

  GLint obj_tiles_s = glGetUniformLocation(program, "obj_tiles_s");
  glUniform1i(obj_tiles_s, OBJ_TILES_S_TEXTURE);

  glActiveTexture(GL_TEXTURE0 + OBJ_TILES_S_TEXTURE);
  glBindTexture(GL_TEXTURE_2D, context->obj_s_tiles);

  GLint obj_tiles_d = glGetUniformLocation(program, "obj_tiles_d");
  glUniform1i(obj_tiles_d, OBJ_TILES_D_TEXTURE);

  glActiveTexture(GL_TEXTURE0 + OBJ_TILES_D_TEXTURE);
  glBindTexture(GL_TEXTURE_2D, context->obj_d_tiles);
}

void OpenGlTilesReloadContext(OpenGlTiles* context) {
  glGenTextures(1u, &context->bg_s_tiles);
  glBindTexture(GL_TEXTURE_2D, context->bg_s_tiles);
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

  glGenTextures(1u, &context->bg_d_tiles);
  glBindTexture(GL_TEXTURE_2D, context->bg_d_tiles);
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

  glGenTextures(1u, &context->obj_s_tiles);
  glBindTexture(GL_TEXTURE_2D, context->obj_s_tiles);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, /*level=*/0, /*internal_format=*/GL_LUMINANCE,
               /*width=*/GBA_TILE_1D_SIZE,
               /*height=*/GBA_TILE_1D_SIZE * GBA_TILE_MODE_NUM_OBJECT_S_TILES,
               /*border=*/0, /*format=*/GL_LUMINANCE, /*type=*/GL_UNSIGNED_BYTE,
               /*pixels=*/NULL);

  glGenTextures(1u, &context->obj_d_tiles);
  glBindTexture(GL_TEXTURE_2D, context->obj_d_tiles);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, /*level=*/0, /*internal_format=*/GL_LUMINANCE,
               /*width=*/GBA_TILE_1D_SIZE,
               /*height=*/GBA_TILE_1D_SIZE * GBA_TILE_MODE_NUM_OBJECT_D_TILES,
               /*border=*/0, /*format=*/GL_LUMINANCE,
               /*type=*/GL_UNSIGNED_BYTE,
               /*pixels=*/NULL);

  glBindTexture(GL_TEXTURE_2D, 0);
}

void OpenGlTilesDestroy(OpenGlTiles* context) {
  glDeleteTextures(1u, &context->bg_s_tiles);
  glDeleteTextures(1u, &context->bg_d_tiles);
}