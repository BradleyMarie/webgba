#include "emulator/screen.h"

#include <assert.h>
#include <stdio.h>
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
  GLuint renderbuffer_texture;
  GLsizei renderbuffer_width;
  GLsizei renderbuffer_height;
  uint8_t *subpixels;
  GLsizei pixels_width;
  GLsizei pixels_height;
  GLuint pixels_staging[2u];
  uint8_t pixels_staging_index;
  GLuint upscale_fbo;
  GLuint upscale_pixels;
};

static GLuint ScreenCreateUpscaleFbo() {
  GLuint program = glCreateProgram();

  static const char *vertex_shader_source =
      "#version 300 es\n"
      "out highp vec2 texcoord;\n"
      "void main() {\n"
      "  highp float x = -1.0 + float((gl_VertexID & 1) << 2);\n"
      "  highp float y = -1.0 + float((gl_VertexID & 2) << 1);\n"
      "  texcoord.x = (x + 1.0) * 0.5;\n"
      "  texcoord.y = (y + 1.0) * 0.5;\n"
      "  gl_Position = vec4(x, y, 0.0, 1.0);\n"
      "}\n";

  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1u, &vertex_shader_source, NULL);
  glCompileShader(vertex_shader);
  glAttachShader(program, vertex_shader);
  glDeleteShader(vertex_shader);

  static const char *fragment_shader_source =
      "#version 300 es\n"
      "uniform lowp sampler2D image;\n"
      "in mediump vec2 texcoord;\n"
      "out lowp vec4 color;"
      "void main() {\n"
      "  lowp vec4 texcolor = texture(image, texcoord);\n"
      "  color = vec4(texcolor.rgb, 1.0);\n"
      "}\n";

  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1u, &fragment_shader_source, NULL);
  glCompileShader(fragment_shader);
  glAttachShader(program, fragment_shader);
  glDeleteShader(fragment_shader);

  glLinkProgram(program);

  GLint success = 0;
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (success != GL_TRUE) {
    printf("ERROR: shader linking failed\n");

    GLchar message[500u];
    glGetProgramInfoLog(program, 500, NULL, message);
    printf("%s\n", message);

    exit(EXIT_FAILURE);
  }

  return program;
}

static GLuint ScreenCreateUpscalePixels() {
  GLuint program = glCreateProgram();

  static const char *vertex_shader_source =
      "#version 300 es\n"
      "out highp vec2 texcoord;\n"
      "void main() {\n"
      "  highp float x = -1.0 + float((gl_VertexID & 1) << 2);\n"
      "  highp float y = -1.0 + float((gl_VertexID & 2) << 1);\n"
      "  texcoord.x = (x + 1.0) * 0.5;\n"
      "  texcoord.y = (y - 1.0) * -0.5;\n"
      "  gl_Position = vec4(x, y, 0.0, 1.0);\n"
      "}\n";

  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1u, &vertex_shader_source, NULL);
  glCompileShader(vertex_shader);
  glAttachShader(program, vertex_shader);
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
  glAttachShader(program, fragment_shader);
  glDeleteShader(fragment_shader);

  glLinkProgram(program);

  GLint success = 0;
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (success != GL_TRUE) {
    printf("ERROR: shader linking failed\n");

    GLchar message[500u];
    glGetProgramInfoLog(program, 500, NULL, message);
    printf("%s\n", message);

    exit(EXIT_FAILURE);
  }

  return program;
}

