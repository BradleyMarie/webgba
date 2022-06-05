#include "emulator/ppu/gba/opengl/window.h"

#include <string.h>

#include "emulator/ppu/gba/opengl/texture_bindings.h"

void OpenGlWindowReload(OpenGlWindow* context, const GbaPpuRegisters* registers,
                        GbaPpuDirtyBits* dirty_bits) {
  context->staging.windows[0u].obj = registers->winin.win0.obj;
  context->staging.windows[0u].bg0 = registers->winin.win0.bg0;
  context->staging.windows[0u].bg1 = registers->winin.win0.bg1;
  context->staging.windows[0u].bg2 = registers->winin.win0.bg2;
  context->staging.windows[0u].bg3 = registers->winin.win0.bg3;
  context->staging.windows[0u].bld = registers->winin.win0.bld;

  context->staging.windows[1u].obj = registers->winin.win1.obj;
  context->staging.windows[1u].bg0 = registers->winin.win1.bg0;
  context->staging.windows[1u].bg1 = registers->winin.win1.bg1;
  context->staging.windows[1u].bg2 = registers->winin.win1.bg2;
  context->staging.windows[1u].bg3 = registers->winin.win1.bg3;
  context->staging.windows[1u].bld = registers->winin.win1.bld;

  context->staging.windows[2u].obj = registers->winout.winobj.obj;
  context->staging.windows[2u].bg0 = registers->winout.winobj.bg0;
  context->staging.windows[2u].bg1 = registers->winout.winobj.bg1;
  context->staging.windows[2u].bg2 = registers->winout.winobj.bg2;
  context->staging.windows[2u].bg3 = registers->winout.winobj.bg3;
  context->staging.windows[2u].bld = registers->winout.winobj.bld;

  if (registers->dispcnt.win0_enable || registers->dispcnt.win1_enable ||
      registers->dispcnt.winobj_enable) {
    context->staging.windows[3u].obj = registers->winout.winout.obj;
    context->staging.windows[3u].bg0 = registers->winout.winout.bg0;
    context->staging.windows[3u].bg1 = registers->winout.winout.bg1;
    context->staging.windows[3u].bg2 = registers->winout.winout.bg2;
    context->staging.windows[3u].bg3 = registers->winout.winout.bg3;
    context->staging.windows[3u].bld = registers->winout.winout.bld;
  } else {
    context->staging.windows[3u].obj = true;
    context->staging.windows[3u].bg0 = true;
    context->staging.windows[3u].bg1 = true;
    context->staging.windows[3u].bg2 = true;
    context->staging.windows[3u].bg3 = true;
    context->staging.windows[3u].bld = true;
  }

  if (registers->dispcnt.win0_enable) {
    uint16_t start = (registers->win0h.start < GBA_SCREEN_WIDTH)
                         ? registers->win0h.start
                         : GBA_SCREEN_WIDTH;
    uint16_t end = (registers->win0h.end < GBA_SCREEN_WIDTH)
                       ? registers->win0h.end
                       : GBA_SCREEN_WIDTH;

    if (start <= end) {
      context->staging.shift[0u][0u] = GBA_SCREEN_WIDTH - start;
      context->staging.bounds[0u][0u] = end - start;
    } else {
      context->staging.shift[0u][0u] = GBA_SCREEN_WIDTH - start;
      context->staging.bounds[0u][0u] = GBA_SCREEN_WIDTH - (start - end);
    }

    start = (registers->win0v.start < GBA_SCREEN_HEIGHT)
                ? registers->win0v.start
                : GBA_SCREEN_HEIGHT;
    end = (registers->win0v.end < GBA_SCREEN_HEIGHT) ? registers->win0v.end
                                                     : GBA_SCREEN_HEIGHT;

    if (start <= end) {
      context->staging.shift[0u][1u] = GBA_SCREEN_HEIGHT - start;
      context->staging.bounds[0u][1u] = end - start;
    } else {
      context->staging.shift[0u][1u] = GBA_SCREEN_HEIGHT - start;
      context->staging.bounds[0u][1u] = GBA_SCREEN_HEIGHT - (start - end);
    }
  } else {
    context->staging.shift[0u][0u] = 1u;
    context->staging.shift[0u][1u] = 1u;
    context->staging.bounds[0u][0u] = 0u;
    context->staging.bounds[0u][1u] = 0u;
  }

  if (registers->dispcnt.win1_enable) {
    uint16_t start = (registers->win1h.start < GBA_SCREEN_WIDTH)
                         ? registers->win1h.start
                         : GBA_SCREEN_WIDTH;
    uint16_t end = (registers->win1h.end < GBA_SCREEN_WIDTH)
                       ? registers->win1h.end
                       : GBA_SCREEN_WIDTH;

    if (start <= end) {
      context->staging.shift[1u][0u] = GBA_SCREEN_WIDTH - start;
      context->staging.bounds[1u][0u] = end - start;
    } else {
      context->staging.shift[1u][0u] = GBA_SCREEN_WIDTH - start;
      context->staging.bounds[1u][0u] = GBA_SCREEN_WIDTH - (start - end);
    }

    start = (registers->win1v.start < GBA_SCREEN_HEIGHT)
                ? registers->win1v.start
                : GBA_SCREEN_HEIGHT;
    end = (registers->win1v.end < GBA_SCREEN_HEIGHT) ? registers->win1v.end
                                                     : GBA_SCREEN_HEIGHT;

    if (start <= end) {
      context->staging.shift[1u][1u] = GBA_SCREEN_HEIGHT - start;
      context->staging.bounds[1u][1u] = end - start;
    } else {
      context->staging.shift[1u][1u] = GBA_SCREEN_HEIGHT - start;
      context->staging.bounds[1u][1u] = GBA_SCREEN_HEIGHT - (start - end);
    }
  } else {
    context->staging.shift[1u][0u] = 1u;
    context->staging.shift[1u][1u] = 1u;
    context->staging.bounds[1u][0u] = 0u;
    context->staging.bounds[1u][1u] = 0u;
  }

  context->staging.winobj_enabled = registers->dispcnt.winobj_enable;

  glBindBuffer(GL_UNIFORM_BUFFER, context->buffer);
  glBufferSubData(GL_UNIFORM_BUFFER, /*offset=*/0,
                  /*size=*/sizeof(context->staging),
                  /*data=*/&context->staging);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  dirty_bits->composite.window = false;
}

void OpenGlWindowBind(const OpenGlWindow* context, GLuint program) {
  GLint window = glGetUniformBlockIndex(program, "Windows");
  glUniformBlockBinding(program, window, WINDOW_BUFFER);

  glBindBuffer(GL_UNIFORM_BUFFER, context->buffer);
  glBindBufferBase(GL_UNIFORM_BUFFER, WINDOW_BUFFER, context->buffer);
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