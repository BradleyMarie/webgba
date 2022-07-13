#include "emulator/ppu/gba/opengl/window.h"

bool OpenGlWindowStage(OpenGlWindow* context, const GbaPpuRegisters* registers,
                       GbaPpuDirtyBits* dirty_bits) {
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

  uint32_t win0_win0shift = registers->winin.win0.value | (shifts[0u] << 16u);
  uint32_t win1_win0bound = registers->winin.win1.value | (bounds[0u] << 16u);
  uint32_t winobj_win1_shift = registers->winout.winobj.value |
                               (registers->dispcnt.winobj_enable << 5u) |
                               (shifts[1u] << 16u);

  uint32_t winout_win1_bound = bounds[1u] << 16u;
  if (registers->dispcnt.win0_enable || registers->dispcnt.win1_enable ||
      registers->dispcnt.winobj_enable) {
    winout_win1_bound |= registers->winout.winout.value;
  } else {
    winout_win1_bound |= 0x3Fu;
  }

  if (context->staging[0u] != win0_win0shift) {
    context->staging[0u] = win0_win0shift;
    context->dirty = true;
  }

  if (context->staging[1u] != win1_win0bound) {
    context->staging[1u] = win1_win0bound;
    context->dirty = true;
  }

  if (context->staging[2u] != winobj_win1_shift) {
    context->staging[2u] = winobj_win1_shift;
    context->dirty = true;
  }

  if (context->staging[3u] != winout_win1_bound) {
    context->staging[3u] = winout_win1_bound;
    context->dirty = true;
  }

  return context->dirty;
}

void OpenGlWindowBind(const OpenGlWindow* context, GLuint program) {
  GLint window_and_bounds = glGetUniformLocation(program, "window_and_bounds");
  glUniform4ui(window_and_bounds, context->window[0u], context->window[1u],
               context->window[2u], context->window[3u]);
}

void OpenGlWindowReload(OpenGlWindow* context) {
  if (context->dirty) {
    context->window[0u] = context->staging[0u];
    context->window[1u] = context->staging[1u];
    context->window[2u] = context->staging[2u];
    context->window[3u] = context->staging[3u];
    context->dirty = false;
  }
}