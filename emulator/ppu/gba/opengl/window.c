#include "emulator/ppu/gba/opengl/window.h"

#include <assert.h>

#include "emulator/ppu/gba/opengl/texture_bindings.h"

bool OpenGlWindowLoad(OpenGlWindow* context, const GbaPpuRegisters* registers,
                      GbaPpuDirtyBits* dirty_bits) {
  assert(registers->vcount < GBA_SCREEN_HEIGHT);

  if (!context->dirty) {
    context->dirty_start = registers->vcount;
  }

  bool row_dirty = false;
  if (context->staging[registers->vcount].win0 != registers->winin.win0.value) {
    context->staging[registers->vcount].win0 = registers->winin.win0.value;
    row_dirty = true;
  }

  if (context->staging[registers->vcount].win1 != registers->winin.win1.value) {
    context->staging[registers->vcount].win1 = registers->winin.win1.value;
    row_dirty = true;
  }

  uint8_t winobj_value =
      registers->winout.winobj.value | (registers->dispcnt.winobj_enable << 5u);
  if (context->staging[registers->vcount].winobj != winobj_value) {
    context->staging[registers->vcount].winobj = winobj_value;
    row_dirty = true;
  }

  uint8_t winout_value;
  if (registers->dispcnt.win0_enable || registers->dispcnt.win1_enable ||
      registers->dispcnt.winobj_enable) {
    winout_value = registers->winout.winout.value;
  } else {
    winout_value = 0x3Fu;
  }

  if (context->staging[registers->vcount].winout != winout_value) {
    context->staging[registers->vcount].winout = winout_value;
    row_dirty = true;
  }

  uint16_t shifts[2u];
  uint16_t bounds[2u];
  if (registers->dispcnt.win0_enable) {
    uint16_t start = (registers->win0v.start < GBA_SCREEN_HEIGHT)
                         ? registers->win0v.start
                         : GBA_SCREEN_HEIGHT;
    uint16_t end = (registers->win0v.end < GBA_SCREEN_HEIGHT)
                       ? registers->win0v.end
                       : GBA_SCREEN_HEIGHT;

    if ((start <= registers->vcount && registers->vcount < end) ||
        (start > end &&
         (registers->vcount < end || registers->vcount > start))) {
      start = (registers->win0h.start < GBA_SCREEN_WIDTH)
                  ? registers->win0h.start
                  : GBA_SCREEN_WIDTH;
      end = (registers->win0h.end < GBA_SCREEN_WIDTH) ? registers->win0h.end
                                                      : GBA_SCREEN_WIDTH;

      if (start <= end) {
        shifts[0u] = GBA_SCREEN_WIDTH - start;
        bounds[0u] = end - start;
      } else {
        shifts[0u] = GBA_SCREEN_WIDTH - start;
        bounds[0u] = GBA_SCREEN_WIDTH - (start - end);
      }
    } else {
      shifts[0u] = 1u;
      bounds[0u] = 0u;
    }
  } else {
    shifts[0u] = 1u;
    bounds[0u] = 0u;
  }

  if (registers->dispcnt.win1_enable) {
    uint16_t start = (registers->win1v.start < GBA_SCREEN_HEIGHT)
                         ? registers->win1v.start
                         : GBA_SCREEN_HEIGHT;
    uint16_t end = (registers->win1v.end < GBA_SCREEN_HEIGHT)
                       ? registers->win1v.end
                       : GBA_SCREEN_HEIGHT;

    if ((start <= registers->vcount && registers->vcount < end) ||
        (start > end &&
         (registers->vcount < end || registers->vcount > start))) {
      start = (registers->win1h.start < GBA_SCREEN_WIDTH)
                  ? registers->win1h.start
                  : GBA_SCREEN_WIDTH;
      end = (registers->win1h.end < GBA_SCREEN_WIDTH) ? registers->win1h.end
                                                      : GBA_SCREEN_WIDTH;

      if (start <= end) {
        shifts[1u] = GBA_SCREEN_WIDTH - start;
        bounds[1u] = end - start;
      } else {
        shifts[1u] = GBA_SCREEN_WIDTH - start;
        bounds[1u] = GBA_SCREEN_WIDTH - (start - end);
      }
    } else {
      shifts[1u] = 1u;
      bounds[1u] = 0u;
    }
  } else {
    shifts[1u] = 1u;
    bounds[1u] = 0u;
  }

  if (context->staging[registers->vcount].win0_shift != shifts[0u]) {
    context->staging[registers->vcount].win0_shift = shifts[0u];
    row_dirty = true;
  }

  if (context->staging[registers->vcount].win0_bound != bounds[0u]) {
    context->staging[registers->vcount].win0_bound = bounds[0u];
    row_dirty = true;
  }

  if (context->staging[registers->vcount].win1_shift != shifts[1u]) {
    context->staging[registers->vcount].win1_shift = shifts[1u];
    row_dirty = true;
  }

  if (context->staging[registers->vcount].win1_bound != bounds[1u]) {
    context->staging[registers->vcount].win1_bound = bounds[1u];
    row_dirty = true;
  }

  if (row_dirty) {
    context->dirty_end = registers->vcount;
    context->dirty = true;
  }

  return context->dirty;
}

void OpenGlWindowBind(OpenGlWindow* context, GLuint program) {
  GLint window_rows = glGetUniformLocation(program, "window_rows");
  glUniform1i(window_rows, WINDOW_TEXTURE);

  glActiveTexture(GL_TEXTURE0 + WINDOW_TEXTURE);
  glBindTexture(GL_TEXTURE_2D, context->texture);

  if (context->dirty) {
    glTexSubImage2D(GL_TEXTURE_2D, /*level=*/0, /*xoffset=*/0,
                    /*yoffset=*/context->dirty_start, /*width=*/1u,
                    /*height=*/context->dirty_end - context->dirty_start + 1u,
                    /*format=*/GL_RGBA_INTEGER, /*type=*/GL_UNSIGNED_INT,
                    /*pixels=*/&context->staging[context->dirty_start]);
    context->dirty = false;
  }
}

void OpenGlWindowReloadContext(OpenGlWindow* context) {
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

void OpenGlWindowDestroy(OpenGlWindow* context) {
  glDeleteTextures(1u, &context->texture);
}