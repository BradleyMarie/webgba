#version 300 es

#define OBJECTS 0
#define SCROLLING_BACKGROUND_0 0
#define SCROLLING_BACKGROUND_1 0
#define SCROLLING_BACKGROUND_2 0
#define SCROLLING_BACKGROUND_3 0
#define AFFINE_BACKGROUND_2 0
#define AFFINE_BACKGROUND_3 0
#define BITMAP_BACKGROUND 0
#define SMALL_BITMAP_BACKGROUND 0
#define LARGE_BITMAP_BACKGROUND 0
#define PALETTE_BITMAP_BACKGROUND 0

// Outputs
out lowp vec4 frag_color;

// Render Scale
uniform lowp uint render_scale;

// Background Control
uniform highp uvec4 background_control;

// Palettes
uniform lowp sampler2D background_palette;
uniform lowp sampler2D object_palette;

// Tiles
uniform lowp usampler2DArray background_tiles;
uniform lowp usampler2D object_tiles;

// Tilemaps
uniform lowp usampler2DArray affine_tilemap;
uniform lowp usampler2DArray scrolling_tilemap;

// Background Bitmaps
uniform lowp sampler2D bitmap;
uniform lowp usampler2D palette_bitmap;

// Objects
uniform mediump sampler2D object_transformations;
uniform highp usampler2D object_attributes;
uniform highp usampler2D object_rows;
uniform highp usampler2D object_columns;
uniform highp usampler2D object_indicies_masks;

// Background Coordinates
uniform highp usampler2D scrolling_coordinates;
uniform highp sampler2D affine_coordinates;

// Window
uniform highp usampler2D window_rows;

// Blend
uniform mediump uvec3 blend_control_ev;

// Blend Unit
struct BlendUnit {
  lowp vec3 color[2];
  lowp uint priority[2];
  bool top[2];
  bool bottom[2];
  bool semi_transparent[2];
};

BlendUnit CreateBlendUnit(lowp uint screen_row) {
  BlendUnit result;
  result.color[0] = vec3(0.0, 0.0, 0.0);
  result.color[1] = vec3(0.0, 0.0, 0.0);
  result.priority[0] = 6u;
  result.priority[1] = 6u;
  result.top[0] = false;
  result.top[1] = false;
  result.bottom[0] = false;
  result.bottom[1] = false;
  result.semi_transparent[0] = false;
  result.semi_transparent[1] = false;
  return result;
}

BlendUnit BlendUnitAddObject(BlendUnit blend_unit, lowp vec3 color,
                             lowp uint priority, bool blended) {
  blend_unit.color[0] = color.bgr;
  blend_unit.priority[0] = priority;
  blend_unit.top[0] = blended || bool(blend_control_ev.y & 0x10u);
  blend_unit.bottom[0] = bool(blend_control_ev.z & 0x10u);
  blend_unit.semi_transparent[0] = blended;
  return blend_unit;
}

// Assumes priority is less than blend_unit.priority[1]
BlendUnit BlendUnitAddBackground(BlendUnit blend_unit, highp uint bgcnt,
                                 lowp uint bg, lowp vec3 color) {
  lowp uint priority = bgcnt & 0x3u;

  blend_unit.color[1] = color.bgr;
  blend_unit.priority[1] = priority;
  blend_unit.top[1] = bool(blend_control_ev.y & (1u << bg));
  blend_unit.bottom[1] = bool(blend_control_ev.z & (1u << bg));
  blend_unit.semi_transparent[1] = false;

  if (blend_unit.priority[1] < blend_unit.priority[0]) {
    lowp vec3 temp_color = blend_unit.color[0];
    blend_unit.color[0] = blend_unit.color[1];
    blend_unit.color[1] = temp_color;

    lowp uint temp_uint = blend_unit.priority[0];
    blend_unit.priority[0] = blend_unit.priority[1];
    blend_unit.priority[1] = temp_uint;

    bool temp_bool = blend_unit.top[0];
    blend_unit.top[0] = blend_unit.top[1];
    blend_unit.top[1] = temp_bool;

    temp_bool = blend_unit.bottom[0];
    blend_unit.bottom[0] = blend_unit.bottom[1];
    blend_unit.bottom[1] = temp_bool;

    temp_bool = blend_unit.semi_transparent[0];
    blend_unit.semi_transparent[0] = blend_unit.semi_transparent[1];
    blend_unit.semi_transparent[1] = temp_bool;
  }

  return blend_unit;
}

