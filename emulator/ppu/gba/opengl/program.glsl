// version is set in program.c

// Backdrop
uniform lowp vec4 backdrop;

// Background Bitmaps
uniform lowp sampler2D bg_mode3;
uniform mediump sampler2D bg_mode4;
uniform mediump sampler2D bg_mode5;

// Background Mosaic
uniform highp vec2 bg2_mosaic;

// Background Palettes
uniform lowp sampler2D bg_large_palette;

// Constants
const mediump float large_palette_offset;
const highp vec2 mode34_bitmap_size = vec2(240.0, 160.0);
const highp vec2 mode5_bitmap_size = vec2(160.0, 128.0);

// Inputs
varying highp vec2 bg2_texcoord;  // In screen space

vec4 Background2Mode3() {
  highp vec2 lookup =
      bg2_texcoord - mod(bg2_texcoord, bg2_mosaic) + vec2(0.5, 0.5);
  lowp vec4 color = texture2D(bg_mode3, lookup / mode34_bitmap_size);
  color *= step(bg2_texcoord.x, mode34_bitmap_size.x);
  color *= step(bg2_texcoord.y, mode34_bitmap_size.y);
  color *= step(-bg2_texcoord.x, 0.0);
  color *= step(-bg2_texcoord.y, 0.0);
  return color;
}

vec4 Background2Mode4() {
  highp vec2 lookup =
      bg2_texcoord - mod(bg2_texcoord, bg2_mosaic) + vec2(0.5, 0.5);
  mediump vec4 index = texture2D(bg_mode4, lookup / mode34_bitmap_size);
  lowp vec4 color =
      texture2D(bg_large_palette, vec2(index.r + large_palette_offset, 0.5));
  color *= step(bg2_texcoord.x, mode34_bitmap_size.x);
  color *= step(bg2_texcoord.y, mode34_bitmap_size.y);
  color *= step(-bg2_texcoord.x, 0.0);
  color *= step(-bg2_texcoord.y, 0.0);
  return color;
}

vec4 Background2Mode5() {
  highp vec2 lookup =
      bg2_texcoord - mod(bg2_texcoord, bg2_mosaic) + vec2(0.5, 0.5);
  lowp vec4 color = texture2D(bg_mode5, lookup / mode5_bitmap_size);
  color *= step(bg2_texcoord.x, mode5_bitmap_size.x);
  color *= step(bg2_texcoord.y, mode5_bitmap_size.y);
  color *= step(-bg2_texcoord.x, 0.0);
  color *= step(-bg2_texcoord.y, 0.0);
  return color;
}

vec4 Mode0() {
  return vec4(0.0, 0.0, 0.0, 1.0);
}

vec4 Mode1() {
  return vec4(0.0, 0.0, 0.0, 1.0);
}

vec4 Mode2() {
  return vec4(0.0, 0.0, 0.0, 1.0);
}

vec4 Mode3() {
#if defined(BG2_ENABLED)
  vec4 bg2 = Background2Mode3();
#else
  vec4 bg2 = vec4(0.0, 0.0, 0.0, 1.0);
#endif
  return bg2;
}

vec4 Mode4() {
#if defined(BG2_ENABLED)
  vec4 bg2 = Background2Mode4();
#else
  vec4 bg2 = vec4(0.0, 0.0, 0.0, 1.0);
#endif
  return bg2;
}

vec4 Mode5() {
#if defined(BG2_ENABLED)
  vec4 bg2 = Background2Mode5();
#else
  vec4 bg2 = vec4(0.0, 0.0, 0.0, 1.0);
#endif
}

void main() {
#if defined(MODE0)
  gl_FragColor = Mode0();
#elif defined(MODE1)
  gl_FragColor = Mode1();
#elif defined(MODE2)
  gl_FragColor = Mode2();
#elif defined(MODE3)
  gl_FragColor = Mode3();
#elif defined(MODE4)
  gl_FragColor = Mode4();
#elif defined(MODE5)
  gl_FragColor = Mode5();
#else
  gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
#endif
}