#version 300 es

// Inputs
in highp vec2 bg2_affine; // In GBA screen space
in highp vec2 bg3_affine; // In GBA screen space

// Outputs
out mediump vec2 affine_screencoord[2];
out mediump vec2 screencoord;

void main() {
  highp float x = -1.0 + float((gl_VertexID & 1) << 2);
  highp float y = -1.0 + float((gl_VertexID & 2) << 1);
  affine_screencoord[0] = bg2_affine;
  affine_screencoord[1] = bg3_affine;
  screencoord.x = (x + 1.0) * 120.0;
  screencoord.y = (y - 1.0) * -80.0;
  gl_Position = vec4(x, y, 0.0, 1.0);
}