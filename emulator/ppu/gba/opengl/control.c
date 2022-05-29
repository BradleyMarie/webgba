#include "emulator/ppu/gba/opengl/control.h"

#include <stdio.h>
#include <string.h>

void OpenGlControlReload(OpenGlControl* context,
                         const GbaPpuRegisters* registers,
                         GbaPpuDirtyBits* dirty_bits) {
  if (!dirty_bits->io.dispcnt) {
    return;
  }

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
  uint8_t scrolling_backgrounds[GBA_PPU_NUM_BACKGROUNDS];
  uint8_t num_scrolling_backgrounds = 0u;
  uint8_t affine_backgrounds[GBA_PPU_NUM_AFFINE_BACKGROUNDS];
  uint8_t num_affine_backgrounds = 0u;
  bool mode3 = context->mode == 3u;
  bool mode4 = context->mode == 4u;
  bool mode5 = context->mode == 5u;

  if (context->mode == 0u) {
    for (uint8_t bg = 0; bg < GBA_PPU_NUM_BACKGROUNDS; bg++) {
      if (context->bg_enabled[bg]) {
        scrolling_backgrounds[num_scrolling_backgrounds++] = bg;
      }
    }
  } else if (context->mode == 1u) {
    for (uint8_t bg = 0; bg < 2u; bg++) {
      if (context->bg_enabled[bg]) {
        scrolling_backgrounds[num_scrolling_backgrounds++] = bg;
      }
    }

    if (context->bg_enabled[2u]) {
      affine_backgrounds[num_affine_backgrounds++] = 2u;
    }
  } else if (context->mode == 2u) {
    for (uint8_t bg = 2u; bg < 2u + GBA_PPU_NUM_AFFINE_BACKGROUNDS; bg++) {
      if (context->bg_enabled[bg]) {
        affine_backgrounds[num_affine_backgrounds++] = bg;
      }
    }
  }

  for (uint8_t i = 0u; i < num_scrolling_backgrounds; i++) {
    char variable_name[100u];
    sprintf(variable_name, "scrolling_backgrounds[%u]", i);
    GLint location = glGetUniformLocation(program, variable_name);
    glUniform1ui(location, scrolling_backgrounds[i]);
  }

  GLint num_scrolling_backgrounds_loc =
      glGetUniformLocation(program, "num_scrolling_backgrounds");
  glUniform1ui(num_scrolling_backgrounds_loc, num_scrolling_backgrounds);

  for (uint8_t i = 0u; i < num_affine_backgrounds; i++) {
    char variable_name[100u];
    sprintf(variable_name, "affine_backgrounds[%u]", i);
    GLint location = glGetUniformLocation(program, variable_name);
    glUniform1ui(location, affine_backgrounds[i]);
  }

  GLint num_affine_backgrounds_loc =
      glGetUniformLocation(program, "num_affine_backgrounds");
  glUniform1ui(num_affine_backgrounds_loc, num_affine_backgrounds);

  GLint bitmap_backgrounds_mode3 =
      glGetUniformLocation(program, "bitmap_backgrounds_mode3");
  glUniform1i(bitmap_backgrounds_mode3, mode3);

  GLint bitmap_backgrounds_mode4 =
      glGetUniformLocation(program, "bitmap_backgrounds_mode4");
  glUniform1i(bitmap_backgrounds_mode4, mode4);

  GLint bitmap_backgrounds_mode5 =
      glGetUniformLocation(program, "bitmap_backgrounds_mode5");
  glUniform1i(bitmap_backgrounds_mode5, mode5);

  GLint obj_mode = glGetUniformLocation(program, "obj_mode");
  glUniform1i(obj_mode, context->obj_mode);

  GLint obj_enabled = glGetUniformLocation(program, "obj_enabled");
  glUniform1i(obj_enabled, context->obj_enabled);

  GLint win0_enabled = glGetUniformLocation(program, "win0_enabled");
  glUniform1i(win0_enabled, context->win0_enabled);

  GLint win1_enabled = glGetUniformLocation(program, "win1_enabled");
  glUniform1i(win1_enabled, context->win1_enabled);

  GLint winobj_enabled = glGetUniformLocation(program, "winobj_enabled");
  glUniform1i(winobj_enabled, context->winobj_enabled);
}