#include "emulator/ppu/gba/object_state.h"

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

void GbaPpuObjectStateClear(const GbaPpuObjectAttributeMemory* oam,
                            uint_fast8_t object, GbaPpuObjectState* state) {
  if (!oam->object_attributes[object].affine &&
      oam->object_attributes[object].flex_param_1) {
    return;
  }

  int_fast16_t x_start = oam->object_attributes[object].x_coordinate;
  int_fast16_t x_size =
      shape_size_to_x_size_pixels[oam->object_attributes[object].obj_shape]
                                 [oam->object_attributes[object].obj_size]
      << oam->object_attributes[object].flex_param_1;
  int_fast16_t x_end = x_start + x_size;

  if (x_start < 0) {
    x_start = 0;
  }

  if (x_end >= GBA_FULL_FRAME_WIDTH) {
    x_end = GBA_FULL_FRAME_WIDTH;
  }

  for (int_fast16_t x = x_start; x < x_end; x++) {
    GbaPpuObjectSetRemove(state->x_sets + x, object);
  }

  int_fast16_t y_start = oam->object_attributes[object].y_coordinate;
  int_fast16_t y_size =
      shape_size_to_y_size_pixels[oam->object_attributes[object].obj_shape]
                                 [oam->object_attributes[object].obj_size]
      << oam->object_attributes[object].flex_param_1;
  int_fast16_t y_end = y_start + y_size;

  if (y_start < 0) {
    y_start = 0;
  }

  if (y_end > GBA_FULL_FRAME_HEIGHT) {
    y_end = GBA_FULL_FRAME_HEIGHT;
  }

  for (int_fast16_t y = y_start; y < y_end; y++) {
    GbaPpuObjectSetRemove(state->y_sets + y, object);
  }
}

void GbaPpuObjectStateAdd(const GbaPpuObjectAttributeMemory* oam,
                          uint_fast8_t object, GbaPpuObjectState* state) {
  if (!oam->object_attributes[object].affine &&
      oam->object_attributes[object].flex_param_1) {
    return;
  }

  int_fast16_t x_start = oam->object_attributes[object].x_coordinate;
  int_fast16_t x_size =
      shape_size_to_x_size_pixels[oam->object_attributes[object].obj_shape]
                                 [oam->object_attributes[object].obj_size]
      << oam->object_attributes[object].flex_param_1;
  int_fast16_t x_end = x_start + x_size;

  if (x_start < 0) {
    x_start = 0;
  }

  if (x_end > GBA_FULL_FRAME_WIDTH) {
    x_end = GBA_FULL_FRAME_WIDTH;
  }

  for (int_fast16_t x = x_start; x < x_end; x++) {
    GbaPpuObjectSetAdd(state->x_sets + x, object);
  }
  int_fast16_t y_start = oam->object_attributes[object].y_coordinate;
  int_fast16_t y_size =
      shape_size_to_y_size_pixels[oam->object_attributes[object].obj_shape]
                                 [oam->object_attributes[object].obj_size]
      << oam->object_attributes[object].flex_param_1;
  int_fast16_t y_end = y_start + y_size;

  if (y_start < 0) {
    y_start = 0;
  }

  if (y_end >= GBA_FULL_FRAME_HEIGHT) {
    y_end = GBA_FULL_FRAME_HEIGHT;
  }

  for (int_fast16_t y = y_start; y < y_end; y++) {
    GbaPpuObjectSetAdd(state->y_sets + y, object);
  }
}