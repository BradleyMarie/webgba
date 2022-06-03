#version 300 es

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

// Display Controls
uniform lowp uint scrolling_backgrounds[4];
uniform lowp uint num_scrolling_backgrounds;
uniform lowp uint affine_backgrounds[2];
uniform lowp uint num_affine_backgrounds;
uniform bool bitmap_backgrounds_mode3;
uniform bool bitmap_backgrounds_mode4;
uniform bool bitmap_backgrounds_mode5;

uniform bool obj_mode;
uniform bool obj_enabled;
uniform bool win0_enabled;
uniform bool win1_enabled;
uniform bool winobj_enabled;

// Blend Unit
uniform int blend_mode;
uniform lowp float blend_eva;
uniform lowp float blend_evb;
uniform lowp float blend_evy;
uniform bool obj_top;
uniform bool obj_bottom;
uniform bool bg_top[5];
uniform bool bg_bottom[5];

lowp uint blend_priorities[3];
lowp uint blend_order[3];
bool blend_top[3];
bool blend_bottom[3];
bool blend_is_semi_transparent[3];
lowp vec3 blend_layers[3];

void BlendUnitInitialize() {
  blend_priorities[0] = 6u;
  blend_priorities[1] = 6u;
  blend_priorities[2] = 6u;
  blend_top[0] = false;
  blend_top[1] = false;
  blend_top[2] = false;
  blend_bottom[0] = false;
  blend_bottom[1] = false;
  blend_bottom[2] = false;
  blend_is_semi_transparent[0] = false;
  blend_is_semi_transparent[1] = false;
  blend_is_semi_transparent[2] = false;
  blend_order[0] = 0u;
  blend_order[1] = 1u;
  blend_order[2] = 2u;
}

void BlendUnitAddObject(lowp vec4 color, lowp uint priority, bool blended) {
  bool inserted = (color.a != 0.0);
  blend_priorities[0] = inserted ? priority : 6u;
  blend_layers[0] = color.rgb;
  blend_top[0] = inserted && (blended || obj_top);
  blend_bottom[0] = inserted && obj_bottom;
  blend_is_semi_transparent[0] = inserted && blended;
}

void BlendUnitAddBackground(lowp uint bg, lowp vec4 color, lowp uint priority) {
  priority = (color.a != 0.0) ? priority : 6u;

  lowp uint insert_index = blend_order[2];
  blend_priorities[insert_index] = priority;
  blend_layers[insert_index] = color.rgb;
  blend_top[insert_index] = bg_top[bg];
  blend_bottom[insert_index] = bg_bottom[bg];
  blend_is_semi_transparent[insert_index] = false;

  lowp uint bottom_index = blend_order[1];
  bool do_first_swap =
      blend_priorities[insert_index] < blend_priorities[bottom_index];
  blend_order[1] = do_first_swap ? insert_index : bottom_index;
  blend_order[2] = do_first_swap ? bottom_index : insert_index;
  bottom_index = blend_order[1];

  lowp uint top_index = blend_order[0];
  bool do_second_swap =
      blend_priorities[bottom_index] < blend_priorities[top_index];
  blend_order[0] = do_second_swap ? bottom_index : top_index;
  blend_order[1] = do_second_swap ? top_index : bottom_index;
}

void BlendUnitAddBackdrop(lowp vec4 color) {
  BlendUnitAddBackground(4u, color, 5u);
}

lowp vec3 BlendUnitNoBlend() {
  lowp uint top_index = blend_order[0];
  lowp uint bottom_index = blend_order[1];

  bool do_blend = (blend_is_semi_transparent[top_index] ||
                   blend_is_semi_transparent[bottom_index]) &&
                  blend_top[top_index] && blend_bottom[bottom_index];

  lowp float eva = do_blend ? blend_eva : 1.0;
  lowp float evb = do_blend ? blend_evb : 0.0;

  return min(
      (blend_layers[top_index] * eva) + (blend_layers[bottom_index] * evb),
      1.0);
}

lowp vec3 BlendUnitAdditiveBlend() {
  lowp uint top_index = blend_order[0];
  lowp uint bottom_index = blend_order[1];

  bool do_blend = blend_top[top_index] && blend_bottom[bottom_index];
  lowp float eva = do_blend ? blend_eva : 1.0;
  lowp float evb = do_blend ? blend_evb : 0.0;

  return min(
      (blend_layers[top_index] * eva) + (blend_layers[bottom_index] * evb),
      1.0);
}

lowp vec3 BlendUnitBrighten() {
  lowp uint top_index = blend_order[0];
  lowp uint bottom_index = blend_order[1];

  bool do_brighten = blend_top[top_index];
  bool do_blend = (blend_is_semi_transparent[top_index] ||
                   blend_is_semi_transparent[bottom_index]) &&
                  blend_top[top_index] && blend_bottom[bottom_index];

  lowp float eva = do_blend ? blend_eva : 1.0;
  lowp float evy = do_brighten ? blend_evy : 0.0;
  lowp float evb = do_blend ? blend_evb : evy;
  lowp vec3 bottom =
      do_blend ? blend_layers[bottom_index] : 1.0 - blend_layers[top_index];

  return min((blend_layers[top_index] * eva) + (bottom * evb), 1.0);
}

