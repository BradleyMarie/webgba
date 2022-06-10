#include "emulator/screen.h"

#include <assert.h>
#include <stdlib.h>

typedef enum _RenderMode {
  RENDER_MODE_DIRECT = 0,
  RENDER_MODE_SOFTWARE = 1,
  RENDER_MODE_HARDWARE = 2
} RenderMode;

struct _Screen {
  RenderMode render_mode;
  GLuint framebuffer;
  GLsizei framebuffer_width;
  GLsizei framebuffer_height;
  GLuint renderbuffer;
  GLsizei renderbuffer_width;
  GLsizei renderbuffer_height;
  uint16_t *pixels;
  GLsizei pixels_width;
  GLsizei pixels_height;
  GLuint program;
};

Screen *ScreenAllocate() { return calloc(1u, sizeof(Screen)); }

void ScreenAttachFramebuffer(Screen *screen, GLuint framebuffer, GLsizei width,
                             GLsizei height) {
  screen->framebuffer = framebuffer;
  screen->framebuffer_width = width;
  screen->framebuffer_height = height;
}

uint16_t *ScreenGetPixelBuffer(Screen *screen, GLsizei width, GLsizei height) {
  assert(width != 0 && height != 0);

  ScreenGetRenderBuffer(screen, width, height);

  screen->render_mode = RENDER_MODE_SOFTWARE;

  if (screen->pixels_width == width && screen->pixels_height == height) {
    return screen->pixels;
  }

  screen->pixels = realloc(screen->pixels, sizeof(uint16_t) * width * height);

  if (screen->pixels != NULL) {
    screen->pixels_width = width;
    screen->pixels_height = height;
  }

  return screen->pixels;
}

GLuint ScreenGetRenderBuffer(Screen *screen, GLsizei width, GLsizei height) {
  assert(width != 0 && height != 0);

  if (screen->framebuffer_width == width &&
      screen->framebuffer_height == height) {
    screen->render_mode = RENDER_MODE_DIRECT;
    return screen->framebuffer;
  }

  screen->render_mode = RENDER_MODE_HARDWARE;

  if (screen->renderbuffer_width == width &&
      screen->renderbuffer_height == height) {
    return screen->renderbuffer;
  }

  if (screen->renderbuffer_width == 0u && screen->renderbuffer_height == 0u) {
    glGenTextures(1u, &screen->renderbuffer);
  }

  screen->renderbuffer_width = width;
  screen->renderbuffer_height = height;

  glBindTexture(GL_TEXTURE_2D, screen->renderbuffer);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, /*level=*/0, /*internal_format=*/GL_RGBA,
               /*width=*/width, /*height=*/height,
               /*border=*/0, /*format=*/GL_RGBA,
               /*type=*/GL_UNSIGNED_SHORT_5_5_5_1,
               /*pixels=*/screen->pixels);
  glBindTexture(GL_TEXTURE_2D, 0);

  return screen->renderbuffer;
}

void ScreenRenderToFramebuffer(const Screen *screen) {
  if (screen->render_mode == RENDER_MODE_DIRECT ||
      screen->framebuffer_height == 0u || screen->framebuffer_width == 0u) {
    return;
  }

  glUseProgram(screen->program);

  GLint texture_location = glGetUniformLocation(screen->program, "image");
  glUniform1i(texture_location, 0);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, screen->renderbuffer);

  if (screen->render_mode == RENDER_MODE_SOFTWARE) {
    glTexSubImage2D(GL_TEXTURE_2D, /*level=*/0, /*xoffset=*/0, /*yoffset=*/0,
                    /*width=*/screen->pixels_width,
                    /*height=*/screen->pixels_height,
                    /*format=*/GL_RGBA, /*type=*/GL_UNSIGNED_SHORT_5_5_5_1,
                    /*pixels=*/screen->pixels);
  }

  glBindFramebuffer(GL_FRAMEBUFFER, screen->framebuffer);
  glViewport(0, 0, screen->framebuffer_width, screen->framebuffer_height);
  glDrawArrays(GL_TRIANGLES, 0, 3u);
}

void ScreenReloadContext(Screen *screen) {
  screen->program = glCreateProgram();

  static const char *vertex_shader_source =
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

  static const char *fragment_shader_source =
      "#version 300 es\n"
      "uniform lowp sampler2D image;\n"
      "in mediump vec2 texcoord;\n"
      "out lowp vec4 color;"
      "void main() {\n"
      "  lowp vec4 texcolor = texture(image, texcoord);\n"
      "  color = vec4(texcolor.bgr, 1.0);\n"
      "}\n";

  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1u, &fragment_shader_source, NULL);
  glCompileShader(fragment_shader);
  glAttachShader(screen->program, fragment_shader);
  glDeleteShader(fragment_shader);

  glLinkProgram(screen->program);

  screen->render_mode = RENDER_MODE_DIRECT;

  screen->framebuffer = 0u;
  screen->framebuffer_width = 0u;
  screen->framebuffer_height = 0u;

  GLsizei renderbuffer_width = screen->renderbuffer_width;
  GLsizei renderbuffer_height = screen->renderbuffer_height;
  screen->renderbuffer_width = 0u;
  screen->renderbuffer_height = 0u;

  ScreenGetRenderBuffer(screen, renderbuffer_width, renderbuffer_height);
}

void ScreenFree(Screen *screen) {
  glDeleteProgram(screen->program);
  glDeleteTextures(1u, &screen->renderbuffer);
  free(screen->pixels);
  free(screen);
}