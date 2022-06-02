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
      context->transformations[object][0u] = pa;
      context->transformations[object][1u] = pc;
      context->transformations[object][2u] = pb;
      context->transformations[object][3u] = pd;
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
        memory->oam.object_attributes[i].character_name;

    if (registers->dispcnt.mode >= 3 &&
        character_name < GBA_BITMAP_MODE_NUM_OBJECT_S_TILES) {
      continue;
    }

    int_fast16_t sprite_size[2u];
    sprite_size[0u] =
        shape_size_to_x_size_pixels[memory->oam.object_attributes[i].obj_shape]
                                   [memory->oam.object_attributes[i].obj_size];
    sprite_size[1u] =
        shape_size_to_y_size_pixels[memory->oam.object_attributes[i].obj_shape]
                                   [memory->oam.object_attributes[i].obj_size];

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

    context->center_and_half_size[i][0u] = origin[0u] + render_size[0u] / 2u;
    context->center_and_half_size[i][1u] = origin[1u] + render_size[1u] / 2u;
    context->center_and_half_size[i][2u] = sprite_size[0u] / 2;
    context->center_and_half_size[i][3u] = sprite_size[1u] / 2;

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

    context->attributes[i].tile_base = character_name * 8;
    context->attributes[i].affine = memory->oam.object_attributes[i].affine;
    context->attributes[i].large_palette =
        memory->oam.object_attributes[i].palette_mode;
    context->attributes[i].window =
        (memory->oam.object_attributes[i].obj_mode == 2u);
    context->attributes[i].semi_transparent =
        (memory->oam.object_attributes[i].obj_mode == 1u);
    context->attributes[i].palette =
        memory->oam.object_attributes[i].palette * 16;
    context->attributes[i].priority = memory->oam.object_attributes[i].priority;

    for (uint8_t rot = 0; rot < OAM_NUM_ROTATE_SCALE_GROUPS; rot++) {
      GbaPpuSetRemove(&context->rotations[rot], i);
    }

    if (memory->oam.object_attributes[i].affine) {
      uint8_t rot = memory->oam.object_attributes[i].flex_param_1;
      context->transformations[i][0u] =
          FixedToFloat(memory->oam.rotate_scale[rot].pa);
      context->transformations[i][1u] =
          FixedToFloat(memory->oam.rotate_scale[rot].pb);
      context->transformations[i][2u] =
          FixedToFloat(memory->oam.rotate_scale[rot].pc);
      context->transformations[i][3u] =
          FixedToFloat(memory->oam.rotate_scale[rot].pd);
      GbaPpuSetAdd(&context->rotations[rot], i);

      context->attributes[i].flip_x = false;
      context->attributes[i].flip_y = false;
    } else {
      context->transformations[i][0u] = 1.0;
      context->transformations[i][1u] = 0.0;
      context->transformations[i][2u] = 0.0;
      context->transformations[i][3u] = 1.0;

      context->attributes[i].flip_x =
          (memory->oam.object_attributes[i].flex_param_1 & 0x08u);
      context->attributes[i].flip_y =
          (memory->oam.object_attributes[i].flex_param_1 & 0x10u);
    }

    if (memory->oam.object_attributes[i].obj_mosaic) {
      context->attributes[i].mosaic[0u] = 1 + registers->mosaic.obj_horiz;
      context->attributes[i].mosaic[1u] = 1 + registers->mosaic.obj_vert;
    } else {
      context->attributes[i].mosaic[0u] = 1;
      context->attributes[i].mosaic[1u] = 1;
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

  glBindTexture(GL_TEXTURE_2D, context->object_center_and_half_size);
  glTexSubImage2D(GL_TEXTURE_2D, /*level=*/0, /*xoffset=*/0,
                  /*yoffset=*/0, /*width=*/OAM_NUM_OBJECTS, /*height=*/1,
                  /*format=*/GL_RGBA, /*type=*/GL_FLOAT,
                  /*pixels=*/context->center_and_half_size);

  glBindTexture(GL_TEXTURE_2D, context->object_transformations);
  glTexSubImage2D(GL_TEXTURE_2D, /*level=*/0, /*xoffset=*/0,
                  /*yoffset=*/0, /*width=*/OAM_NUM_OBJECTS, /*height=*/1,
                  /*format=*/GL_RGBA, /*type=*/GL_FLOAT,
                  /*pixels=*/context->transformations);

  glBindTexture(GL_TEXTURE_2D, context->object_visibility);
  glTexSubImage2D(GL_TEXTURE_2D, /*level=*/0, /*xoffset=*/0,
                  /*yoffset=*/0, /*width=*/GBA_SCREEN_WIDTH, /*height=*/2,
                  /*format=*/GL_RGBA_INTEGER, /*type=*/GL_UNSIGNED_INT,
                  /*pixels=*/context->visibility_staging);

  glBindTexture(GL_TEXTURE_2D, 0);
}

