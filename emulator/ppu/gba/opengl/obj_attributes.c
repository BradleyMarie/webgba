#include "emulator/ppu/gba/opengl/obj_attributes.h"

#include <stdio.h>

#include "emulator/ppu/gba/opengl/texture_bindings.h"

static GLfloat FixedToFloat(int16_t value) { return value / (GLfloat)256.0; }

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

    for (uint8_t x = 0; x < GBA_SCREEN_WIDTH; x++) {
      GbaPpuSetRemove(context->columns + x, i);
    }

    for (uint8_t y = 0; y < GBA_SCREEN_HEIGHT; y++) {
      GbaPpuSetRemove(context->rows + y, i);
    }

    if (!memory->oam.object_attributes[i].affine &&
        memory->oam.object_attributes[i].flex_param_0) {
      continue;
    }

    if (memory->oam.object_attributes[i].obj_shape == 3u) {
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
        continue;
      }
    }

    int_fast16_t sprite_size[2u];
    sprite_size[0u] =
        shape_size_to_x_size_pixels[memory->oam.object_attributes[i].obj_shape]
                                   [memory->oam.object_attributes[i].obj_size];
    sprite_size[1u] =
        shape_size_to_y_size_pixels[memory->oam.object_attributes[i].obj_shape]
                                   [memory->oam.object_attributes[i].obj_size];

    context->attributes[i].sprite_size[0u] = sprite_size[0u];
    context->attributes[i].sprite_size[1u] = sprite_size[1u];

    int_fast16_t render_size[2u];
    if (memory->oam.object_attributes[i].flex_param_0) {
      render_size[0u] = 2u * sprite_size[0];
      render_size[1u] = 2u * sprite_size[1];
    } else {
      render_size[0u] = sprite_size[0];
      render_size[1u] = sprite_size[1];
    }

    int_fast16_t origin[2u];
    origin[0] = memory->oam.object_attributes[i].x_coordinate;
    origin[1] = memory->oam.object_attributes[i].y_coordinate;

    if (origin[1u] + render_size[1u] < 0u) {
      origin[1u] = memory->oam.object_attributes[i].y_coordinate_u;
    }

    context->attributes[i].center[0u] = origin[0u] + render_size[0u] / 2u;
    context->attributes[i].center[1u] = origin[1u] + render_size[1u] / 2u;

    origin[0u] = (origin[0u] < 0u) ? 0u : origin[0u];
    origin[1u] = (origin[1u] < 0u) ? 0u : origin[1u];

    int_fast16_t max[2u];
    max[0u] = origin[0u] + render_size[0u];
    max[1u] = origin[1u] + render_size[1u];

    max[0u] = (max[0u] < GBA_SCREEN_WIDTH) ? max[0u] : GBA_SCREEN_WIDTH;
    max[1u] = (max[1u] < GBA_SCREEN_HEIGHT) ? max[1u] : GBA_SCREEN_HEIGHT;

    for (int_fast16_t x = origin[0u]; x < max[0u]; x++) {
      GbaPpuSetAdd(context->columns + x, i);
    }

    for (int_fast16_t y = origin[1u]; y < max[1u]; y++) {
      GbaPpuSetAdd(context->rows + y, i);
    }

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
          FixedToFloat(memory->oam.rotate_scale[rot].pb);
      context->attributes[i].affine[1u][0u] =
          FixedToFloat(memory->oam.rotate_scale[rot].pc);
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

  for (uint8_t x = 0; x < GBA_SCREEN_WIDTH; x++) {
    context->visibility_staging[0u][x][0u] = context->columns[x].objects[0u];
    context->visibility_staging[0u][x][1u] =
        context->columns[x].objects[0u] >> 32u;
    context->visibility_staging[0u][x][2u] = context->columns[x].objects[1u];
    context->visibility_staging[0u][x][3u] =
        context->columns[x].objects[1u] >> 32u;
  }

  for (uint8_t y = 0; y < GBA_SCREEN_HEIGHT; y++) {
    context->visibility_staging[1u][y][0u] = context->rows[y].objects[0u];
    context->visibility_staging[1u][y][1u] =
        context->rows[y].objects[0u] >> 32u;
    context->visibility_staging[1u][y][2u] = context->rows[y].objects[1u];
    context->visibility_staging[1u][y][3u] =
        context->rows[y].objects[1u] >> 32u;
  }

  glBindTexture(GL_TEXTURE_2D, context->visibility);
  glTexSubImage2D(GL_TEXTURE_2D, /*level=*/0, /*xoffset=*/0,
                  /*yoffset=*/0, /*width=*/GBA_SCREEN_WIDTH, /*height=*/2,
                  /*format=*/GL_RGBA_INTEGER, /*type=*/GL_UNSIGNED_INT,
                  /*pixels=*/context->visibility_staging);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void OpenGlObjectAttributesReloadContext(OpenGlObjectAttributes* context) {
  glGenTextures(1u, &context->visibility);
  glBindTexture(GL_TEXTURE_2D, context->visibility);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, /*level=*/0, /*internal_format=*/GL_RGBA32UI,
               /*width=*/GBA_SCREEN_WIDTH,
               /*height=*/2u,
               /*border=*/0, /*format=*/GL_RGBA_INTEGER,
               /*type=*/GL_UNSIGNED_INT,
               /*pixels=*/NULL);
}

