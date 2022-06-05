#include "emulator/ppu/gba/software/obj.h"

bool GbaPpuObjectPixel(const GbaPpuMemory* memory,
                       const GbaPpuRegisters* registers,
                       const GbaPpuObjectVisibility* visibility,
                       const uint_fast8_t x, uint_fast8_t y, uint16_t* color,
                       uint8_t* priority, bool* semi_transparent,
                       bool* on_obj_mask) {
  *priority = UINT8_MAX;
  *on_obj_mask = false;
  bool found = false;

  GbaPpuSet objects = GbaPpuObjectVisibilityGet(visibility, x, y);
  while (!GbaPpuSetEmpty(&objects)) {
    uint_fast8_t object = GbaPpuSetPop(&objects);
    assert(memory->oam.object_attributes[object].affine ||
           !memory->oam.object_attributes[object].flex_param_0);

    int32_t lookup_x = x;
    int32_t lookup_y = y;

    if (memory->oam.object_attributes[object].affine) {
      unsigned char group = memory->oam.object_attributes[object].flex_param_1;

      int_fast16_t center_x =
          visibility->object_coordinates[object].true_x_center;
      int_fast16_t center_y =
          visibility->object_coordinates[object].true_y_center;

      int_fast16_t from_center_x = lookup_x - center_x;
      int_fast16_t from_center_y = lookup_y - center_y;

      int_fast32_t x_rotation =
          memory->oam.rotate_scale[group].pa * from_center_x +
          memory->oam.rotate_scale[group].pb * from_center_y;

      int_fast32_t y_rotation =
          memory->oam.rotate_scale[group].pc * from_center_x +
          memory->oam.rotate_scale[group].pd * from_center_y;

      lookup_x = (visibility->object_coordinates[object].pixel_x_size >> 1) +
                 (x_rotation >> 8u);

      if (lookup_x < 0 ||
          lookup_x >= visibility->object_coordinates[object].pixel_x_size) {
        continue;
      }

      lookup_y = (visibility->object_coordinates[object].pixel_y_size >> 1) +
                 (y_rotation >> 8u);

      if (lookup_y < 0 ||
          lookup_y >= visibility->object_coordinates[object].pixel_y_size) {
        continue;
      }

      if (memory->oam.object_attributes[object].obj_mosaic) {
        lookup_x -= lookup_x % (registers->mosaic.obj_horiz + 1u);
        lookup_y -= lookup_y % (registers->mosaic.obj_vert + 1u);
      }
    } else {
      lookup_x -= visibility->object_coordinates[object].true_x_start;
      lookup_y -= visibility->object_coordinates[object].true_y_start;

      if (memory->oam.object_attributes[object].obj_mosaic) {
        lookup_x -= lookup_x % (registers->mosaic.obj_horiz + 1u);
        lookup_y -= lookup_y % (registers->mosaic.obj_vert + 1u);
      }

      // Horizontal Flip
      if (memory->oam.object_attributes[object].flex_param_1 & 0x8u) {
        lookup_x =
            visibility->object_coordinates[object].pixel_x_size - lookup_x - 1;
      }

      // Vertical Flip
      if (memory->oam.object_attributes[object].flex_param_1 & 0x10u) {
        lookup_y =
            visibility->object_coordinates[object].pixel_y_size - lookup_y - 1;
      }
    }

    unsigned short character_name =
        memory->oam.object_attributes[object].character_name >>
        memory->oam.object_attributes[object].palette_mode;
    unsigned short x_tile = lookup_x / 8u;
    unsigned short y_tile = lookup_y / 8u;

    unsigned short tile_index;
    if (registers->dispcnt.object_mode) {
      // One Dimensional Lookup
      unsigned short x_size_tiles =
          (visibility->object_coordinates[object].pixel_x_size / 8u);
      tile_index = character_name + y_tile * x_size_tiles + x_tile;
    } else {
      // Two Dimensional Lookup
      unsigned short row_width =
          32u >> memory->oam.object_attributes[object].palette_mode;
      tile_index = character_name + y_tile * row_width + x_tile;
    }

    if (registers->dispcnt.mode >= 3u) {
      static const uint_least16_t minimum_index[2] = {
          GBA_BITMAP_MODE_NUM_OBJECT_S_TILES,
          GBA_BITMAP_MODE_NUM_OBJECT_D_TILES};
      if (character_name <
          minimum_index[memory->oam.object_attributes[object].palette_mode]) {
        continue;
      }
    }

    uint_fast8_t x_tile_pixel = lookup_x & 0x7u;
    uint_fast8_t y_tile_pixel = lookup_y & 0x7u;

    uint16_t obj_color;
    if (memory->oam.object_attributes[object].palette_mode) {
      uint8_t color_index;
      if (memory->oam.object_attributes[object].character_name & 1) {
        color_index = memory->vram.mode_012.obj.offset_d_tiles[tile_index]
                          .pixels[y_tile_pixel][x_tile_pixel];
      } else {
        color_index = memory->vram.mode_012.obj.d_tiles[tile_index]
                          .pixels[y_tile_pixel][x_tile_pixel];
      }

      if (color_index == 0u) {
        continue;
      }

      if (memory->oam.object_attributes[object].obj_mode == 2u) {
        *on_obj_mask = true;
        continue;
      }

      obj_color = memory->palette.obj.large_palette[color_index];
    } else {
      uint8_t color_index_pair = memory->vram.mode_012.obj.s_tiles[tile_index]
                                     .pixels[y_tile_pixel][x_tile_pixel >> 1u]
                                     .value;

      // Select lower 4 bits if lookup_x_tile_pixel is even and upper 4 bits
      // if lookup_x_tile_pixel is odd.
      uint_fast8_t color_index =
          (color_index_pair >> ((x_tile_pixel & 1u) << 2u)) & 0xFu;
      if (color_index == 0u) {
        continue;
      }

      if (memory->oam.object_attributes[object].obj_mode == 2u) {
        *on_obj_mask = true;
        continue;
      }

      obj_color =
          memory->palette.obj
              .small_palettes[memory->oam.object_attributes[object].palette]
                             [color_index];
    }

    found = true;
    if (memory->oam.object_attributes[object].priority < *priority) {
      *priority = memory->oam.object_attributes[object].priority;
      *color = obj_color;
      *semi_transparent = memory->oam.object_attributes[object].obj_mode == 1u;
    }
  }

  return found;
}