#include "emulator/ppu/gba/opengl/bg_control.h"

#include <string.h>

#include "emulator/ppu/gba/opengl/texture_bindings.h"

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
    context->staging[i].size[0u] =
        tilemap_sizes[tilemap_index[registers->dispcnt.mode][i]]
                     [registers->bgcnt[i].size][0u];
    context->staging[i].size[1u] =
        tilemap_sizes[tilemap_index[registers->dispcnt.mode][i]]
                     [registers->bgcnt[i].size][1u];
    context->staging[i].priority = registers->bgcnt[i].priority;
    context->staging[i].tilemap_base =
        registers->bgcnt[i].tile_map_base_block * GBA_TILE_MAP_BLOCK_1D_SIZE *
        GBA_TILE_MAP_BLOCK_1D_SIZE * sizeof(TileMapEntry);
    context->staging[i].tile_base = registers->bgcnt[i].tile_base_block *
                                    GBA_TILE_MODE_TILE_BLOCK_NUM_S_TILES *
                                    GBA_TILE_1D_SIZE;
    context->staging[i].large_palette = registers->bgcnt[i].large_palette;
    context->staging[i].wraparound = registers->bgcnt[i].wraparound;

    if (registers->bgcnt[i].mosaic) {
      context->staging[i].mosaic[0u] = registers->mosaic.bg_horiz + 1;
      context->staging[i].mosaic[1u] = registers->mosaic.bg_vert + 1;
    } else {
      context->staging[i].mosaic[0u] = 1;
      context->staging[i].mosaic[1u] = 1;
    }

    dirty_bits->io.bg_control[i] = false;
  }

  if (registers->dispcnt.mode == 3u) {
    context->staging[2u].size[0] = GBA_SCREEN_WIDTH;
    context->staging[2u].size[1] = GBA_SCREEN_WIDTH;
  } else if (registers->dispcnt.mode == 4u) {
    context->staging[2u].size[0] = GBA_SCREEN_WIDTH;
    context->staging[2u].size[1] = GBA_SCREEN_WIDTH;
  } else if (registers->dispcnt.mode == 5u) {
    context->staging[2u].size[0] = GBA_REDUCED_FRAME_WIDTH;
    context->staging[2u].size[1] = GBA_REDUCED_FRAME_HEIGHT;
  }

  glBindBuffer(GL_UNIFORM_BUFFER, context->buffer);
  glBufferSubData(GL_UNIFORM_BUFFER, /*offset=*/0,
                  /*size=*/sizeof(context->staging),
                  /*data=*/&context->staging);

  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  dirty_bits->composite.bg_mosaic = false;
}

void OpenGlBgControlBind(const OpenGlBgControl* context, GLuint program) {
  GLint backgrounds = glGetUniformBlockIndex(program, "Backgrounds");
  glUniformBlockBinding(program, backgrounds, BACKGROUNDS_BUFFER);

  glBindBuffer(GL_UNIFORM_BUFFER, context->buffer);
  glBindBufferBase(GL_UNIFORM_BUFFER, BACKGROUNDS_BUFFER, context->buffer);
}

void OpenGlBgControlReloadContext(OpenGlBgControl* context) {
  glGenBuffers(1, &context->buffer);
  glBindBuffer(GL_UNIFORM_BUFFER, context->buffer);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(context->staging), NULL,
               GL_DYNAMIC_DRAW);

  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void OpenGlBgControlDestroy(OpenGlBgControl* context) {
  glDeleteBuffers(1, &context->buffer);
}