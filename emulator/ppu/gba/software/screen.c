#include "emulator/ppu/gba/software/screen.h"

#include <string.h>

void GbaPpuScreenRenderToFbo(GbaPpuScreen* screen, GLuint fbo, GLsizei width,
                             GLsizei height) {
  if (!screen->initialized) {
    return;
  }

  for (uint_fast8_t y = 0u; y < GBA_SCREEN_HEIGHT; y++) {
    for (uint_fast8_t x = 0u; x < GBA_SCREEN_WIDTH; x++) {
      screen->pixels[y][x] <<= 1u;
    }
  }

  glUseProgram(screen->program);

  GLuint coord_attrib = glGetAttribLocation(screen->program, "coord");
  glBindBuffer(GL_ARRAY_BUFFER, screen->vertices);
  glVertexAttribPointer(coord_attrib, /*size=*/2, /*type=*/GL_FLOAT,
                        /*normalized=*/false, /*stride=*/0, /*pointer=*/NULL);
  glEnableVertexAttribArray(coord_attrib);

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
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4u);
}

void GbaPpuScreenReloadContext(GbaPpuScreen* screen) {
  screen->program = glCreateProgram();

  static const char* vertex_shader_source =
      "#version 100\n"
      "attribute highp vec2 coord;\n"
      "varying mediump vec2 texcoord;\n"
      "void main() {\n"
      "  texcoord.x = (coord.x + 1.0) * 0.5;\n"
      "  texcoord.y = (coord.y - 1.0) * -0.5;\n"
      "  gl_Position = vec4(coord, 0.0, 1.0);\n"
      "}\n";

  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1u, &vertex_shader_source, NULL);
  glCompileShader(vertex_shader);
  glAttachShader(screen->program, vertex_shader);
  glDeleteShader(vertex_shader);

  static const char* fragment_shader_source =
      "#version 100\n"
      "uniform lowp sampler2D image;\n"
      "varying mediump vec2 texcoord;\n"
      "void main() {\n"
      "  lowp vec4 color = texture2D(image, texcoord);\n"
      "  gl_FragColor = vec4(color.b, color.g, color.r, 0.0);\n"
      "}\n";

  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1u, &fragment_shader_source, NULL);
  glCompileShader(fragment_shader);
  glAttachShader(screen->program, fragment_shader);
  glDeleteShader(fragment_shader);

  glLinkProgram(screen->program);

  static const GLfloat vertices[8u] = {-1.0, -1.0, -1.0, 1.0,
                                       1.0,  1.0,  1.0,  -1.0};

  glGenBuffers(1, &screen->vertices);
  glBindBuffer(GL_ARRAY_BUFFER, screen->vertices);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

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

  screen->initialized = true;
}

void GbaPpuScreenDestroy(GbaPpuScreen* screen) {
  glDeleteProgram(screen->program);
  glDeleteBuffers(1u, &screen->vertices);
  glDeleteTextures(1u, &screen->texture);
  screen->initialized = false;
}