BlendUnit BlendUnitAddBackdrop(BlendUnit blend_unit, lowp vec3 color) {
  if (5u < blend_unit.priority[1]) {
    if (5u < blend_unit.priority[0]) {
      blend_unit.color[0] = color.bgr;
      blend_unit.top[0] = bool(blend_control_ev.y & 0x20u);
      blend_unit.bottom[0] = bool(blend_control_ev.z & 0x20u);
      blend_unit.semi_transparent[0] = false;
    } else {
      blend_unit.color[1] = color.bgr;
      blend_unit.top[1] = bool(blend_control_ev.y & 0x20u);
      blend_unit.bottom[1] = bool(blend_control_ev.z & 0x20u);
      blend_unit.semi_transparent[1] = false;
    }
  }
  return blend_unit;
}

lowp vec3 BlendUnitNoBlend(BlendUnit blend_unit) {
  bool do_blend =
      (blend_unit.semi_transparent[0] || blend_unit.semi_transparent[1]) &&
      blend_unit.top[0] && blend_unit.bottom[1];

  lowp float eva, evb;
  if (do_blend) {
    eva = float(blend_control_ev.x >> 8u) * (1.0 / 16.0);
    evb = float(blend_control_ev.y >> 8u) * (1.0 / 16.0);
  } else {
    eva = 1.0;
    evb = 0.0;
  }

  return min((blend_unit.color[0] * eva) + (blend_unit.color[1] * evb), 1.0);
}

lowp vec3 BlendUnitAdditiveBlend(BlendUnit blend_unit) {
  bool do_blend = blend_unit.top[0] && blend_unit.bottom[1];

  lowp float eva, evb;
  if (do_blend) {
    eva = float(blend_control_ev.x >> 8u) * (1.0 / 16.0);
    evb = float(blend_control_ev.y >> 8u) * (1.0 / 16.0);
  } else {
    eva = 1.0;
    evb = 0.0;
  }

  return min((blend_unit.color[0] * eva) + (blend_unit.color[1] * evb), 1.0);
}

lowp vec3 BlendUnitBrighten(BlendUnit blend_unit) {
  bool do_brighten = blend_unit.top[0];
  bool do_blend =
      (blend_unit.semi_transparent[0] || blend_unit.semi_transparent[1]) &&
      blend_unit.top[0] && blend_unit.bottom[1];

  lowp float evy;
  if (do_brighten) {
    evy = float(blend_control_ev.z >> 8u) * (1.0 / 16.0);
  } else {
    evy = 0.0;
  }

  lowp vec3 bottom;
  lowp float eva, evb;
  if (do_blend) {
    eva = float(blend_control_ev.x >> 8u) * (1.0 / 16.0);
    evb = float(blend_control_ev.y >> 8u) * (1.0 / 16.0);
    bottom = blend_unit.color[1];
  } else {
    eva = 1.0;
    evb = evy;
    bottom = 1.0 - blend_unit.color[0];
  }

  return min((blend_unit.color[0] * eva) + (bottom * evb), 1.0);
}

lowp vec3 BlendUnitDarken(BlendUnit blend_unit) {
  bool do_darken = blend_unit.top[0];
  bool do_blend =
      (blend_unit.semi_transparent[0] || blend_unit.semi_transparent[1]) &&
      blend_unit.top[0] && blend_unit.bottom[1];

  lowp float evy;
  if (do_darken) {
    evy = 1.0 - float(blend_control_ev.z >> 8u) * (1.0 / 16.0);
  } else {
    evy = 1.0;
  }

  lowp float eva, evb;
  if (do_blend) {
    eva = float(blend_control_ev.x >> 8u) * (1.0 / 16.0);
    evb = float(blend_control_ev.y >> 8u) * (1.0 / 16.0);
  } else {
    eva = evy;
    evb = 0.0;
  }

  return min((blend_unit.color[0] * eva) + (blend_unit.color[1] * evb), 1.0);
}

