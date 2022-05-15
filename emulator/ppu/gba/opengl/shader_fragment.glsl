#version 100

// Controls
uniform bool blank;
uniform int mode;

uniform bool bg0_enabled;
uniform bool bg1_enabled;
uniform bool bg2_enabled;
uniform bool bg3_enabled;

uniform bool win0_enabled;
uniform bool win1_enabled;
uniform bool winobj_enabled;

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
uniform lowp sampler2D bg_palette;
const mediump float bg_palette_sample_offset = 1.0 / 512.0;

// Inputs
varying highp vec2 bg2_affine_screencoord;
varying highp vec2 bg3_affine_screencoord;
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

vec4 Backdrop() {
  return texture2D(bg_palette,
                   vec2(bg_palette_sample_offset, bg_palette_sample_offset));
}

vec4 Background2Mode3() {
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

vec4 Background2Mode4() {
  const highp vec2 bitmap_size = vec2(240.0, 160.0);
  highp vec2 lookup = bg2_affine_screencoord -
                      mod(bg2_affine_screencoord, bg2_mosaic) + vec2(0.5, 0.5);
  mediump vec4 index = texture2D(bg_mode4, lookup / bitmap_size);
  lowp vec4 color =
      texture2D(bg_palette, vec2(index.r + bg_palette_sample_offset, 0.5));
  color *= step(bg2_affine_screencoord.x, bitmap_size.x);
  color *= step(bg2_affine_screencoord.y, bitmap_size.y);
  color *= step(-bg2_affine_screencoord.x, 0.0);
  color *= step(-bg2_affine_screencoord.y, 0.0);
  return color;
}

vec4 Background2Mode5() {
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

vec4 Mode0(WindowContents window) { return Backdrop(); }

vec4 Mode1(WindowContents window) { return Backdrop(); }

vec4 Mode2(WindowContents window) { return Backdrop(); }

vec4 Mode3(WindowContents window) {
  if (window.bg2 && bg2_enabled) {
    return Background2Mode3();
  }
  return Backdrop();
}

vec4 Mode4(WindowContents window) {
  if (window.bg2 && bg2_enabled) {
    return Background2Mode4();
  }
  return Backdrop();
}

vec4 Mode5(WindowContents window) {
  if (window.bg2 && bg2_enabled) {
    return Background2Mode5();
  }
  return Backdrop();
}

void main(WindowContents window) {
  if (blank) {
    gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    return;
  }

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