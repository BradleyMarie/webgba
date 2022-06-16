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
in mediump vec2 samplecoord;
in mediump vec2 screencoord;

// Outputs
out lowp vec4 frag_color;

// Uniforms
uniform highp float render_scale;

// Palettes
layout(std140) uniform BackgroundPalette { lowp vec4 background_palette[256]; };
layout(std140) uniform ObjectPalette { lowp vec4 object_palette[256]; };

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
};

layout(std140) uniform Objects {
  Object objects[128];
  bool object_linear_tiles;
};

// Object Visibility
layout(std140) uniform ObjectVisibility {
  highp uvec4 object_columns[240];
  highp uvec4 object_rows[160];
  lowp uint object_indices[128];
  highp uvec4 object_window;
  highp uvec4 object_drawn;
};

// Backgrounds
struct Background {
  mediump ivec2 size;
  lowp ivec2 mosaic;
  mediump int tilemap_base;
  lowp int tile_base;
  lowp uint priority;
  bool large_palette;
  bool wraparound;
};

layout(std140) uniform Backgrounds { Background backgrounds[4]; };

// Background Coordinates
struct ScrollingRow {
  mediump vec2 origins[4];
};

layout(std140) uniform ScrollingBackgrounds {
  ScrollingRow scrolling_rows[160];
};

struct AffineRow {
  mediump vec2 bases[2];
  mediump vec2 scale[2];
};

layout(std140) uniform AffineBackgrounds { AffineRow affine_rows[161]; };

// Window
struct Window {
  bool obj;
  bool bg0;
  bool bg1;
  bool bg2;
  bool bg3;
  bool bld;
};

layout(std140) uniform Windows {
  Window window0;
  Window window1;
  Window window_object;
  Window window_outside;
  mediump uvec2 window_shift[2];
  mediump uvec2 window_bounds[2];
  bool window_object_enabled;
};

// Blend
struct BlendRow {
  lowp uvec3 bldcnt;  // mode, top, bottom
  lowp vec3 ev;       // eva, evb, evy
};

layout(std140) uniform Blend { BlendRow blend_rows[160]; };

// Blend Unit
struct BlendUnit {
  lowp vec3 color[2];
  lowp uint priority[2];
  bool top[2];
  bool bottom[2];
  bool semi_transparent[2];
};

BlendUnit CreateBlendUnit() {
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
  blend_unit.top[0] =
      blended || bool(blend_rows[int(screencoord.y)].bldcnt.y & 0x10u);
  blend_unit.bottom[0] = bool(blend_rows[int(screencoord.y)].bldcnt.z & 0x10u);
  blend_unit.semi_transparent[0] = blended;
  return blend_unit;
}