lowp vec4 BlendUnitBlend(BlendUnit blend_unit, bool enable_blend) {
  if (!enable_blend) {
    return vec4(blend_unit.color[0], 1.0);
  }

  lowp uint mode = blend_control_ev.x & 0x3u;

  lowp vec3 color;
  if (mode == 0u) {
    color = BlendUnitNoBlend(blend_unit);
  } else if (mode == 1u) {
    color = BlendUnitAdditiveBlend(blend_unit);
  } else if (mode == 2u) {
    color = BlendUnitBrighten(blend_unit);
  } else {
    color = BlendUnitDarken(blend_unit);
  }

  return vec4(color, 1.0);
}

// Window
lowp uint CheckWindow(lowp uint screen_column, lowp uint screen_row,
                      bool on_object) {
  highp uvec4 window_and_bounds =
      texelFetch(window_rows, ivec2(0, screen_row), 0);

  mediump uint window0_location =
      (screen_column + (window_and_bounds.x >> 16u)) % 240u;
  if (window0_location < window_and_bounds.y >> 16u) {
    return window_and_bounds.x;
  }

  mediump uint window1_location =
      (screen_column + (window_and_bounds.z >> 16u)) % 240u;
  if (window1_location < window_and_bounds.w >> 16u) {
    return window_and_bounds.y;
  }

  if (on_object && bool(window_and_bounds.z & 0x20u)) {
    return window_and_bounds.z;
  }

  return window_and_bounds.w;
}

// Objects
lowp uint ObjectColorIndex(highp vec2 samplecoord, lowp uint object_set,
                           highp uvec4 object) {
  mediump ivec2 canvas_top = ivec2(object.x & 0xFFFFu, object.x >> 16u);
  mediump ivec2 canvas_size = ivec2((object.y >> 16u) & 0xFFu, object.y >> 24u);
  mediump ivec2 canvas_half_size = canvas_size >> 1;
  mediump ivec2 canvas_center = canvas_top - canvas_half_size;

  highp vec2 from_center = samplecoord - vec2(canvas_center);
  highp mat2 transformation = mat2(texelFetch(
      object_transformations, ivec2((object.z >> 16u) & 0xFFu, object_set), 0));
  highp vec2 lookup_fp = transformation * from_center;

  mediump int sprite_size_x = int(object.w & 0xFFu);
  mediump int sprite_size_y = int((object.w >> 8u) & 0xFFu);
  highp vec2 sprite_half_size = vec2(sprite_size_x >> 1, sprite_size_y >> 1);

  mediump ivec2 lookup = ivec2(lookup_fp + sprite_half_size);
  if (lookup.x < 0 || lookup.x >= sprite_size_x || lookup.y < 0 ||
      lookup.y >= sprite_size_y) {
    return 0u;
  }

  if (bool(object.w & 0x80000u)) {
    lookup.x = sprite_size_x - lookup.x - 1;
  }

  if (bool(object.w & 0x100000u)) {
    lookup.y = sprite_size_y - lookup.y - 1;
  }

  lookup.x -= lookup.x % int((object.z >> 0u) & 0xFFu);
  lookup.y -= lookup.y % int((object.z >> 8u) & 0xFFu);

  mediump int tile_index;
  lowp ivec2 tile = lookup / 8;
  if (bool(object.w & 0x400000u)) {
    tile_index = tile.x + tile.y * sprite_size_x / 8;
  } else {
    lowp int shift_amount = int((object.w >> 18u) & 1u);
    tile_index = tile.x + tile.y * (32 >> shift_amount);
  }

  lowp ivec2 tile_pixel = lookup % 8;

  lowp uint color_index;
  if (bool(object.w & 0x40000u)) {
    tile_index <<= 1u;
    color_index = texelFetch(object_tiles,
                             ivec2(tile_pixel.x + 8 * (tile_pixel.y % 4),
                                   int(object.y & 0xFFFFu) + tile_index +
                                       (tile_pixel.y / 4)),
                             0)
                      .r;
  } else {
    color_index = texelFetch(object_tiles,
                             ivec2(tile_pixel.x / 2 + 4 * tile_pixel.y,
                                   int(object.y & 0xFFFFu) + tile_index),
                             0)
                      .r;
    color_index >>= 4u * (uint(tile_pixel.x) & 1u);
    color_index &= 0xFu;
  }

  return color_index;
}

