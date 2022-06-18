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
  if (context->staging[registers->vcount].windows[0u] !=
      registers->winin.win0.value) {
    context->staging[registers->vcount].windows[0u] =
        registers->winin.win0.value;
    row_dirty = true;
  }

  if (context->staging[registers->vcount].windows[1u] !=
      registers->winin.win1.value) {
    context->staging[registers->vcount].windows[1u] =
        registers->winin.win1.value;
    row_dirty = true;
  }

  uint8_t winobj_value =
      registers->winout.winobj.value | (registers->dispcnt.winobj_enable << 5u);
  if (context->staging[registers->vcount].windows[2u] != winobj_value) {
    context->staging[registers->vcount].windows[2u] = winobj_value;
    row_dirty = true;
  }

  uint8_t winout_value;
  if (registers->dispcnt.win0_enable || registers->dispcnt.win1_enable ||
      registers->dispcnt.winobj_enable) {
    winout_value = registers->winout.winout.value;
  } else {
    winout_value = 0x3Fu;
  }

  if (context->staging[registers->vcount].windows[3u] != winout_value) {
    context->staging[registers->vcount].windows[3u] = winout_value;
    row_dirty = true;
  }

  GLuint shift_bounds[4u];
  if (registers->dispcnt.win0_enable) {
    uint16_t start = (registers->win0v.start < GBA_SCREEN_HEIGHT)
                         ? registers->win0v.start
                         : GBA_SCREEN_HEIGHT;
    uint16_t end = (registers->win0v.end < GBA_SCREEN_HEIGHT)
                       ? registers->win0v.end
                       : GBA_SCREEN_HEIGHT;

    if ((start <= registers->vcount && registers->vcount <= end) ||
        (start > end &&
         (registers->vcount < end || registers->vcount > start))) {
      start = (registers->win0h.start < GBA_SCREEN_WIDTH)
                  ? registers->win0h.start
                  : GBA_SCREEN_WIDTH;
      end = (registers->win0h.end < GBA_SCREEN_WIDTH) ? registers->win0h.end
                                                      : GBA_SCREEN_WIDTH;

      if (start <= end) {
        shift_bounds[0u] = GBA_SCREEN_WIDTH - start;
        shift_bounds[1u] = end - start;
      } else {
        shift_bounds[0u] = GBA_SCREEN_WIDTH - start;
        shift_bounds[1u] = GBA_SCREEN_WIDTH - (start - end);
      }
    } else {
      shift_bounds[0u] = 1u;
      shift_bounds[1u] = 0u;
    }
  } else {
    shift_bounds[0u] = 1u;
    shift_bounds[1u] = 0u;
  }

  if (registers->dispcnt.win1_enable) {
    uint16_t start = (registers->win1v.start < GBA_SCREEN_HEIGHT)
                         ? registers->win1v.start
                         : GBA_SCREEN_HEIGHT;
    uint16_t end = (registers->win1v.end < GBA_SCREEN_HEIGHT)
                       ? registers->win1v.end
                       : GBA_SCREEN_HEIGHT;

    if ((start <= registers->vcount && registers->vcount <= end) ||
        (start > end &&
         (registers->vcount < end || registers->vcount > start))) {
      start = (registers->win1h.start < GBA_SCREEN_WIDTH)
                  ? registers->win1h.start
                  : GBA_SCREEN_WIDTH;
      end = (registers->win1h.end < GBA_SCREEN_WIDTH) ? registers->win1h.end
                                                      : GBA_SCREEN_WIDTH;

      if (start <= end) {
        shift_bounds[2u] = GBA_SCREEN_WIDTH - start;
        shift_bounds[3u] = end - start;
      } else {
        shift_bounds[2u] = GBA_SCREEN_WIDTH - start;
        shift_bounds[3u] = GBA_SCREEN_WIDTH - (start - end);
      }
    } else {
      shift_bounds[2u] = 1u;
      shift_bounds[3u] = 0u;
    }
  } else {
    shift_bounds[2u] = 1u;
    shift_bounds[3u] = 0u;
  }

  if (context->staging[registers->vcount].shift_bounds[0u] !=
      shift_bounds[0u]) {
    context->staging[registers->vcount].shift_bounds[0u] = shift_bounds[0u];
    row_dirty = true;
  }

  if (context->staging[registers->vcount].shift_bounds[1u] !=
      shift_bounds[1u]) {
    context->staging[registers->vcount].shift_bounds[1u] = shift_bounds[1u];
    row_dirty = true;
  }

  if (context->staging[registers->vcount].shift_bounds[2u] !=
      shift_bounds[2u]) {
    context->staging[registers->vcount].shift_bounds[2u] = shift_bounds[2u];
    row_dirty = true;
  }

  if (context->staging[registers->vcount].shift_bounds[3u] !=
      shift_bounds[3u]) {
    context->staging[registers->vcount].shift_bounds[3u] = shift_bounds[3u];
    row_dirty = true;
  }

  if (row_dirty) {
    context->dirty_end = registers->vcount;
    context->dirty = true;
  }

  return context->dirty;
}

void OpenGlWindowBind(OpenGlWindow* context, GLuint program) {
  GLint window = glGetUniformBlockIndex(program, "Windows");
  glUniformBlockBinding(program, window, WINDOW_BUFFER);

  glBindBuffer(GL_UNIFORM_BUFFER, context->buffer);
  glBindBufferBase(GL_UNIFORM_BUFFER, WINDOW_BUFFER, context->buffer);

  if (context->dirty) {
    glBufferSubData(GL_UNIFORM_BUFFER,
                    /*offset=*/sizeof(OpenGlWindowRow) * context->dirty_start,
                    /*size=*/sizeof(OpenGlWindowRow) *
                        (context->dirty_end - context->dirty_start + 1u),
                    /*data=*/&context->staging[context->dirty_start]);
    context->dirty = false;
  }

  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void OpenGlWindowReloadContext(OpenGlWindow* context) {
  glGenBuffers(1, &context->buffer);
  glBindBuffer(GL_UNIFORM_BUFFER, context->buffer);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(context->staging), &context->staging,
               GL_DYNAMIC_DRAW);
}

void OpenGlWindowDestroy(OpenGlWindow* context) {
  glDeleteTextures(1u, &context->buffer);
}