lowp vec3 BlendUnitDarken() {
  lowp uint top_index = blend_order[0];
  lowp uint bottom_index = blend_order[1];

  bool do_darken = blend_top[top_index];
  bool do_blend = (blend_is_semi_transparent[top_index] ||
                   blend_is_semi_transparent[bottom_index]) &&
                  blend_top[top_index] && blend_bottom[bottom_index];

  lowp float evy = do_darken ? (1.0 - blend_evy) : 1.0;
  lowp float eva = do_blend ? blend_eva : evy;
  lowp float evb = do_blend ? blend_evb : 0.0;

  return min(
      (blend_layers[top_index] * eva) + (blend_layers[bottom_index] * evb),
      1.0);
}

lowp vec4 BlendUnitBlend(bool enable_blend) {
  lowp vec3 color;
  if (!enable_blend) {
    color = blend_layers[blend_order[0]];
  } else if (blend_mode == 0) {
    color = BlendUnitNoBlend();
  } else if (blend_mode == 1) {
    color = BlendUnitAdditiveBlend();
  } else if (blend_mode == 2) {
    color = BlendUnitBrighten();
  } else {
    color = BlendUnitDarken();
  }

  return vec4(color.bgr, 1.0);
}

// Window
struct WindowContents {
  bool bg[4];
  bool obj;
  bool bld;
};

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

// Backdrop
lowp vec4 Backdrop() { return background_palette[0]; }

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

// Main
void main() {
  BlendUnitInitialize();

  // Sort Objects and Check Object Window
  lowp vec4 obj_color = vec4(0.0, 0.0, 0.0, 0.0);
  lowp uint obj_priority = 5u;
  bool obj_blended = false;
  bool on_object_window = false;

  if (obj_enabled) {
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
  }

  // Window
  WindowContents window = CheckWindow(on_object_window);

  // Objects
  if (window.obj) {
    BlendUnitAddObject(obj_color, obj_priority, obj_blended);
  }

  // Scrolling Backgrounds
  lowp uint enabled_scrolling_backgrounds[4];
  lowp uint num_enabled_scrolling_backgrounds = 0u;
  for (lowp uint i = 0u; i < num_scrolling_backgrounds; i++) {
    lowp uint bg = scrolling_backgrounds[i];
    enabled_scrolling_backgrounds[num_enabled_scrolling_backgrounds] = bg;
    num_enabled_scrolling_backgrounds += uint(window.bg[bg]);
  }

  for (lowp uint i = 0u; i < num_enabled_scrolling_backgrounds; i++) {
    lowp uint bg = enabled_scrolling_backgrounds[i];
    lowp vec4 color = ScrollingBackground(bg);
    BlendUnitAddBackground(bg, color, backgrounds[bg].priority);
  }

  // Affine Backgrounds
  lowp uint enabled_affine_backgrounds[2];
  lowp uint num_enabled_affine_backgrounds = 0u;
  for (lowp uint i = 0u; i < num_affine_backgrounds; i++) {
    lowp uint bg = affine_backgrounds[i];
    enabled_affine_backgrounds[num_enabled_affine_backgrounds] = bg;
    bool above_min =
        all(greaterThan(affine_screencoord[bg - 2u], vec2(0.0, 0.0)));
    bool below_max =
        all(lessThan(affine_screencoord[bg - 2u], vec2(backgrounds[bg].size)));
    num_enabled_affine_backgrounds +=
        uint(window.bg[bg] &&
             (backgrounds[bg].wraparound || (above_min && below_max)));
  }

  for (lowp uint i = 0u; i < num_enabled_affine_backgrounds; i++) {
    lowp uint bg = enabled_affine_backgrounds[i];
    lowp vec4 color = AffineBackground(bg);
    BlendUnitAddBackground(bg, color, backgrounds[bg].priority);
  }

  // Bitmap Backgrounds
  if (bitmap_backgrounds_mode3 && window.bg[2]) {
    lowp vec4 color = BitmapBackground();
    BlendUnitAddBackground(2u, color, backgrounds[2].priority);
  } else if (bitmap_backgrounds_mode4 && window.bg[2]) {
    lowp vec4 color = PaletteBitmapBackground();
    BlendUnitAddBackground(2u, color, backgrounds[2].priority);
  } else if (bitmap_backgrounds_mode5 && window.bg[2]) {
    lowp vec4 color = BitmapBackground();
    BlendUnitAddBackground(2u, color, backgrounds[2].priority);
  }

  // Backdrop
  lowp vec4 backdrop = Backdrop();
  BlendUnitAddBackdrop(backdrop);

  // Blend
  frag_color = BlendUnitBlend(window.bld);
}