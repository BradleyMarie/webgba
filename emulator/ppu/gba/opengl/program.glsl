#version 100

// Controls
uniform int mode;

uniform bool bg0_enabled;
uniform bool bg1_enabled;
uniform bool bg2_enabled;
uniform bool bg3_enabled;

uniform bool win0_enabled;
uniform bool win1_enabled;
uniform bool winobj_enabled;

// Backdrop
uniform lowp vec4 backdrop;

// Background Bitmaps
uniform lowp sampler2D bg_mode3;
uniform mediump sampler2D bg_mode4;
uniform mediump sampler2D bg_mode5;

// Background Mosaic
uniform highp vec2 bg2_mosaic;

// Palettes
uniform lowp sampler2D bg_large_palette;
const mediump float large_palette_offset = 1.0 / 512.0;

// Inputs
varying highp vec2 bg2_texcoord;  // In screen space
varying highp vec2 screencoord;

// Window
struct WindowContents {
  bool bg0;
  bool bg1;
  bool bg2;
  bool bg3;
  bool obj;
  bool blend;
};

uniform WindowContents win0_contents;
uniform WindowContents win1_contents;
uniform WindowContents winobj_contents;
uniform WindowContents winout_contents;

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
  if (win0_enabled && IsInsideWindow2D(win0_start, win0_end, screencoord)) {
    return win0_contents;
  }

  if (win1_enabled && IsInsideWindow2D(win1_start, win1_end, screencoord)) {
    return win1_contents;
  }

  if (winobj_enabled && on_object) {
    return winobj_contents;
  }

  if (win0_enabled || win1_enabled || winobj_enabled) {
    return winobj_contents;
  }

  WindowContents result;
  result.bg0 = true;
  result.bg1 = true;
  result.bg2 = true;
  result.bg3 = true;
  result.obj = true;
  result.blend = true;
  return result;
}

vec4 Background2Mode3() {
  const highp vec2 bitmap_size = vec2(240.0, 160.0);
  highp vec2 lookup =
      bg2_texcoord - mod(bg2_texcoord, bg2_mosaic) + vec2(0.5, 0.5);
  lowp vec4 color = texture2D(bg_mode3, lookup / bitmap_size);
  color *= step(bg2_texcoord.x, bitmap_size.x);
  color *= step(bg2_texcoord.y, bitmap_size.y);
  color *= step(-bg2_texcoord.x, 0.0);
  color *= step(-bg2_texcoord.y, 0.0);
  return color;
}

vec4 Background2Mode4() {
  const highp vec2 bitmap_size = vec2(240.0, 160.0);
  highp vec2 lookup =
      bg2_texcoord - mod(bg2_texcoord, bg2_mosaic) + vec2(0.5, 0.5);
  mediump vec4 index = texture2D(bg_mode4, lookup / bitmap_size);
  lowp vec4 color =
      texture2D(bg_large_palette, vec2(index.r + large_palette_offset, 0.5));
  color *= step(bg2_texcoord.x, bitmap_size.x);
  color *= step(bg2_texcoord.y, bitmap_size.y);
  color *= step(-bg2_texcoord.x, 0.0);
  color *= step(-bg2_texcoord.y, 0.0);
  return color;
}

vec4 Background2Mode5() {
  const highp vec2 bitmap_size = vec2(160.0, 128.0);
  highp vec2 lookup =
      bg2_texcoord - mod(bg2_texcoord, bg2_mosaic) + vec2(0.5, 0.5);
  lowp vec4 color = texture2D(bg_mode5, lookup / bitmap_size);
  color *= step(bg2_texcoord.x, bitmap_size.x);
  color *= step(bg2_texcoord.y, bitmap_size.y);
  color *= step(-bg2_texcoord.x, 0.0);
  color *= step(-bg2_texcoord.y, 0.0);
  return color;
}

vec4 Mode0(WindowContents window) { return vec4(0.0, 0.0, 0.0, 1.0); }

vec4 Mode1(WindowContents window) { return vec4(0.0, 0.0, 0.0, 1.0); }

vec4 Mode2(WindowContents window) { return vec4(0.0, 0.0, 0.0, 1.0); }

vec4 Mode3(WindowContents window) {
  if (window.bg2 && bg2_enabled) {
    return Background2Mode3();
  }
  return vec4(0.0, 0.0, 0.0, 1.0);
}

vec4 Mode4(WindowContents window) {
  if (window.bg2 && bg2_enabled) {
    return Background2Mode4();
  }
  return vec4(0.0, 0.0, 0.0, 1.0);
}

vec4 Mode5(WindowContents window) {
  if (window.bg2 && bg2_enabled) {
    return Background2Mode5();
  }
  return vec4(0.0, 0.0, 0.0, 1.0);
}

void main(WindowContents window) {
  WindowContents window = CheckWindow(false);

  if (mode == 0) {
    gl_FragColor = Mode0(window);
  } else if (mode == 1) {
    gl_FragColor = Mode1(window);
  } else if (mode == 2) {
    gl_FragColor = Mode2(window);
  } else if (mode == 3) {
    gl_FragColor = Mode3(window);
  } else if (mode == 4) {
    gl_FragColor = Mode4(window);
  } else if (mode == 5) {
    gl_FragColor = Mode5(window);
  } else {
    gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
  }
}