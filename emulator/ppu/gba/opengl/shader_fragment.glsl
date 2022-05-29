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
uniform mediump sampler2D bg_affine_tilemap;
uniform highp sampler2D bg_scrolling_tilemap_indices;
uniform lowp sampler2D bg_scrolling_tilemap_params;

// Tiles
uniform lowp sampler2D bg_tiles;
uniform lowp sampler2D bg_palette;

// Background Bitmaps
uniform lowp sampler2D bg_mode3;
uniform mediump sampler2D bg_mode4;
uniform mediump sampler2D bg_mode5;

// Background Mosaic
uniform highp vec2 bg_mosaic[4];

// Inputs
in highp vec2 scrolling_screencoord[4];
in highp vec2 affine_screencoord[2];
in highp vec2 screencoord;

// Objects
struct ObjectAttributes {
  highp mat2 affine;
  highp vec2 origin;
  highp vec2 sprite_size;
  highp vec2 render_size;
  mediump vec2 mosaic;
  highp float tile_base;
  lowp float palette;
  bool large_palette;
  bool rendered;
  bool blended;
  lowp vec2 flip;
  lowp uint priority;
};

uniform ObjectAttributes obj_attributes[128];
uniform lowp sampler2D obj_tiles;
uniform lowp sampler2D obj_palette;
uniform int obj_count;

struct ObjectLayer {
  lowp vec4 color;
  lowp uint priority;
  bool winobj;
  bool blended;
};