// Backgrounds
BlendUnit ScrollingBackground(BlendUnit blend_unit, highp vec2 samplecoord,
                              highp uint origin, highp uint bgcnt,
                              lowp uint bg) {
  lowp uint priority = bgcnt & 0x3u;
  if (priority >= blend_unit.priority[1]) {
    return blend_unit;
  }

  mediump ivec2 bg_size = ivec2(256, 256);
  bg_size.x <<= int((bgcnt >> 14u) & 0x1u);
  bg_size.y <<= int((bgcnt >> 15u) & 0x1u);

  mediump ivec2 tilemap_pixel =
      ivec2(samplecoord) + ivec2(origin & 0xFFFFu, origin >> 16u);
  tilemap_pixel &= bg_size - 1;

  tilemap_pixel.x -= tilemap_pixel.x % int(bgcnt >> 16u & 0x1Fu);
  tilemap_pixel.y -= tilemap_pixel.y % int(bgcnt >> 24u & 0x1Fu);

  lowp ivec2 tilemap_block = tilemap_pixel / 256;
  lowp int tilemap_block_index =
      tilemap_block.x + tilemap_block.y * (bg_size.x / 256);
  mediump ivec2 tilemap_block_tile = (tilemap_pixel / 8) % 32;

  mediump ivec3 tilemap_index =
      ivec3(tilemap_block_tile.x, tilemap_block_tile.y,
            int((bgcnt >> 8u) & 0x1Fu) + tilemap_block_index);

  mediump uvec4 tilemap_entry = texelFetch(scrolling_tilemap, tilemap_index, 0);
  mediump int tileblock_offset = int(tilemap_entry.x & 0x3FFu);

  lowp ivec2 tile_pixel = tilemap_pixel % 8;
  if (bool(tilemap_entry.x & 0x400u)) {
    tile_pixel.x = 7 - tile_pixel.x;
  }

  if (bool(tilemap_entry.x & 0x800u)) {
    tile_pixel.y = 7 - tile_pixel.y;
  }

  lowp uint color_index;
  if (bool(bgcnt & 0x80u)) {
    tileblock_offset <<= 1u;
    color_index =
        texelFetch(background_tiles,
                   ivec3(tile_pixel.x + 8 * (tile_pixel.y % 4),
                         (tile_pixel.y / 4) + tileblock_offset % 512,
                         int((bgcnt >> 2u) & 0x3u) + tileblock_offset / 512),
                   0)
            .r;
    if (color_index == 0u) {
      return blend_unit;
    }
  } else {
    color_index =
        texelFetch(
            background_tiles,
            ivec3(tile_pixel.x / 2 + 4 * tile_pixel.y, tileblock_offset % 512,
                  int((bgcnt >> 2u) & 0x3u) + tileblock_offset / 512),
            0)
            .r;
    color_index >>= 4u * (uint(tile_pixel.x) & 1u);
    color_index &= 0xFu;
    if (color_index == 0u) {
      return blend_unit;
    }
    color_index += 16u * (tilemap_entry.x >> 12u);
  }

  lowp vec4 color = texelFetch(background_palette, ivec2(color_index, 0), 0);
  return BlendUnitAddBackground(blend_unit, bgcnt, bg, color.rgb);
}

