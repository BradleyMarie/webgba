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
    context->staging.start[0u][0u] = registers->win0h.start;
    context->staging.start[0u][1u] = registers->win0v.start;
    context->staging.end[0u][0u] = registers->win0h.end;
    context->staging.end[0u][1u] = registers->win0v.end;
  } else {
    context->staging.start[0u][0u] = 241.0;
    context->staging.start[0u][1u] = 161.0;
    context->staging.end[0u][0u] = 242.0;
    context->staging.end[0u][1u] = 162.0;
  }

  if (registers->dispcnt.win1_enable) {
    context->staging.start[1u][0u] = registers->win1h.start;
    context->staging.start[1u][1u] = registers->win1v.start;
    context->staging.end[1u][0u] = registers->win1h.end;
    context->staging.end[1u][1u] = registers->win1v.end;
  } else {
    context->staging.start[1u][0u] = 241.0;
    context->staging.start[1u][1u] = 161.0;
    context->staging.end[1u][0u] = 242.0;
    context->staging.end[1u][1u] = 162.0;
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
  glBufferData(GL_UNIFORM_BUFFER, sizeof(context->staging), NULL,
               GL_DYNAMIC_DRAW);
}

void OpenGlWindowDestroy(OpenGlWindow* context) {
  glDeleteTextures(1u, &context->buffer);
}