#include "emulator/ppu/gba/opengl/render.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "emulator/ppu/gba/draw_manager.h"
#include "emulator/ppu/gba/opengl/bg_affine.h"
#include "emulator/ppu/gba/opengl/bg_bitmap_mode3.h"
#include "emulator/ppu/gba/opengl/bg_bitmap_mode4.h"
#include "emulator/ppu/gba/opengl/bg_bitmap_mode5.h"
#include "emulator/ppu/gba/opengl/control.h"
#include "emulator/ppu/gba/opengl/mosaic.h"
#include "emulator/ppu/gba/opengl/palette.h"
#include "emulator/ppu/gba/opengl/shader_fragment.h"
#include "emulator/ppu/gba/opengl/shader_vertex.h"
#include "emulator/ppu/gba/opengl/window.h"

#define NUM_LAYERS (GBA_PPU_NUM_BACKGROUNDS + 1u)

struct _GbaPpuOpenGlRenderer {
  GbaPpuDrawManager draw_manager;
  OpenGlBgAffine affine;
  OpenGlBgBitmapMode3 bg_bitmap_mode3;
  OpenGlBgBitmapMode4 bg_bitmap_mode4;
  OpenGlBgBitmapMode5 bg_bitmap_mode5;
  OpenGlBgPalette bg_palette;
  OpenGlControl control;
  OpenGlBgMosaic mosaic;
  OpenGlWindow window;
  uint8_t flush_start_row;
  uint8_t flush_size;
  uint8_t next_render_scale;
  uint8_t render_scale;
  uint8_t last_frame_contents;
  GLuint staging_texture;
  GLuint staging_fbo;
  GLuint vertices;
  GLuint render_program;
  GLuint upscale_program;
  bool initialized;
};

static void CreateStagingTexture(GLuint* texture, uint8_t render_scale) {
  glGenTextures(1u, texture);
  glBindTexture(GL_TEXTURE_2D, *texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, /*level=*/0, /*internal_format=*/GL_RGBA,
               /*width=*/GBA_SCREEN_WIDTH * render_scale,
               /*height=*/GBA_SCREEN_HEIGHT * render_scale,
               /*border=*/0,
               /*format=*/GL_RGBA, /*type=*/GL_UNSIGNED_BYTE,
               /*pixels=*/NULL);
  glBindTexture(GL_TEXTURE_2D, 0);
}

static void UpdateStagingFbo(GLuint fbo, GLuint staging_texture) {
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         staging_texture, /*level=*/0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

static void CreateStagingFbo(GLuint* fbo, GLuint staging_texture) {
  glGenFramebuffers(1u, fbo);
  UpdateStagingFbo(*fbo, staging_texture);
}

static void CreateVertices(GLuint* vertices) {
  static const GLfloat data[8u] = {-1.0, -1.0, -1.0, 1.0, 1.0, 1.0, 1.0, -1.0};
  glGenBuffers(1, vertices);
  glBindBuffer(GL_ARRAY_BUFFER, *vertices);
  glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

static void CreateUpscaleProgram(GLuint* program) {
  *program = glCreateProgram();

  static const char* upscale_vertex_shader_source =
      "#version 100\n"
      "attribute highp vec2 coord;\n"
      "varying mediump vec2 texcoord;\n"
      "void main() {\n"
      "  texcoord.x = (coord.x + 1.0) * 0.5;\n"
      "  texcoord.y = (coord.y + 1.0) * 0.5;\n"
      "  gl_Position = vec4(coord, 0.0, 1.0);\n"
      "}\n";

  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1u, &upscale_vertex_shader_source, NULL);
  glCompileShader(vertex_shader);
  glAttachShader(*program, vertex_shader);
  glDeleteShader(vertex_shader);

  static const char* upscale_fragment_shader_source =
      "#version 100\n"
      "uniform lowp sampler2D image;\n"
      "varying mediump vec2 texcoord;\n"
      "void main() {\n"
      "  lowp vec4 color = texture2D(image, texcoord);\n"
      "  gl_FragColor = vec4(color.r, color.g, color.b, 1.0);\n"
      "}\n";

  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1u, &upscale_fragment_shader_source, NULL);
  glCompileShader(fragment_shader);
  glAttachShader(*program, fragment_shader);
  glDeleteShader(fragment_shader);

  glLinkProgram(*program);
}

