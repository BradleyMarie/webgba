#include "emulator/ppu/gba/opengl/objects.h"

#include <assert.h>

#include "emulator/ppu/gba/opengl/texture_bindings.h"

typedef union {
  struct {
    // x
    unsigned short canvas_top_x;
    unsigned short canvas_top_y;

    // y
    unsigned short character_name;
    unsigned char canvas_size_x;
    unsigned char canvas_size_y;

    // z
    unsigned char mosaic_x;
    unsigned char mosaic_y;
    unsigned char affine_index;
    unsigned char palette;

    // w
    unsigned char sprite_size_x;
    unsigned char sprite_size_y;
    unsigned char priority : 2u;
    bool large_palette : 1u;
    bool flip_x : 1u;
    bool flip_y : 1u;
    bool semi_transparent : 1u;
    bool linear_tiles : 1u;
    unsigned short unused : 9u;
  };
  GLuint values[4u];
} OpenGlObjectAttribute;

static GLfloat FixedToFloat(int16_t value) { return value / (GLfloat)256.0; }

bool OpenGlObjectsStage(OpenGlObjects* context, const GbaPpuMemory* memory,
                        const GbaPpuRegisters* registers,
                        GbaPpuDirtyBits* dirty_bits) {
  if (!registers->dispcnt.object_enable) {
    return false;
  }

  if (!dirty_bits->oam.overall && !dirty_bits->io.obj_mosaic) {
    return false;
  }

  context->staging.object_transformations[0u][0u][0u] = 1.0;
  context->staging.object_transformations[0u][0u][1u] = 0.0;
  context->staging.object_transformations[0u][1u][0u] = 0.0;
  context->staging.object_transformations[0u][1u][1u] = 1.0;

  for (uint8_t i = 0u; i < OAM_NUM_ROTATE_SCALE_GROUPS; i++) {
    context->staging.object_transformations[i + 1u][0u][0u] =
        FixedToFloat(memory->oam.rotate_scale[i].pa);
    context->staging.object_transformations[i + 1u][0u][1u] =
        FixedToFloat(memory->oam.rotate_scale[i].pc);
    context->staging.object_transformations[i + 1u][1u][0u] =
        FixedToFloat(memory->oam.rotate_scale[i].pb);
    context->staging.object_transformations[i + 1u][1u][1u] =
        FixedToFloat(memory->oam.rotate_scale[i].pd);
  }

  uint8_t insert_index = 0u;
  uint8_t object_indices[OAM_NUM_OBJECTS];

  GbaPpuSet window;
  GbaPpuSetClear(&window);

  GbaPpuSet window_copy = memory->oam.internal.window;
  while (!GbaPpuSetEmpty(&window_copy)) {
    uint8_t obj = GbaPpuSetPop(&window_copy);

    if (registers->dispcnt.mode >= 3 &&
        memory->oam.object_attributes[obj].character_name <
            GBA_BITMAP_MODE_NUM_OBJECT_S_TILES) {
      continue;
    }

    GbaPpuSetAdd(&window, insert_index);
    object_indices[insert_index++] = obj;
  }

  context->staging.object_window[0u] = window.objects[0u];
  context->staging.object_window[1u] = window.objects[0u] >> 32u;
  context->staging.object_window[2u] = window.objects[1u];
  context->staging.object_window[3u] = window.objects[1u] >> 32u;

  GbaPpuSet drawn;
  GbaPpuSetClear(&drawn);

  for (uint8_t pri = 0u; pri < 4; pri++) {
    GbaPpuSet layer_copy = memory->oam.internal.layers[pri];
    while (!GbaPpuSetEmpty(&layer_copy)) {
      uint8_t obj = GbaPpuSetPop(&layer_copy);

      if (registers->dispcnt.mode >= 3 &&
          memory->oam.object_attributes[obj].character_name <
              GBA_BITMAP_MODE_NUM_OBJECT_S_TILES) {
        continue;
      }

      GbaPpuSetAdd(&drawn, insert_index);
      object_indices[insert_index++] = obj;
    }
  }

  context->staging.object_drawn[0u] = drawn.objects[0u];
  context->staging.object_drawn[1u] = drawn.objects[0u] >> 32u;
  context->staging.object_drawn[2u] = drawn.objects[1u];
  context->staging.object_drawn[3u] = drawn.objects[1u] >> 32u;

  for (uint8_t x = 0u; x < GBA_SCREEN_WIDTH; x++) {
    GbaPpuSetClear(context->columns + x);
  }

  for (uint8_t y = 0u; y < GBA_SCREEN_HEIGHT; y++) {
    GbaPpuSetClear(context->rows + y);
  }

  for (uint8_t i = 0; i < insert_index; i++) {
    uint8_t obj = object_indices[i];
    for (int_fast16_t x =
             memory->oam.internal.object_coordinates[obj].pixel_x_start;
         x < memory->oam.internal.object_coordinates[obj].pixel_x_end; x++) {
      GbaPpuSetAdd(context->columns + x, i);
    }

    for (int_fast16_t y =
             memory->oam.internal.object_coordinates[obj].pixel_y_start;
         y < memory->oam.internal.object_coordinates[obj].pixel_y_end; y++) {
      GbaPpuSetAdd(context->rows + y, i);
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

  for (uint8_t i = 0; i < insert_index; i++) {
    uint8_t obj = object_indices[i];

    OpenGlObjectAttribute attribute;
    attribute.canvas_top_x =
        memory->oam.internal.object_coordinates[obj].true_x_start +
        memory->oam.internal.object_coordinates[obj].true_x_size;
    attribute.canvas_top_y =
        memory->oam.internal.object_coordinates[obj].true_y_start +
        memory->oam.internal.object_coordinates[obj].true_y_size;
    attribute.canvas_size_x =
        memory->oam.internal.object_coordinates[obj].true_x_size;
    attribute.canvas_size_y =
        memory->oam.internal.object_coordinates[obj].true_y_size;
    attribute.sprite_size_x =
        memory->oam.internal.object_coordinates[obj].pixel_x_size;
    attribute.sprite_size_y =
        memory->oam.internal.object_coordinates[obj].pixel_y_size;
    attribute.character_name =
        memory->oam.object_attributes[obj].character_name;

    if (memory->oam.object_attributes[obj].obj_mosaic) {
      attribute.mosaic_x = 1u + registers->mosaic.obj_horiz;
      attribute.mosaic_y = 1u + registers->mosaic.obj_vert;
    } else {
      attribute.mosaic_x = 1u;
      attribute.mosaic_y = 1u;
    }

    if (memory->oam.object_attributes[obj].affine) {
      attribute.affine_index =
          memory->oam.object_attributes[obj].flex_param_1 + 1u;
      attribute.flip_x = false;
      attribute.flip_y = false;
    } else {
      attribute.affine_index = 0u;
      attribute.flip_x =
          !!(memory->oam.object_attributes[obj].flex_param_1 & 0x08u);
      attribute.flip_y =
          !!(memory->oam.object_attributes[obj].flex_param_1 & 0x10u);
    }

    if (memory->oam.object_attributes[obj].palette_mode) {
      attribute.large_palette = true;
      attribute.palette = 0u;
    } else {
      attribute.large_palette = false;
      attribute.palette = memory->oam.object_attributes[obj].palette * 16u;
    }

    attribute.priority = memory->oam.object_attributes[obj].priority;
    attribute.semi_transparent =
        (memory->oam.object_attributes[obj].obj_mode == 1u);
    attribute.linear_tiles = registers->dispcnt.object_mode;

    context->staging.object_attributes[i][0u] = attribute.values[0u];
    context->staging.object_attributes[i][1u] = attribute.values[1u];
    context->staging.object_attributes[i][2u] = attribute.values[2u];
    context->staging.object_attributes[i][3u] = attribute.values[3u];
  }

  dirty_bits->oam.overall = false;
  dirty_bits->io.obj_mosaic = false;
  context->dirty = true;

  return true;
}

void OpenGlObjectsBind(const OpenGlObjects* context, GLuint program) {
  GLint objects = glGetUniformBlockIndex(program, "Objects");
  if (objects >= 0) {
    glUniformBlockBinding(program, objects, OBJECTS_BUFFER);

    glBindBuffer(GL_UNIFORM_BUFFER, context->buffer);
    glBindBufferBase(GL_UNIFORM_BUFFER, OBJECTS_BUFFER, context->buffer);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
  }
}

void OpenGlObjectsReload(OpenGlObjects* context) {
  if (context->dirty) {
    glBindBuffer(GL_UNIFORM_BUFFER, context->buffer);
    glBufferSubData(GL_UNIFORM_BUFFER, /*offset=*/0,
                    /*size=*/sizeof(context->staging),
                    /*data=*/&context->staging);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    context->dirty = false;
  }
}

void OpenGlObjectsReloadContext(OpenGlObjects* context) {
  glGenBuffers(1, &context->buffer);
  glBindBuffer(GL_UNIFORM_BUFFER, context->buffer);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(context->staging), &context->staging,
               GL_DYNAMIC_DRAW);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void OpenGlObjectsDestroy(OpenGlObjects* context) {
  glDeleteBuffers(1, &context->buffer);
}