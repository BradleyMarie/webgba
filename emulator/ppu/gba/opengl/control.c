#include "emulator/ppu/gba/opengl/control.h"

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

  dirty_bits->io.dispcnt = false;
}

void OpenGlControlBind(const OpenGlControl* context, GLuint program) {
  GLint obj_mode = glGetUniformLocation(program, "obj_mode");
  glUniform1i(obj_mode, context->obj_mode);
}