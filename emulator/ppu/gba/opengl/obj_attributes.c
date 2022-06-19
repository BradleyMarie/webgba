#include "emulator/ppu/gba/opengl/obj_attributes.h"

#include <assert.h>

#include "emulator/ppu/gba/opengl/texture_bindings.h"

static GLfloat FixedToFloat(int16_t value) { return value / (GLfloat)256.0; }

bool OpenGlObjectAttributesStage(OpenGlObjectAttributes* context,
                                 const GbaPpuMemory* memory,
                                 const GbaPpuRegisters* registers,
                                 GbaPpuDirtyBits* dirty_bits) {
  if (!registers->dispcnt.object_enable) {
    return false;
  }

  if (dirty_bits->oam.objects) {
    for (uint8_t i = 0; i < OAM_NUM_OBJECTS; i++) {
      unsigned short character_name =
          memory->oam.object_attributes[i].character_name;

      if (registers->dispcnt.mode >= 3 &&
          character_name < GBA_BITMAP_MODE_NUM_OBJECT_S_TILES) {
        continue;
      }

      context->object_staging.objects[i].center[0u] =
          (double)memory->oam.internal.object_coordinates[i].true_x_center +
          0.5;
      context->object_staging.objects[i].center[1u] =
          (double)memory->oam.internal.object_coordinates[i].true_y_center +
          0.5;
      context->object_staging.objects[i].half_size[0u] =
          memory->oam.internal.object_coordinates[i].true_x_size / 2u;
      context->object_staging.objects[i].half_size[1u] =
          memory->oam.internal.object_coordinates[i].true_y_size / 2u;
      context->object_staging.objects[i].tile_base = character_name;
      context->object_staging.objects[i].large_palette =
          memory->oam.object_attributes[i].palette_mode;
      context->object_staging.objects[i].semi_transparent =
          (memory->oam.object_attributes[i].obj_mode == 1u);
      context->object_staging.objects[i].palette =
          memory->oam.object_attributes[i].palette_mode
              ? 0u
              : memory->oam.object_attributes[i].palette * 16u;
      context->object_staging.objects[i].priority =
          memory->oam.object_attributes[i].priority;

      if (memory->oam.object_attributes[i].affine) {
        uint8_t rot = memory->oam.object_attributes[i].flex_param_1;
        context->object_staging.objects[i].transformation[0u][0u] =
            FixedToFloat(memory->oam.rotate_scale[rot].pa);
        context->object_staging.objects[i].transformation[0u][1u] =
            FixedToFloat(memory->oam.rotate_scale[rot].pc);
        context->object_staging.objects[i].transformation[1u][0u] =
            FixedToFloat(memory->oam.rotate_scale[rot].pb);
        context->object_staging.objects[i].transformation[1u][1u] =
            FixedToFloat(memory->oam.rotate_scale[rot].pd);

        context->object_staging.objects[i].flip[0] = 0;
        context->object_staging.objects[i].flip[1] = 0;
      } else {
        context->object_staging.objects[i].transformation[0u][0u] = 1.0;
        context->object_staging.objects[i].transformation[0u][1u] = 0.0;
        context->object_staging.objects[i].transformation[1u][0u] = 0.0;
        context->object_staging.objects[i].transformation[1u][1u] = 1.0;

        context->object_staging.objects[i].flip[0] =
            (memory->oam.object_attributes[i].flex_param_1 & 0x08u)
                ? memory->oam.internal.object_coordinates[i].true_x_size - 1
                : 0;
        context->object_staging.objects[i].flip[1] =
            (memory->oam.object_attributes[i].flex_param_1 & 0x10u)
                ? memory->oam.internal.object_coordinates[i].true_y_size - 1
                : 0;
      }

      if (memory->oam.object_attributes[i].obj_mosaic) {
        context->object_staging.objects[i].mosaic[0u] =
            1 + registers->mosaic.obj_horiz;
        context->object_staging.objects[i].mosaic[1u] =
            1 + registers->mosaic.obj_vert;
      } else {
        context->object_staging.objects[i].mosaic[0u] = 1;
        context->object_staging.objects[i].mosaic[1u] = 1;
      }
    }

    // dirty_bits->oam.objects = false;
    context->objects_dirty = true;
    context->visibility_dirty = true;
  }

  if (!context->visibility_dirty) {
    return context->objects_dirty;
  }

  context->object_staging.linear_tiles = registers->dispcnt.object_mode;

  uint8_t insert_index = 0u;

  GbaPpuSet window;
  GbaPpuSetClear(&window);

  GbaPpuSet window_copy = memory->oam.internal.window;
  while (!GbaPpuSetEmpty(&window_copy)) {
    uint8_t obj = GbaPpuSetPop(&window_copy);

    GbaPpuSetAdd(&window, insert_index);
    context->visibility_staging.object_indices[insert_index++][0u] = obj;
  }

  context->visibility_staging.object_window[0u] = window.objects[0u];
  context->visibility_staging.object_window[1u] = window.objects[0u] >> 32u;
  context->visibility_staging.object_window[2u] = window.objects[1u];
  context->visibility_staging.object_window[3u] = window.objects[1u] >> 32u;

  GbaPpuSet drawn;
  GbaPpuSetClear(&drawn);

  for (uint8_t pri = 0u; pri < 4; pri++) {
    GbaPpuSet layer_copy = memory->oam.internal.layers[pri];
    while (!GbaPpuSetEmpty(&layer_copy)) {
      uint8_t obj = GbaPpuSetPop(&layer_copy);

      GbaPpuSetAdd(&drawn, insert_index);
      context->visibility_staging.object_indices[insert_index++][0u] = obj;
    }
  }

  context->visibility_staging.object_drawn[0u] = drawn.objects[0u];
  context->visibility_staging.object_drawn[1u] = drawn.objects[0u] >> 32u;
  context->visibility_staging.object_drawn[2u] = drawn.objects[1u];
  context->visibility_staging.object_drawn[3u] = drawn.objects[1u] >> 32u;

  for (uint8_t x = 0u; x < GBA_SCREEN_WIDTH; x++) {
    GbaPpuSetClear(context->columns + x);
  }

  for (uint8_t y = 0u; y < GBA_SCREEN_HEIGHT; y++) {
    GbaPpuSetClear(context->rows + y);
  }

  for (uint8_t i = 0; i < insert_index; i++) {
    uint8_t obj = context->visibility_staging.object_indices[i][0u];
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
    context->visibility_staging.object_columns[x][0u] =
        context->columns[x].objects[0u];
    context->visibility_staging.object_columns[x][1u] =
        context->columns[x].objects[0u] >> 32u;
    context->visibility_staging.object_columns[x][2u] =
        context->columns[x].objects[1u];
    context->visibility_staging.object_columns[x][3u] =
        context->columns[x].objects[1u] >> 32u;
  }

  for (uint8_t y = 0; y < GBA_SCREEN_HEIGHT; y++) {
    context->visibility_staging.object_rows[y][0u] =
        context->rows[y].objects[0u];
    context->visibility_staging.object_rows[y][1u] =
        context->rows[y].objects[0u] >> 32u;
    context->visibility_staging.object_rows[y][2u] =
        context->rows[y].objects[1u];
    context->visibility_staging.object_rows[y][3u] =
        context->rows[y].objects[1u] >> 32u;
  }

  return true;
}

