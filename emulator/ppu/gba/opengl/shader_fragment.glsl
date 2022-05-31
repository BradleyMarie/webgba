#version 300 es

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

// Tilemaps
uniform mediump isampler2D bg_affine_tilemap;
uniform mediump isampler2D bg_scrolling_tilemap;

// Tiles
uniform mediump isampler2D bg_tiles;
uniform lowp sampler2D bg_palette;

// Background Bitmaps
uniform lowp sampler2D bg_mode3;
uniform lowp usampler2D bg_mode4;
uniform lowp sampler2D bg_mode5;

// Background Mosaic
uniform lowp ivec2 bg_mosaic[4];

// Inputs
in highp vec2 scrolling_screencoord[4];
in highp vec2 affine_screencoord[2];
in highp vec2 screencoord;

// Objects
uniform highp usampler2D object_visibility;
uniform highp uvec4 object_semi_transparent;

struct ObjectAttributes {
  highp mat2 affine;
  highp vec2 sprite_size;
  highp vec2 center;
  mediump vec2 mosaic;
  highp float tile_base;
  lowp float palette;
  bool large_palette;
  bool rendered;
  lowp vec2 flip;
  lowp uint priority;
};

uniform ObjectAttributes obj_attributes[128];
uniform lowp sampler2D obj_tiles;
uniform lowp sampler2D obj_palette;

bool ObjectSemiTransparent(lowp uint obj) {
  return (object_semi_transparent[obj / 32u] & (1u << (obj % 32u))) != 0u;
}

lowp float GetObjectColorIndex(lowp uint obj) {
  highp vec2 from_center = screencoord - obj_attributes[obj].center;

  highp vec2 half_sprite_size = obj_attributes[obj].sprite_size / 2.0;
  highp vec2 lookup =
      obj_attributes[obj].affine * from_center + half_sprite_size;

  if (lookup.x < 0.0 || obj_attributes[obj].sprite_size.x < lookup.x ||
      lookup.y < 0.0 || obj_attributes[obj].sprite_size.y < lookup.y) {
    return 0.0;
  }

  lookup = lookup - mod(lookup, obj_attributes[obj].mosaic) + vec2(0.5, 0.5);
  lookup = abs(obj_attributes[obj].flip - lookup);

  highp vec2 tile = floor(lookup / 8.0);

  highp float tile_index;
  if (obj_mode) {
    tile_index =
        tile.x + tile.y * obj_attributes[obj].sprite_size.x / 8.0;
  } else {
    highp float row_width = (obj_attributes[obj].large_palette) ? 16.0 : 32.0;
    tile_index = tile.x + tile.y * row_width;
  }

  highp vec2 tile_pixel = mod(lookup, 8.0) / 8.0;
  lowp vec4 color_indices = texture(
      obj_tiles, vec2(tile_pixel.x, obj_attributes[obj].tile_base +
                                        (tile_index + tile_pixel.y) / 1024.0));
  return obj_attributes[obj].large_palette ? color_indices.r : color_indices.a;
}

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

// Backgrounds
struct BackgroundControl {
  mediump ivec2 size;
  mediump int tilemap_base;
  mediump int tile_base;
  lowp uint priority;
  bool large_palette;
  bool wraparound;
};

uniform BackgroundControl bg_cnt[4];

lowp vec4 ScrollingBackground(lowp uint bg) {
  highp ivec2 tilemap_pixel = ivec2(scrolling_screencoord[bg]);
  tilemap_pixel &= bg_cnt[bg].size - 1;
  tilemap_pixel -= tilemap_pixel % bg_mosaic[bg];

  lowp ivec2 tilemap_block = tilemap_pixel / 256;
  lowp int tilemap_block_index =
      tilemap_block.x + tilemap_block.y * (bg_cnt[bg].size.x / 256);

  mediump ivec2 tilemap_block_tile = (tilemap_pixel / 8) % 32;
  mediump ivec2 tilemap_index =
      ivec2(tilemap_block_tile.x, bg_cnt[bg].tilemap_base / 64 +
                                      32 * tilemap_block_index +
                                      tilemap_block_tile.y);

  mediump ivec4 tilemap_entry =
      texelFetch(bg_scrolling_tilemap, tilemap_index, 0);
  mediump int tileblock_offset =
      tilemap_entry.x >> int(bg_cnt[bg].large_palette);
  lowp ivec2 flip = tilemap_entry.yz;
  lowp int palette = tilemap_entry.w;

  lowp ivec2 tile_pixel = abs(flip - (tilemap_pixel % 8));

  mediump ivec4 color_indices =
      texelFetch(bg_tiles,
                 ivec2(tile_pixel.x, bg_cnt[bg].tile_base +
                                         tileblock_offset * 8 + tile_pixel.y),
                 0);

  mediump int color_index =
      bg_cnt[bg].large_palette ? color_indices.r : color_indices.g;

  mediump int palette_base = bg_cnt[bg].large_palette ? 0 : palette;
  lowp vec4 color =
      texelFetch(bg_palette, ivec2(palette_base + color_index, 0), 0);
  return vec4(color.rgb, float(color_index != 0));
}

