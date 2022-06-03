#version 300 es

#define OBJECTS 0
#define SCROLLING_BACKGROUND_0 0
#define SCROLLING_BACKGROUND_1 0
#define SCROLLING_BACKGROUND_2 0
#define SCROLLING_BACKGROUND_3 0
#define AFFINE_BACKGROUND_2 0
#define AFFINE_BACKGROUND_3 0
#define BITMAP_BACKGROUND 0
#define PALETTE_BITMAP_BACKGROUND 0

// Inputs
in mediump vec2 scrolling_screencoord[4];
in mediump vec2 affine_screencoord[2];
in mediump vec2 screencoord;

// Outputs
out lowp vec4 frag_color;

// Palettes
layout(std140) uniform BackgroundPalette { lowp vec4 background_palette[256]; };
layout(std140) uniform ObjectPalette { lowp vec4 object_palette[256]; };

// Tiles
uniform lowp usampler2D background_tiles;
uniform lowp usampler2D object_tiles;

// Tilemaps
uniform mediump isampler2D affine_tilemap;
uniform mediump isampler2D scrolling_tilemap;

// Background Bitmaps
uniform lowp sampler2D bitmap;
uniform lowp usampler2D palette_bitmap;

// Objects
struct Object {
  mediump mat2 transformation;
  mediump vec2 center;
  mediump ivec2 half_size;
  lowp ivec2 mosaic;
  lowp ivec2 flip;
  mediump int tile_base;
  lowp uint palette;
  lowp uint priority;
  bool semi_transparent;
  bool large_palette;
  bool window;
};

layout(std140) uniform Objects {
  highp uvec4 object_columns[240];
  highp uvec4 object_rows[160];
  Object objects[128];
};

uniform bool obj_mode;

// Backgrounds
struct Background {
  mediump ivec2 size;
  lowp ivec2 mosaic;
  mediump int tilemap_base;
  mediump int tile_base;
  lowp uint priority;
  bool large_palette;
  bool wraparound;
};

layout(std140) uniform Backgrounds { Background backgrounds[4]; };

// Blend Unit
uniform int blend_mode;
uniform lowp float blend_eva;
uniform lowp float blend_evb;
uniform lowp float blend_evy;
uniform bool obj_top;
uniform bool obj_bottom;
uniform bool bg_top[5];
uniform bool bg_bottom[5];

struct BlendUnit {
  lowp vec3 color[2];
  lowp uint priority[2];
  bool top[2];
  bool bottom[2];
  bool semi_transparent[2];
};

