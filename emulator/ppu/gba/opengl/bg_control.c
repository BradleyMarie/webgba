#include "emulator/ppu/gba/opengl/bg_control.h"

#include <assert.h>

#include "emulator/ppu/gba/opengl/texture_bindings.h"

static void UpdateBackgroundSize(OpenGlBgControl* context,
                                 const GbaPpuRegisters* registers,
                                 uint8_t layer) {
  assert(layer < GBA_PPU_NUM_BACKGROUNDS);

  static const GLint scrolling_sizes[4u][2u] = {
      {256, 256}, {512, 256}, {256, 512}, {512, 512}};
  static const GLint affine_sizes[4u][2u] = {
      {128, 128}, {256, 256}, {512, 512}, {1024, 1024}};

  GLint new_size[2u];
  if (registers->dispcnt.mode == 0u) {
    new_size[0u] = scrolling_sizes[registers->bgcnt[layer].size][0u];
    new_size[1u] = scrolling_sizes[registers->bgcnt[layer].size][1u];
  } else if (registers->dispcnt.mode == 1u) {
    assert(layer != 3u);
    if (layer == 2u) {
      new_size[0u] = affine_sizes[registers->bgcnt[layer].size][0u];
      new_size[1u] = affine_sizes[registers->bgcnt[layer].size][1u];
    } else {
      new_size[0u] = scrolling_sizes[registers->bgcnt[layer].size][0u];
      new_size[1u] = scrolling_sizes[registers->bgcnt[layer].size][1u];
    }
  } else if (registers->dispcnt.mode == 2u) {
    assert(layer == 2u || layer == 3u);
    new_size[0u] = affine_sizes[registers->bgcnt[layer].size][0u];
    new_size[1u] = affine_sizes[registers->bgcnt[layer].size][1u];
  } else if (registers->dispcnt.mode == 3u) {
    new_size[0] = GBA_SCREEN_WIDTH;
    new_size[1] = GBA_SCREEN_HEIGHT;
  } else if (registers->dispcnt.mode == 4u) {
    new_size[0] = GBA_SCREEN_WIDTH;
    new_size[1] = GBA_SCREEN_HEIGHT;
  } else {
    assert(registers->dispcnt.mode == 5u);
    new_size[0] = GBA_REDUCED_FRAME_WIDTH;
    new_size[1] = GBA_REDUCED_FRAME_HEIGHT;
  }

  if (new_size[0u] != context->staging[layer].size[0u]) {
    context->dirty = true;
  }

  context->staging[layer].size[0u] = new_size[0u];

  if (new_size[1u] != context->staging[layer].size[1u]) {
    context->dirty = true;
  }

  context->staging[layer].size[1u] = new_size[1u];
}

bool OpenGlBgControlStage(OpenGlBgControl* context,
                          const GbaPpuRegisters* registers,
                          GbaPpuDirtyBits* dirty_bits) {
  for (uint8_t i = 0u; i < GBA_PPU_NUM_BACKGROUNDS; i++) {
    if (i == 0u &&
        (!registers->dispcnt.bg0_enable || registers->dispcnt.mode > 1u)) {
      continue;
    }

    if (i == 1u &&
        (!registers->dispcnt.bg1_enable || registers->dispcnt.mode > 1u)) {
      continue;
    }

    if (i == 2u && !registers->dispcnt.bg2_enable) {
      continue;
    }

    if (i == 3u &&
        (!registers->dispcnt.bg1_enable ||
         (registers->dispcnt.mode != 0u && registers->dispcnt.mode != 2u))) {
      continue;
    }

    UpdateBackgroundSize(context, registers, i);

    if (!dirty_bits->io.bg_control[i] &&
        (!registers->bgcnt[i].mosaic || !dirty_bits->io.bg_mosaic)) {
      continue;
    }

    context->staging[i].priority = registers->bgcnt[i].priority;
    context->staging[i].tilemap_base = registers->bgcnt[i].tile_map_base_block;
    context->staging[i].tile_base = registers->bgcnt[i].tile_base_block;
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
    context->dirty = true;
  }

  dirty_bits->io.bg_mosaic = false;

  return context->dirty;
}

void OpenGlBgControlBind(const OpenGlBgControl* context, GLuint program) {
  GLint backgrounds = glGetUniformBlockIndex(program, "Backgrounds");
  glUniformBlockBinding(program, backgrounds, BACKGROUNDS_BUFFER);

  glBindBuffer(GL_UNIFORM_BUFFER, context->buffer);
  glBindBufferBase(GL_UNIFORM_BUFFER, BACKGROUNDS_BUFFER, context->buffer);
}

void OpenGlBgControlReload(OpenGlBgControl* context) {
  if (context->dirty) {
    glBindBuffer(GL_UNIFORM_BUFFER, context->buffer);
    glBufferSubData(GL_UNIFORM_BUFFER, /*offset=*/0,
                    /*size=*/sizeof(context->staging),
                    /*data=*/&context->staging);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    context->dirty = false;
  }
}

void OpenGlBgControlReloadContext(OpenGlBgControl* context) {
  glGenBuffers(1, &context->buffer);
  glBindBuffer(GL_UNIFORM_BUFFER, context->buffer);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(context->staging), context->staging,
               GL_DYNAMIC_DRAW);

  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void OpenGlBgControlDestroy(OpenGlBgControl* context) {
  glDeleteBuffers(1, &context->buffer);
}