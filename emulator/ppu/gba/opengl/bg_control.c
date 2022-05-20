#include "emulator/ppu/gba/opengl/bg_control.h"

#include <string.h>

void OpenGlBgControlReload(OpenGlBgControl* context,
                           const GbaPpuRegisters* registers,
                           GbaPpuDirtyBits* dirty_bits) {
  const static GLfloat tilemap_sizes[4u][2u] = {
      {256.0, 256.0}, {512.0, 256.0}, {256.0, 512.0}, {512.0, 512.0}};

  for (uint8_t i = 0u; i < GBA_PPU_NUM_BACKGROUNDS; i++) {
    if (!dirty_bits->io.bg_control[i]) {
      continue;
    }

    context->priority[i] = registers->bgcnt[i].priority;
    context->size[i][0u] = tilemap_sizes[registers->bgcnt[i].size][0u];
    context->size[i][1u] = tilemap_sizes[registers->bgcnt[i].size][1u];
    context->tilemap_base[i] =
        (GLfloat)registers->bgcnt[i].tile_map_base_block /
        (GLfloat)GBA_TILE_MODE_NUM_BACKGROUND_TILE_MAP_BLOCKS;
    context->tilemap_base[i] =
        (GLfloat)registers->bgcnt[i].tile_base_block /
        (GLfloat)GBA_TILE_MODE_NUM_BACKGROUND_TILE_BLOCKS;
    context->large_palette[i] = registers->bgcnt[i].large_palette;

    dirty_bits->io.bg_control[i] = false;
  }
}

void OpenGlBgControlBind(const OpenGlBgControl* context, GLuint program) {
  GLint bg0_priority = glGetUniformLocation(program, "bg0_priority");
  glUniform1i(bg0_priority, context->priority[0u]);

  GLint bg0_size = glGetUniformLocation(program, "bg0_size");
  glUniform2f(bg0_size, context->size[0u][0u], context->size[0u][1u]);

  GLint bg0_tilemap_base = glGetUniformLocation(program, "bg0_tilemap_base");
  glUniform1f(bg0_tilemap_base, context->tilemap_base[0u]);

  GLint bg0_tile_base = glGetUniformLocation(program, "bg0_tile_base");
  glUniform1f(bg0_tile_base, context->tile_base[0u]);

  GLint bg0_large_palette = glGetUniformLocation(program, "bg0_large_palette");
  glUniform1i(bg0_large_palette, context->large_palette[0u]);

  GLint bg1_priority = glGetUniformLocation(program, "bg1_priority");
  glUniform1i(bg1_priority, context->priority[1u]);

  GLint bg1_size = glGetUniformLocation(program, "bg1_size");
  glUniform2f(bg1_size, context->size[1u][0u], context->size[1u][1u]);

  GLint bg1_tilemap_base = glGetUniformLocation(program, "bg1_tilemap_base");
  glUniform1f(bg1_tilemap_base, context->tilemap_base[1u]);

  GLint bg1_tile_base = glGetUniformLocation(program, "bg1_tile_base");
  glUniform1f(bg1_tile_base, context->tile_base[1u]);

  GLint bg1_large_palette = glGetUniformLocation(program, "bg1_large_palette");
  glUniform1i(bg1_large_palette, context->large_palette[1u]);

  GLint bg2_priority = glGetUniformLocation(program, "bg2_priority");
  glUniform1i(bg2_priority, context->priority[2u]);

  GLint bg2_size = glGetUniformLocation(program, "bg2_size");
  glUniform2f(bg2_size, context->size[2u][0u], context->size[2u][1u]);

  GLint bg2_tilemap_base = glGetUniformLocation(program, "bg2_tilemap_base");
  glUniform1f(bg2_tilemap_base, context->tilemap_base[2u]);

  GLint bg2_tile_base = glGetUniformLocation(program, "bg2_tile_base");
  glUniform1f(bg2_tile_base, context->tile_base[2u]);

  GLint bg2_large_palette = glGetUniformLocation(program, "bg2_large_palette");
  glUniform1i(bg2_large_palette, context->large_palette[2u]);

  GLint bg3_priority = glGetUniformLocation(program, "bg3_priority");
  glUniform1i(bg3_priority, context->priority[3u]);

  GLint bg3_size = glGetUniformLocation(program, "bg3_size");
  glUniform2f(bg3_size, context->size[3u][0u], context->size[3u][1u]);

  GLint bg3_tilemap_base = glGetUniformLocation(program, "bg3_tilemap_base");
  glUniform1f(bg3_tilemap_base, context->tilemap_base[3u]);

  GLint bg3_tile_base = glGetUniformLocation(program, "bg3_tile_base");
  glUniform1f(bg3_tile_base, context->tile_base[3u]);

  GLint bg3_large_palette = glGetUniformLocation(program, "bg3_large_palette");
  glUniform1i(bg3_large_palette, context->large_palette[3u]);
}