BlendUnit CreateBlendUnit() {
  BlendUnit result;
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

BlendUnit BlendUnitAddObject(BlendUnit blend_unit, lowp vec4 color,
                             lowp uint priority, bool blended) {
  if (color.a != 0.0) {
    blend_unit.color[0] = color.rgb;
    blend_unit.priority[0] = priority;
    blend_unit.top[0] = blended || obj_top;
    blend_unit.bottom[0] = obj_bottom;
    blend_unit.semi_transparent[0] = blended;
  }
  return blend_unit;
}

BlendUnit BlendUnitAddBackground(BlendUnit blend_unit, lowp uint bg,
                                 lowp vec4 color, lowp uint priority) {
  if (color.a != 0.0) {
    if (priority < blend_unit.priority[1]) {
      blend_unit.color[1] = color.rgb;
      blend_unit.priority[1] = priority;
      blend_unit.top[1] = bg_top[bg];
      blend_unit.bottom[1] = bg_bottom[bg];
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
    }
  }
  return blend_unit;
}

BlendUnit BlendUnitAddBackdrop(BlendUnit blend_unit, lowp vec4 color) {
  return BlendUnitAddBackground(blend_unit, 4u, color, 5u);
}

lowp vec3 BlendUnitNoBlend(BlendUnit blend_unit) {
  bool do_blend =
      (blend_unit.semi_transparent[0] || blend_unit.semi_transparent[1]) &&
      blend_unit.top[0] && blend_unit.bottom[1];

  lowp float eva = do_blend ? blend_eva : 1.0;
  lowp float evb = do_blend ? blend_evb : 0.0;

  return min((blend_unit.color[0] * eva) + (blend_unit.color[1] * evb), 1.0);
}

lowp vec3 BlendUnitAdditiveBlend(BlendUnit blend_unit) {
  bool do_blend = blend_unit.top[0] && blend_unit.bottom[1];
  lowp float eva = do_blend ? blend_eva : 1.0;
  lowp float evb = do_blend ? blend_evb : 0.0;

  return min((blend_unit.color[0] * eva) + (blend_unit.color[1] * evb), 1.0);
}

lowp vec3 BlendUnitBrighten(BlendUnit blend_unit) {
  bool do_brighten = blend_unit.top[0];
  bool do_blend =
      (blend_unit.semi_transparent[0] || blend_unit.semi_transparent[1]) &&
      blend_unit.top[0] && blend_unit.bottom[1];

  lowp float eva = do_blend ? blend_eva : 1.0;
  lowp float evy = do_brighten ? blend_evy : 0.0;
  lowp float evb = do_blend ? blend_evb : evy;
  lowp vec3 bottom = do_blend ? blend_unit.color[1] : 1.0 - blend_unit.color[0];

  return min((blend_unit.color[0] * eva) + (bottom * evb), 1.0);
}

lowp vec3 BlendUnitDarken(BlendUnit blend_unit) {
  bool do_darken = blend_unit.top[0];
  bool do_blend =
      (blend_unit.semi_transparent[0] || blend_unit.semi_transparent[1]) &&
      blend_unit.top[0] && blend_unit.bottom[1];

  lowp float evy = do_darken ? (1.0 - blend_evy) : 1.0;
  lowp float eva = do_blend ? blend_eva : evy;
  lowp float evb = do_blend ? blend_evb : 0.0;

  return min((blend_unit.color[0] * eva) + (blend_unit.color[1] * evb), 1.0);
}

lowp vec4 BlendUnitBlend(BlendUnit blend_unit, bool enable_blend) {
  lowp vec3 color;
  if (!enable_blend) {
    color = blend_unit.color[0];
  } else if (blend_mode == 0) {
    color = BlendUnitNoBlend(blend_unit);
  } else if (blend_mode == 1) {
    color = BlendUnitAdditiveBlend(blend_unit);
  } else if (blend_mode == 2) {
    color = BlendUnitBrighten(blend_unit);
  } else {
    color = BlendUnitDarken(blend_unit);
  }

  return vec4(color.bgr, 1.0);
}

// Window
struct WindowContents {
  bool bg[4];
  bool obj;
  bool bld;
};

uniform bool win0_enabled;
uniform bool win1_enabled;
uniform bool winobj_enabled;

uniform WindowContents win0;
uniform WindowContents win1;
uniform WindowContents winobj;
uniform WindowContents winout;

uniform highp vec2 win0_start;
uniform highp vec2 win0_end;
uniform highp vec2 win1_start;
uniform highp vec2 win1_end;

bool IsInsideWindow1D(highp float start, highp float end,
                      highp float location) {
  bool between = start <= location && location <= end;
  return (start < end && between) || (start >= end && !between);
}

bool IsInsideWindow2D(highp vec2 start, highp vec2 end, highp vec2 location) {
  return IsInsideWindow1D(start.x, end.x, location.x) &&
         IsInsideWindow1D(start.y, end.y, location.y);
}

WindowContents CheckWindow(bool on_object) {
  bool use_win0 =
      win0_enabled && IsInsideWindow2D(win0_start, win0_end, screencoord);
  bool done = use_win0;

  bool use_win1 = !done && win1_enabled &&
                  IsInsideWindow2D(win1_start, win1_end, screencoord);
  done = done || use_win1;

  bool use_winobj = !done && winobj_enabled && on_object;
  done = done || use_winobj;

  bool use_winout = !done && (win0_enabled || win1_enabled || winobj_enabled);
  done = done || use_winout;

  WindowContents result;
  result.bg[0] = (use_win0 && win0.bg[0]) || (use_win1 && win1.bg[0]) ||
                 (use_winobj && winobj.bg[0]) || (use_winout && winout.bg[0]) ||
                 (!done && true);
  result.bg[1] = (use_win0 && win0.bg[1]) || (use_win1 && win1.bg[1]) ||
                 (use_winobj && winobj.bg[1]) || (use_winout && winout.bg[1]) ||
                 (!done && true);
  result.bg[2] = (use_win0 && win0.bg[2]) || (use_win1 && win1.bg[2]) ||
                 (use_winobj && winobj.bg[2]) || (use_winout && winout.bg[2]) ||
                 (!done && true);
  result.bg[3] = (use_win0 && win0.bg[3]) || (use_win1 && win1.bg[3]) ||
                 (use_winobj && winobj.bg[3]) || (use_winout && winout.bg[3]) ||
                 (!done && true);
  result.obj = (use_win0 && win0.obj) || (use_win1 && win1.obj) ||
               (use_winobj && winobj.obj) || (use_winout && winout.obj) ||
               (!done && true);
  result.bld = (use_win0 && win0.bld) || (use_win1 && win1.bld) ||
               (use_winobj && winobj.bld) || (use_winout && winout.bld) ||
               (!done && true);

  return result;
}

// Objects
lowp uint ObjectColorIndex(lowp uint obj) {
  mediump ivec2 lookup =
      ivec2(objects[obj].transformation * (screencoord - objects[obj].center));
  if (any(lessThan(lookup, -objects[obj].half_size)) ||
      any(greaterThanEqual(lookup, objects[obj].half_size))) {
    return 0u;
  }

  lookup = lookup * objects[obj].flip + objects[obj].half_size;
  lookup -= lookup % objects[obj].mosaic;

  mediump int tile_index;
  lowp ivec2 tile = lookup / 8;
  if (obj_mode) {
    tile_index = tile.x + tile.y * objects[obj].half_size.x / 4;
  } else {
    lowp int row_width = objects[obj].large_palette ? 16 : 32;
    tile_index = tile.x + tile.y * row_width;
  }

  lowp ivec2 tile_pixel = lookup % 8;
  lowp int tile_height = objects[obj].large_palette ? 16 : 8;
  lowp uvec4 color_indices = texelFetch(
      object_tiles,
      ivec2(tile_pixel.x,
            objects[obj].tile_base + tile_height * tile_index + tile_pixel.y),
      0);
  return objects[obj].large_palette ? color_indices.r : color_indices.g;
}

// Backgrounds
lowp vec4 ScrollingBackground(lowp uint bg) {
  highp ivec2 tilemap_pixel = ivec2(scrolling_screencoord[bg]);
  tilemap_pixel &= backgrounds[bg].size - 1;
  tilemap_pixel -= tilemap_pixel % backgrounds[bg].mosaic;

  lowp ivec2 tilemap_block = tilemap_pixel / 256;
  lowp int tilemap_block_index =
      tilemap_block.x + tilemap_block.y * (backgrounds[bg].size.x / 256);

  mediump ivec2 tilemap_block_tile = (tilemap_pixel / 8) % 32;
  mediump ivec2 tilemap_index =
      ivec2(tilemap_block_tile.x, backgrounds[bg].tilemap_base / 64 +
                                      32 * tilemap_block_index +
                                      tilemap_block_tile.y);

  mediump ivec4 tilemap_entry = texelFetch(scrolling_tilemap, tilemap_index, 0);
  mediump int tileblock_offset =
      tilemap_entry.x >> int(backgrounds[bg].large_palette);
  lowp ivec2 flip = tilemap_entry.yz;
  lowp uint palette = uint(tilemap_entry.w);

  lowp ivec2 tile_pixel = abs(flip - (tilemap_pixel % 8));

  lowp uvec4 color_indices =
      texelFetch(background_tiles,
                 ivec2(tile_pixel.x, backgrounds[bg].tile_base +
                                         tileblock_offset * 8 + tile_pixel.y),
                 0);

  lowp uint color_index =
      backgrounds[bg].large_palette ? color_indices.r : color_indices.g;

  lowp uint palette_base = backgrounds[bg].large_palette ? 0u : palette;
  lowp vec4 color = background_palette[palette_base + color_index];
  return vec4(color.rgb, float(color_index != 0u));
}

lowp vec4 AffineBackground(lowp uint bg) {
  highp ivec2 tilemap_pixel = ivec2(affine_screencoord[bg - 2u]);
  tilemap_pixel &= backgrounds[bg].size - 1;
  tilemap_pixel -= tilemap_pixel % backgrounds[bg].mosaic;

  lowp ivec2 tile = tilemap_pixel / 8;
  lowp ivec2 tile_pixel = tilemap_pixel % 8;

  mediump int tile_offset = tile.x + tile.y * (backgrounds[bg].size.x / 8);
  mediump int tile_index = backgrounds[bg].tilemap_base + tile_offset;

  mediump int index =
      texelFetch(affine_tilemap, ivec2(tile_index % 256, tile_index / 256), 0)
          .r;

  lowp uint color_index =
      texelFetch(background_tiles,
                 ivec2(tile_pixel.x,
                       backgrounds[bg].tile_base + index * 8 + tile_pixel.y),
                 0)
          .r;
  lowp vec4 color = background_palette[color_index];
  return vec4(color.rgb, float(color_index != 0u));
}

lowp vec4 BitmapBackground() {
  highp ivec2 lookup = ivec2(affine_screencoord[0]);
  if (any(lessThan(lookup, ivec2(0, 0))) ||
      any(greaterThan(lookup, backgrounds[2].size))) {
    return vec4(0.0, 0.0, 0.0, 0.0);
  }
  lookup -= lookup % backgrounds[2].mosaic;
  return texelFetch(bitmap, lookup, 0);
}

lowp vec4 PaletteBitmapBackground() {
  highp ivec2 lookup = ivec2(affine_screencoord[0]);
  if (any(lessThan(lookup, ivec2(0, 0))) ||
      any(greaterThan(lookup, backgrounds[2].size))) {
    return vec4(0.0, 0.0, 0.0, 0.0);
  }
  lookup -= lookup % backgrounds[2].mosaic;
  lowp uint index = texelFetch(palette_bitmap, lookup, 0).r;
  return background_palette[index];
}

// Count Trailing Zeroes
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
  BlendUnit blend_unit = CreateBlendUnit();

#if OBJECTS != 0
  lowp vec4 obj_color = vec4(0.0, 0.0, 0.0, 0.0);
  lowp uint obj_priority = 5u;
  bool obj_blended = false;
  bool on_object_window = false;

  highp uvec4 visible_objects =
      object_columns[uint(screencoord.x)] & object_rows[uint(screencoord.y)];

  while (NotEmpty(visible_objects)) {
    lowp uint obj = CountTrailingZeroes(visible_objects);
    visible_objects = FlipBit(visible_objects, obj);

    lowp uint color_index = ObjectColorIndex(obj);
    if (color_index == 0u) {
      continue;
    }

    if (objects[obj].window) {
      on_object_window = true;
      continue;
    }

    lowp uint palette =
        objects[obj].large_palette ? 0u : objects[obj].palette;
    lowp vec4 color = object_palette[palette + color_index];

    if (objects[obj].priority < obj_priority) {
      obj_color = color;
      obj_priority = objects[obj].priority;
      obj_blended = objects[obj].semi_transparent;
    }
  }
#else
  bool on_object_window = false;
#endif  // OBJECTS != 0

  WindowContents window = CheckWindow(on_object_window);

#if OBJECTS != 0
  if (window.obj) {
    blend_unit =
        BlendUnitAddObject(blend_unit, obj_color, obj_priority, obj_blended);
  }
#endif  // OBJECTS != 0

#if SCROLLING_BACKGROUND_0 != 0
  if (window.bg[0]) {
    lowp vec4 color = ScrollingBackground(0u);
    blend_unit =
        BlendUnitAddBackground(blend_unit, 0u, color, backgrounds[0].priority);
  }
#endif  // SCROLLING_BACKGROUND_0 != 0

#if SCROLLING_BACKGROUND_1 != 0
  if (window.bg[1]) {
    lowp vec4 color = ScrollingBackground(1u);
    blend_unit =
        BlendUnitAddBackground(blend_unit, 1u, color, backgrounds[1].priority);
  }
#endif  // SCROLLING_BACKGROUND_1 != 0

#if SCROLLING_BACKGROUND_2 != 0
  if (window.bg[2]) {
    lowp vec4 color = ScrollingBackground(2u);
    blend_unit =
        BlendUnitAddBackground(blend_unit, 2u, color, backgrounds[2].priority);
  }
#endif  // SCROLLING_BACKGROUND_2 != 0

#if SCROLLING_BACKGROUND_3 != 0
  if (window.bg[3]) {
    lowp vec4 color = ScrollingBackground(3u);
    blend_unit =
        BlendUnitAddBackground(blend_unit, 3u, color, backgrounds[3].priority);
  }
#endif  // SCROLLING_BACKGROUND_3 != 0

#if AFFINE_BACKGROUND_2 != 0
  if (window.bg[2]) {
    lowp vec4 color = AffineBackground(2u);
    blend_unit =
        BlendUnitAddBackground(blend_unit, 2u, color, backgrounds[2].priority);
  }
#endif  // AFFINE_BACKGROUND_2 != 0

#if AFFINE_BACKGROUND_3 != 0
  if (window.bg[3]) {
    lowp vec4 color = AffineBackground(3u);
    blend_unit =
        BlendUnitAddBackground(blend_unit, 3u, color, backgrounds[3].priority);
  }
#endif  // AFFINE_BACKGROUND_2 != 0

#if BITMAP_BACKGROUND != 0
  if (window.bg[2]) {
    lowp vec4 color = BitmapBackground();
    blend_unit =
        BlendUnitAddBackground(blend_unit, 2u, color, backgrounds[2].priority);
  }
#endif  // BITMAP_BACKGROUND != 0

#if PALETTE_BITMAP_BACKGROUND != 0
  if (window.bg[2]) {
    lowp vec4 color = PaletteBitmapBackground();
    blend_unit =
        BlendUnitAddBackground(blend_unit, 2u, color, backgrounds[2].priority);
  }
#endif  // PALETTE_BITMAP_BACKGROUND != 0

  blend_unit = BlendUnitAddBackdrop(blend_unit, background_palette[0]);

  frag_color = BlendUnitBlend(blend_unit, window.bld);
}