mediump ivec2 AffinePixel(lowp uint bg, lowp uint screen_row,
                          highp vec2 samplecoord) {
  lowp uint column = bg - 2u;

  highp vec4 this_row =
      texelFetch(affine_coordinates, ivec2(column, screen_row), 0);
  highp vec4 next_row =
      texelFetch(affine_coordinates, ivec2(column, screen_row + 1u), 0);

  highp float interp = samplecoord.y - float(screen_row);
  highp vec2 base = mix(this_row.xy, next_row.xy, interp);
  highp vec2 scale = mix(this_row.zw, next_row.zw, interp);
  return ivec2(floor(base + scale * samplecoord.x));
}

BlendUnit AffineBackground(BlendUnit blend_unit, lowp uint screen_row,
                           highp vec2 samplecoord, highp uint bgcnt,
                           lowp uint bg) {
  lowp uint priority = bgcnt & 0x3u;
  if (priority >= blend_unit.priority[1]) {
    return blend_unit;
  }

  mediump ivec2 tilemap_pixel = AffinePixel(bg, screen_row, samplecoord);

  mediump ivec2 bg_size = ivec2(128, 128) << int((bgcnt >> 14u) & 0x3u);
  if (!bool(bgcnt & 0x2000u)) {
    if (any(lessThan(tilemap_pixel, ivec2(0, 0))) ||
        any(greaterThanEqual(tilemap_pixel, bg_size))) {
      return blend_unit;
    }
  } else {
    tilemap_pixel &= bg_size - 1;
  }

  tilemap_pixel.x -= tilemap_pixel.x % int((bgcnt) >> 16u & 0x1Fu);
  tilemap_pixel.y -= tilemap_pixel.y % int((bgcnt) >> 24u & 0x1Fu);

  lowp ivec2 tile = tilemap_pixel / 8;
  lowp ivec2 tile_pixel = tilemap_pixel % 8;
  mediump int tile_offset = tile.x + tile.y * (bg_size.x / 8);

  mediump uint t_index =
      texelFetch(affine_tilemap,
                 ivec3(tile_offset % 64, (tile_offset % 2048) / 64,
                       int((bgcnt >> 8u) & 0x1Fu) + tile_offset / 2048),
                 0)
          .r;

  lowp uint index = texelFetch(background_tiles,
                               ivec3(tile_pixel.x + 8 * (tile_pixel.y % 4),
                                     int(t_index << 1u) + (tile_pixel.y / 4),
                                     int((bgcnt >> 2u) & 0x3u)),
                               0)
                        .r;
  if (index == 0u) {
    return blend_unit;
  }

  lowp vec4 color = texelFetch(background_palette, ivec2(index, 0), 0);
  return BlendUnitAddBackground(blend_unit, bgcnt, bg, color.rgb);
}

BlendUnit BitmapBackground(BlendUnit blend_unit, lowp uint screen_row,
                           highp vec2 samplecoord, ivec2 size,
                           highp uint bgcnt) {
  mediump ivec2 lookup = AffinePixel(2u, screen_row, samplecoord);
  if (any(lessThan(lookup, ivec2(0, 0))) ||
      any(greaterThanEqual(lookup, size))) {
    return blend_unit;
  }
  lookup.x -= lookup.x % int(bgcnt >> 16u & 0x1Fu);
  lookup.y -= lookup.y % int(bgcnt >> 24u & 0x1Fu);
  lowp vec4 color = texelFetch(bitmap, lookup, 0);
  return BlendUnitAddBackground(blend_unit, bgcnt, 2u, color.rgb);
}

BlendUnit PaletteBitmapBackground(BlendUnit blend_unit, lowp uint screen_row,
                                  highp vec2 samplecoord, highp uint bgcnt) {
  mediump ivec2 lookup = AffinePixel(2u, screen_row, samplecoord);
  if (any(lessThan(lookup, ivec2(0, 0))) ||
      any(greaterThanEqual(lookup, ivec2(240, 160)))) {
    return blend_unit;
  }
  lookup.x -= lookup.x % int(bgcnt >> 16u & 0x1Fu);
  lookup.y -= lookup.y % int(bgcnt >> 24u & 0x1Fu);
  lowp uint index = texelFetch(palette_bitmap, lookup, 0).r;
  if (index == 0u) {
    return blend_unit;
  }

  lowp vec4 color = texelFetch(background_palette, ivec2(index, 0), 0);
  return BlendUnitAddBackground(blend_unit, bgcnt, 2u, color.rgb);
}