BlendUnit BlendUnitAddBackground(BlendUnit blend_unit, lowp uint bg,
                                 lowp vec3 color) {
  if (backgrounds[bg].priority < blend_unit.priority[1]) {
    blend_unit.color[1] = color.bgr;
    blend_unit.priority[1] = backgrounds[bg].priority;
    blend_unit.top[1] =
        bool(blend_rows[int(screencoord.y)].bldcnt.y & (1u << bg));
    blend_unit.bottom[1] =
        bool(blend_rows[int(screencoord.y)].bldcnt.z & (1u << bg));
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
  return blend_unit;
}

BlendUnit BlendUnitAddBackdrop(BlendUnit blend_unit, lowp vec3 color) {
  if (5u < blend_unit.priority[1]) {
    if (5u < blend_unit.priority[0]) {
      blend_unit.color[0] = color.bgr;
      blend_unit.top[0] = bool(blend_rows[int(screencoord.y)].bldcnt.y & 0x20u);
      blend_unit.bottom[0] =
          bool(blend_rows[int(screencoord.y)].bldcnt.z & 0x20u);
      blend_unit.semi_transparent[0] = false;
    } else {
      blend_unit.color[1] = color.bgr;
      blend_unit.top[1] = bool(blend_rows[int(screencoord.y)].bldcnt.y & 0x20u);
      blend_unit.bottom[1] =
          bool(blend_rows[int(screencoord.y)].bldcnt.z & 0x20u);
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
    eva = blend_rows[int(screencoord.y)].ev.x;
    evb = blend_rows[int(screencoord.y)].ev.y;
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
    eva = blend_rows[int(screencoord.y)].ev.x;
    evb = blend_rows[int(screencoord.y)].ev.y;
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
    evy = blend_rows[int(screencoord.y)].ev.z;
  } else {
    evy = 0.0;
  }

  lowp vec3 bottom;
  lowp float eva, evb;
  if (do_blend) {
    eva = blend_rows[int(screencoord.y)].ev.x;
    evb = blend_rows[int(screencoord.y)].ev.y;
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
    evy = 1.0 - blend_rows[int(screencoord.y)].ev.z;
  } else {
    evy = 1.0;
  }

  lowp float eva, evb;
  if (do_blend) {
    eva = blend_rows[int(screencoord.y)].ev.x;
    evb = blend_rows[int(screencoord.y)].ev.y;
  } else {
    eva = evy;
    evb = 0.0;
  }

  return min((blend_unit.color[0] * eva) + (blend_unit.color[1] * evb), 1.0);
}

lowp vec4 BlendUnitBlend(BlendUnit blend_unit, bool enable_blend) {
  lowp vec3 color;
  if (!enable_blend) {
    color = blend_unit.color[0];
  } else if (blend_rows[int(screencoord.y)].bldcnt.x == 0u) {
    color = BlendUnitNoBlend(blend_unit);
  } else if (blend_rows[int(screencoord.y)].bldcnt.x == 1u) {
    color = BlendUnitAdditiveBlend(blend_unit);
  } else if (blend_rows[int(screencoord.y)].bldcnt.x == 2u) {
    color = BlendUnitBrighten(blend_unit);
  } else {
    color = BlendUnitDarken(blend_unit);
  }

  return vec4(color, 1.0);
}

// Window
Window CheckWindow(bool on_object) {
  const mediump uvec2 screen_size = uvec2(240u, 160u);
  mediump uvec2 pixel = uvec2(screencoord);

  mediump uvec2 window0_location = (pixel + window_shift[0]) % screen_size;
  if (all(lessThan(window0_location, window_bounds[0]))) {
    return window0;
  }

  mediump uvec2 window1_location = (pixel + window_shift[1]) % screen_size;
  if (all(lessThan(window1_location, window_bounds[1]))) {
    return window1;
  }

  if (on_object && window_object_enabled) {
    return window_object;
  }

  return window_outside;
}

// Objects
lowp uint ObjectColorIndex(lowp uint obj) {
  mediump vec2 lookup_fp =
      objects[obj].transformation * (screencoord - objects[obj].center);
  mediump ivec2 lookup = ivec2(floor(lookup_fp));
  if (any(lessThan(lookup, -objects[obj].half_size)) ||
      any(greaterThanEqual(lookup, objects[obj].half_size))) {
    return 0u;
  }

  lookup = abs(objects[obj].flip - (lookup + objects[obj].half_size));
  lookup -= lookup % objects[obj].mosaic;

  mediump int tile_index;
  lowp ivec2 tile = lookup / 8;
  if (object_linear_tiles) {
    tile_index = tile.x + tile.y * objects[obj].half_size.x / 4;
  } else {
    lowp int row_width;
    if (objects[obj].large_palette) {
      row_width = 16;
    } else {
      row_width = 32;
    }
    tile_index = tile.x + tile.y * row_width;
  }

  if (objects[obj].large_palette) {
    tile_index <<= 1;
  }

  lowp ivec2 tile_pixel = lookup % 8;
  lowp uvec4 color_indices =
      texelFetch(object_tiles,
                 ivec2(tile_pixel.x + 8 * tile_pixel.y,
                       objects[obj].tile_base + tile_index),
                 0);

  if (objects[obj].large_palette) {
    return color_indices.r;
  }

  return color_indices.g;
}

// Backgrounds
BlendUnit ScrollingBackground(BlendUnit blend_unit, lowp uint bg) {
  mediump ivec2 tilemap_pixel =
      ivec2(screencoord + scrolling_rows[int(screencoord.y)].origins[bg]);
  tilemap_pixel &= backgrounds[bg].size - 1;
  tilemap_pixel -= tilemap_pixel % backgrounds[bg].mosaic;

  lowp ivec2 tilemap_block = tilemap_pixel / 256;
  lowp int tilemap_block_index =
      tilemap_block.x + tilemap_block.y * (backgrounds[bg].size.x / 256);
  mediump ivec2 tilemap_block_tile = (tilemap_pixel / 8) % 32;

  mediump ivec3 tilemap_index =
      ivec3(tilemap_block_tile.x, tilemap_block_tile.y,
            backgrounds[bg].tilemap_base + tilemap_block_index);

  mediump uvec4 tilemap_entry = texelFetch(scrolling_tilemap, tilemap_index, 0);
  mediump int tileblock_offset = int(tilemap_entry.x);
  if (backgrounds[bg].large_palette) {
    tileblock_offset <<= 1;
  }

  lowp ivec2 flip = ivec2(tilemap_entry.yz);
  lowp uint palette = tilemap_entry.w;

  lowp ivec2 tile_pixel = abs(flip - (tilemap_pixel % 8));

  lowp uvec4 color_indices =
      texelFetch(background_tiles,
                 ivec3(tile_pixel.x + 8 * tile_pixel.y, tileblock_offset % 512,
                       backgrounds[bg].tile_base + tileblock_offset / 512),
                 0);

  lowp uint color_index;
  if (backgrounds[bg].large_palette) {
    color_index = color_indices.r;
  } else {
    color_index = color_indices.g;
  }

  if (color_index == 0u) {
    return blend_unit;
  }

  lowp uint palette_base;
  if (backgrounds[bg].large_palette) {
    palette_base = 0u;
  } else {
    palette_base = palette;
  }

  lowp uint color = palette_base + color_index;
  return BlendUnitAddBackground(blend_unit, bg, background_palette[color].rgb);
}

mediump ivec2 AffinePixel(lowp uint bg) {
  mediump vec2 pixel = floor(samplecoord) / render_scale;
  mediump float interp = mod(pixel.y, 1.0);
  lowp uint row = uint(pixel.y);
  mediump vec2 base = mix(affine_rows[row].bases[bg - 2u],
                          affine_rows[row + 1u].bases[bg - 2u], interp);
  mediump vec2 scale = mix(affine_rows[row].scale[bg - 2u],
                           affine_rows[row + 1u].scale[bg - 2u], interp);
  return ivec2(floor(base + scale * pixel.x));
}

BlendUnit AffineBackground(BlendUnit blend_unit, lowp uint bg) {
  mediump ivec2 tilemap_pixel = AffinePixel(bg);
  if (!backgrounds[bg].wraparound) {
    if (any(lessThan(tilemap_pixel, ivec2(0, 0))) ||
        any(greaterThanEqual(tilemap_pixel, backgrounds[bg].size))) {
      return blend_unit;
    }
  } else {
    tilemap_pixel &= backgrounds[bg].size - 1;
  }

  tilemap_pixel -= tilemap_pixel % backgrounds[bg].mosaic;

  lowp ivec2 tile = tilemap_pixel / 8;
  lowp ivec2 tile_pixel = tilemap_pixel % 8;
  mediump int tile_offset = tile.x + tile.y * (backgrounds[bg].size.x / 8);

  mediump uint index =
      texelFetch(affine_tilemap,
                 ivec3(tile_offset % 64, (tile_offset % 2048) / 64,
                       backgrounds[bg].tilemap_base + tile_offset / 2048),
                 0)
          .r;
  index <<= 1;

  lowp uint color = texelFetch(background_tiles,
                               ivec3(tile_pixel.x + 8 * tile_pixel.y, index,
                                     backgrounds[bg].tile_base),
                               0)
                        .r;
  if (color == 0u) {
    return blend_unit;
  }

  return BlendUnitAddBackground(blend_unit, bg, background_palette[color].rgb);
}

BlendUnit BitmapBackground(BlendUnit blend_unit) {
  mediump ivec2 lookup = AffinePixel(2u);
  if (any(lessThan(lookup, ivec2(0, 0))) ||
      any(greaterThan(lookup, backgrounds[2].size))) {
    return blend_unit;
  }
  lookup -= lookup % backgrounds[2].mosaic;
  lowp vec4 color = texelFetch(bitmap, lookup, 0);
  return BlendUnitAddBackground(blend_unit, 2u, color.rgb);
}

BlendUnit PaletteBitmapBackground(BlendUnit blend_unit) {
  mediump ivec2 lookup = AffinePixel(2u);
  if (any(lessThan(lookup, ivec2(0, 0))) ||
      any(greaterThan(lookup, backgrounds[2].size))) {
    return blend_unit;
  }
  lookup -= lookup % backgrounds[2].mosaic;
  lowp uint index = texelFetch(palette_bitmap, lookup, 0).r;
  if (index == 0u) {
    return blend_unit;
  }

  return BlendUnitAddBackground(blend_unit, 2u, background_palette[index].rgb);
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
  BlendUnit blend_unit = CreateBlendUnit();

  bool on_object_window = false;

#if OBJECTS != 0
  highp uvec4 visible_objects =
      object_columns[uint(screencoord.x)] & object_rows[int(screencoord.y)];

  highp uvec4 window_objects = visible_objects & object_window;
  while (NotEmpty(window_objects)) {
    lowp uint bit = CountTrailingZeroes(window_objects);
    window_objects = FlipBit(window_objects, bit);

    lowp uint obj = object_indices[bit];

    lowp uint color_index = ObjectColorIndex(obj);
    if (color_index != 0u) {
      on_object_window = true;
      break;
    }
  }
#endif  // OBJECTS != 0

  Window window = CheckWindow(on_object_window);

#if OBJECTS != 0
  if (window.obj) {
    highp uvec4 drawn_objects = visible_objects & object_drawn;
    while (NotEmpty(drawn_objects)) {
      lowp uint bit = CountTrailingZeroes(drawn_objects);
      drawn_objects = FlipBit(drawn_objects, bit);

      lowp uint obj = object_indices[bit];

      lowp uint color_index = ObjectColorIndex(obj);
      if (color_index != 0u) {
        lowp vec3 color =
            object_palette[objects[obj].palette + color_index].rgb;
        blend_unit =
            BlendUnitAddObject(blend_unit, color, objects[obj].priority,
                               objects[obj].semi_transparent);
        break;
      }
    }
  }
#endif  // OBJECTS != 0

#if SCROLLING_BACKGROUND_0 != 0
  if (window.bg0) {
    blend_unit = ScrollingBackground(blend_unit, 0u);
  }
#endif  // SCROLLING_BACKGROUND_0 != 0

#if SCROLLING_BACKGROUND_1 != 0
  if (window.bg1) {
    blend_unit = ScrollingBackground(blend_unit, 1u);
  }
#endif  // SCROLLING_BACKGROUND_1 != 0

#if SCROLLING_BACKGROUND_2 != 0
  if (window.bg2) {
    blend_unit = ScrollingBackground(blend_unit, 2u);
  }
#endif  // SCROLLING_BACKGROUND_2 != 0

#if SCROLLING_BACKGROUND_3 != 0
  if (window.bg3) {
    blend_unit = ScrollingBackground(blend_unit, 3u);
  }
#endif  // SCROLLING_BACKGROUND_3 != 0

#if AFFINE_BACKGROUND_2 != 0
  if (window.bg2) {
    blend_unit = AffineBackground(blend_unit, 2u);
  }
#endif  // AFFINE_BACKGROUND_2 != 0

#if AFFINE_BACKGROUND_3 != 0
  if (window.bg3) {
    blend_unit = AffineBackground(blend_unit, 3u);
  }
#endif  // AFFINE_BACKGROUND_2 != 0

#if BITMAP_BACKGROUND != 0
  if (window.bg2) {
    blend_unit = BitmapBackground(blend_unit);
  }
#endif  // BITMAP_BACKGROUND != 0

#if PALETTE_BITMAP_BACKGROUND != 0
  if (window.bg2) {
    blend_unit = PaletteBitmapBackground(blend_unit);
  }
#endif  // PALETTE_BITMAP_BACKGROUND != 0

  blend_unit = BlendUnitAddBackdrop(blend_unit, background_palette[0].rgb);

  frag_color = BlendUnitBlend(blend_unit, window.bld);
}