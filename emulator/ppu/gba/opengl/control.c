#include "emulator/ppu/gba/opengl/control.h"

#include <string.h>

void OpenGlControlReload(OpenGlControl* context,
                         const GbaPpuRegisters* registers,
                         GbaPpuDirtyBits* dirty_bits) {
  if (!dirty_bits->io.dispcnt) {
    return;
  }

  context->blank = registers->dispcnt.forced_blank;
  context->mode = registers->dispcnt.mode;
  context->bg_enabled[0u] = registers->dispcnt.bg0_enable;
  context->bg_enabled[1u] = registers->dispcnt.bg1_enable;
  context->bg_enabled[2u] = registers->dispcnt.bg2_enable;
  context->bg_enabled[3u] = registers->dispcnt.bg3_enable;
  context->win0_enabled = registers->dispcnt.win0_enable;
  context->win1_enabled = registers->dispcnt.win1_enable;
  context->winobj_enabled = registers->dispcnt.winobj_enable;

  dirty_bits->composite.window = false;
}

void OpenGlControlBind(const OpenGlControl* context, GLuint program) {
  GLint blank = glGetUniformLocation(program, "blank");
  glUniform1i(blank, context->blank);

  GLint mode = glGetUniformLocation(program, "mode");
  glUniform1i(mode, context->mode);

  GLint bg_enabled = glGetUniformLocation(program, "bg_enabled");
  glUniform4i(bg_enabled, context->bg_enabled[0u], context->bg_enabled[1u],
              context->bg_enabled[2u], context->bg_enabled[3u]);

  GLint win0_enabled = glGetUniformLocation(program, "win0_enabled");
  glUniform1i(win0_enabled, context->win0_enabled);

  GLint win1_enabled = glGetUniformLocation(program, "win1_enabled");
  glUniform1i(win1_enabled, context->win1_enabled);

  GLint winobj_enabled = glGetUniformLocation(program, "winobj_enabled");
  glUniform1i(winobj_enabled, context->winobj_enabled);
}