ObjectLayer Objects() {
  ObjectLayer result;
  result.color = vec4(0.0, 0.0, 0.0, 0.0);
  result.priority = 5u;
  result.winobj = false;
  result.blended = false;

  for (int i = 0; i < obj_count; i++) {
    if (screencoord.x < obj_attributes[i].origin.x ||
        screencoord.y < obj_attributes[i].origin.y) {
      continue;
    }

    highp vec2 end = obj_attributes[i].origin + obj_attributes[i].render_size;

    if (end.x < screencoord.x || end.y < screencoord.y) {
      continue;
    }

    highp vec2 half_render_size = obj_attributes[i].render_size / 2.0;
    highp vec2 center = obj_attributes[i].origin + half_render_size;
    highp vec2 from_center = screencoord - center;

    highp vec2 half_sprite_size = obj_attributes[i].sprite_size / 2.0;
    highp vec2 lookup =
        obj_attributes[i].affine * from_center + half_sprite_size;

    if (lookup.x < 0.0 || obj_attributes[i].sprite_size.x < lookup.x ||
        lookup.y < 0.0 || obj_attributes[i].sprite_size.y < lookup.y) {
      continue;
    }

    lookup = lookup - mod(lookup, obj_attributes[i].mosaic) + vec2(0.5, 0.5);
    lookup = abs(obj_attributes[i].flip - lookup);

    highp vec2 lookup_tile = floor(lookup / 8.0);

    highp float tile_index;
    if (obj_mode) {
      tile_index =
          lookup_tile.x + lookup_tile.y * obj_attributes[i].sprite_size.x / 8.0;
    } else {
      highp float row_width = (obj_attributes[i].large_palette) ? 16.0 : 32.0;
      tile_index = lookup_tile.x + lookup_tile.y * row_width;
    }

    highp vec2 tile_pixel = mod(lookup, 8.0) / 8.0;
    lowp vec4 color_indices =
        texture2D(obj_tiles,
                  vec2(tile_pixel.x, obj_attributes[i].tile_base +
                                         (tile_index + tile_pixel.y) / 1024.0));
    lowp float color_index =
        obj_attributes[i].large_palette ? color_indices.r : color_indices.a;
    if (color_index == 0.0) {
      continue;
    }

    lowp float palette =
        obj_attributes[i].large_palette ? 0.0 : obj_attributes[i].palette;
    lowp float palette_offset = (color_index * 255.0 + 0.5) / 256.0;
    lowp vec4 color =
        texture2D(obj_palette, vec2(palette + palette_offset, 0.5));

    if (!obj_attributes[i].rendered) {
      result.winobj = true;
      continue;
    }

    if (obj_attributes[i].priority < result.priority) {
      result.priority = obj_attributes[i].priority;
      result.color = color;
      result.blended = obj_attributes[i].blended;
    }
  }

  return result;
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

void BlendUnitAddObject(ObjectLayer obj) {
  bool inserted = (obj.color.a != 0.0);
  blend_priorities[0] = inserted ? obj.priority : 6u;
  blend_layers[0] = obj.color.rgb;
  blend_top[0] = inserted && (obj.blended || obj_top);
  blend_bottom[0] = inserted && obj_bottom;
  blend_is_semi_transparent[0] = inserted && obj.blended;
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
  BlendUnitAddBackground(5u, color, 5u);
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
  lowp uint priority;
  highp vec2 size;
  highp float tilemap_base;
  highp float tile_base;
  bool large_palette;
  bool wraparound;
};

uniform BackgroundControl bg_cnt[4];

lowp vec4 ScrollingBackground(lowp uint bg) {
  const highp float number_of_tilemap_blocks = 32.0;
  const highp float tilemap_block_size_tiles = 32.0;
  const highp float tile_size = 8.0;
  const highp float tilemap_block_size_pixels =
      tilemap_block_size_tiles * tile_size;

  highp vec2 tilemap_pixel = scrolling_screencoord[bg];
  tilemap_pixel = mod(tilemap_pixel, bg_cnt[bg].size);
  tilemap_pixel -= mod(tilemap_pixel, bg_mosaic[bg]);
  tilemap_pixel += vec2(0.5, 0.5);

  highp vec2 tilemap_block_index_2d =
      floor(tilemap_pixel / tilemap_block_size_pixels);
  highp float tilemap_width_blocks =
      bg_cnt[bg].size.x / tilemap_block_size_pixels;
  highp float tilemap_block_index_1d =
      tilemap_block_index_2d.x +
      tilemap_block_index_2d.y * tilemap_width_blocks;

  highp vec2 tilemap_block_pixel =
      mod(tilemap_pixel, tilemap_block_size_pixels) / tilemap_block_size_pixels;

  highp vec2 tile = vec2(tilemap_block_pixel.x,
                         bg_cnt[bg].tilemap_base +
                             (tilemap_block_pixel.y + tilemap_block_index_1d) /
                                 number_of_tilemap_blocks);

  highp vec4 indices = texture2D(bg_scrolling_tilemap_indices, tile);
  lowp vec4 params = texture2D(bg_scrolling_tilemap_params, tile);

  mediump float num_tiles = 2048.0;
  mediump float tile_block_position =
      (indices.a * 255.0 * 256.0 + indices.r * 255.0) / num_tiles;

  highp vec2 tile_pixel = mod(tilemap_pixel, tile_size) / tile_size;
  tile_pixel = abs(params.xy - tile_pixel);

  lowp vec4 color_indices = texture2D(
      bg_tiles, vec2(tile_pixel.x, bg_cnt[bg].tile_base + tile_block_position +
                                       tile_pixel.y / num_tiles));
  lowp float color_index =
      bg_cnt[bg].large_palette ? color_indices.r : color_indices.a;

  lowp float palette =
      bg_cnt[bg].large_palette ? 0.0 : (params.z * 31.0) / 32.0;
  lowp float palette_offset = (color_index * 255.0 + 0.5) / 256.0;
  lowp vec4 color = texture2D(bg_palette, vec2(palette + palette_offset, 0.5));
  return vec4(color.rgb, sign(color_index));
}

lowp vec4 AffineBackground(lowp uint bg) {
  const highp float num_tiles = 2048.0;
  const highp float tile_size = 8.0;

  highp vec2 tilemap_pixel = affine_screencoord[bg - 2u];
  highp vec2 wrapped_tilemap_pixel = bg_cnt[bg].wraparound
                                         ? mod(tilemap_pixel, bg_cnt[bg].size)
                                         : tilemap_pixel;
  highp vec2 lookup_pixel =
      wrapped_tilemap_pixel - mod(wrapped_tilemap_pixel, bg_mosaic[bg]);

  highp vec2 lookup_tile = floor(lookup_pixel / tile_size);
  highp vec2 tilemap_size_tiles = bg_cnt[bg].size / tile_size;

  highp float tile_index = lookup_tile.x + lookup_tile.y * tilemap_size_tiles.x;
  highp float lookup_tile_1d =
      bg_cnt[bg].tilemap_base + (tile_index + 0.5) / 65536.0;

  mediump vec4 raw_index =
      texture2D(bg_affine_tilemap,
                vec2(lookup_tile_1d / 256.0, mod(lookup_tile_1d, 256.0)));
  mediump float index = raw_index.r * 255.0 / 256.0;

  highp vec2 tile_pixel = mod(lookup_pixel, tile_size) / tile_size;

  lowp vec4 color_indices = texture2D(
      bg_tiles, vec2(tile_pixel.x, bg_cnt[bg].tile_base + index / 8.0 +
                                       tile_pixel.y / num_tiles));
  lowp float color_index = color_indices.r;

  lowp vec4 color = texture2D(bg_palette, vec2(color_index, 0.5));

  lowp float visible = step(0.0, wrapped_tilemap_pixel.x) *
                       step(0.0, wrapped_tilemap_pixel.y) *
                       step(wrapped_tilemap_pixel.x, bg_cnt[bg].size.x) *
                       step(wrapped_tilemap_pixel.y, bg_cnt[bg].size.y);
  return vec4(color.rgb, sign(color_index) * sign(visible));
}

lowp vec4 BitmapBackgroundMode3() {
  const highp vec2 bitmap_size = vec2(240.0, 160.0);
  highp vec2 lookup = affine_screencoord[0] -
                      mod(affine_screencoord[0], bg_mosaic[2]) +
                      vec2(0.5, 0.5);
  lowp vec4 color = texture2D(bg_mode3, lookup / bitmap_size);
  color *= step(affine_screencoord[0].x, bitmap_size.x);
  color *= step(affine_screencoord[0].y, bitmap_size.y);
  color *= step(-affine_screencoord[0].x, 0.0);
  color *= step(-affine_screencoord[0].y, 0.0);
  return color;
}

lowp vec4 BitmapBackgroundMode4() {
  const highp vec2 bitmap_size = vec2(240.0, 160.0);
  highp vec2 lookup = affine_screencoord[0] -
                      mod(affine_screencoord[0], bg_mosaic[2]) +
                      vec2(0.5, 0.5);
  mediump vec4 normalized_index = texture2D(bg_mode4, lookup / bitmap_size);
  mediump float index = (normalized_index.r * 255.0 + 0.5) / 256.0;
  lowp vec4 color = texture2D(bg_palette, vec2(index, 0.5));
  color *= step(affine_screencoord[0].x, bitmap_size.x);
  color *= step(affine_screencoord[0].y, bitmap_size.y);
  color *= step(-affine_screencoord[0].x, 0.0);
  color *= step(-affine_screencoord[0].y, 0.0);
  return color;
}

lowp vec4 BitmapBackgroundMode5() {
  const highp vec2 bitmap_size = vec2(160.0, 128.0);
  highp vec2 lookup = affine_screencoord[0] -
                      mod(affine_screencoord[0], bg_mosaic[2]) +
                      vec2(0.5, 0.5);
  lowp vec4 color = texture2D(bg_mode5, lookup / bitmap_size);
  color *= step(affine_screencoord[0].x, bitmap_size.x);
  color *= step(affine_screencoord[0].y, bitmap_size.y);
  color *= step(-affine_screencoord[0].x, 0.0);
  color *= step(-affine_screencoord[0].y, 0.0);
  return color;
}

// Backdrop
lowp vec4 Backdrop() { return texture2D(bg_palette, vec2(1.0 / 512.0, 0.5)); }

// Main
out lowp vec4 fragColor;

void main() {
  BlendUnitInitialize();

  ObjectLayer object;
  if (obj_enabled) {
    object = Objects();
  } else {
    object.color = vec4(0.0, 0.0, 0.0, 0.0);
    object.priority = 5u;
    object.winobj = false;
    object.blended = false;
  }

  WindowContents window = CheckWindow(object.winobj);
  object.color.a *= float(window.obj);
  BlendUnitAddObject(object);

  for (lowp uint i = 0u; i < num_scrolling_backgrounds; i++) {
    lowp uint bg = scrolling_backgrounds[i];
    lowp vec4 color = ScrollingBackground(bg);
    color.a *= float(window.bg[bg]);
    BlendUnitAddBackground(bg, color, bg_cnt[bg].priority);
  }

  for (lowp uint i = 0u; i < num_affine_backgrounds; i++) {
    lowp uint bg = affine_backgrounds[i];
    lowp vec4 color = AffineBackground(bg);
    color.a *= float(window.bg[bg]);
    BlendUnitAddBackground(bg, color, bg_cnt[bg].priority);
  }

  if (bitmap_backgrounds_mode3) {
    lowp vec4 color = BitmapBackgroundMode3();
    color.a *= float(window.bg[2]);
    BlendUnitAddBackground(2u, color, bg_cnt[2].priority);
  } else if (bitmap_backgrounds_mode4) {
    lowp vec4 color = BitmapBackgroundMode4();
    color.a *= float(window.bg[2]);
    BlendUnitAddBackground(2u, color, bg_cnt[2].priority);
  } else if (bitmap_backgrounds_mode5) {
    lowp vec4 color = BitmapBackgroundMode5();
    color.a *= float(window.bg[2]);
    BlendUnitAddBackground(2u, color, bg_cnt[2].priority);
  }

  lowp vec4 backdrop = Backdrop();
  BlendUnitAddBackdrop(backdrop);

  fragColor = BlendUnitBlend(window.bld);
}