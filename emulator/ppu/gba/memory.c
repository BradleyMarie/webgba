#include "emulator/ppu/gba/memory.h"

void GbaPpuObjectVisibilityHidden(GbaPpuObjectAttributeMemory* oam,
                                  uint_fast8_t object) {
  for (uint8_t x = oam->internal.object_coordinates[object].pixel_x_start;
       x < oam->internal.object_coordinates[object].pixel_x_end; x++) {
    GbaPpuSetRemove(oam->internal.x_sets + x, object);
  }

  for (uint8_t y = oam->internal.object_coordinates[object].pixel_y_start;
       y < oam->internal.object_coordinates[object].pixel_y_end; y++) {
    GbaPpuSetRemove(oam->internal.y_sets + y, object);
  }

  if (oam->object_attributes[object].obj_mode == 2u) {
    GbaPpuSetRemove(&oam->internal.window, object);
  } else {
    GbaPpuSetRemove(
        oam->internal.layers + oam->object_attributes[object].priority, object);
  }
}

void GbaPpuObjectVisibilityDrawn(GbaPpuObjectAttributeMemory* oam,
                                 uint_fast8_t object) {
  assert(oam->internal.object_coordinates[object].pixel_x_size == 0u);

  static const int_fast16_t shape_size_to_x_size_pixels[4][4] = {
      {8u, 16u, 32u, 64u},
      {16u, 32u, 32u, 64u},
      {8u, 8u, 16u, 32u},
      {0u, 0u, 0u, 0u}};

  static const int_fast16_t shape_size_to_y_size_pixels[4][4] = {
      {8u, 16u, 32u, 64u},
      {8u, 8u, 16u, 32u},
      {16u, 32u, 32u, 64u},
      {0u, 0u, 0u, 0u}};

  if (!oam->object_attributes[object].affine &&
      oam->object_attributes[object].flex_param_0) {
    return;
  }

  int_fast16_t x_start = oam->object_attributes[object].x_coordinate;
  int_fast16_t x_texture_size =
      shape_size_to_x_size_pixels[oam->object_attributes[object].obj_shape]
                                 [oam->object_attributes[object].obj_size];
  int_fast16_t x_render_size = x_texture_size
                               << oam->object_attributes[object].flex_param_0;
  int_fast16_t x_end = x_start + x_render_size;

  oam->internal.object_coordinates[object].true_x_start = x_start;
  oam->internal.object_coordinates[object].true_x_center =
      x_start + (x_render_size >> 1u);
  oam->internal.object_coordinates[object].true_x_size = x_render_size;

  if (x_start < 0) {
    x_start = 0;
  } else if (x_start > GBA_SCREEN_WIDTH) {
    x_start = GBA_SCREEN_WIDTH;
  }

  if (x_end < 0) {
    x_end = 0;
  } else if (x_end > GBA_SCREEN_WIDTH) {
    x_end = GBA_SCREEN_WIDTH;
  }

  int_fast16_t y_start = oam->object_attributes[object].y_coordinate;
  int_fast16_t y_texture_size =
      shape_size_to_y_size_pixels[oam->object_attributes[object].obj_shape]
                                 [oam->object_attributes[object].obj_size];
  int_fast16_t y_render_size = y_texture_size
                               << oam->object_attributes[object].flex_param_0;
  int_fast16_t y_end = y_start + y_render_size;

  // Since there are only 8 bits for storing an object's y coordinate in OAM,
  // there are not enough bits to fully express all of its possible locations
  // on screen as a signed value. To address this, if an object is wholly below
  // the screen, the GBA instead treats the value as an unsigned integer to
  // allow drawing at line 128 and above.
  if (y_end < 0) {
    y_start = oam->object_attributes[object].y_coordinate_u;
    y_end = y_start + y_render_size;
  }

  oam->internal.object_coordinates[object].true_y_start = y_start;
  oam->internal.object_coordinates[object].true_y_center =
      y_start + (y_render_size >> 1u);
  oam->internal.object_coordinates[object].true_y_size = y_render_size;

  if (y_start < 0) {
    y_start = 0;
  } else if (y_start > GBA_SCREEN_HEIGHT) {
    y_start = GBA_SCREEN_HEIGHT;
  }

  if (y_end < 0) {
    y_end = 0;
  } else if (y_end > GBA_SCREEN_HEIGHT) {
    y_end = GBA_SCREEN_HEIGHT;
  }

  for (int_fast16_t x = x_start; x < x_end; x++) {
    GbaPpuSetAdd(oam->internal.x_sets + x, object);
  }

  for (int_fast16_t y = y_start; y < y_end; y++) {
    GbaPpuSetAdd(oam->internal.y_sets + y, object);
  }

  if (oam->object_attributes[object].obj_mode == 2u) {
    GbaPpuSetAdd(&oam->internal.window, object);
  } else {
    GbaPpuSetAdd(oam->internal.layers + oam->object_attributes[object].priority,
                 object);
  }

  oam->internal.object_coordinates[object].pixel_x_start = x_start;
  oam->internal.object_coordinates[object].pixel_x_end = x_end;
  oam->internal.object_coordinates[object].pixel_x_size = x_texture_size;
  oam->internal.object_coordinates[object].pixel_y_start = y_start;
  oam->internal.object_coordinates[object].pixel_y_end = y_end;
  oam->internal.object_coordinates[object].pixel_y_size = y_texture_size;
}