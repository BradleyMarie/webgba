#include "emulator/ppu/gba/opengl/window.h"

void OpenGlWindowReload(OpenGlWindow* context, const GbaPpuRegisters* registers,
                        GbaPpuDirtyBits* dirty_bits) {
  if (!dirty_bits->composite.window) {
    return;
  }

  context->win0[0u] = registers->winin.win0.bg0;
  context->win0[1u] = registers->winin.win0.bg1;
  context->win0[2u] = registers->winin.win0.bg2;
  context->win0[3u] = registers->winin.win0.bg3;
  context->win0[4u] = registers->winin.win0.obj;
  context->win0[5u] = registers->winin.win0.bld;

  context->win1[0u] = registers->winin.win1.bg0;
  context->win1[1u] = registers->winin.win1.bg1;
  context->win1[2u] = registers->winin.win1.bg2;
  context->win1[3u] = registers->winin.win1.bg3;
  context->win1[4u] = registers->winin.win1.obj;
  context->win1[5u] = registers->winin.win1.bld;

  context->winobj[0u] = registers->winout.winobj.bg0;
  context->winobj[1u] = registers->winout.winobj.bg1;
  context->winobj[2u] = registers->winout.winobj.bg2;
  context->winobj[3u] = registers->winout.winobj.bg3;
  context->winobj[4u] = registers->winout.winobj.obj;
  context->winobj[5u] = registers->winout.winobj.bld;

  context->winout[0u] = registers->winout.winout.bg0;
  context->winout[1u] = registers->winout.winout.bg1;
  context->winout[2u] = registers->winout.winout.bg2;
  context->winout[3u] = registers->winout.winout.bg3;
  context->winout[4u] = registers->winout.winout.obj;
  context->winout[5u] = registers->winout.winout.bld;

  context->win0_start[0u] = registers->win0h.start;
  context->win0_start[1u] = registers->win0v.start;
  context->win0_end[0u] = registers->win0h.end;
  context->win0_end[1u] = registers->win0v.end;

  context->win1_start[0u] = registers->win1h.start;
  context->win1_start[1u] = registers->win1v.start;
  context->win1_end[0u] = registers->win1h.end;
  context->win1_end[1u] = registers->win1v.end;

  dirty_bits->composite.window = false;
}

void OpenGlWindowBind(const OpenGlWindow* context, GLuint program) {
  GLint win0_contents = glGetUniformLocation(program, "win0_contents");
  glUniform1iv(win0_contents, 6u, context->win0);

  GLint win1_contents = glGetUniformLocation(program, "win1_contents");
  glUniform1iv(win1_contents, 6u, context->win1);

  GLint winobj_contents = glGetUniformLocation(program, "winobj_contents");
  glUniform1iv(winobj_contents, 6u, context->winobj);

  GLint winout_contents = glGetUniformLocation(program, "winout_contents");
  glUniform1iv(winout_contents, 6u, context->winout);

  GLint win0_start = glGetUniformLocation(program, "win0_start");
  glUniform2f(win0_start, context->win0_start[0u], context->win0_start[1u]);

  GLint win0_end = glGetUniformLocation(program, "win0_end");
  glUniform2f(win0_end, context->win0_end[0u], context->win0_end[1u]);

  GLint win1_start = glGetUniformLocation(program, "win1_start");
  glUniform2f(win1_start, context->win1_start[0u], context->win1_start[1u]);

  GLint win1_end = glGetUniformLocation(program, "win1_end");
  glUniform2f(win1_end, context->win1_end[0u], context->win1_end[1u]);
}