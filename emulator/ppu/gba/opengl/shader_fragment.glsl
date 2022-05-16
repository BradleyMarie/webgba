#version 100

// Display Controls
uniform bool blank;
uniform int mode;
uniform bool bg0_enabled;
uniform bool bg1_enabled;
uniform bool bg2_enabled;
uniform bool bg3_enabled;
uniform bool win0_enabled;
uniform bool win1_enabled;
uniform bool winobj_enabled;

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
uniform int bg3_priority;
uniform highp vec2 bg3_size;
uniform highp float bg3_tilemap_base;
uniform highp float bg3_tile_base;
uniform bool bg3_large_palette;

// Layer Controls
uniform int obj_priority;
const int bd_priority = 5;

// Tilemap
uniform lowp sampler2D tilemap_palette;
uniform lowp sampler2D tilemap_flip_x;
uniform lowp sampler2D tilemap_flip_y;
uniform highp sampler2D tilemap_tile;

// Tiles
uniform lowp sampler2D tiles_s;
uniform mediump sampler2D tiles_d;

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
const mediump float bg_large_palette_sample_offset = 1.0 / 512.0;
uniform lowp sampler2D bg_small_palette;
const mediump float bg_small_palette_sample_offset = 1.0 / 32.0;

// Inputs
varying highp vec2 bg0_scrolling_screencoord;
varying highp vec2 bg1_scrolling_screencoord;
varying highp vec2 bg2_scrolling_screencoord;
varying highp vec2 bg3_scrolling_screencoord;
varying highp vec2 bg2_affine_screencoord;
varying highp vec2 bg3_affine_screencoord;
varying highp vec2 screencoord;

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

