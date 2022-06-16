#include "emulator/ppu/gba/opengl/window.h"

#include "emulator/ppu/gba/opengl/texture_bindings.h"

bool OpenGlWindowLoad(OpenGlWindow* context, const GbaPpuRegisters* registers,
                      GbaPpuDirtyBits* dirty_bits) {
  if (context->staging[registers->vcount].windows[0u] !=
      registers->winin.win0.value) {
    context->dirty = true;
  }

  context->staging[registers->vcount].windows[0u] = registers->winin.win0.value;

  if (context->staging[registers->vcount].windows[1u] !=
      registers->winin.win1.value) {
    context->dirty = true;
  }

  context->staging[registers->vcount].windows[1u] = registers->winin.win1.value;

  uint8_t winobj_value =
      registers->winout.winobj.value | (registers->dispcnt.winobj_enable << 5u);
  if (context->staging[registers->vcount].windows[2u] != winobj_value) {
    context->dirty = true;
  }

  context->staging[registers->vcount].windows[2u] = winobj_value;

  uint8_t winout_value;
  if (registers->dispcnt.win0_enable || registers->dispcnt.win1_enable ||
      registers->dispcnt.winobj_enable) {
    winout_value = registers->winout.winout.value;
  } else {
    winout_value = 0x3Fu;
  }

  if (context->staging[registers->vcount].windows[3u] != winout_value) {
    context->dirty = true;
  }

  context->staging[registers->vcount].windows[3u] = winout_value;

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
        context->staging[registers->vcount].shift_bounds[0u] =
            GBA_SCREEN_WIDTH - start;
        context->staging[registers->vcount].shift_bounds[1u] = end - start;
      } else {
        context->staging[registers->vcount].shift_bounds[0u] =
            GBA_SCREEN_WIDTH - start;
        context->staging[registers->vcount].shift_bounds[1u] =
            GBA_SCREEN_WIDTH - (start - end);
      }
    } else {
      context->staging[registers->vcount].shift_bounds[0u] = 1u;
      context->staging[registers->vcount].shift_bounds[1u] = 0u;
    }
  } else {
    context->staging[registers->vcount].shift_bounds[0u] = 1u;
    context->staging[registers->vcount].shift_bounds[1u] = 0u;
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
        context->staging[registers->vcount].shift_bounds[2u] =
            GBA_SCREEN_WIDTH - start;
        context->staging[registers->vcount].shift_bounds[3u] = end - start;
      } else {
        context->staging[registers->vcount].shift_bounds[2u] =
            GBA_SCREEN_WIDTH - start;
        context->staging[registers->vcount].shift_bounds[3u] =
            GBA_SCREEN_WIDTH - (start - end);
      }
    } else {
      context->staging[registers->vcount].shift_bounds[2u] = 1u;
      context->staging[registers->vcount].shift_bounds[3u] = 0u;
    }
  } else {
    context->staging[registers->vcount].shift_bounds[2u] = 1u;
    context->staging[registers->vcount].shift_bounds[3u] = 0u;
  }

  context->dirty = true;

  return true;
}

void OpenGlWindowBind(OpenGlWindow* context, GLint start, GLint end,
                      GLuint program) {
  GLint window = glGetUniformBlockIndex(program, "Windows");
  glUniformBlockBinding(program, window, WINDOW_BUFFER);

  glBindBuffer(GL_UNIFORM_BUFFER, context->buffer);
  glBindBufferBase(GL_UNIFORM_BUFFER, WINDOW_BUFFER, context->buffer);

  if (context->dirty) {
    glBufferSubData(GL_UNIFORM_BUFFER,
                    /*offset=*/sizeof(OpenGlWindowRow) * start,
                    /*size=*/sizeof(OpenGlWindowRow) * (end - start),
                    /*data=*/&context->staging[start]);
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