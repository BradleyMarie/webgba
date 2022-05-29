#version 300 es

// Inputs
in highp vec2 bg0_scrolling; // In GBA screen space
in highp vec2 bg1_scrolling; // In GBA screen space
in highp vec2 bg2_scrolling; // In GBA screen space
in highp vec2 bg3_scrolling; // In GBA screen space
in highp vec2 bg2_affine; // In GBA screen space
in highp vec2 bg3_affine; // In GBA screen space

// Outputs
out highp vec2 bg0_scrolling_screencoord;
out highp vec2 bg1_scrolling_screencoord;
out highp vec2 bg2_scrolling_screencoord;
out highp vec2 bg3_scrolling_screencoord;
out highp vec2 bg2_affine_screencoord;
out highp vec2 bg3_affine_screencoord;
out highp vec2 screencoord;

void main() {
  highp float x = -1.0 + float((gl_VertexID & 1) << 2);
  highp float y = -1.0 + float((gl_VertexID & 2) << 1);
  bg0_scrolling_screencoord = bg0_scrolling;
  bg1_scrolling_screencoord = bg1_scrolling;
  bg2_scrolling_screencoord = bg2_scrolling;
  bg3_scrolling_screencoord = bg3_scrolling;
  bg2_affine_screencoord = bg2_affine;
  bg3_affine_screencoord = bg3_affine;
  screencoord.x = (x + 1.0) * 120.0;
  screencoord.y = (y - 1.0) * -80.0;
  gl_Position = vec4(x, y, 0.0, 1.0);
}