// Bit Set
const lowp uint clz_lut[32] = uint[32](
    0u, 1u, 28u, 2u, 29u, 14u, 24u, 3u, 30u, 22u, 20u, 15u, 25u, 17u, 4u, 8u,
    31u, 27u, 13u, 23u, 21u, 19u, 16u, 7u, 26u, 12u, 18u, 6u, 11u, 5u, 10u, 9u);

lowp uint CountTrailingZeroes32(highp uint value) {
  const highp uint debrujin_sequence = 0x077CB531u;
  return clz_lut[((value & -value) * debrujin_sequence) >> 27u];
}

lowp uint CountTrailingZeroes(highp uvec4 value) {
  if (value.x != 0u) {
    return CountTrailingZeroes32(value.x);
  }
  if (value.y != 0u) {
    return 32u + CountTrailingZeroes32(value.y);
  }
  if (value.z != 0u) {
    return 64u + CountTrailingZeroes32(value.z);
  }
  return 96u + CountTrailingZeroes32(value.w);
}

highp uvec4 FlipBit(highp uvec4 value, lowp uint index) {
  if (index < 32u) {
    value.x ^= (1u << index);
  } else if (index < 64u) {
    value.y ^= (1u << (index - 32u));
  } else if (index < 96u) {
    value.z ^= (1u << (index - 64u));
  } else {
    value.w ^= (1u << (index - 96u));
  }
  return value;
}

bool NotEmpty(highp uvec4 value) {
  const uvec4 zero = uvec4(0u, 0u, 0u, 0u);
  return any(notEqual(value, zero));
}

