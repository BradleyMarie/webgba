#include "emulator/ppu/gba/copy_to_fbo.h"

#include <stddef.h>

void GbaPpuCopyToFbo(const GbaPpuCopyToFboResources* resources,
                     const GbaPpuFrameBuffer* framebuffer, GLuint fbo) {
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, resources->texture);
  glTexSubImage2D(GL_TEXTURE_2D, /*level=*/0, /*xoffset=*/0, /*yoffset=*/0,
                  /*width=*/GBA_SCREEN_WIDTH, /*height=*/GBA_SCREEN_HEIGHT,
                  /*format=*/GL_RGBA, /*type=*/GL_UNSIGNED_SHORT_5_5_5_1,
                  /*pixels=*/framebuffer->pixels);

  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glViewport(0, 0, GBA_SCREEN_WIDTH, GBA_SCREEN_HEIGHT);
  glUseProgram(resources->program);
  glDrawArrays(GL_TRIANGLES, 0, 3u);
}

void GbaPpuCopyToFboReloadContext(GbaPpuCopyToFboResources* resources) {
  resources->program = glCreateProgram();

  static const char* vertex_shader_source[9u] = {
      "#version 130\n",
      "out vec2 texCoord;\n",
      "void main() {\n",
      "  float x = -1.0 + float((gl_VertexID & 1) << 2);\n",
      "  float y = -1.0 + float((gl_VertexID & 2) << 1);\n",
      "  texCoord.x = (x+1.0)*0.5;\n",
      "  texCoord.y = (y+1.0)*0.5;\n",
      "  gl_Position = vec4(x, y, 0.0, 1.0);\n",
      "}\n",
  };

  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 9u, vertex_shader_source, NULL);
  glCompileShader(vertex_shader);
  glAttachShader(resources->program, vertex_shader);
  glDeleteShader(vertex_shader);

  static const char* fragment_shader_source[6u] = {
      "#version 130\n",
      "uniform sampler2D image;\n",
      "in vec2 texCoord;\n",
      "void main() {\n",
      "  gl_FragColor = textureLod(image, texCoord, 0.0);\n",
      "}\n",
  };

  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 6u, fragment_shader_source, NULL);
  glCompileShader(fragment_shader);
  glAttachShader(resources->program, fragment_shader);
  glDeleteShader(fragment_shader);

  glLinkProgram(resources->program);

  GLint texture_location = glGetUniformLocation(resources->program, "image");

  glUseProgram(resources->program);
  glUniform1i(texture_location, 0);
  glUseProgram(0);

  glGenTextures(1u, &resources->texture);
  glBindTexture(GL_TEXTURE_2D, resources->texture);
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
}

void GbaPpuCopyToFboDestroy(GbaPpuCopyToFboResources* resources) {
  glDeleteProgram(resources->program);
  glDeleteTextures(1u, &resources->texture);
}