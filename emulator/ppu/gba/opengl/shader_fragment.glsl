#version 100

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
uniform lowp sampler2D bg_tiles_s;
uniform mediump sampler2D bg_tiles_d;

// Background Bitmaps
uniform lowp sampler2D bg_mode3;
uniform mediump sampler2D bg_mode4;
uniform mediump sampler2D bg_mode5;

// Background Mosaic
uniform highp vec2 bg0_mosaic;
uniform highp vec2 bg1_mosaic;
uniform highp vec2 bg2_mosaic;
uniform highp vec2 bg3_mosaic;

// Palettes
uniform lowp sampler2D bg_large_palette;
uniform lowp sampler2D bg_small_palette;

// Inputs
varying highp vec2 bg0_scrolling_screencoord;
varying highp vec2 bg1_scrolling_screencoord;
varying highp vec2 bg2_scrolling_screencoord;
varying highp vec2 bg3_scrolling_screencoord;
varying highp vec2 bg2_affine_screencoord;
varying highp vec2 bg3_affine_screencoord;
varying highp vec2 screencoord;

// Objects
struct ObjectAttributes {
  bool enabled;
  highp mat2 affine;
  highp vec2 origin;
  highp vec2 size;
  mediump vec2 mosaic;
  highp float tile_base;
  bool large_palette;
  bool rendered;
  bool blended;
  bool flip_x;
  bool flip_y;
  int palette;
  int priority;
};

