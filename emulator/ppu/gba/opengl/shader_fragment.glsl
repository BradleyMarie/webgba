#version 300 es

// Display Controls
uniform bool blank;
uniform int mode;
uniform bool bg0_enabled;
uniform bool bg1_enabled;
uniform bool bg2_enabled;
uniform bool bg3_enabled;
uniform bool obj_enabled;
uniform bool win0_enabled;
uniform bool win1_enabled;
uniform bool winobj_enabled;
uniform bool obj_mode;

// Background Control
uniform int bg0_priority;
uniform highp vec2 bg0_size;
uniform highp float bg0_tilemap_base;
uniform highp float bg0_tile_base;
uniform bool bg0_large_palette;
uniform int bg1_priority;
uniform highp vec2 bg1_size;
uniform highp float bg1_tilemap_base;
uniform highp float bg1_tile_base;
uniform bool bg1_large_palette;
uniform int bg2_priority;
uniform highp vec2 bg2_size;
uniform highp float bg2_tilemap_base;
uniform highp float bg2_tile_base;
uniform bool bg2_large_palette;
uniform bool bg2_wraparound;
uniform int bg3_priority;
uniform highp vec2 bg3_size;
uniform highp float bg3_tilemap_base;
uniform highp float bg3_tile_base;
uniform bool bg3_large_palette;
uniform bool bg3_wraparound;

// Layer Controls
uniform int obj_priority;
const int bd_priority = 5;

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
uniform highp vec2 bg0_mosaic;
uniform highp vec2 bg1_mosaic;
uniform highp vec2 bg2_mosaic;
uniform highp vec2 bg3_mosaic;

// Inputs
in highp vec2 bg0_scrolling_screencoord;
in highp vec2 bg1_scrolling_screencoord;
in highp vec2 bg2_scrolling_screencoord;
in highp vec2 bg3_scrolling_screencoord;
in highp vec2 bg2_affine_screencoord;
in highp vec2 bg3_affine_screencoord;
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
  int priority;
};

uniform ObjectAttributes obj_attributes[128];
uniform lowp sampler2D obj_tiles;
uniform lowp sampler2D obj_palette;
uniform int obj_count;

struct ObjectLayer {
  lowp vec4 color;
  int priority;
  bool winobj;
  bool blended;
};