static void ScreenAllocateRenderbuffer(Screen *screen) {
  glBindTexture(GL_TEXTURE_2D, screen->renderbuffer_texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, /*level=*/0, /*internal_format=*/GL_RGBA,
               /*width=*/screen->renderbuffer_width,
               /*height=*/screen->renderbuffer_height,
               /*border=*/0, /*format=*/GL_RGBA, /*type=*/GL_UNSIGNED_BYTE,
               /*pixels=*/NULL);
  glBindTexture(GL_TEXTURE_2D, 0);

  glBindFramebuffer(GL_FRAMEBUFFER, screen->renderbuffer);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         screen->renderbuffer_texture, /*level=*/0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

static void ScreenAllocateStaging(Screen *screen) {
  for (uint8_t i = 0u; i < 2u; i++) {
    glBindTexture(GL_TEXTURE_2D, screen->pixels_staging[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, /*level=*/0, /*internal_format=*/GL_RGB,
                 /*width=*/screen->pixels_width,
                 /*height=*/screen->pixels_height, /*border=*/0,
                 /*format=*/GL_RGB, /*type=*/GL_UNSIGNED_BYTE,
                 /*pixels=*/screen->subpixels);
  }
  glBindTexture(GL_TEXTURE_2D, 0);
}

Screen *ScreenAllocate() { return calloc(1u, sizeof(Screen)); }

void ScreenAttachFramebuffer(Screen *screen, GLuint framebuffer, GLsizei width,
                             GLsizei height) {
  screen->framebuffer = framebuffer;
  screen->framebuffer_width = width;
  screen->framebuffer_height = height;
}

uint8_t *ScreenGetPixelBuffer(Screen *screen, GLsizei width, GLsizei height) {
  assert(width != 0 && height != 0);

  screen->pixels_staging_index = (screen->pixels_staging_index + 1u) % 2u;
  screen->render_mode = RENDER_MODE_SOFTWARE;

  if (screen->pixels_width == width && screen->pixels_height == height) {
    return screen->subpixels;
  }

  void *new_buffer =
      realloc(screen->subpixels, 3u * sizeof(uint8_t) * width * height);

  if (new_buffer == NULL) {
    return NULL;
  }

  screen->subpixels = new_buffer;

  if (screen->pixels_width == 0u && screen->pixels_height == 0u) {
    glGenTextures(2u, screen->pixels_staging);
  }

  screen->pixels_width = width;
  screen->pixels_height = height;

  ScreenAllocateStaging(screen);

  return screen->subpixels;
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
    glGenFramebuffers(1u, &screen->renderbuffer);
    glGenTextures(1u, &screen->renderbuffer_texture);
  }

  screen->renderbuffer_width = width;
  screen->renderbuffer_height = height;

  ScreenAllocateRenderbuffer(screen);

  return screen->renderbuffer;
}

void ScreenClear(const Screen *screen) {
  if (screen->framebuffer_height == 0u || screen->framebuffer_width == 0u) {
    return;
  }

  glBindFramebuffer(GL_FRAMEBUFFER, screen->framebuffer);
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);
}

void ScreenRenderToFramebuffer(const Screen *screen) {
  if (screen->render_mode == RENDER_MODE_DIRECT ||
      screen->framebuffer_height == 0u || screen->framebuffer_width == 0u) {
    return;
  }

  GLuint program = (screen->render_mode == RENDER_MODE_SOFTWARE)
                       ? screen->upscale_pixels
                       : screen->upscale_fbo;
  glUseProgram(program);

  GLint texture_location = glGetUniformLocation(program, "image");
  glUniform1i(texture_location, 0);

  glActiveTexture(GL_TEXTURE0);
  if (screen->render_mode == RENDER_MODE_SOFTWARE) {
    glBindTexture(GL_TEXTURE_2D,
                  screen->pixels_staging[screen->pixels_staging_index]);
    glTexSubImage2D(GL_TEXTURE_2D, /*level=*/0, /*xoffset=*/0, /*yoffset=*/0,
                    /*width=*/screen->pixels_width,
                    /*height=*/screen->pixels_height,
                    /*format=*/GL_RGB, /*type=*/GL_UNSIGNED_BYTE,
                    /*pixels=*/screen->subpixels);
  } else {
    glBindTexture(GL_TEXTURE_2D, screen->renderbuffer_texture);
  }

  glBindFramebuffer(GL_FRAMEBUFFER, screen->framebuffer);
  glViewport(0, 0, screen->framebuffer_width, screen->framebuffer_height);

  glValidateProgram(program);

  GLint success = 0;
  glGetProgramiv(program, GL_VALIDATE_STATUS, &success);
  if (success != GL_TRUE) {
    printf("ERROR: shader validation failed\n");

    GLchar message[500u];
    glGetProgramInfoLog(program, 500, NULL, message);
    printf("%s\n", message);
    exit(EXIT_FAILURE);
  }

  glDrawArrays(GL_TRIANGLES, 0, 3u);
}

void ScreenReloadContext(Screen *screen) {
  screen->upscale_fbo = ScreenCreateUpscaleFbo();
  screen->upscale_pixels = ScreenCreateUpscalePixels();
  screen->render_mode = RENDER_MODE_DIRECT;

  screen->framebuffer = 0u;
  screen->framebuffer_width = 0u;
  screen->framebuffer_height = 0u;

  if (screen->renderbuffer_width != 0u && screen->renderbuffer_height != 0u) {
    glGenFramebuffers(1u, &screen->renderbuffer);
    glGenTextures(1u, &screen->renderbuffer_texture);
    ScreenAllocateRenderbuffer(screen);
  }

  if (screen->pixels_width != 0u && screen->pixels_width != 0u) {
    glGenTextures(2u, screen->pixels_staging);
    ScreenAllocateStaging(screen);
  }
}

void ScreenFree(Screen *screen) {
  glDeleteProgram(screen->upscale_fbo);
  glDeleteProgram(screen->upscale_pixels);
  glDeleteFramebuffers(1, &screen->renderbuffer);
  glDeleteTextures(1u, &screen->renderbuffer_texture);
  glDeleteTextures(2u, screen->pixels_staging);
  free(screen->subpixels);
  free(screen);
}