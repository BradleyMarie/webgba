#version 100

// Inputs
attribute highp vec2 bg2_affine; // In GBA screen space
attribute highp vec2 bg3_affine; // In GBA screen space
attribute highp vec2 vertex;

// Outputs
varying highp vec2 bg2_affine_screencoord;
varying highp vec2 bg3_affine_screencoord;
varying highp vec2 screencoord;

void main() {
  bg2_affine_screencoord = bg2_affine;
  bg3_affine_screencoord = bg3_affine;
  screencoord.x = (vertex.x + 1.0) * 120.0;
  screencoord.y = (vertex.y + 1.0) * 80.0;
  gl_Position = vec4(vertex, 0.0, 1.0);
}