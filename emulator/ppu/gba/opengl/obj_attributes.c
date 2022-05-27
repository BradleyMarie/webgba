#include "emulator/ppu/gba/opengl/obj_attributes.h"

#include <stdio.h>

static GLfloat FixedToFloat(int16_t value) { return value / (GLfloat)16.0; }

void OpenGlObjectAttributesReload(OpenGlObjectAttributes* context,
                                  const GbaPpuMemory* memory,
                                  const GbaPpuRegisters* registers,
                                  GbaPpuDirtyBits* dirty_bits) {
  static const int_fast16_t shape_size_to_x_size_pixels[3][4] = {
      {8u, 16u, 32u, 64u}, {16u, 32u, 32u, 64u}, {8u, 8u, 16u, 32u}};

  static const int_fast16_t shape_size_to_y_size_pixels[3][4] = {
      {8u, 16u, 32u, 64u}, {8u, 8u, 16u, 32u}, {16u, 32u, 32u, 64u}};

  while (!GbaPpuSetEmpty(&dirty_bits->oam.rotations)) {
    uint8_t i = GbaPpuSetPop(&dirty_bits->oam.rotations);

    GLfloat pa = FixedToFloat(memory->oam.rotate_scale[i].pa);
    GLfloat pb = FixedToFloat(memory->oam.rotate_scale[i].pc);
    GLfloat pc = FixedToFloat(memory->oam.rotate_scale[i].pb);
    GLfloat pd = FixedToFloat(memory->oam.rotate_scale[i].pd);

    GbaPpuSet objects = context->rotations[i];
    while (!GbaPpuSetEmpty(&objects)) {
      uint_fast8_t object = GbaPpuSetPop(&objects);
      context->attributes[object].affine[0u][0u] = pa;
      context->attributes[object].affine[0u][1u] = pc;
      context->attributes[object].affine[1u][0u] = pb;
      context->attributes[object].affine[1u][1u] = pd;
    }
  }

  while (!GbaPpuSetEmpty(&dirty_bits->oam.objects)) {
    uint8_t i = GbaPpuSetPop(&dirty_bits->oam.objects);

    if (!memory->oam.object_attributes[i].affine &&
        memory->oam.object_attributes[i].flex_param_0) {
      context->attributes[i].enabled = false;
      continue;
    }

    if (memory->oam.object_attributes[i].obj_shape == 3u) {
      context->attributes[i].enabled = false;
      continue;
    }

    unsigned short character_name =
        memory->oam.object_attributes[i].character_name >>
        memory->oam.object_attributes[i].palette_mode;

    if (registers->dispcnt.mode >= 3) {
      static const uint_least16_t minimum_index[2] = {
          GBA_BITMAP_MODE_NUM_OBJECT_S_TILES,
          GBA_BITMAP_MODE_NUM_OBJECT_D_TILES};
      if (character_name <
          minimum_index[memory->oam.object_attributes[i].palette_mode]) {
        context->attributes[i].enabled = false;
        continue;
      }
    }

    context->attributes[i].sprite_size[0u] =
        shape_size_to_x_size_pixels[memory->oam.object_attributes[i].obj_shape]
                                   [memory->oam.object_attributes[i].obj_size];
    context->attributes[i].sprite_size[1u] =
        shape_size_to_y_size_pixels[memory->oam.object_attributes[i].obj_shape]
                                   [memory->oam.object_attributes[i].obj_size];

    if (memory->oam.object_attributes[i].flex_param_0) {
      context->attributes[i].render_size[0u] =
          2.0 * context->attributes[i].sprite_size[0u];
      context->attributes[i].render_size[1u] =
          2.0 * context->attributes[i].sprite_size[1u];
    } else {
      context->attributes[i].render_size[0u] =
          context->attributes[i].sprite_size[0u];
      context->attributes[i].render_size[1u] =
          context->attributes[i].sprite_size[1u];
    }

    context->attributes[i].origin[0u] =
        memory->oam.object_attributes[i].x_coordinate;
    context->attributes[i].origin[1u] =
        memory->oam.object_attributes[i].y_coordinate;

    if (context->attributes[i].origin[1u] +
            context->attributes[i].render_size[1u] <
        0.0) {
      context->attributes[i].origin[1u] =
          memory->oam.object_attributes[i].y_coordinate_u;
    }

    if (context->attributes[i].origin[0] >= (GLfloat)GBA_SCREEN_WIDTH ||
        context->attributes[i].origin[1] >= (GLfloat)GBA_SCREEN_HEIGHT ||
        context->attributes[i].origin[0] +
                context->attributes[i].render_size[0] <=
            0.0 ||
        context->attributes[i].origin[1] +
                context->attributes[i].render_size[1] <=
            0.0) {
      context->attributes[i].enabled = false;
      continue;
    }

    context->attributes[i].enabled = true;
    context->attributes[i].tile_base =
        (GLfloat)character_name / (GLfloat)GBA_TILE_MODE_NUM_OBJECT_S_TILES;
    context->attributes[i].large_palette =
        memory->oam.object_attributes[i].palette_mode;
    context->attributes[i].rendered =
        (memory->oam.object_attributes[i].obj_mode != 2u);
    context->attributes[i].blended =
        (memory->oam.object_attributes[i].obj_mode == 1u);
    context->attributes[i].palette =
        (GLfloat)memory->oam.object_attributes[i].palette / 16.0;
    context->attributes[i].priority = memory->oam.object_attributes[i].priority;

    for (uint8_t rot = 0; rot < OAM_NUM_ROTATE_SCALE_GROUPS; rot++) {
      GbaPpuSetRemove(&context->rotations[rot], i);
    }

    if (memory->oam.object_attributes[i].affine) {
      uint8_t rot = memory->oam.object_attributes[i].flex_param_1;
      context->attributes[i].affine[0u][0u] =
          FixedToFloat(memory->oam.rotate_scale[rot].pa);
      context->attributes[i].affine[0u][1u] =
          FixedToFloat(memory->oam.rotate_scale[rot].pc);
      context->attributes[i].affine[1u][0u] =
          FixedToFloat(memory->oam.rotate_scale[rot].pb);
      context->attributes[i].affine[1u][1u] =
          FixedToFloat(memory->oam.rotate_scale[rot].pd);
      GbaPpuSetAdd(&context->rotations[rot], i);

      context->attributes[i].flip[0] = 0.0;
      context->attributes[i].flip[1] = 0.0;
    } else {
      context->attributes[i].affine[0u][0u] = 1.0;
      context->attributes[i].affine[0u][1u] = 0.0;
      context->attributes[i].affine[1u][0u] = 0.0;
      context->attributes[i].affine[1u][1u] = 1.0;

      context->attributes[i].flip[0] =
          (memory->oam.object_attributes[i].flex_param_1 & 0x08u)
              ? context->attributes[i].sprite_size[0]
              : 0.0;
      context->attributes[i].flip[1] =
          (memory->oam.object_attributes[i].flex_param_1 & 0x10u)
              ? context->attributes[i].sprite_size[0]
              : 0.0;
    }

    if (memory->oam.object_attributes[i].obj_mosaic) {
      context->attributes[i].mosaic[0u] = 1.0 + registers->mosaic.obj_horiz;
      context->attributes[i].mosaic[1u] = 1.0 + registers->mosaic.obj_vert;
    } else {
      context->attributes[i].mosaic[0u] = 1.0;
      context->attributes[i].mosaic[1u] = 1.0;
    }
  }
}

