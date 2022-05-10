#include "emulator/ppu/gba/opengl/render.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "emulator/ppu/gba/opengl/affine.h"
#include "emulator/ppu/gba/opengl/bg_bitmap.h"
#include "emulator/ppu/gba/opengl/mosaic.h"
#include "emulator/ppu/gba/opengl/palette_large.h"
#include "emulator/ppu/gba/opengl/palette_small.h"

#define NUM_LAYERS (GBA_PPU_NUM_BACKGROUNDS + 1u)

struct _GbaPpuOpenGlRenderer {
  GbaPpuOpenGlAffine affine;
  GbaPpuOpenGlBgBitmap bg_bitmap;
  GbaPpuOpenGlMosaic mosaic;
  GbaPpuOpenGlLargePalette palette_large;
  GbaPpuOpenGlSmallPalette palette_small;
  uint8_t next_render_scale;
  uint8_t render_scale;
  GLuint layer_textures[NUM_LAYERS];
  GLuint layer_fbos[NUM_LAYERS];
  GLuint vertices;
  GLuint program;
  bool initialized;
};

static void UpdateTextures(GbaPpuOpenGlRenderer* renderer) {
  assert(renderer->render_scale != 0u);

  for (uint8_t i = 0; i < NUM_LAYERS; i++) {
    glBindTexture(GL_TEXTURE_2D, renderer->layer_textures[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, /*level=*/0, /*internal_format=*/GL_RGBA,
                 /*width=*/GBA_SCREEN_WIDTH * renderer->render_scale,
                 /*height=*/GBA_SCREEN_HEIGHT * renderer->render_scale,
                 /*border=*/0,
                 /*format=*/GL_RGBA, /*type=*/GL_UNSIGNED_BYTE,
                 /*pixels=*/NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
  }
}

GbaPpuOpenGlRenderer* GbaPpuOpenGlRendererAllocate() {
  GbaPpuOpenGlRenderer* renderer = calloc(1u, sizeof(GbaPpuOpenGlRenderer));

  if (renderer != NULL) {
    renderer->next_render_scale = 1u;
    renderer->render_scale = 1u;
  }

  return renderer;
}

void GbaPpuOpenGlRendererDrawRow(GbaPpuOpenGlRenderer* renderer,
                                 const GbaPpuMemory* memory,
                                 const GbaPpuRegisters* registers,
                                 GbaPpuDirtyBits* dirty_bits) {
  if (!renderer->initialized) {
    return;
  }

  // Hack to workaround missing dirty bits
  if (registers->vcount == 0u) {
    GbaPpuDirtyBitsAllDirty(dirty_bits);
  }

  if (registers->vcount == 0u &&
      renderer->render_scale != renderer->next_render_scale) {
    renderer->render_scale = renderer->next_render_scale;
    UpdateTextures(renderer);
  }

  glEnable(GL_SCISSOR_TEST);

  glScissor(0u, registers->vcount * renderer->render_scale, GBA_SCREEN_WIDTH,
            renderer->render_scale);
  glViewport(0u, 0u, GBA_SCREEN_WIDTH * renderer->render_scale,
             GBA_SCREEN_HEIGHT * renderer->render_scale);

  switch (registers->dispcnt.mode) {
    case 0:
      break;
    case 1:
      break;
    case 2:
      break;
    case 3:
      GbaPpuOpenGlBgBitmapMode3(&renderer->bg_bitmap, memory, registers,
                                &renderer->affine, &renderer->mosaic,
                                dirty_bits, renderer->layer_fbos[2u]);
      break;
    case 4:
      GbaPpuOpenGlBgBitmapMode4(&renderer->bg_bitmap, memory, registers,
                                &renderer->affine, &renderer->mosaic,
                                &renderer->palette_large, dirty_bits,
                                renderer->layer_fbos[2u]);
      break;
    case 5:
      GbaPpuOpenGlBgBitmapMode5(&renderer->bg_bitmap, memory, registers,
                                &renderer->affine, &renderer->mosaic,
                                dirty_bits, renderer->layer_fbos[2u]);
      break;
  }

  glDisable(GL_SCISSOR_TEST);
}

void GbaPpuOpenGlRendererPresent(GbaPpuOpenGlRenderer* renderer, GLuint fbo,
                                 GLsizei width, GLsizei height) {
  if (!renderer->initialized) {
    return;
  }

  glUseProgram(renderer->program);

  GLuint coord_attrib = glGetAttribLocation(renderer->program, "coord");
  glBindBuffer(GL_ARRAY_BUFFER, renderer->vertices);
  glVertexAttribPointer(coord_attrib, /*size=*/2, /*type=*/GL_FLOAT,
                        /*normalized=*/false, /*stride=*/0, /*pointer=*/NULL);
  glEnableVertexAttribArray(coord_attrib);

  GLint texture_location = glGetUniformLocation(renderer->program, "image");
  glUniform1i(texture_location, 0);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, renderer->layer_textures[2u]);

  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glViewport(0, 0, width, height);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4u);
}

