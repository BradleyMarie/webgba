#include "emulator/ppu/gba/opengl/bg_scrolling.h"

#include <assert.h>
#include <string.h>

#include "emulator/ppu/gba/opengl/texture_bindings.h"

bool OpenGlBgScrollingLoad(OpenGlBgScrolling* context,
                           const GbaPpuRegisters* registers,
                           GbaPpuDirtyBits* dirty_bits) {
  assert(registers->vcount < GBA_SCREEN_HEIGHT);

  if (registers->dispcnt.mode > 1u) {
    return false;
  }

  if (!context->dirty) {
    context->dirty_start = registers->vcount;
  }

  bool row_dirty = false;
  for (uint8_t i = 0; i < GBA_PPU_NUM_BACKGROUNDS; i++) {
    if (i == 0u && !registers->dispcnt.bg0_enable) {
      continue;
    }

    if (i == 1u && !registers->dispcnt.bg1_enable) {
      continue;
    }

    if (i == 2u &&
        (!registers->dispcnt.bg2_enable || registers->dispcnt.mode != 0u)) {
      continue;
    }

    if (i == 3u &&
        (!registers->dispcnt.bg3_enable || registers->dispcnt.mode != 0u)) {
      continue;
    }

    GLuint value = registers->bg_offsets[i].y;
    value <<= 16u;
    value |= registers->bg_offsets[i].x;

    if (context->staging[registers->vcount][i] != value) {
      context->staging[registers->vcount][i] = value;
      row_dirty = true;
    }
  }

  if (row_dirty) {
    context->dirty_end = registers->vcount;
    context->dirty = true;
  }

  return context->dirty;
}

void OpenGlBgScrollingBind(OpenGlBgScrolling* context, GLuint program) {
  GLint scrolling_coordinates =
      glGetUniformLocation(program, "scrolling_coordinates");
  if (scrolling_coordinates < 0) {
    return;
  }

  glUniform1i(scrolling_coordinates, BG_SCROLLING_COORDINATES_TEXTURE);
  glActiveTexture(GL_TEXTURE0 + BG_SCROLLING_COORDINATES_TEXTURE);
  glBindTexture(GL_TEXTURE_2D, context->texture);

  if (context->dirty) {
    glTexSubImage2D(GL_TEXTURE_2D, /*level=*/0, /*xoffset=*/0,
                    /*yoffset=*/context->dirty_start, /*width=*/1u,
                    /*height=*/context->dirty_end - context->dirty_start + 1u,
                    /*format=*/GL_RGBA_INTEGER, /*type=*/GL_UNSIGNED_INT,
                    /*pixels=*/context->staging[context->dirty_start]);
    context->dirty = false;
  }
}

void OpenGlBgScrollingReloadContext(OpenGlBgScrolling* context) {
  glGenTextures(1, &context->texture);
  glBindTexture(GL_TEXTURE_2D, context->texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, /*level=*/0, /*internal_format=*/GL_RGBA32UI,
               /*width=*/1u, /*height=*/GBA_SCREEN_HEIGHT, /*border=*/0,
               /*format=*/GL_RGBA_INTEGER, /*type=*/GL_UNSIGNED_INT,
               /*pixels=*/context->staging);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void OpenGlBgScrollingDestroy(OpenGlBgScrolling* context) {
  glDeleteTextures(1u, &context->texture);
}