void OpenGlObjectAttributesReloadContext(OpenGlObjectAttributes* context) {
  glGenTextures(1u, &context->object_center_and_half_size);
  glBindTexture(GL_TEXTURE_2D, context->object_center_and_half_size);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, /*level=*/0, /*internal_format=*/GL_RGBA32F,
               /*width=*/OAM_NUM_OBJECTS,
               /*height=*/1,
               /*border=*/0, /*format=*/GL_RGBA,
               /*type=*/GL_FLOAT,
               /*pixels=*/NULL);

  glGenTextures(1u, &context->object_transformations);
  glBindTexture(GL_TEXTURE_2D, context->object_transformations);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, /*level=*/0, /*internal_format=*/GL_RGBA32F,
               /*width=*/OAM_NUM_OBJECTS,
               /*height=*/1,
               /*border=*/0, /*format=*/GL_RGBA,
               /*type=*/GL_FLOAT,
               /*pixels=*/NULL);

  glGenTextures(1u, &context->object_visibility);
  glBindTexture(GL_TEXTURE_2D, context->object_visibility);
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
  GLint object_center_and_half_size =
      glGetUniformLocation(program, "object_center_and_half_size");
  glUniform1i(object_center_and_half_size, OBJ_CENTER_AND_HALF_SIZE_TEXTURE);

  glActiveTexture(GL_TEXTURE0 + OBJ_CENTER_AND_HALF_SIZE_TEXTURE);
  glBindTexture(GL_TEXTURE_2D, context->object_center_and_half_size);

  GLint object_transformations =
      glGetUniformLocation(program, "object_transformations");
  glUniform1i(object_transformations, OBJ_TRANSFORMATIONS_TEXTURE);

  glActiveTexture(GL_TEXTURE0 + OBJ_TRANSFORMATIONS_TEXTURE);
  glBindTexture(GL_TEXTURE_2D, context->object_transformations);

  GLint object_visibility = glGetUniformLocation(program, "object_visibility");
  glUniform1i(object_visibility, OBJ_VISIBILITY_TEXTURE);

  glActiveTexture(GL_TEXTURE0 + OBJ_VISIBILITY_TEXTURE);
  glBindTexture(GL_TEXTURE_2D, context->object_visibility);

  GbaPpuSet affine;
  GbaPpuSetClear(&affine);

  GbaPpuSet flip_x;
  GbaPpuSetClear(&flip_x);

  GbaPpuSet flip_y;
  GbaPpuSetClear(&flip_y);

  GbaPpuSet large_palette;
  GbaPpuSetClear(&large_palette);

  GbaPpuSet semi_transparent;
  GbaPpuSetClear(&semi_transparent);

  GbaPpuSet window;
  GbaPpuSetClear(&window);

  for (uint8_t i = 0; i < OAM_NUM_OBJECTS; i++) {
    char variable_name[100u];
    sprintf(variable_name, "obj_attributes[%u].mosaic", i);
    GLint mosaic = glGetUniformLocation(program, variable_name);
    glUniform2i(mosaic, context->attributes[i].mosaic[0u],
                context->attributes[i].mosaic[1u]);

    sprintf(variable_name, "obj_attributes[%u].tile_base", i);
    GLint tile_base = glGetUniformLocation(program, variable_name);
    glUniform1i(tile_base, context->attributes[i].tile_base);

    sprintf(variable_name, "obj_attributes[%u].palette", i);
    GLint palette = glGetUniformLocation(program, variable_name);
    glUniform1i(palette, context->attributes[i].palette);

    sprintf(variable_name, "obj_attributes[%u].priority", i);
    GLint priority = glGetUniformLocation(program, variable_name);
    glUniform1ui(priority, context->attributes[i].priority);

    if (context->attributes[i].affine) {
      GbaPpuSetAdd(&affine, i);
    }

    if (context->attributes[i].flip_x) {
      GbaPpuSetAdd(&flip_x, i);
    }

    if (context->attributes[i].flip_y) {
      GbaPpuSetAdd(&flip_y, i);
    }

    if (context->attributes[i].large_palette) {
      GbaPpuSetAdd(&large_palette, i);
    }

    if (context->attributes[i].large_palette) {
      GbaPpuSetAdd(&large_palette, i);
    }

    if (context->attributes[i].semi_transparent) {
      GbaPpuSetAdd(&semi_transparent, i);
    }

    if (context->attributes[i].window) {
      GbaPpuSetAdd(&window, i);
    }
  }

  GLint object_affine = glGetUniformLocation(program, "object_affine");
  glUniform4ui(object_affine, affine.objects[0u], affine.objects[0u] >> 32u,
               affine.objects[1u], affine.objects[1u] >> 32u);

  GLint object_flip_x = glGetUniformLocation(program, "object_flip_x");
  glUniform4ui(object_flip_x, flip_x.objects[0u], flip_x.objects[0u] >> 32u,
               flip_x.objects[1u], flip_x.objects[1u] >> 32u);

  GLint object_flip_y = glGetUniformLocation(program, "object_flip_y");
  glUniform4ui(object_flip_y, flip_y.objects[0u], flip_y.objects[0u] >> 32u,
               flip_y.objects[1u], flip_y.objects[1u] >> 32u);

  GLint object_large_palette =
      glGetUniformLocation(program, "object_large_palette");
  glUniform4ui(object_large_palette, large_palette.objects[0u],
               large_palette.objects[0u] >> 32u, large_palette.objects[1u],
               large_palette.objects[1u] >> 32u);

  GLint object_semi_transparent =
      glGetUniformLocation(program, "object_semi_transparent");
  glUniform4ui(object_semi_transparent, semi_transparent.objects[0u],
               semi_transparent.objects[0u] >> 32u,
               semi_transparent.objects[1u],
               semi_transparent.objects[1u] >> 32u);

  GLint object_window = glGetUniformLocation(program, "object_window");
  glUniform4ui(object_window, window.objects[0u], window.objects[0u] >> 32u,
               window.objects[1u], window.objects[1u] >> 32u);
}

void OpenGlObjectAttributesDestroy(OpenGlObjectAttributes* context) {
  glDeleteTextures(1u, &context->object_visibility);
}