void OpenGlObjectAttributesBind(const OpenGlObjectAttributes* context,
                                GLuint program) {
  GLint objects = glGetUniformBlockIndex(program, "Objects");
  if (objects >= 0) {
    glUniformBlockBinding(program, objects, OBJECTS_BUFFER);

    glBindBuffer(GL_UNIFORM_BUFFER, context->buffers[0u]);
    glBindBufferBase(GL_UNIFORM_BUFFER, OBJECTS_BUFFER, context->buffers[0u]);
  }

  GLint visibility = glGetUniformBlockIndex(program, "ObjectVisibility");
  if (visibility >= 0) {
    glUniformBlockBinding(program, visibility, OBJECT_VISIBILITY_BUFFER);

    glBindBuffer(GL_UNIFORM_BUFFER, context->buffers[1u]);
    glBindBufferBase(GL_UNIFORM_BUFFER, OBJECT_VISIBILITY_BUFFER,
                     context->buffers[1u]);
  }

  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void OpenGlObjectAttributesReload(OpenGlObjectAttributes* context) {
  if (context->objects_dirty) {
    glBindBuffer(GL_UNIFORM_BUFFER, context->buffers[0u]);
    glBufferSubData(GL_UNIFORM_BUFFER, /*offset=*/0,
                    /*size=*/sizeof(context->object_staging),
                    /*data=*/&context->object_staging);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    context->objects_dirty = false;
  }

  if (context->visibility_dirty) {
    glBindBuffer(GL_UNIFORM_BUFFER, context->buffers[1u]);
    glBufferSubData(GL_UNIFORM_BUFFER, /*offset=*/0,
                    /*size=*/sizeof(context->visibility_staging),
                    /*data=*/&context->visibility_staging);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    context->visibility_dirty = false;
  }
}

void OpenGlObjectAttributesReloadContext(OpenGlObjectAttributes* context) {
  glGenBuffers(2, context->buffers);
  glBindBuffer(GL_UNIFORM_BUFFER, context->buffers[0u]);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(context->object_staging),
               &context->object_staging, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_UNIFORM_BUFFER, context->buffers[1u]);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(context->visibility_staging),
               &context->visibility_staging, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void OpenGlObjectAttributesDestroy(OpenGlObjectAttributes* context) {
  glDeleteBuffers(2, context->buffers);
}