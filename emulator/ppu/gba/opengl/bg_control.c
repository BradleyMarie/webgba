#include "emulator/ppu/gba/opengl/bg_control.h"

#include <assert.h>

#include "emulator/ppu/gba/opengl/texture_bindings.h"

bool OpenGlBgControlLoad(OpenGlBgControl* context,
                         const GbaPpuRegisters* registers,
                         GbaPpuDirtyBits* dirty_bits) {
  if (!context->dirty) {
    context->dirty_start = registers->vcount;
  }

  bool row_dirty = false;
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
        (!registers->dispcnt.bg3_enable ||
         (registers->dispcnt.mode != 0u && registers->dispcnt.mode != 2u))) {
      continue;
    }

    GLuint new_value = registers->bgcnt[i].value;

    if (registers->bgcnt[i].mosaic) {
      new_value |= (registers->mosaic.bg_horiz + 1u) << 16u;
      new_value |= (registers->mosaic.bg_vert + 1u) << 24u;
    } else {
      new_value |= 1u << 16u;
      new_value |= 1u << 24u;
    }

    if (new_value != context->staging[registers->vcount][i]) {
      context->staging[registers->vcount][i] = new_value;
      row_dirty = true;
    }
  }

  if (row_dirty) {
    context->dirty_end = registers->vcount;
    context->dirty = true;
  }

  return context->dirty;
}

void OpenGlBgControlBind(OpenGlBgControl* context, GLuint program) {
  GLint backgrounds = glGetUniformLocation(program, "backgrounds");
  glUniform1i(backgrounds, BACKGROUNDS_TEXTURE);

  glActiveTexture(GL_TEXTURE0 + BACKGROUNDS_TEXTURE);
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

void OpenGlBgControlReloadContext(OpenGlBgControl* context) {
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

void OpenGlBgControlDestroy(OpenGlBgControl* context) {
  glDeleteTextures(1u, &context->texture);
}