void BlendUnitAddBackground(int priority, bool top, bool bottom,
                            lowp vec4 color) {
  if (color.a == 0.0 || blend_priorities[1] <= priority) {
    return;
  }

  blend_contains_object = blend_contains_object ^ ^blend_is_object[1];

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
  return max((blend_layers[0] * blend_eva) + (blend_layers[1] * blend_evb),
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

  return min(blend_layers[0] * (1.0 - blend_evy), 0.0);
}

lowp vec3 BlendUnitDarken() {
  if (!blend_top[0]) {
    return BlendUnitNoBlendImpl();
  }

  if (blend_contains_object && blend_obj_semi_transparent && blend_bottom[1]) {
    return BlendUnitAdditiveBlendImpl();
  }

  return max(blend_layers[0] * blend_evy, 1.0);
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

  return vec4(color.b, color.g, color.r, 1.0);
}

// Window
struct WindowContents {
  bool bg0;
  bool bg1;
  bool bg2;
  bool bg3;
  bool obj;
  bool blend;
};

uniform bool win0_contents[6];
uniform bool win1_contents[6];
uniform bool winobj_contents[6];
uniform bool winout_contents[6];

uniform highp vec2 win0_start;
uniform highp vec2 win0_end;
uniform highp vec2 win1_start;
uniform highp vec2 win1_end;

bool IsInsideWindow1D(highp float start, highp float end,
                      highp float location) {
  return ((start <= end && start <= location && location <= end) ||
          (end > start && (location < start || end < location)));
}

bool IsInsideWindow2D(highp vec2 start, highp vec2 end, highp vec2 location) {
  return IsInsideWindow1D(start.x, end.x, location.x) &&
         IsInsideWindow1D(start.y, end.y, location.y);
}

WindowContents CheckWindow(bool on_object) {
  WindowContents result;
  if (win0_enabled && IsInsideWindow2D(win0_start, win0_end, screencoord)) {
    result.bg0 = win0_contents[0];
    result.bg1 = win0_contents[1];
    result.bg2 = win0_contents[2];
    result.bg3 = win0_contents[3];
    result.obj = win0_contents[4];
    result.blend = win0_contents[5];
    return result;
  }

  if (win1_enabled && IsInsideWindow2D(win1_start, win1_end, screencoord)) {
    result.bg0 = win1_contents[0];
    result.bg1 = win1_contents[1];
    result.bg2 = win1_contents[2];
    result.bg3 = win1_contents[3];
    result.obj = win1_contents[4];
    result.blend = win1_contents[5];
    return result;
  }

  if (winobj_enabled && on_object) {
    result.bg0 = winobj_contents[0];
    result.bg1 = winobj_contents[1];
    result.bg2 = winobj_contents[2];
    result.bg3 = winobj_contents[3];
    result.obj = winobj_contents[4];
    result.blend = winobj_contents[5];
    return result;
  }

  if (win0_enabled || win1_enabled || winobj_enabled) {
    result.bg0 = winout_contents[0];
    result.bg1 = winout_contents[1];
    result.bg2 = winout_contents[2];
    result.bg3 = winout_contents[3];
    result.obj = winout_contents[4];
    result.blend = winout_contents[5];
    return result;
  }

  result.bg0 = true;
  result.bg1 = true;
  result.bg2 = true;
  result.bg3 = true;
  result.obj = true;
  result.blend = true;
  return result;
}

// Background Layers
highp vec2 TileMapEntryCoordinate(highp float tilemap_base, highp vec2 lookup,
                                  float tilemap_width) {
  const highp float tilemap_block_size_pixels = 32.0;
  const highp float tilemap_block_size_entries =
      tilemap_block_size_pixels * tilemap_block_size_pixels;

  highp vec2 tilemap_block = lookup / tilemap_block_size_pixels;

  highp float tilemap_block_index =
      tilemap_block.y * tilemap_width / tilemap_block_size_pixels +
      tilemap_block.x;

  highp float tilemap_entry_index =
      tilemap_block_index * tilemap_block_size_entries +
      mod(lookup.y, tilemap_block_size_pixels) * tilemap_block_size_pixels +
      mod(lookup.x, tilemap_block_size_pixels);

  highp float offset = (float(tilemap_entry_index) + 0.5) / float(96 * 1024);

  return vec2(tilemap_base + offset, 0.5);
}

lowp vec4 ScrollingBackgroundImpl(highp vec2 offset, highp vec2 size,
                                  highp vec2 mosaic, highp float tilemap_base,
                                  highp float tile_base, bool large_palette) {
  const highp float tile_size = 8.0;

  highp vec2 lookup = mod(mod(screencoord + offset, size), mosaic);
  highp vec2 tile_pixel = mod(lookup, float(tile_size));

  highp vec2 tilemap_entry =
      TileMapEntryCoordinate(tilemap_base, lookup, size.x);

  if (texture2D(tilemap_flip_x, tilemap_entry).x != 0.0) {
    tile_pixel.x = tile_size - tile_pixel.x;
  }

  if (texture2D(tilemap_flip_y, tilemap_entry).x != 0.0) {
    tile_pixel.y = tile_size - tile_pixel.y;
  }

  lowp vec4 color;
  if (large_palette) {
    const highp float num_dtiles = 2048.0;
    highp float tile_location =
        (float(tile_pixel.y * tile_size + tile_pixel.x) + 0.5) / num_dtiles;
    mediump vec4 color_index =
        texture2D(tiles_d, vec2(tile_base + tile_location, 0.5));
    if (color_index.r == 0.0) {
      return vec4(0.0, 0.0, 0.0, 0.0);
    }
    color =
        texture2D(bg_large_palette,
                  vec2(color_index.r + bg_large_palette_sample_offset, 0.5));
  } else {
    const highp float num_dtiles = 1024.0;
    highp float tile_location =
        (float(tile_pixel.y * tile_size + tile_pixel.x) + 0.5) / num_dtiles;
    mediump vec4 color_index =
        texture2D(tiles_s, vec2(tile_base + tile_location, 0.5));
    if (color_index.r == 0.0) {
      return vec4(0.0, 0.0, 0.0, 0.0);
    }
    mediump float palette = texture2D(tilemap_palette, tilemap_entry).r;
    color = texture2D(bg_large_palette,
                      vec2(color_index.r + bg_small_palette_sample_offset,
                           palette + bg_small_palette_sample_offset));
  }

  return color;
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
  mediump vec4 index = texture2D(bg_mode4, lookup / bitmap_size);
  lowp vec4 color = texture2D(
      bg_large_palette, vec2(index.r + bg_large_palette_sample_offset, 0.5));
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
  return texture2D(bg_large_palette, vec2(bg_large_palette_sample_offset, 0.5));
}

void main() {
  if (blank) {
    gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    return;
  }

  BlendUnitInitialize();

  WindowContents window = CheckWindow(false);

  if (mode == 0) {
    // TODO
  } else if (mode == 1) {
    // TODO
  } else if (mode == 2) {
    // TODO
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

  gl_FragColor = BlendUnitBlend(window.blend);
}