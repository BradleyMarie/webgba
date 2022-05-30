#include "emulator/ppu/gba/opengl/bg_control.h"

#include <stdio.h>
#include <string.h>

void OpenGlBgControlReload(OpenGlBgControl* context,
                           const GbaPpuRegisters* registers,
                           GbaPpuDirtyBits* dirty_bits) {
  static const GLint tilemap_sizes[2u][4u][2u] = {
      {{256, 256}, {512, 256}, {256, 512}, {512, 512}},
      {{128, 128}, {256, 256}, {512, 512}, {1024, 1024}}};

  static const bool tilemap_index[6u][4u] = {
      {false, false, false, false}, {false, false, true, true},
      {true, true, true, true},     {false, false, false, false},
      {false, false, false, false}, {false, false, false, false},
  };

  for (uint8_t i = 0u; i < GBA_PPU_NUM_BACKGROUNDS; i++) {
    context->size[i][0u] =
        tilemap_sizes[tilemap_index[registers->dispcnt.mode][i]]
                     [registers->bgcnt[i].size][0u];
    context->size[i][1u] =
        tilemap_sizes[tilemap_index[registers->dispcnt.mode][i]]
                     [registers->bgcnt[i].size][1u];

    if (!dirty_bits->io.bg_control[i]) {
      continue;
    }

    context->priority[i] = registers->bgcnt[i].priority;
    context->tilemap_base[i] =
        registers->bgcnt[i].tile_map_base_block * GBA_TILE_MAP_BLOCK_1D_SIZE *
        GBA_TILE_MAP_BLOCK_1D_SIZE * sizeof(TileMapEntry);
    context->tile_base[i] = registers->bgcnt[i].tile_base_block *
                            GBA_TILE_MODE_TILE_BLOCK_NUM_S_TILES *
                            GBA_TILE_1D_SIZE;
    context->large_palette[i] = registers->bgcnt[i].large_palette;
    context->wraparound[i] = registers->bgcnt[i].wraparound;

    dirty_bits->io.bg_control[i] = false;
  }
}

void OpenGlBgControlBind(const OpenGlBgControl* context, GLuint program) {
  for (uint8_t i = 0u; i < GBA_PPU_NUM_BACKGROUNDS; i++) {
    char variable_name[100u];
    sprintf(variable_name, "bg_cnt[%u].size", i);
    GLint size = glGetUniformLocation(program, variable_name);
    glUniform2i(size, context->size[i][0u], context->size[i][1u]);

    sprintf(variable_name, "bg_cnt[%u].tilemap_base", i);
    GLint tilemap_base = glGetUniformLocation(program, variable_name);
    glUniform1i(tilemap_base, context->tilemap_base[i]);

    sprintf(variable_name, "bg_cnt[%u].tile_base", i);
    GLint tile_base = glGetUniformLocation(program, variable_name);
    glUniform1i(tile_base, context->tile_base[i]);

    sprintf(variable_name, "bg_cnt[%u].priority", i);
    GLint priority = glGetUniformLocation(program, variable_name);
    glUniform1ui(priority, context->priority[i]);

    sprintf(variable_name, "bg_cnt[%u].large_palette", i);
    GLint large_palette = glGetUniformLocation(program, variable_name);
    glUniform1i(large_palette, context->large_palette[i]);

    sprintf(variable_name, "bg_cnt[%u].wraparound", i);
    GLint wraparound = glGetUniformLocation(program, variable_name);
    glUniform1i(wraparound, context->wraparound[i]);
  }
}