static void CreateRenderProgram(GLuint* program) {
  *program = glCreateProgram();

  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1u, &vertex_shader_source, NULL);
  glCompileShader(vertex_shader);
  glAttachShader(*program, vertex_shader);
  glDeleteShader(vertex_shader);

  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1u, &fragment_shader_source, NULL);
  glCompileShader(fragment_shader);
  glAttachShader(*program, fragment_shader);
  glDeleteShader(fragment_shader);

  glLinkProgram(*program);
}

static void GbaPpuOpenGlRendererDraw(const GbaPpuOpenGlRenderer* renderer,
                                     GLuint fbo) {
  glUseProgram(renderer->render_program);

  OpenGlControlBind(&renderer->control, renderer->render_program);
  OpenGlBgPaletteBind(&renderer->bg_palette, renderer->render_program);
  OpenGlBgMosaicBind(&renderer->mosaic, renderer->render_program);
  OpenGlWindowBind(&renderer->window, renderer->render_program);
  OpenGlBgAffineBind(&renderer->affine, renderer->render_program);
  OpenGlBgBitmapMode3Bind(&renderer->bg_bitmap_mode3, renderer->render_program);
  OpenGlBgBitmapMode4Bind(&renderer->bg_bitmap_mode4, renderer->render_program);
  OpenGlBgBitmapMode5Bind(&renderer->bg_bitmap_mode5, renderer->render_program);

  GLuint vertex = glGetAttribLocation(renderer->render_program, "vertex");
  glBindBuffer(GL_ARRAY_BUFFER, renderer->vertices);
  glVertexAttribPointer(vertex, /*size=*/2, /*type=*/GL_FLOAT,
                        /*normalized=*/false, /*stride=*/0, /*pointer=*/NULL);
  glEnableVertexAttribArray(vertex);

  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4u);
}

static void GbaPpuOpenGlRendererFlush(const GbaPpuOpenGlRenderer* renderer) {
  glEnable(GL_SCISSOR_TEST);
  glScissor(0u, renderer->flush_start_row * renderer->render_scale,
            GBA_SCREEN_WIDTH, renderer->flush_size * renderer->render_scale);
  glViewport(0u, 0u, GBA_SCREEN_WIDTH * renderer->render_scale,
             GBA_SCREEN_HEIGHT * renderer->render_scale);
  GbaPpuOpenGlRendererDraw(renderer, renderer->staging_fbo);
  glDisable(GL_SCISSOR_TEST);
}

static void GbaPpuOpenGlRendererReload(GbaPpuOpenGlRenderer* renderer,
                                       const GbaPpuMemory* memory,
                                       const GbaPpuRegisters* registers,
                                       GbaPpuDirtyBits* dirty_bits) {
  OpenGlBgAffineReload(&renderer->affine, registers, dirty_bits);
  OpenGlControlReload(&renderer->control, registers, dirty_bits);

  if (registers->dispcnt.forced_blank) {
    return;
  }

  OpenGlBgPaletteReload(&renderer->bg_palette, memory, dirty_bits);
  OpenGlBgMosaicReload(&renderer->mosaic, registers, dirty_bits);
  OpenGlWindowReload(&renderer->window, registers, dirty_bits);

  switch (registers->dispcnt.mode) {
    case 0u:
      // TuODO
      break;
    case 1u:
      // TODO
      break;
    case 2u:
      // TODO
      break;
    case 3u:
      OpenGlBgBitmapMode3Reload(&renderer->bg_bitmap_mode3, memory, registers,
                                dirty_bits);
      break;
    case 4u:
      OpenGlBgBitmapMode4Reload(&renderer->bg_bitmap_mode4, memory, registers,
                                dirty_bits);
      break;
    case 5u:
      OpenGlBgBitmapMode5Reload(&renderer->bg_bitmap_mode5, memory, registers,
                                dirty_bits);
      break;
  }
}