void OpenGlBgObjectAttributesBind(const OpenGlObjectAttributes* context,
                                  GLuint program) {
  uint8_t object_count = 0u;
  for (uint8_t i = 0; i < OAM_NUM_OBJECTS; i++) {
    if (!context->attributes[i].enabled) {
      continue;
    }

    char variable_name[100u];
    sprintf(variable_name, "obj_attributes[%u].affine", object_count);
    GLint affine = glGetUniformLocation(program, variable_name);
    glUniformMatrix2fv(affine, 1, false,
                       &context->attributes[i].affine[0u][0u]);

    sprintf(variable_name, "obj_attributes[%u].origin", object_count);
    GLint origin = glGetUniformLocation(program, variable_name);
    glUniform2f(origin, context->attributes[i].origin[0u],
                context->attributes[i].origin[1u]);

    sprintf(variable_name, "obj_attributes[%u].sprite_size", object_count);
    GLint sprite_size = glGetUniformLocation(program, variable_name);
    glUniform2f(sprite_size, context->attributes[i].sprite_size[0u],
                context->attributes[i].sprite_size[1u]);

    sprintf(variable_name, "obj_attributes[%u].render_size", object_count);
    GLint render_size = glGetUniformLocation(program, variable_name);
    glUniform2f(render_size, context->attributes[i].render_size[0u],
                context->attributes[i].render_size[1u]);

    sprintf(variable_name, "obj_attributes[%u].mosaic", object_count);
    GLint mosaic = glGetUniformLocation(program, variable_name);
    glUniform2f(mosaic, context->attributes[i].mosaic[0u],
                context->attributes[i].mosaic[1u]);

    sprintf(variable_name, "obj_attributes[%u].flip", object_count);
    GLint flip = glGetUniformLocation(program, variable_name);
    glUniform2f(flip, context->attributes[i].flip[0u],
                context->attributes[i].flip[1u]);

    sprintf(variable_name, "obj_attributes[%u].tile_base", object_count);
    GLint tile_base = glGetUniformLocation(program, variable_name);
    glUniform1f(tile_base, context->attributes[i].tile_base);

    sprintf(variable_name, "obj_attributes[%u].palette", object_count);
    GLint palette = glGetUniformLocation(program, variable_name);
    glUniform1f(palette, context->attributes[i].palette);

    sprintf(variable_name, "obj_attributes[%u].large_palette", object_count);
    GLint large_palette = glGetUniformLocation(program, variable_name);
    glUniform1i(large_palette, context->attributes[i].large_palette);

    sprintf(variable_name, "obj_attributes[%u].rendered", object_count);
    GLint rendered = glGetUniformLocation(program, variable_name);
    glUniform1i(rendered, context->attributes[i].rendered);

    sprintf(variable_name, "obj_attributes[%u].blended", object_count);
    GLint blended = glGetUniformLocation(program, variable_name);
    glUniform1i(blended, context->attributes[i].blended);

    sprintf(variable_name, "obj_attributes[%u].priority", object_count);
    GLint priority = glGetUniformLocation(program, variable_name);
    glUniform1i(priority, context->attributes[i].priority);

    object_count += 1u;
  }

  GLint obj_count = glGetUniformLocation(program, "obj_count");
  glUniform1i(obj_count, object_count);
}