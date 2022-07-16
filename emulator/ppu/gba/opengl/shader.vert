#version 300 es

uniform mediump int row_start;
uniform mediump int num_rows;

void main() {
  mediump int screen_x = ((gl_VertexID & 1) >> 0) * 240 - 120;
  mediump int screen_y = row_start - ((gl_VertexID & 2) >> 1) * num_rows;
  gl_Position = vec4(float(screen_x) / 120.0, float(screen_y) / 80.0, 0.0, 1.0);
}