uniform ObjectAttributes obj_attributes[128];
uniform mediump sampler2D obj_tiles_d;
uniform lowp sampler2D obj_tiles_s;
uniform lowp sampler2D obj_large_palette;
uniform lowp sampler2D obj_small_palette;

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

  for (int i = 0; i < 128; i++) {
    if (!obj_attributes[i].enabled) {
      continue;
    }

    if (screencoord.x < obj_attributes[i].origin.x ||
        screencoord.y < obj_attributes[i].origin.y) {
      continue;
    }

    highp vec2 end = obj_attributes[i].origin + obj_attributes[i].size;

    if (end.x < screencoord.x || end.y < screencoord.y) {
      continue;
    }

    highp vec2 half_size = obj_attributes[i].size / 2.0;
    highp vec2 center = obj_attributes[i].origin + half_size;
    highp vec2 from_center = screencoord - center;

    highp vec2 lookup = obj_attributes[i].affine * from_center + half_size;

    if (lookup.x < 0.0 || obj_attributes[i].size.x < lookup.x ||
        lookup.y < 0.0 || obj_attributes[i].size.y < lookup.y) {
      continue;
    }

    lookup = mod(lookup, obj_attributes[i].mosaic) + vec2(0.5, 0.5);

    highp vec2 lookup_tile = floor(lookup / 8.0);

    highp float tile_index = obj_attributes[i].tile_base;
    if (obj_mode) {
      tile_index +=
          lookup_tile.x + lookup_tile.y * obj_attributes[i].size.x / 8.0;
    } else {
      highp float row_width = (obj_attributes[i].large_palette) ? 16.0 : 32.0;
      tile_index += lookup_tile.x + lookup_tile.y * row_width;
    }

    highp vec2 tile_pixel = mod(lookup_tile, 8.0) / 8.0;

    if (obj_attributes[i].flip_x) {
      tile_pixel.x = 1.0 - tile_pixel.x;
    }

    if (obj_attributes[i].flip_y) {
      tile_pixel.y = 1.0 - tile_pixel.y;
    }

    lowp vec4 color;
    if (obj_attributes[i].large_palette) {
      const highp float num_tiles = 512.0;
      mediump vec4 color_index = texture2D(
          obj_tiles_d,
          vec2(tile_pixel.x, (tile_index + tile_pixel.y) / num_tiles));
      if (color_index.r == 0.0) {
        continue;
      }

      color = texture2D(obj_large_palette, vec2(color_index.r, 0.5));
    } else {
      const highp float num_tiles = 1024.0;
      mediump vec4 color_index = texture2D(
          obj_tiles_s,
          vec2(tile_pixel.x, (tile_index + tile_pixel.y) / num_tiles));
      if (color_index.r == 0.0) {
        continue;
      }

      color = texture2D(obj_small_palette,
                        vec2(color_index.r, obj_attributes[i].palette));
    }

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

int blend_priorities[2];
bool blend_top[2];
bool blend_bottom[2];
bool blend_is_object[2];
bool blend_contains_object;
bool blend_obj_semi_transparent;
lowp vec3 blend_layers[2];

void BlendUnitInitialize() {
  blend_priorities[0] = 6;
  blend_priorities[1] = 6;
  blend_top[0] = false;
  blend_top[1] = false;
  blend_bottom[0] = false;
  blend_bottom[1] = false;
  blend_is_object[0] = false;
  blend_is_object[1] = false;
  blend_contains_object = false;
  blend_obj_semi_transparent = false;
}

void BlendUnitAddObject(ObjectLayer obj) {
  if (obj.color.a == 0.0) {
    return;
  }

  blend_priorities[0] = obj.priority;
  blend_layers[0] = obj.color.rgb;
  blend_top[0] = obj.blended || obj_top;
  blend_bottom[0] = obj_bottom;
  blend_is_object[0] = true;
  blend_contains_object = true;
  blend_obj_semi_transparent = obj.blended;
}

void BlendUnitAddBackground(int priority, bool top, bool bottom,
                            lowp vec4 color) {
  if (color.a == 0.0 || blend_priorities[1] <= priority) {
    return;
  }

  blend_contains_object = blend_contains_object ^^ blend_is_object[1];

  if (blend_priorities[0] > priority) {
    blend_priorities[1] = blend_priorities[0];
    blend_layers[1] = blend_layers[0];
    blend_top[1] = blend_top[0];
    blend_bottom[1] = blend_bottom[0];
    blend_is_object[1] = blend_is_object[0];
    blend_priorities[0] = priority;
    blend_layers[0] = color.rgb;
    blend_top[0] = top;
    blend_bottom[0] = bottom;
    blend_is_object[0] = false;
  } else {
    blend_priorities[1] = priority;
    blend_layers[1] = color.rgb;
    blend_top[1] = top;
    blend_bottom[1] = bottom;
    blend_is_object[1] = false;
  }
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

lowp vec3 BlendUnitAdditiveBlendImpl() {
  return min((blend_layers[0] * blend_eva) + (blend_layers[1] * blend_evb),
             1.0);
}

lowp vec3 BlendUnitNoBlendImpl() { return blend_layers[0]; }

lowp vec3 BlendUnitNoBlend() {
  if (blend_contains_object && blend_obj_semi_transparent && blend_top[0] &&
      blend_bottom[1]) {
    return BlendUnitAdditiveBlendImpl();
  }

  return BlendUnitNoBlendImpl();
}

lowp vec3 BlendUnitAdditiveBlend() {
  if (!blend_top[0] || !blend_bottom[1]) {
    return BlendUnitNoBlendImpl();
  }

  return BlendUnitAdditiveBlendImpl();
}

lowp vec3 BlendUnitBrighten() {
  if (!blend_top[0]) {
    return BlendUnitNoBlendImpl();
  }

  if (blend_contains_object && blend_obj_semi_transparent && blend_bottom[1]) {
    return BlendUnitAdditiveBlendImpl();
  }

  return max(blend_layers[0] * (1.0 - blend_evy), 0.0);
}

lowp vec3 BlendUnitDarken() {
  if (!blend_top[0]) {
    return BlendUnitNoBlendImpl();
  }

  if (blend_contains_object && blend_obj_semi_transparent && blend_bottom[1]) {
    return BlendUnitAdditiveBlendImpl();
  }

  return min(blend_layers[0] * blend_evy, 1.0);
}

lowp vec4 BlendUnitBlend(bool enable_blend) {
  lowp vec3 color;
  if (!enable_blend) {
    color = BlendUnitNoBlendImpl();
  } else {
    if (blend_mode == 0) {
      color = BlendUnitNoBlend();
    } else if (blend_mode == 1) {
      color = BlendUnitAdditiveBlend();
    } else if (blend_mode == 2) {
      color = BlendUnitBrighten();
    } else {
      color = BlendUnitDarken();
    }
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
  return ((start <= end && start <= location && location <= end) ||
          (end < start && (location < start || end < location)));
}

bool IsInsideWindow2D(highp vec2 start, highp vec2 end, highp vec2 location) {
  return IsInsideWindow1D(start.x, end.x, location.x) &&
         IsInsideWindow1D(start.y, end.y, location.y);
}

WindowContents CheckWindow(bool on_object) {
  if (win0_enabled && IsInsideWindow2D(win0_start, win0_end, screencoord)) {
    return win0;
  }

  if (win1_enabled && IsInsideWindow2D(win1_start, win1_end, screencoord)) {
    return win1;
  }

  if (winobj_enabled && on_object) {
    return winobj;
  }

  if (win0_enabled || win1_enabled || winobj_enabled) {
    return winout;
  }

  WindowContents result;
  result.bg0 = true;
  result.bg1 = true;
  result.bg2 = true;
  result.bg3 = true;
  result.obj = true;
  result.bld = true;

  return result;
}

// Scrolling Tile Map
struct ScrollingTilemapEntry {
  highp float tile_block_position;
  bool flip_x;
  bool flip_y;
  lowp float palette;
};

ScrollingTilemapEntry GetScrollingTileMapEntry(highp float tilemap_base,
                                               highp vec2 tilemap_size_pixels,
                                               highp vec2 tilemap_pixel) {
  const highp float number_of_tilemap_blocks = 32.0;
  const highp float tile_size_pixels = 8.0;
  const highp float tilemap_block_size_pixels =
      number_of_tilemap_blocks * tile_size_pixels;

  highp vec2 tilemap_block_index_2d =
      floor(tilemap_pixel / tilemap_block_size_pixels);
  highp float tilemap_width_blocks =
      tilemap_size_pixels.x / tilemap_block_size_pixels;
  highp float tilemap_block_index_1d =
      tilemap_block_index_2d.x +
      tilemap_block_index_2d.y * tilemap_width_blocks;

  highp vec2 tilemap_block_pixel =
      mod(tilemap_pixel, tilemap_block_size_pixels) / tilemap_block_size_pixels;

  highp vec2 lookup =
      vec2(tilemap_block_pixel.x,
           tilemap_base + (tilemap_block_pixel.y + tilemap_block_index_1d) /
                              number_of_tilemap_blocks);

  highp vec4 indices = texture2D(bg_scrolling_tilemap_indices, lookup);
  lowp vec4 params = texture2D(bg_scrolling_tilemap_params, lookup);

  ScrollingTilemapEntry result;
  result.flip_x = params.x != 0.0;
  result.flip_y = params.y != 0.0;
  result.palette = (params.z * 15.0 + 0.5) / 16.0;
  result.tile_block_position =
      (indices.r != 0.0)
          ? (indices.a * 255.0 * 256.0 + indices.r * 255.0) / 1024.0
          : 0.0;

  return result;
}

lowp vec4 ScrollingBackgroundImpl(highp float tilemap_base,
                                  highp vec2 tilemap_size_pixels,
                                  highp vec2 tilemap_pixel, highp vec2 mosaic,
                                  highp float tile_base, bool large_palette) {
  highp vec2 wrapped_tilemap_pixel = mod(tilemap_pixel, tilemap_size_pixels);
  highp vec2 lookup = wrapped_tilemap_pixel -
                      mod(wrapped_tilemap_pixel, mosaic) + vec2(0.5, 0.5);

  ScrollingTilemapEntry entry =
      GetScrollingTileMapEntry(tilemap_base, tilemap_size_pixels, lookup);

  const highp float tile_size = 8.0;
  highp vec2 tile_pixel = mod(lookup, tile_size) / tile_size;

  if (entry.flip_x) {
    tile_pixel.x = 1.0 - tile_pixel.x;
  }

  if (entry.flip_y) {
    tile_pixel.y = 1.0 - tile_pixel.y;
  }

  lowp vec4 color;
  if (large_palette) {
    const highp float num_tiles = 1024.0;
    mediump vec4 color_index = texture2D(
        bg_tiles_d, vec2(tile_pixel.x, tile_base + entry.tile_block_position +
                                           tile_pixel.y / num_tiles));
    if (color_index.r == 0.0) {
      return vec4(0.0, 0.0, 0.0, 0.0);
    }

    color = texture2D(bg_large_palette, vec2(color_index.r, 0.5));
  } else {
    const highp float num_tiles = 2048.0;
    mediump vec4 color_index = texture2D(
        bg_tiles_s,
        vec2(tile_pixel.x, tile_base + entry.tile_block_position / 2.0 +
                               tile_pixel.y / num_tiles));
    if (color_index.r == 0.0) {
      return vec4(0.0, 0.0, 0.0, 0.0);
    }

    color = texture2D(bg_small_palette, vec2(color_index.r, entry.palette));
  }

  return color;
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

lowp vec4 AffineBackgroundImpl(highp float tilemap_base,
                               highp vec2 tilemap_size_pixels,
                               highp vec2 tilemap_pixel, highp vec2 mosaic,
                               highp float tile_base, bool wraparound) {
  if (!wraparound && tilemap_pixel.x < 0.0 || tilemap_pixel.y < 0.0 ||
      tilemap_pixel.x > tilemap_size_pixels.x ||
      tilemap_pixel.y > tilemap_size_pixels.y) {
    return vec4(0.0, 0.0, 0.0, 0.0);
  }

  highp vec2 wrapped_tilemap_pixel = mod(tilemap_pixel, tilemap_size_pixels);
  highp vec2 lookup_pixel =
      wrapped_tilemap_pixel - mod(wrapped_tilemap_pixel, mosaic);

  const highp float tile_size = 8.0;
  highp vec2 lookup_tile = floor(lookup_pixel / tile_size);
  highp vec2 tilemap_size_tiles = tilemap_size_pixels / tile_size;

  highp float tile_index = lookup_tile.x + lookup_tile.y * tilemap_size_tiles.x;
  highp float lookup_tile_1d = tilemap_base + (tile_index + 0.5) / 65536.0;

  mediump vec4 raw_index =
      texture2D(bg_affine_tilemap,
                vec2(lookup_tile_1d / 256.0, mod(lookup_tile_1d, 256.0)));
  mediump float index = raw_index.r * 255.0 / 256.0;

  highp vec2 tile_pixel = mod(lookup_pixel, tile_size) / tile_size;

  const highp float num_tiles = 1024.0;
  mediump vec4 color_index = texture2D(
      bg_tiles_d,
      vec2(tile_pixel.x, tile_base + index / 4.0 + tile_pixel.y / num_tiles));
  if (color_index.r == 0.0) {
    return vec4(0.0, 0.0, 0.0, 0.0);
  }

  return texture2D(bg_large_palette, vec2(color_index.r, 0.5));
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

lowp vec4 Background2Mode3() {
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

lowp vec4 Background2Mode4() {
  const highp vec2 bitmap_size = vec2(240.0, 160.0);
  highp vec2 lookup = bg2_affine_screencoord -
                      mod(bg2_affine_screencoord, bg2_mosaic) + vec2(0.5, 0.5);
  mediump vec4 normalized_index = texture2D(bg_mode4, lookup / bitmap_size);
  mediump float index = (normalized_index.r * 255.0 + 0.5) / 256.0;
  lowp vec4 color = texture2D(bg_large_palette, vec2(index, 0.5));
  color *= step(bg2_affine_screencoord.x, bitmap_size.x);
  color *= step(bg2_affine_screencoord.y, bitmap_size.y);
  color *= step(-bg2_affine_screencoord.x, 0.0);
  color *= step(-bg2_affine_screencoord.y, 0.0);
  return color;
}

lowp vec4 Background2Mode5() {
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

lowp vec4 Backdrop() {
  return texture2D(bg_large_palette, vec2(1.0 / 512.0, 0.5));
}

void main() {
  if (blank) {
    gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    return;
  }

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
  if (window.obj) {
    BlendUnitAddObject(object);
  }

  if (mode == 0) {
    if (bg0_enabled && window.bg0) {
      lowp vec4 bg0 = ScrollingBackground0();
      BlendUnitAddBackground0(bg0);
    }
    if (bg1_enabled && window.bg1) {
      lowp vec4 bg1 = ScrollingBackground1();
      BlendUnitAddBackground1(bg1);
    }
    if (bg2_enabled && window.bg2) {
      lowp vec4 bg2 = ScrollingBackground2();
      BlendUnitAddBackground2(bg2);
    }
    if (bg3_enabled && window.bg3) {
      lowp vec4 bg3 = ScrollingBackground3();
      BlendUnitAddBackground3(bg3);
    }
  } else if (mode == 1) {
    if (bg0_enabled && window.bg0) {
      lowp vec4 bg0 = ScrollingBackground0();
      BlendUnitAddBackground0(bg0);
    }
    if (bg1_enabled && window.bg1) {
      lowp vec4 bg1 = ScrollingBackground1();
      BlendUnitAddBackground1(bg1);
    }
    if (bg2_enabled && window.bg2) {
      lowp vec4 bg2 = AffineBackground2();
      BlendUnitAddBackground2(bg2);
    }
  } else if (mode == 2) {
    if (bg2_enabled && window.bg2) {
      lowp vec4 bg2 = AffineBackground2();
      BlendUnitAddBackground2(bg2);
    }
    if (bg3_enabled && window.bg3) {
      lowp vec4 bg3 = AffineBackground3();
      BlendUnitAddBackground3(bg3);
    }
  } else if (mode == 3) {
    if (bg2_enabled && window.bg2) {
      lowp vec4 bg2 = Background2Mode3();
      BlendUnitAddBackground2(bg2);
    }
  } else if (mode == 4) {
    if (bg2_enabled && window.bg2) {
      lowp vec4 bg2 = Background2Mode4();
      BlendUnitAddBackground2(bg2);
    }
  } else if (mode == 5) {
    if (bg2_enabled && window.bg2) {
      lowp vec4 bg2 = Background2Mode5();
      BlendUnitAddBackground2(bg2);
    }
  }

  lowp vec4 backdrop = Backdrop();
  BlendUnitAddBackdrop(backdrop);

  gl_FragColor = BlendUnitBlend(window.bld);
}