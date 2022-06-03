#include "emulator/ppu/gba/opengl/control.h"

#include <stdio.h>
#include <string.h>

void OpenGlControlReload(OpenGlControl* context,
                         const GbaPpuRegisters* registers,
                         GbaPpuDirtyBits* dirty_bits) {
  context->blank = registers->dispcnt.forced_blank;
  context->mode = registers->dispcnt.mode;
  context->obj_mode = registers->dispcnt.object_mode;
  context->obj_enabled = registers->dispcnt.object_enable;
  context->bg_enabled[0] = registers->dispcnt.bg0_enable;
  context->bg_enabled[1] = registers->dispcnt.bg1_enable;
  context->bg_enabled[2] = registers->dispcnt.bg2_enable;
  context->bg_enabled[3] = registers->dispcnt.bg3_enable;
  context->win0_enabled = registers->dispcnt.win0_enable;
  context->win1_enabled = registers->dispcnt.win1_enable;
  context->winobj_enabled = registers->dispcnt.winobj_enable;

  dirty_bits->io.dispcnt = false;
}

void OpenGlControlBind(const OpenGlControl* context, GLuint program) {
  GLint obj_mode = glGetUniformLocation(program, "obj_mode");
  glUniform1i(obj_mode, context->obj_mode);

  GLint win0_enabled = glGetUniformLocation(program, "win0_enabled");
  glUniform1i(win0_enabled, context->win0_enabled);

  GLint win1_enabled = glGetUniformLocation(program, "win1_enabled");
  glUniform1i(win1_enabled, context->win1_enabled);

  GLint winobj_enabled = glGetUniformLocation(program, "winobj_enabled");
  glUniform1i(winobj_enabled, context->winobj_enabled);
}