lowp vec4 AffineBackground(lowp uint bg) {
  highp ivec2 tilemap_pixel = ivec2(affine_screencoord[bg - 2u]);
  tilemap_pixel &= bg_cnt[bg].size - 1;
  tilemap_pixel -= tilemap_pixel % bg_mosaic[bg];

  lowp ivec2 tile = tilemap_pixel / 8;
  lowp ivec2 tile_pixel = tilemap_pixel % 8;

  mediump int tile_offset = tile.x + tile.y * (bg_cnt[bg].size.x / 8);
  mediump int tile_index = bg_cnt[bg].tilemap_base + tile_offset;

  mediump int index =
      texelFetch(bg_affine_tilemap,
                ivec2(tile_index % 256, tile_index / 256), 0).r;

  mediump int color_index = texelFetch(
      bg_tiles, ivec2(tile_pixel.x, bg_cnt[bg].tile_base + index * 8 +
                                       tile_pixel.y), 0).r;
  lowp vec4 color = texelFetch(bg_palette, ivec2(color_index, 0), 0);
  return vec4(color.rgb, float(color_index != 0));
}

lowp vec4 BitmapBackgroundMode3() {
  if (any(lessThan(affine_screencoord[0], vec2(0.0, 0.0))) ||
      any(greaterThan(affine_screencoord[0], vec2(240.0, 160.0)))) {
    return vec4(0.0, 0.0, 0.0, 0.0);
  }
  highp ivec2 lookup = ivec2(affine_screencoord[0]);
  lookup -= lookup % bg_mosaic[2];
  return texelFetch(bg_mode3, lookup, 0);
}

lowp vec4 BitmapBackgroundMode4() {
  if (any(lessThan(affine_screencoord[0], vec2(0.0, 0.0))) ||
      any(greaterThan(affine_screencoord[0], vec2(240.0, 160.0)))) {
    return vec4(0.0, 0.0, 0.0, 0.0);
  }
  highp ivec2 lookup = ivec2(affine_screencoord[0]);
  lookup -= lookup % bg_mosaic[2];
  lowp uint index = texelFetch(bg_mode4, lookup, 0).r;
  return texelFetch(bg_palette, ivec2(index, 0), 0);
}

lowp vec4 BitmapBackgroundMode5() {
  if (any(lessThan(affine_screencoord[0], vec2(0.0, 0.0))) ||
      any(greaterThan(affine_screencoord[0], vec2(160.0, 120.0)))) {
    return vec4(0.0, 0.0, 0.0, 0.0);
  }
  highp ivec2 lookup = ivec2(affine_screencoord[0]);
  lookup -= lookup % bg_mosaic[2];
  return texelFetch(bg_mode5, lookup, 0);
}

// Backdrop
lowp vec4 Backdrop() { return texelFetch(bg_palette, ivec2(0, 0), 0); }

// Count Trailing Zeroes
lowp uint CountTrailingZeroes(highp uint value) {
  const highp uint debrujin_sequence = 0x077CB531u;
  const lowp uint position[32] =
      uint[32](0u, 1u, 28u, 2u, 29u, 14u, 24u, 3u, 30u, 22u, 20u, 15u, 25u, 17u,
               4u, 8u, 31u, 27u, 13u, 23u, 21u, 19u, 16u, 7u, 26u, 12u, 18u, 6u,
               11u, 5u, 10u, 9u);
  return position[((value & -value) * debrujin_sequence) >> 27u];
}

// Main
out lowp vec4 fragColor;

