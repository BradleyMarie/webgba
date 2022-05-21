#include "emulator/ppu/gba/opengl/window.h"

void OpenGlWindowReload(OpenGlWindow* context, const GbaPpuRegisters* registers,
                        GbaPpuDirtyBits* dirty_bits) {
  if (!dirty_bits->composite.window) {
    return;
  }

  context->win0_bg0 = registers->winin.win0.bg0;
  context->win0_bg1 = registers->winin.win0.bg1;
  context->win0_bg2 = registers->winin.win0.bg2;
  context->win0_bg3 = registers->winin.win0.bg3;
  context->win0_obj = registers->winin.win0.obj;
  context->win0_bld = registers->winin.win0.bld;

  context->win1_bg0 = registers->winin.win1.bg0;
  context->win1_bg1 = registers->winin.win1.bg1;
  context->win1_bg2 = registers->winin.win1.bg2;
  context->win1_bg3 = registers->winin.win1.bg3;
  context->win1_obj = registers->winin.win1.obj;
  context->win1_bld = registers->winin.win1.bld;

  context->winobj_bg0 = registers->winout.winobj.bg0;
  context->winobj_bg1 = registers->winout.winobj.bg1;
  context->winobj_bg2 = registers->winout.winobj.bg2;
  context->winobj_bg3 = registers->winout.winobj.bg3;
  context->winobj_obj = registers->winout.winobj.obj;
  context->winobj_bld = registers->winout.winobj.bld;

  context->winout_bg0 = registers->winout.winout.bg0;
  context->winout_bg1 = registers->winout.winout.bg1;
  context->winout_bg2 = registers->winout.winout.bg2;
  context->winout_bg3 = registers->winout.winout.bg3;
  context->winout_obj = registers->winout.winout.obj;
  context->winout_bld = registers->winout.winout.bld;

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
  GLint win0_bg0 = glGetUniformLocation(program, "win0.bg0");
  glUniform1i(win0_bg0, context->win0_bg0);

  GLint win0_bg1 = glGetUniformLocation(program, "win0.bg1");
  glUniform1i(win0_bg1, context->win0_bg1);

  GLint win0_bg2 = glGetUniformLocation(program, "win0.bg2");
  glUniform1i(win0_bg2, context->win0_bg2);

  GLint win0_bg3 = glGetUniformLocation(program, "win0.bg3");
  glUniform1i(win0_bg3, context->win0_bg3);

  GLint win0_obj = glGetUniformLocation(program, "win0.obj");
  glUniform1i(win0_obj, context->win0_obj);

  GLint win0_bld = glGetUniformLocation(program, "win0.bld");
  glUniform1i(win0_bld, context->win0_bld);

  GLint win1_bg0 = glGetUniformLocation(program, "win1.bg0");
  glUniform1i(win1_bg0, context->win1_bg0);

  GLint win1_bg1 = glGetUniformLocation(program, "win1.bg1");
  glUniform1i(win1_bg1, context->win1_bg1);

  GLint win1_bg2 = glGetUniformLocation(program, "win1.bg2");
  glUniform1i(win1_bg2, context->win1_bg2);

  GLint win1_bg3 = glGetUniformLocation(program, "win1.bg3");
  glUniform1i(win1_bg3, context->win1_bg3);

  GLint win1_obj = glGetUniformLocation(program, "win1.obj");
  glUniform1i(win1_obj, context->win1_obj);

  GLint win1_bld = glGetUniformLocation(program, "win1.bld");
  glUniform1i(win1_bld, context->win1_bld);

  GLint winobj_bg0 = glGetUniformLocation(program, "winobj.bg0");
  glUniform1i(winobj_bg0, context->winobj_bg0);

  GLint winobj_bg1 = glGetUniformLocation(program, "winobj.bg1");
  glUniform1i(winobj_bg1, context->winobj_bg1);

  GLint winobj_bg2 = glGetUniformLocation(program, "winobj.bg2");
  glUniform1i(winobj_bg2, context->winobj_bg2);

  GLint winobj_bg3 = glGetUniformLocation(program, "winobj.bg3");
  glUniform1i(winobj_bg3, context->winobj_bg3);

  GLint winobj_obj = glGetUniformLocation(program, "winobj.obj");
  glUniform1i(winobj_obj, context->winobj_obj);

  GLint winobj_bld = glGetUniformLocation(program, "winobj.bld");
  glUniform1i(winobj_bld, context->winobj_bld);

  GLint winout_bg0 = glGetUniformLocation(program, "winout.bg0");
  glUniform1i(winout_bg0, context->winout_bg0);

  GLint winout_bg1 = glGetUniformLocation(program, "winout.bg1");
  glUniform1i(winout_bg1, context->winout_bg1);

  GLint winout_bg2 = glGetUniformLocation(program, "winout.bg2");
  glUniform1i(winout_bg2, context->winout_bg2);

  GLint winout_bg3 = glGetUniformLocation(program, "winout.bg3");
  glUniform1i(winout_bg3, context->winout_bg3);

  GLint winout_obj = glGetUniformLocation(program, "winout.obj");
  glUniform1i(winout_obj, context->winout_obj);

  GLint winout_bld = glGetUniformLocation(program, "winout.bld");
  glUniform1i(winout_bld, context->winout_bld);

  GLint win0_start = glGetUniformLocation(program, "win0_start");
  glUniform2f(win0_start, context->win0_start[0u], context->win0_start[1u]);

  GLint win0_end = glGetUniformLocation(program, "win0_end");
  glUniform2f(win0_end, context->win0_end[0u], context->win0_end[1u]);

  GLint win1_start = glGetUniformLocation(program, "win1_start");
  glUniform2f(win1_start, context->win1_start[0u], context->win1_start[1u]);

  GLint win1_end = glGetUniformLocation(program, "win1_end");
  glUniform2f(win1_end, context->win1_end[0u], context->win1_end[1u]);
}