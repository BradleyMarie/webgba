#include "emulator/ppu/gba/screen.h"

#include <string.h>

void GbaPpuScreenInitialize(GbaPpuScreen* screen) {
  mtx_init(&screen->resource_mutex, mtx_plain);
}

void GbaPpuScreenRenderToFbo(GbaPpuScreen* screen, GLuint fbo) {
  for (uint_fast8_t y = 0u; y < GBA_SCREEN_HEIGHT; y++) {
    for (uint_fast8_t x = 0u; x < GBA_SCREEN_WIDTH; x++) {
      screen->pixels[y][x] <<= 1u;
    }
  }

  mtx_lock(&screen->resource_mutex);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, screen->texture);
  glTexSubImage2D(GL_TEXTURE_2D, /*level=*/0, /*xoffset=*/0, /*yoffset=*/0,
                  /*width=*/GBA_SCREEN_WIDTH, /*height=*/GBA_SCREEN_HEIGHT,
                  /*format=*/GL_RGBA, /*type=*/GL_UNSIGNED_SHORT_5_5_5_1,
                  /*pixels=*/screen->pixels);

  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glViewport(0, 0, GBA_SCREEN_WIDTH, GBA_SCREEN_HEIGHT);
  glUseProgram(screen->program);
  glDrawArrays(GL_TRIANGLES, 0, 3u);

  mtx_unlock(&screen->resource_mutex);
}

void GbaPpuScreenReloadContext(GbaPpuScreen* screen) {
  mtx_lock(&screen->resource_mutex);

  screen->program = glCreateProgram();

  static const char* vertex_shader_source[9u] = {
      "#version 130\n",
      "out vec2 texCoord;\n",
      "void main() {\n",
      "  float x = -1.0 + float((gl_VertexID & 1) << 2);\n",
      "  float y = -1.0 + float((gl_VertexID & 2) << 1);\n",
      "  texCoord.x = (x+1.0)*0.5;\n",
      "  texCoord.y = (y-1.0)*-0.5;\n",
      "  gl_Position = vec4(x, y, 0.0, 1.0);\n",
      "}\n",
  };

  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 9u, vertex_shader_source, NULL);
  glCompileShader(vertex_shader);
  glAttachShader(screen->program, vertex_shader);
  glDeleteShader(vertex_shader);

  static const char* fragment_shader_source[7u] = {
      "#version 130\n",
      "uniform sampler2D image;\n",
      "in vec2 texCoord;\n",
      "void main() {\n",
      "  vec4 color = textureLod(image, texCoord, 0.0);\n",
      "  gl_FragColor = vec4(color.b, color.g, color.r, 0.0);\n",
      "}\n",
  };

  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 7u, fragment_shader_source, NULL);
  glCompileShader(fragment_shader);
  glAttachShader(screen->program, fragment_shader);
  glDeleteShader(fragment_shader);

  glLinkProgram(screen->program);

  GLint texture_location = glGetUniformLocation(screen->program, "image");

  glUseProgram(screen->program);
  glUniform1i(texture_location, 0);
  glUseProgram(0);

  glGenTextures(1u, &screen->texture);
  glBindTexture(GL_TEXTURE_2D, screen->texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexImage2D(GL_TEXTURE_2D, /*level=*/0, /*internal_format=*/GL_RGB,
               /*width=*/GBA_SCREEN_WIDTH, /*height=*/GBA_SCREEN_HEIGHT,
               /*border=*/0, /*format=*/GL_RGBA,
               /*type=*/GL_UNSIGNED_SHORT_5_5_5_1,
               /*pixels=*/NULL);
  glBindTexture(GL_TEXTURE_2D, 0);

  mtx_unlock(&screen->resource_mutex);
}

void GbaPpuScreenDestroy(GbaPpuScreen* screen) {
  mtx_destroy(&screen->resource_mutex);
  glDeleteProgram(screen->program);
  glDeleteTextures(1u, &screen->texture);
}