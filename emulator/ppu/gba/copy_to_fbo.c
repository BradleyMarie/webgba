#include "emulator/ppu/gba/copy_to_fbo.h"

#include <stddef.h>

void GbaPpuCopyToFbo(const GbaPpuCopyToFboResources* resources,
                     const GbaPpuFrameBuffer* framebuffer, GLuint fbo) {
  glTexSubImage2D(resources->texture, /*level=*/0, /*xoffset=*/0, /*yoffset=*/0,
                  /*width=*/GBA_SCREEN_WIDTH, /*height=*/GBA_SCREEN_HEIGHT,
                  /*format=*/GL_RGB, /*type=*/GL_UNSIGNED_SHORT_5_5_5_1,
                  /*pixels=*/framebuffer->pixels);

  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glViewport(0, 0, GBA_SCREEN_WIDTH, GBA_SCREEN_HEIGHT);
  glUseProgram(resources->program);
  glBindTexture(GL_TEXTURE_2D, resources->texture);
  glDrawArrays(GL_TRIANGLES, 0, 3u);
}

void GbaPpuCopyToFboReloadContext(GbaPpuCopyToFboResources* resources) {
  resources->program = glCreateProgram();

  static const char* vertex_shader_source[8u] = {
      "out vec2 texCoord;",
      "void main() {",
      "  float x = -1.0 + float((gl_VertexID & 1) << 2);",
      "  float y = -1.0 + float((gl_VertexID & 2) << 1);",
      "  texCoord.x = (x+1.0)*0.5;",
      "  texCoord.y = (y+1.0)*0.5;",
      "  gl_Position = vec4(x, y, 0.0, 1.0);"
      "}",
  };

  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 8u, vertex_shader_source, NULL);
  glCompileShader(vertex_shader);
  glAttachShader(resources->program, vertex_shader);
  glDeleteShader(vertex_shader);

  static const char* fragment_shader_source[5u] = {
      "uniform sampler2D image;",
      "in vec2 texCoord;",
      "void main() {",
      "  gl_FragColor = textureLod(image, texCoord, 0.0);",
      "}",
  };

  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 5u, fragment_shader_source, NULL);
  glCompileShader(fragment_shader);
  glAttachShader(resources->program, fragment_shader);
  glDeleteShader(fragment_shader);

  glLinkProgram(resources->program);

  glGenTextures(1u, &resources->texture);
  glTexImage2D(resources->texture, /*level=*/0, /*internal_format=*/GL_RGB,
               /*width=*/GBA_SCREEN_WIDTH, /*height=*/GBA_SCREEN_WIDTH,
               /*border=*/0, /*format=*/GL_RGB,
               /*type=*/GL_UNSIGNED_SHORT_5_5_5_1,
               /*pixels=*/NULL);
}

void GbaPpuCopyToFboDestroy(GbaPpuCopyToFboResources* resources) {
  glDeleteProgram(resources->program);
  glDeleteTextures(1u, &resources->texture);
}