void GbaPpuOpenGlRendererReloadContext(GbaPpuOpenGlRenderer* renderer) {
  GbaPpuOpenGlAffineReloadContext(&renderer->affine);
  GbaPpuOpenGlBgBitmapReloadContext(&renderer->bg_bitmap);
  GbaPpuOpenGlLargePaletteReloadContext(&renderer->palette_large);
  GbaPpuOpenGlSmallPaletteReloadContext(&renderer->palette_small);

  glGenTextures(/*n=*/NUM_LAYERS, renderer->layer_textures);
  UpdateTextures(renderer);

  glGenFramebuffers(/*n=*/NUM_LAYERS, renderer->layer_fbos);
  for (uint8_t i = 0; i < NUM_LAYERS; i++) {
    glBindFramebuffer(GL_FRAMEBUFFER, renderer->layer_fbos[i]);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           renderer->layer_textures[i], /*level=*/0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  renderer->program = glCreateProgram();

  static const char* vertex_shader_source =
      "#version 100\n"
      "attribute highp vec2 coord;\n"
      "varying mediump vec2 texcoord;\n"
      "void main() {\n"
      "  texcoord.x = (coord.x + 1.0) * 0.5;\n"
      "  texcoord.y = (coord.y + 1.0) * 0.5;\n"
      "  gl_Position = vec4(coord, 0.0, 1.0);\n"
      "}\n";

  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1u, &vertex_shader_source, NULL);
  glCompileShader(vertex_shader);
  glAttachShader(renderer->program, vertex_shader);
  glDeleteShader(vertex_shader);

  static const char* fragment_shader_source =
      "#version 100\n"
      "uniform lowp sampler2D image;\n"
      "varying mediump vec2 texcoord;\n"
      "void main() {\n"
      "  lowp vec4 color = texture2D(image, texcoord);\n"
      "  gl_FragColor = vec4(color.r, color.g, color.b, 0.0);\n"
      "}\n";

  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1u, &fragment_shader_source, NULL);
  glCompileShader(fragment_shader);
  glAttachShader(renderer->program, fragment_shader);
  glDeleteShader(fragment_shader);

  glLinkProgram(renderer->program);

  static const GLfloat vertices[8u] = {-1.0, -1.0, -1.0, 1.0,
                                       1.0,  1.0,  1.0,  -1.0};

  glGenBuffers(1, &renderer->vertices);
  glBindBuffer(GL_ARRAY_BUFFER, renderer->vertices);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  renderer->initialized = true;
}

void GbaPpuOpenGlRendererSetScale(GbaPpuOpenGlRenderer* renderer,
                                  uint8_t render_scale) {
  renderer->next_render_scale = render_scale;
}

void GbaPpuOpenGlRendererFree(GbaPpuOpenGlRenderer* renderer) {
  if (renderer->initialized) {
    GbaPpuOpenGlAffineDestroy(&renderer->affine);
    GbaPpuOpenGlBgBitmapDestroy(&renderer->bg_bitmap);
    GbaPpuOpenGlLargePaletteDestroy(&renderer->palette_large);
    GbaPpuOpenGlSmallPaletteDestroy(&renderer->palette_small);
    glDeleteFramebuffers(NUM_LAYERS, renderer->layer_fbos);
    glDeleteTextures(NUM_LAYERS, renderer->layer_textures);
  }

  free(renderer);
}