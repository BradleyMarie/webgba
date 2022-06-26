#version 300 es

void main() {
  highp float x = -1.0 + float((gl_VertexID & 1) << 2);
  highp float y = -1.0 + float((gl_VertexID & 2) << 1);
  gl_Position = vec4(x, y, 0.0, 1.0);
}