GbaPpuOpenGlRenderer* GbaPpuOpenGlRendererAllocate() {
  GbaPpuOpenGlRenderer* renderer = calloc(1u, sizeof(GbaPpuOpenGlRenderer));

  if (renderer != NULL) {
    GbaPpuDrawManagerInitialize(&renderer->draw_manager);
    renderer->next_render_scale = 1u;
    renderer->render_scale = 1u;
    renderer->flush_start_row = 0u;
    renderer->flush_size = 0u;
    renderer->last_frame_contents = 0u;
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
    glDeleteTextures(1u, &renderer->staging_texture);
    CreateStagingTexture(&renderer->staging_texture, renderer->render_scale);
    UpdateStagingFbo(renderer->staging_fbo, renderer->staging_texture);
    GbaPpuDrawManagerInvalidatePreviousFrame(&renderer->draw_manager);
  }

  if (registers->vcount == 0u) {
    GbaPpuDrawManagerStartFrame(&renderer->draw_manager, registers, dirty_bits);
    GbaPpuOpenGlRendererReload(renderer, memory, registers, dirty_bits);
    renderer->flush_start_row = 0u;
    renderer->flush_size = 1u;
    return;
  }

  bool should_flush = GbaPpuDrawManagerShouldFlush(&renderer->draw_manager,
                                                   registers, dirty_bits);

  if (!should_flush) {
    OpenGlBgAffineReload(&renderer->affine, registers, dirty_bits);
    renderer->flush_size += 1u;
    return;
  }

  GbaPpuOpenGlRendererFlush(renderer);
  GbaPpuOpenGlRendererReload(renderer, memory, registers, dirty_bits);
  renderer->flush_start_row = registers->vcount;
  renderer->flush_size = 1u;
}

void GbaPpuOpenGlRendererPresent(GbaPpuOpenGlRenderer* renderer, GLuint fbo,
                                 GLsizei width, GLsizei height,
                                 uint8_t* fbo_contents) {
  if (!renderer->initialized) {
    return;
  }

  bool can_skip = GbaPpuDrawManagerEndFrame(&renderer->draw_manager);
  if (can_skip && *fbo_contents == renderer->last_frame_contents) {
    return;
  }

  if (renderer->flush_size == GBA_SCREEN_HEIGHT) {
    GbaPpuOpenGlRendererDraw(renderer, fbo);
  } else {
    GbaPpuOpenGlRendererFlush(renderer);

    glUseProgram(renderer->upscale_program);

    GLuint coord_attrib =
        glGetAttribLocation(renderer->upscale_program, "coord");
    glBindBuffer(GL_ARRAY_BUFFER, renderer->vertices);
    glVertexAttribPointer(coord_attrib, /*size=*/2, /*type=*/GL_FLOAT,
                          /*normalized=*/false, /*stride=*/0, /*pointer=*/NULL);
    glEnableVertexAttribArray(coord_attrib);

    GLint texture_location =
        glGetUniformLocation(renderer->upscale_program, "image");
    glUniform1i(texture_location, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, renderer->staging_texture);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, width, height);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4u);
  }

  *fbo_contents = ++renderer->last_frame_contents;
}

void GbaPpuOpenGlRendererReloadContext(GbaPpuOpenGlRenderer* renderer) {
  OpenGlBgAffineReloadContext(&renderer->affine);
  OpenGlBgBitmapMode3ReloadContext(&renderer->bg_bitmap_mode3);
  OpenGlBgBitmapMode4ReloadContext(&renderer->bg_bitmap_mode4);
  OpenGlBgBitmapMode5ReloadContext(&renderer->bg_bitmap_mode5);
  OpenGlBgPaletteReloadContext(&renderer->bg_palette);

  CreateStagingTexture(&renderer->staging_texture, renderer->render_scale);
  CreateStagingFbo(&renderer->staging_fbo, renderer->staging_texture);
  CreateRenderProgram(&renderer->render_program);
  CreateUpscaleProgram(&renderer->upscale_program);
  CreateVertices(&renderer->vertices);

  renderer->initialized = true;
}

void GbaPpuOpenGlRendererSetScale(GbaPpuOpenGlRenderer* renderer,
                                  uint8_t render_scale) {
  renderer->next_render_scale = render_scale;
}

void GbaPpuOpenGlRendererFree(GbaPpuOpenGlRenderer* renderer) {
  if (renderer->initialized) {
    OpenGlBgAffineDestroy(&renderer->affine);
    OpenGlBgBitmapMode3Destroy(&renderer->bg_bitmap_mode3);
    OpenGlBgBitmapMode4Destroy(&renderer->bg_bitmap_mode4);
    OpenGlBgBitmapMode5Destroy(&renderer->bg_bitmap_mode5);
    OpenGlBgPaletteDestroy(&renderer->bg_palette);
    glDeleteFramebuffers(1u, &renderer->staging_fbo);
    glDeleteTextures(1u, &renderer->staging_texture);
    glDeleteProgram(renderer->render_program);
    glDeleteProgram(renderer->upscale_program);
    glDeleteBuffers(1u, &renderer->vertices);
  }

  free(renderer);
}