void OpenGlBgObjectAttributesBind(const OpenGlObjectAttributes* context,
                                  GLuint program) {
  GLint object_visibility = glGetUniformLocation(program, "object_visibility");
  glUniform1i(object_visibility, OBJ_VISIBILITY_TEXTURE);

  glActiveTexture(GL_TEXTURE0 + OBJ_VISIBILITY_TEXTURE);
  glBindTexture(GL_TEXTURE_2D, context->visibility);

  for (uint8_t i = 0; i < OAM_NUM_OBJECTS; i++) {
    char variable_name[100u];
    sprintf(variable_name, "obj_attributes[%u].affine", i);
    GLint affine = glGetUniformLocation(program, variable_name);
    glUniformMatrix2fv(affine, 1, false,
                       &context->attributes[i].affine[0u][0u]);

    sprintf(variable_name, "obj_attributes[%u].sprite_size", i);
    GLint sprite_size = glGetUniformLocation(program, variable_name);
    glUniform2f(sprite_size, context->attributes[i].sprite_size[0u],
                context->attributes[i].sprite_size[1u]);

    sprintf(variable_name, "obj_attributes[%u].center", i);
    GLint origin = glGetUniformLocation(program, variable_name);
    glUniform2f(origin, context->attributes[i].center[0u],
                context->attributes[i].center[1u]);

    sprintf(variable_name, "obj_attributes[%u].mosaic", i);
    GLint mosaic = glGetUniformLocation(program, variable_name);
    glUniform2f(mosaic, context->attributes[i].mosaic[0u],
                context->attributes[i].mosaic[1u]);

    sprintf(variable_name, "obj_attributes[%u].flip", i);
    GLint flip = glGetUniformLocation(program, variable_name);
    glUniform2f(flip, context->attributes[i].flip[0u],
                context->attributes[i].flip[1u]);

    sprintf(variable_name, "obj_attributes[%u].tile_base", i);
    GLint tile_base = glGetUniformLocation(program, variable_name);
    glUniform1f(tile_base, context->attributes[i].tile_base);

    sprintf(variable_name, "obj_attributes[%u].palette", i);
    GLint palette = glGetUniformLocation(program, variable_name);
    glUniform1f(palette, context->attributes[i].palette);

    sprintf(variable_name, "obj_attributes[%u].large_palette", i);
    GLint large_palette = glGetUniformLocation(program, variable_name);
    glUniform1i(large_palette, context->attributes[i].large_palette);

    sprintf(variable_name, "obj_attributes[%u].rendered", i);
    GLint rendered = glGetUniformLocation(program, variable_name);
    glUniform1i(rendered, context->attributes[i].rendered);

    sprintf(variable_name, "obj_attributes[%u].blended", i);
    GLint blended = glGetUniformLocation(program, variable_name);
    glUniform1i(blended, context->attributes[i].blended);

    sprintf(variable_name, "obj_attributes[%u].priority", i);
    GLint priority = glGetUniformLocation(program, variable_name);
    glUniform1ui(priority, context->attributes[i].priority);
  }
}

void OpenGlObjectAttributesDestroy(OpenGlObjectAttributes* context) {
  glDeleteTextures(1u, &context->visibility);
}