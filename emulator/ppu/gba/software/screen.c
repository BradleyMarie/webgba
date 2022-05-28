#include "emulator/ppu/gba/software/screen.h"

#include <stdio.h>
#include <string.h>

void GbaPpuScreenRenderToFbo(GbaPpuScreen* screen, GLuint fbo, GLsizei width,
                             GLsizei height) {
  for (uint_fast8_t y = 0u; y < GBA_SCREEN_HEIGHT; y++) {
    for (uint_fast8_t x = 0u; x < GBA_SCREEN_WIDTH; x++) {
      screen->pixels[y][x] = screen->pixels[y][x] << 1u | 1u;
    }
  }

  glUseProgram(screen->program);

  GLint texture_location = glGetUniformLocation(screen->program, "image");
  glUniform1i(texture_location, 0);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, screen->texture);
  glTexSubImage2D(GL_TEXTURE_2D, /*level=*/0, /*xoffset=*/0, /*yoffset=*/0,
                  /*width=*/GBA_SCREEN_WIDTH, /*height=*/GBA_SCREEN_HEIGHT,
                  /*format=*/GL_RGBA, /*type=*/GL_UNSIGNED_SHORT_5_5_5_1,
                  /*pixels=*/screen->pixels);

  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glViewport(0, 0, width, height);
  glDrawArrays(GL_TRIANGLES, 0, 3u);
}

void GbaPpuScreenReloadContext(GbaPpuScreen* screen) {
  screen->program = glCreateProgram();

  static const char* vertex_shader_source =
      "#version 300 es\n"
      "out vec2 texcoord;\n"
      "void main() {\n"
      "  float x = -1.0 + float((gl_VertexID & 1) << 2);\n"
      "  float y = -1.0 + float((gl_VertexID & 2) << 1);\n"
      "  texcoord.x = (x + 1.0) * 0.5;\n"
      "  texcoord.y = (y - 1.0) * -0.5;\n"
      "  gl_Position = vec4(x, y, 0.0, 1.0);\n"
      "}\n";

  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1u, &vertex_shader_source, NULL);
  glCompileShader(vertex_shader);
  glAttachShader(screen->program, vertex_shader);
  glDeleteShader(vertex_shader);

  static const char* fragment_shader_source =
      "#version 300 es\n"
      "uniform sampler2D image;\n"
      "in mediump vec2 texcoord;\n"
      "out lowp vec4 color;"
      "void main() {\n"
      "  lowp vec4 texcolor = texture2D(image, texcoord);\n"
      "  color = vec4(texcolor.bgr, 1.0);\n"
      "}\n";

  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1u, &fragment_shader_source, NULL);
  glCompileShader(fragment_shader);
  glAttachShader(screen->program, fragment_shader);
  glDeleteShader(fragment_shader);

  glLinkProgram(screen->program);

  glGenTextures(1u, &screen->texture);
  glBindTexture(GL_TEXTURE_2D, screen->texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, /*level=*/0, /*internal_format=*/GL_RGBA,
               /*width=*/GBA_SCREEN_WIDTH, /*height=*/GBA_SCREEN_HEIGHT,
               /*border=*/0, /*format=*/GL_RGBA,
               /*type=*/GL_UNSIGNED_SHORT_5_5_5_1,
               /*pixels=*/NULL);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void GbaPpuScreenDestroy(GbaPpuScreen* screen) {
  glDeleteProgram(screen->program);
  glDeleteTextures(1u, &screen->texture);
}