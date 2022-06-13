#version 300 es

// Outputs
out mediump vec2 screencoord;

void main() {
  highp float x = -1.0 + float((gl_VertexID & 1) << 2);
  highp float y = -1.0 + float((gl_VertexID & 2) << 1);
  screencoord.x = (x + 1.0) * 120.0;
  screencoord.y = (y - 1.0) * -80.0;
  gl_Position = vec4(x, y, 0.0, 1.0);
}