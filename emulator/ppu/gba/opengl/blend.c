#include "emulator/ppu/gba/opengl/blend.h"

static GLfloat FixedToFloat(uint8_t fixed) { return (double)fixed / 16.0; }

void OpenGlBlendReload(OpenGlBlend* context, const GbaPpuRegisters* registers,
                       GbaPpuDirtyBits* dirty_bits) {
  if (!dirty_bits->io.blend) {
    return;
  }

  context->blend_mode = registers->bldcnt.mode;
  context->blend_eva = FixedToFloat(registers->bldalpha.eva);
  context->blend_evb = FixedToFloat(registers->bldalpha.evb);
  context->blend_evy = FixedToFloat(registers->bldy.evy);
  context->obj_top = registers->bldcnt.a_obj;
  context->obj_bottom = registers->bldcnt.b_obj;
  context->bg0_top = registers->bldcnt.a_bg0;
  context->bg0_bottom = registers->bldcnt.b_bg0;
  context->bg1_top = registers->bldcnt.a_bg1;
  context->bg1_bottom = registers->bldcnt.b_bg1;
  context->bg2_top = registers->bldcnt.a_bg2;
  context->bg2_bottom = registers->bldcnt.b_bg2;
  context->bg3_top = registers->bldcnt.a_bg3;
  context->bg3_bottom = registers->bldcnt.b_bg3;
  context->bd_top = registers->bldcnt.a_bd;
  context->bd_bottom = registers->bldcnt.b_bd;

  dirty_bits->io.blend = false;
}

void OpenGlBlendBind(const OpenGlBlend* context, GLuint program) {
  GLint blend_mode = glGetUniformLocation(program, "blend_mode");
  glUniform1i(blend_mode, context->blend_mode);

  GLint blend_eva = glGetUniformLocation(program, "blend_eva");
  glUniform1f(blend_eva, context->blend_eva);

  GLint blend_evb = glGetUniformLocation(program, "blend_evb");
  glUniform1f(blend_evb, context->blend_evb);

  GLint blend_evy = glGetUniformLocation(program, "blend_evy");
  glUniform1f(blend_evy, context->blend_evy);

  GLint obj_top = glGetUniformLocation(program, "obj_top");
  glUniform1i(obj_top, context->obj_top);

  GLint obj_bottom = glGetUniformLocation(program, "obj_bottom");
  glUniform1i(obj_bottom, context->obj_bottom);

  GLint bg0_top = glGetUniformLocation(program, "bg0_top");
  glUniform1i(bg0_top, context->bg0_top);

  GLint bg0_bottom = glGetUniformLocation(program, "bg0_bottom");
  glUniform1i(bg0_bottom, context->bg0_bottom);

  GLint bg1_top = glGetUniformLocation(program, "bg1_top");
  glUniform1i(bg1_top, context->bg1_top);

  GLint bg1_bottom = glGetUniformLocation(program, "bg1_bottom");
  glUniform1i(bg1_bottom, context->bg1_bottom);

  GLint bg2_top = glGetUniformLocation(program, "bg2_top");
  glUniform1i(bg2_top, context->bg2_top);

  GLint bg2_bottom = glGetUniformLocation(program, "bg2_bottom");
  glUniform1i(bg2_bottom, context->bg2_bottom);

  GLint bg3_top = glGetUniformLocation(program, "bg3_top");
  glUniform1i(bg3_top, context->bg3_top);

  GLint bg3_bottom = glGetUniformLocation(program, "bg3_bottom");
  glUniform1i(bg3_bottom, context->bg3_bottom);

  GLint bd_top = glGetUniformLocation(program, "bd_top");
  glUniform1i(bd_top, context->bd_top);

  GLint bd_bottom = glGetUniformLocation(program, "bd_bottom");
  glUniform1i(bd_bottom, context->bd_bottom);
}