// Entry Point
void main() {
  lowp uint screen_row = 159u - uint(gl_FragCoord.y) / render_scale;
  lowp uint screen_column = uint(gl_FragCoord.x) / render_scale;

  BlendUnit blend_unit = CreateBlendUnit(screen_row);

  highp vec2 samplecoord =
      vec2(gl_FragCoord.x, float(160u * render_scale) - gl_FragCoord.y);
  samplecoord = max(floor(samplecoord) / float(render_scale),
                    floor(samplecoord / float(render_scale)));

  bool on_object_window = false;

#if OBJECTS != 0
  lowp uint object_set =
      texelFetch(object_indicies_masks, ivec2(0, screen_row), 0).r;

  highp uvec4 row_objects =
      texelFetch(object_rows, ivec2(screen_row, object_set), 0);
  highp uvec4 column_objects =
      texelFetch(object_columns, ivec2(screen_column, object_set), 0);
  highp uvec4 visible_objects = row_objects & column_objects;

  highp uvec4 window_mask =
      texelFetch(object_indicies_masks, ivec2(1, object_set), 0);
  highp uvec4 window_objects = visible_objects & window_mask;
  while (NotEmpty(window_objects)) {
    lowp uint obj = CountTrailingZeroes(window_objects);
    window_objects = FlipBit(window_objects, obj);

    highp uvec4 object =
        texelFetch(object_attributes, ivec2(obj, object_set), 0);
    lowp uint color_index = ObjectColorIndex(samplecoord, object_set, object);
    if (color_index != 0u) {
      on_object_window = true;
      break;
    }
  }
#endif  // OBJECTS != 0

  lowp uint window = CheckWindow(screen_column, screen_row, on_object_window);

#if OBJECTS != 0
  if (bool(window & 0x10u)) {
    highp uvec4 drawn_mask =
        texelFetch(object_indicies_masks, ivec2(2, object_set), 0);
    highp uvec4 drawn_objects = visible_objects & drawn_mask;
    while (NotEmpty(drawn_objects)) {
      lowp uint obj = CountTrailingZeroes(drawn_objects);
      drawn_objects = FlipBit(drawn_objects, obj);

      highp uvec4 object =
          texelFetch(object_attributes, ivec2(obj, object_set), 0);
      lowp uint color_index = ObjectColorIndex(samplecoord, object_set, object);
      if (color_index != 0u) {
        lowp vec4 color = texelFetch(
            object_palette, ivec2((object.z >> 24u) + color_index, 0), 0);
        blend_unit =
            BlendUnitAddObject(blend_unit, color.rgb, (object.w >> 16u) & 0x3u,
                               bool(object.w & 0x200000u));
        break;
      }
    }
  }
#endif  // OBJECTS != 0

#if SCROLLING_BACKGROUND_0 != 0 || SCROLLING_BACKGROUND_1 != 0 || SCROLLING_BACKGROUND_2 != 0 || SCROLLING_BACKGROUND_3 != 0
  uvec4 origins = texelFetch(scrolling_coordinates, ivec2(0, screen_row), 0);
#endif  // SCROLLING_BACKGROUND_0 != 0 || SCROLLING_BACKGROUND_1 != 0 || SCROLLING_BACKGROUND_2 != 0 || SCROLLING_BACKGROUND_3 != 0

#if SCROLLING_BACKGROUND_0 != 0
  if (bool(window & 0x1u)) {
    blend_unit =
        ScrollingBackground(blend_unit, samplecoord, origins[0],
                            background_control[0], 0u);
  }
#endif  // SCROLLING_BACKGROUND_0 != 0

#if SCROLLING_BACKGROUND_1 != 0
  if (bool(window & 0x2u)) {
    blend_unit =
        ScrollingBackground(blend_unit, samplecoord, origins[1],
                            background_control[1], 1u);
  }
#endif  // SCROLLING_BACKGROUND_1 != 0

#if SCROLLING_BACKGROUND_2 != 0
  if (bool(window & 0x4u)) {
    blend_unit =
        ScrollingBackground(blend_unit, samplecoord, origins[2],
                            background_control[2], 2u);
  }
#endif  // SCROLLING_BACKGROUND_2 != 0

#if SCROLLING_BACKGROUND_3 != 0
  if (bool(window & 0x8u)) {
    blend_unit =
        ScrollingBackground(blend_unit, samplecoord, origins[3],
                            background_control[3], 3u);
  }
#endif  // SCROLLING_BACKGROUND_3 != 0

#if AFFINE_BACKGROUND_2 != 0
  if (bool(window & 0x4u)) {
    blend_unit =
        AffineBackground(blend_unit, screen_row, samplecoord,
                         background_control[2], 2u);
  }
#endif  // AFFINE_BACKGROUND_2 != 0

#if AFFINE_BACKGROUND_3 != 0
  if (bool(window & 0x8u)) {
    blend_unit =
        AffineBackground(blend_unit, screen_row, samplecoord,
                         background_control[3], 3u);
  }
#endif  // AFFINE_BACKGROUND_3 != 0

#if SMALL_BITMAP_BACKGROUND != 0
  if (bool(window & 0x4u)) {
    blend_unit = BitmapBackground(blend_unit, screen_row, samplecoord,
                                  ivec2(160, 128), background_control[2]);
  }
#endif  // BITMAP_BACKGROUND != 0

#if LARGE_BITMAP_BACKGROUND != 0
  if (bool(window & 0x4u)) {
    blend_unit = BitmapBackground(blend_unit, screen_row, samplecoord,
                                  ivec2(240, 160), background_control[2]);
  }
#endif  // BITMAP_BACKGROUND != 0

#if PALETTE_BITMAP_BACKGROUND != 0
  if (bool(window & 0x4u)) {
    blend_unit =
        PaletteBitmapBackground(blend_unit, screen_row, samplecoord,
                                background_control[2]);
  }
#endif  // PALETTE_BITMAP_BACKGROUND != 0

  lowp vec4 backdrop = texelFetch(background_palette, ivec2(0, 0), 0);
  blend_unit = BlendUnitAddBackdrop(blend_unit, backdrop.rgb);

  frag_color = BlendUnitBlend(blend_unit, bool(window & 0x20u));
}