ObjectLayer Objects() {
  ObjectLayer result;
  result.color = vec4(0.0, 0.0, 0.0, 0.0);
  result.priority = 5;
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
uniform bool bg0_top;
uniform bool bg0_bottom;
uniform bool bg1_top;
uniform bool bg1_bottom;
uniform bool bg2_top;
uniform bool bg2_bottom;
uniform bool bg3_top;
uniform bool bg3_bottom;
uniform bool bd_top;
uniform bool bd_bottom;

int blend_priorities[3];
bool blend_top[3];
bool blend_bottom[3];
bool blend_is_semi_transparent[3];
lowp vec3 blend_layers[3];
int blend_order[3];

void BlendUnitInitialize() {
  blend_priorities[0] = 6;
  blend_priorities[1] = 6;
  blend_priorities[2] = 6;
  blend_top[0] = false;
  blend_top[1] = false;
  blend_top[2] = false;
  blend_bottom[0] = false;
  blend_bottom[1] = false;
  blend_bottom[2] = false;
  blend_is_semi_transparent[0] = false;
  blend_is_semi_transparent[1] = false;
  blend_is_semi_transparent[2] = false;
  blend_order[0] = 0;
  blend_order[1] = 1;
  blend_order[2] = 2;
}

void BlendUnitAddObject(ObjectLayer obj) {
  bool inserted = (obj.color.a != 0.0);
  blend_priorities[0] = inserted ? obj.priority : 6;
  blend_layers[0] = obj.color.rgb;
  blend_top[0] = inserted && (obj.blended || obj_top);
  blend_bottom[0] = inserted && obj_bottom;
  blend_is_semi_transparent[0] = inserted && obj.blended;
}

void BlendUnitAddBackground(int priority, bool top, bool bottom,
                            lowp vec4 color) {
  priority = (color.a != 0.0) ? priority : 6;

  int insert_index = blend_order[2];
  blend_priorities[insert_index] = priority;
  blend_layers[insert_index] = color.rgb;
  blend_top[insert_index] = top;
  blend_bottom[insert_index] = bottom;
  blend_is_semi_transparent[insert_index] = false;

  int bottom_index = blend_order[1];
  bool do_first_swap =
      blend_priorities[insert_index] < blend_priorities[bottom_index];
  blend_order[1] = do_first_swap ? insert_index : bottom_index;
  blend_order[2] = do_first_swap ? bottom_index : insert_index;
  bottom_index = blend_order[1];

  int top_index = blend_order[0];
  bool do_second_swap =
      blend_priorities[bottom_index] < blend_priorities[top_index];
  blend_order[0] = do_second_swap ? bottom_index : top_index;
  blend_order[1] = do_second_swap ? top_index : bottom_index;
}

void BlendUnitAddBackground0(lowp vec4 color) {
  BlendUnitAddBackground(bg0_priority, bg0_top, bg0_bottom, color);
}

void BlendUnitAddBackground1(lowp vec4 color) {
  BlendUnitAddBackground(bg1_priority, bg1_top, bg1_bottom, color);
}

void BlendUnitAddBackground2(lowp vec4 color) {
  BlendUnitAddBackground(bg2_priority, bg2_top, bg2_bottom, color);
}

void BlendUnitAddBackground3(lowp vec4 color) {
  BlendUnitAddBackground(bg3_priority, bg3_top, bg3_bottom, color);
}

void BlendUnitAddBackdrop(lowp vec4 color) {
  BlendUnitAddBackground(bd_priority, bd_top, bd_bottom, color);
}

lowp vec3 BlendUnitNoBlend() {
  int top_index = blend_order[0];
  int bottom_index = blend_order[1];

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
  int top_index = blend_order[0];
  int bottom_index = blend_order[1];

  bool do_blend = blend_top[top_index] && blend_bottom[bottom_index];
  lowp float eva = do_blend ? blend_eva : 1.0;
  lowp float evb = do_blend ? blend_evb : 0.0;

  return min(
      (blend_layers[top_index] * eva) + (blend_layers[bottom_index] * evb),
      1.0);
}

lowp vec3 BlendUnitBrighten() {
  int top_index = blend_order[0];
  int bottom_index = blend_order[1];

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
  int top_index = blend_order[0];
  int bottom_index = blend_order[1];

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
  bool bg0;
  bool bg1;
  bool bg2;
  bool bg3;
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
  result.bg0 = (use_win0 && win0.bg0) || (use_win1 && win1.bg0) ||
               (use_winobj && winobj.bg0) || (use_winout && winout.bg0) ||
               (!done && true);
  result.bg1 = (use_win0 && win0.bg1) || (use_win1 && win1.bg1) ||
               (use_winobj && winobj.bg1) || (use_winout && winout.bg1) ||
               (!done && true);
  result.bg2 = (use_win0 && win0.bg2) || (use_win1 && win1.bg2) ||
               (use_winobj && winobj.bg2) || (use_winout && winout.bg2) ||
               (!done && true);
  result.bg3 = (use_win0 && win0.bg3) || (use_win1 && win1.bg3) ||
               (use_winobj && winobj.bg3) || (use_winout && winout.bg3) ||
               (!done && true);
  result.obj = (use_win0 && win0.obj) || (use_win1 && win1.obj) ||
               (use_winobj && winobj.obj) || (use_winout && winout.obj) ||
               (!done && true);
  result.bld = (use_win0 && win0.bld) || (use_win1 && win1.bld) ||
               (use_winobj && winobj.bld) || (use_winout && winout.bld) ||
               (!done && true);

  return result;
}

// Scrolling Backgrounds
lowp vec4 ScrollingBackgroundImpl(highp float tilemap_base,
                                  highp vec2 tilemap_size_pixels,
                                  highp vec2 tilemap_pixel, highp vec2 mosaic,
                                  highp float tile_base, bool large_palette) {
  const highp float number_of_tilemap_blocks = 32.0;
  const highp float tilemap_block_size_tiles = 32.0;
  const highp float tile_size = 8.0;
  const highp float tilemap_block_size_pixels =
      tilemap_block_size_tiles * tile_size;

  tilemap_pixel = mod(tilemap_pixel, tilemap_size_pixels);
  tilemap_pixel -= mod(tilemap_pixel, mosaic);
  tilemap_pixel += vec2(0.5, 0.5);

  highp vec2 tilemap_block_index_2d =
      floor(tilemap_pixel / tilemap_block_size_pixels);
  highp float tilemap_width_blocks =
      tilemap_size_pixels.x / tilemap_block_size_pixels;
  highp float tilemap_block_index_1d =
      tilemap_block_index_2d.x +
      tilemap_block_index_2d.y * tilemap_width_blocks;

  highp vec2 tilemap_block_pixel =
      mod(tilemap_pixel, tilemap_block_size_pixels) / tilemap_block_size_pixels;

  highp vec2 tile =
      vec2(tilemap_block_pixel.x,
           tilemap_base + (tilemap_block_pixel.y + tilemap_block_index_1d) /
                              number_of_tilemap_blocks);

  highp vec4 indices = texture2D(bg_scrolling_tilemap_indices, tile);
  lowp vec4 params = texture2D(bg_scrolling_tilemap_params, tile);

  mediump float num_tiles = 2048.0;
  mediump float tile_block_position =
      (indices.a * 255.0 * 256.0 + indices.r * 255.0) / num_tiles;

  highp vec2 tile_pixel = mod(tilemap_pixel, tile_size) / tile_size;
  tile_pixel = abs(params.xy - tile_pixel);

  lowp vec4 color_indices =
      texture2D(bg_tiles, vec2(tile_pixel.x, tile_base + tile_block_position +
                                                 tile_pixel.y / num_tiles));
  lowp float color_index = large_palette ? color_indices.r : color_indices.a;

  lowp float palette = large_palette ? 0.0 : (params.z * 31.0) / 32.0;
  lowp float palette_offset = (color_index * 255.0 + 0.5) / 256.0;
  lowp vec4 color = texture2D(bg_palette, vec2(palette + palette_offset, 0.5));
  return vec4(color.rgb, sign(color_index));
}

lowp vec4 ScrollingBackground0() {
  return ScrollingBackgroundImpl(bg0_tilemap_base, bg0_size,
                                 bg0_scrolling_screencoord, bg0_mosaic,
                                 bg0_tile_base, bg0_large_palette);
}

lowp vec4 ScrollingBackground1() {
  return ScrollingBackgroundImpl(bg1_tilemap_base, bg1_size,
                                 bg1_scrolling_screencoord, bg1_mosaic,
                                 bg1_tile_base, bg1_large_palette);
}

lowp vec4 ScrollingBackground2() {
  return ScrollingBackgroundImpl(bg2_tilemap_base, bg2_size,
                                 bg2_scrolling_screencoord, bg2_mosaic,
                                 bg2_tile_base, bg2_large_palette);
}

lowp vec4 ScrollingBackground3() {
  return ScrollingBackgroundImpl(bg3_tilemap_base, bg3_size,
                                 bg3_scrolling_screencoord, bg3_mosaic,
                                 bg3_tile_base, bg3_large_palette);
}

// Affine Backgrounds
lowp vec4 AffineBackgroundImpl(highp float tilemap_base,
                               highp vec2 tilemap_size_pixels,
                               highp vec2 tilemap_pixel, highp vec2 mosaic,
                               highp float tile_base, bool wraparound) {
  const highp float num_tiles = 2048.0;
  const highp float tile_size = 8.0;

  highp vec2 wrapped_tilemap_pixel =
      wraparound ? mod(tilemap_pixel, tilemap_size_pixels) : tilemap_pixel;
  highp vec2 lookup_pixel =
      wrapped_tilemap_pixel - mod(wrapped_tilemap_pixel, mosaic);

  highp vec2 lookup_tile = floor(lookup_pixel / tile_size);
  highp vec2 tilemap_size_tiles = tilemap_size_pixels / tile_size;

  highp float tile_index = lookup_tile.x + lookup_tile.y * tilemap_size_tiles.x;
  highp float lookup_tile_1d = tilemap_base + (tile_index + 0.5) / 65536.0;

  mediump vec4 raw_index =
      texture2D(bg_affine_tilemap,
                vec2(lookup_tile_1d / 256.0, mod(lookup_tile_1d, 256.0)));
  mediump float index = raw_index.r * 255.0 / 256.0;

  highp vec2 tile_pixel = mod(lookup_pixel, tile_size) / tile_size;

  lowp vec4 color_indices = texture2D(
      bg_tiles,
      vec2(tile_pixel.x, tile_base + index / 8.0 + tile_pixel.y / num_tiles));
  lowp float color_index = color_indices.r;

  lowp vec4 color = texture2D(bg_palette, vec2(color_index, 0.5));

  lowp float visible = step(0.0, wrapped_tilemap_pixel.x) *
                       step(0.0, wrapped_tilemap_pixel.y) *
                       step(wrapped_tilemap_pixel.x, tilemap_size_pixels.x) *
                       step(wrapped_tilemap_pixel.y, tilemap_size_pixels.y);
  return vec4(color.rgb, sign(color_index) * sign(visible));
}

lowp vec4 AffineBackground2() {
  return AffineBackgroundImpl(bg2_tilemap_base, bg2_size,
                              bg2_affine_screencoord, bg2_mosaic, bg2_tile_base,
                              bg2_wraparound);
}

lowp vec4 AffineBackground3() {
  return AffineBackgroundImpl(bg3_tilemap_base, bg3_size,
                              bg3_affine_screencoord, bg3_mosaic, bg3_tile_base,
                              bg3_wraparound);
}

// Bitmap Backgrounds
lowp vec4 BitmapBackgroundMode3() {
  const highp vec2 bitmap_size = vec2(240.0, 160.0);
  highp vec2 lookup = bg2_affine_screencoord -
                      mod(bg2_affine_screencoord, bg2_mosaic) + vec2(0.5, 0.5);
  lowp vec4 color = texture2D(bg_mode3, lookup / bitmap_size);
  color *= step(bg2_affine_screencoord.x, bitmap_size.x);
  color *= step(bg2_affine_screencoord.y, bitmap_size.y);
  color *= step(-bg2_affine_screencoord.x, 0.0);
  color *= step(-bg2_affine_screencoord.y, 0.0);
  return color;
}

lowp vec4 BitmapBackgroundMode4() {
  const highp vec2 bitmap_size = vec2(240.0, 160.0);
  highp vec2 lookup = bg2_affine_screencoord -
                      mod(bg2_affine_screencoord, bg2_mosaic) + vec2(0.5, 0.5);
  mediump vec4 normalized_index = texture2D(bg_mode4, lookup / bitmap_size);
  mediump float index = (normalized_index.r * 255.0 + 0.5) / 256.0;
  lowp vec4 color = texture2D(bg_palette, vec2(index, 0.5));
  color *= step(bg2_affine_screencoord.x, bitmap_size.x);
  color *= step(bg2_affine_screencoord.y, bitmap_size.y);
  color *= step(-bg2_affine_screencoord.x, 0.0);
  color *= step(-bg2_affine_screencoord.y, 0.0);
  return color;
}

lowp vec4 BitmapBackgroundMode5() {
  const highp vec2 bitmap_size = vec2(160.0, 128.0);
  highp vec2 lookup = bg2_affine_screencoord -
                      mod(bg2_affine_screencoord, bg2_mosaic) + vec2(0.5, 0.5);
  lowp vec4 color = texture2D(bg_mode5, lookup / bitmap_size);
  color *= step(bg2_affine_screencoord.x, bitmap_size.x);
  color *= step(bg2_affine_screencoord.y, bitmap_size.y);
  color *= step(-bg2_affine_screencoord.x, 0.0);
  color *= step(-bg2_affine_screencoord.y, 0.0);
  return color;
}

// Backdrop
lowp vec4 Backdrop() {
  return texture2D(bg_palette, vec2(1.0 / 512.0, 0.5));
}

// Main
out lowp vec4 fragColor;

void main() {
  BlendUnitInitialize();

  ObjectLayer object;
  if (obj_enabled) {
    object = Objects();
  } else {
    object.color = vec4(0.0, 0.0, 0.0, 0.0);
    object.priority = 5;
    object.winobj = false;
    object.blended = false;
  }

  WindowContents window = CheckWindow(object.winobj);
  object.color.a *= float(window.obj);
  BlendUnitAddObject(object);

  if (mode == 0) {
    if (bg0_enabled) {
      lowp vec4 bg0 = ScrollingBackground0();
      bg0.a *= float(window.bg0);
      BlendUnitAddBackground0(bg0);
    }
    if (bg1_enabled) {
      lowp vec4 bg1 = ScrollingBackground1();
      bg1.a *= float(window.bg1);
      BlendUnitAddBackground1(bg1);
    }
    if (bg2_enabled) {
      lowp vec4 bg2 = ScrollingBackground2();
      bg2.a *= float(window.bg2);
      BlendUnitAddBackground2(bg2);
    }
    if (bg3_enabled) {
      lowp vec4 bg3 = ScrollingBackground3();
      bg3.a *= float(window.bg3);
      BlendUnitAddBackground3(bg3);
    }
  } else if (mode == 1) {
    if (bg0_enabled) {
      lowp vec4 bg0 = ScrollingBackground0();
      bg0.a *= float(window.bg0);
      BlendUnitAddBackground0(bg0);
    }
    if (bg1_enabled) {
      lowp vec4 bg1 = ScrollingBackground1();
      bg1.a *= float(window.bg1);
      BlendUnitAddBackground1(bg1);
    }
    if (bg2_enabled) {
      lowp vec4 bg2 = AffineBackground2();
      bg2.a *= float(window.bg2);
      BlendUnitAddBackground2(bg2);
    }
  } else if (mode == 2) {
    if (bg2_enabled) {
      lowp vec4 bg2 = AffineBackground2();
      bg2.a *= float(window.bg2);
      BlendUnitAddBackground2(bg2);
    }
    if (bg3_enabled && window.bg3) {
      lowp vec4 bg3 = AffineBackground3();
      bg3.a *= float(window.bg3);
      BlendUnitAddBackground3(bg3);
    }
  } else if (mode == 3) {
    if (bg2_enabled) {
      lowp vec4 bg2 = BitmapBackgroundMode3();
      bg2.a *= float(window.bg2);
      BlendUnitAddBackground2(bg2);
    }
  } else if (mode == 4) {
    if (bg2_enabled) {
      lowp vec4 bg2 = BitmapBackgroundMode4();
      bg2.a *= float(window.bg2);
      BlendUnitAddBackground2(bg2);
    }
  } else if (mode == 5) {
    if (bg2_enabled) {
      lowp vec4 bg2 = BitmapBackgroundMode5();
      bg2.a *= float(window.bg2);
      BlendUnitAddBackground2(bg2);
    }
  }

  lowp vec4 backdrop = Backdrop();
  BlendUnitAddBackdrop(backdrop);

  fragColor = BlendUnitBlend(window.bld) * float(!blank);
}