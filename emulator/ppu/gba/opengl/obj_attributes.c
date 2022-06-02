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
      context->staging.objects[object].transformation[0u][0u] = pa;
      context->staging.objects[object].transformation[0u][1u] = pc;
      context->staging.objects[object].transformation[1u][0u] = pb;
      context->staging.objects[object].transformation[1u][1u] = pd;
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

    context->staging.objects[i].center[0u] = origin[0u] + render_size[0u] / 2u;
    context->staging.objects[i].center[1u] = origin[1u] + render_size[1u] / 2u;
    context->staging.objects[i].half_size[0u] = sprite_size[0u] / 2;
    context->staging.objects[i].half_size[1u] = sprite_size[1u] / 2;

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

    context->staging.objects[i].tile_base = character_name * 8u;
    context->staging.objects[i].large_palette =
        memory->oam.object_attributes[i].palette_mode;
    context->staging.objects[i].window =
        (memory->oam.object_attributes[i].obj_mode == 2u);
    context->staging.objects[i].semi_transparent =
        (memory->oam.object_attributes[i].obj_mode == 1u);
    context->staging.objects[i].palette =
        memory->oam.object_attributes[i].palette * 16u;
    context->staging.objects[i].priority =
        memory->oam.object_attributes[i].priority;

    for (uint8_t rot = 0; rot < OAM_NUM_ROTATE_SCALE_GROUPS; rot++) {
      GbaPpuSetRemove(&context->rotations[rot], i);
    }

    if (memory->oam.object_attributes[i].affine) {
      uint8_t rot = memory->oam.object_attributes[i].flex_param_1;
      context->staging.objects[i].transformation[0u][0u] =
          FixedToFloat(memory->oam.rotate_scale[rot].pa);
      context->staging.objects[i].transformation[0u][1u] =
          FixedToFloat(memory->oam.rotate_scale[rot].pb);
      context->staging.objects[i].transformation[1u][0u] =
          FixedToFloat(memory->oam.rotate_scale[rot].pc);
      context->staging.objects[i].transformation[1u][1u] =
          FixedToFloat(memory->oam.rotate_scale[rot].pd);
      GbaPpuSetAdd(&context->rotations[rot], i);

      context->staging.objects[i].flip[0] = 1;
      context->staging.objects[i].flip[1] = 1;
    } else {
      context->staging.objects[i].transformation[0u][0u] = 1.0;
      context->staging.objects[i].transformation[0u][1u] = 0.0;
      context->staging.objects[i].transformation[1u][0u] = 0.0;
      context->staging.objects[i].transformation[1u][1u] = 1.0;

      context->staging.objects[i].flip[0] =
          (memory->oam.object_attributes[i].flex_param_1 & 0x08u) ? -1 : 1;
      context->staging.objects[i].flip[1] =
          (memory->oam.object_attributes[i].flex_param_1 & 0x10u) ? -1 : 1;
    }

    if (memory->oam.object_attributes[i].obj_mosaic) {
      context->staging.objects[i].mosaic[0u] = 1 + registers->mosaic.obj_horiz;
      context->staging.objects[i].mosaic[1u] = 1 + registers->mosaic.obj_vert;
    } else {
      context->staging.objects[i].mosaic[0u] = 1;
      context->staging.objects[i].mosaic[1u] = 1;
    }
  }

  for (uint8_t x = 0; x < GBA_SCREEN_WIDTH; x++) {
    context->staging.object_columns[x][0u] = context->columns[x].objects[0u];
    context->staging.object_columns[x][1u] =
        context->columns[x].objects[0u] >> 32u;
    context->staging.object_columns[x][2u] = context->columns[x].objects[1u];
    context->staging.object_columns[x][3u] =
        context->columns[x].objects[1u] >> 32u;
  }

  for (uint8_t y = 0; y < GBA_SCREEN_HEIGHT; y++) {
    context->staging.object_rows[y][0u] = context->rows[y].objects[0u];
    context->staging.object_rows[y][1u] = context->rows[y].objects[0u] >> 32u;
    context->staging.object_rows[y][2u] = context->rows[y].objects[1u];
    context->staging.object_rows[y][3u] = context->rows[y].objects[1u] >> 32u;
  }

  glBindBuffer(GL_UNIFORM_BUFFER, context->buffer);
  glBufferSubData(GL_UNIFORM_BUFFER, /*offset=*/0,
                  /*size=*/sizeof(context->staging),
                  /*data=*/&context->staging);

  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void OpenGlBgObjectAttributesBind(const OpenGlObjectAttributes* context,
                                  GLuint program) {
  GLint objects = glGetUniformBlockIndex(program, "Objects");
  glUniformBlockBinding(program, objects, OBJECTS_BUFFER);

  glBindBuffer(GL_UNIFORM_BUFFER, context->buffer);
  glBindBufferBase(GL_UNIFORM_BUFFER, OBJECTS_BUFFER, context->buffer);
}

void OpenGlObjectAttributesReloadContext(OpenGlObjectAttributes* context) {
  glGenBuffers(1, &context->buffer);
  glBindBuffer(GL_UNIFORM_BUFFER, context->buffer);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(context->staging), NULL,
               GL_DYNAMIC_DRAW);
}

void OpenGlObjectAttributesDestroy(OpenGlObjectAttributes* context) {
  glDeleteBuffers(1u, &context->buffer);
}