void main() {
  BlendUnitInitialize();

  // Sort Objects and Check Object Window
  lowp uint object_window_objects[128];
  lowp uint num_object_window_objects = 0u;
  lowp uint drawn_objects[128];
  lowp uint num_drawn_objects = 0u;
  bool on_object_window = false;

  if (obj_enabled) {
    highp uvec4 visible_objects =
        texelFetch(object_visibility, ivec2(screencoord.x, 0), 0) &
        texelFetch(object_visibility, ivec2(screencoord.y, 1), 0);

    highp uint visible_object_sets[4];
    lowp uint visible_object_set_base[4];
    lowp uint num_object_sets = 0u;
    visible_object_sets[num_object_sets] = visible_objects.x;
    visible_object_set_base[num_object_sets] = 0u;
    num_object_sets += uint(visible_objects.x != 0u);
    visible_object_sets[num_object_sets] = visible_objects.y;
    visible_object_set_base[num_object_sets] = 32u;
    num_object_sets += uint(visible_objects.y != 0u);
    visible_object_sets[num_object_sets] = visible_objects.z;
    visible_object_set_base[num_object_sets] = 64u;
    num_object_sets += uint(visible_objects.z != 0u);
    visible_object_sets[num_object_sets] = visible_objects.w;
    visible_object_set_base[num_object_sets] = 96u;
    num_object_sets += uint(visible_objects.w != 0u);

    lowp uint visible_object_set_index = 0u;
    for (lowp uint i = 0u; i < num_object_sets * 32u; i++) {
      lowp uint base = visible_object_set_base[visible_object_set_index];
      lowp uint index =
          CountTrailingZeroes(visible_object_sets[visible_object_set_index]);
      lowp uint obj = base + index;

      object_window_objects[num_object_window_objects] = obj;
      num_object_window_objects += uint(!obj_attributes[obj].rendered);

      drawn_objects[num_drawn_objects] = obj;
      num_drawn_objects += uint(obj_attributes[obj].rendered);

      visible_object_sets[visible_object_set_index] =
          visible_object_sets[visible_object_set_index] ^ (1u << index);
      visible_object_set_index +=
          uint(visible_object_sets[visible_object_set_index] == 0u);

      if (visible_object_set_index == num_object_sets) {
        break;
      }
    }

    // Check Object Window
    for (lowp uint i = 0u; i < num_object_window_objects; i++) {
      lowp float color_index = GetObjectColorIndex(object_window_objects[i]);
      on_object_window = on_object_window || (color_index != 0.0);
    }
  }

  // Window
  WindowContents window = CheckWindow(on_object_window);

  // Objects
  num_drawn_objects = window.obj ? num_drawn_objects : 0u;
  if (window.obj) {
    lowp vec4 color = vec4(0.0, 0.0, 0.0, 0.0);
    lowp uint priority = 5u;
    bool blended = false;
    for (lowp uint i = 0u; i < num_drawn_objects; i++) {
      lowp uint obj = drawn_objects[i];

      lowp float color_index = GetObjectColorIndex(obj);
      if (color_index == 0.0) {
        continue;
      }

      lowp float palette =
          obj_attributes[obj].large_palette ? 0.0 : obj_attributes[obj].palette;
      lowp float palette_offset = (color_index * 255.0 + 0.5) / 256.0;
      lowp vec4 obj_color =
          texture(obj_palette, vec2(palette + palette_offset, 0.5));

      if (obj_attributes[obj].priority < priority) {
        color = obj_color;
        priority = obj_attributes[obj].priority;
        blended = ObjectSemiTransparent(obj);
      }
    }
    BlendUnitAddObject(color, priority, blended);
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
    BlendUnitAddBackground(bg, color, bg_cnt[bg].priority);
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
        all(lessThan(affine_screencoord[bg - 2u], vec2(bg_cnt[bg].size)));
    num_enabled_affine_backgrounds += uint(
        window.bg[bg] && (bg_cnt[bg].wraparound || (above_min && below_max)));
  }

  for (lowp uint i = 0u; i < num_enabled_affine_backgrounds; i++) {
    lowp uint bg = enabled_affine_backgrounds[i];
    lowp vec4 color = AffineBackground(bg);
    BlendUnitAddBackground(bg, color, bg_cnt[bg].priority);
  }

  // Bitmap Backgrounds
  if (bitmap_backgrounds_mode3 && window.bg[2]) {
    lowp vec4 color = BitmapBackgroundMode3();
    BlendUnitAddBackground(2u, color, bg_cnt[2].priority);
  } else if (bitmap_backgrounds_mode4 && window.bg[2]) {
    lowp vec4 color = BitmapBackgroundMode4();
    BlendUnitAddBackground(2u, color, bg_cnt[2].priority);
  } else if (bitmap_backgrounds_mode5 && window.bg[2]) {
    lowp vec4 color = BitmapBackgroundMode5();
    BlendUnitAddBackground(2u, color, bg_cnt[2].priority);
  }

  // Backdrop
  lowp vec4 backdrop = Backdrop();
  BlendUnitAddBackdrop(backdrop);

  // Blend
  fragColor = BlendUnitBlend(window.bld);
}