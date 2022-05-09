#include "emulator/ppu/gba/opengl/bg_bitmap.h"

#include <string.h>

static void CreateMode34SourceTexture(GLuint* result) {
  glGenTextures(1u, result);
  glBindTexture(GL_TEXTURE_2D, *result);
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

static void CreateMode5SourceTexture(GLuint* result) {
  glGenTextures(1u, result);
  glBindTexture(GL_TEXTURE_2D, *result);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, /*level=*/0, /*internal_format=*/GL_RGBA,
               /*width=*/GBA_REDUCED_FRAME_WIDTH,
               /*height=*/GBA_REDUCED_FRAME_HEIGHT,
               /*border=*/0, /*format=*/GL_RGBA,
               /*type=*/GL_UNSIGNED_SHORT_5_5_5_1,
               /*pixels=*/NULL);
  glBindTexture(GL_TEXTURE_2D, 0);
}

static void CreateVertices(GLuint* result) {
  static const GLfloat vertices[8u] = {-1.0, -1.0, -1.0, 1.0,
                                       1.0,  1.0,  1.0,  -1.0};
  glGenBuffers(1, result);
  glBindBuffer(GL_ARRAY_BUFFER, *result);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 8, vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

static void CreateProgram(GLuint* result) {
  *result = glCreateProgram();

  static const char* vertex_shader_source =
      "#version 100\n"
      "attribute highp vec2 coord;\n"
      "varying highp vec2 texcoord;\n"
      "void main() {\n"
      "  texcoord.x = (coord.x + 1.0) * 0.5;\n"
      "  texcoord.y = (coord.y - 1.0) * -0.5;\n"
      "  gl_Position = vec4(coord, 0.0, 1.0);\n"
      "}\n";

  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
  glCompileShader(vertex_shader);

  static const char* fragment_shader_source =
      "#version 100\n"
      "uniform lowp sampler2D image;\n"
      "varying highp vec2 texcoord;\n"
      "void main() {\n"
      "  lowp vec4 color = texture2D(image, texcoord);\n"
      "  gl_FragColor = vec4(color.b, color.g, color.r, 0.0);\n"
      "}\n";

  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
  glCompileShader(fragment_shader);
  glAttachShader(*result, fragment_shader);
  glAttachShader(*result, vertex_shader);
  glDeleteShader(fragment_shader);
  glDeleteShader(vertex_shader);

  glLinkProgram(*result);
}

static void Render(const GbaPpuOpenGlBgBitmap* context,
                   const GbaPpuMemory* memory, const GbaPpuRegisters* registers,
                   const GbaPpuInternalRegisters* internal_registers,
                   const GbaPpuDirtyBits* dirty_bits, GLuint fbo, GLsizei width,
                   GLsizei height, uint8_t y, GLuint src_texture,
                   GLsizei bg_width, GLsizei bg_height) {
  assert(height % GBA_SCREEN_HEIGHT == 0u);

  // TODO: Update vertices
  GLuint coord_attrib = glGetAttribLocation(context->program, "coord");
  glBindBuffer(GL_ARRAY_BUFFER, context->vertices);
  glVertexAttribPointer(coord_attrib, /*size=*/2, /*type=*/GL_FLOAT,
                        /*normalized=*/false, /*stride=*/0, /*pointer=*/NULL);
  glEnableVertexAttribArray(coord_attrib);

  GLint texture_location = glGetUniformLocation(context->program, "image");
  glUniform1i(texture_location, 0);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, src_texture);

  if (dirty_bits->vram.mode_3.overall || dirty_bits->palette.large_palette) {
    glTexSubImage2D(GL_TEXTURE_2D, /*level=*/0, /*xoffset=*/0, /*yoffset=*/0,
                    /*width=*/bg_width, /*height=*/bg_height,
                    /*format=*/GL_RGBA, /*type=*/GL_UNSIGNED_SHORT_5_5_5_1,
                    /*pixels=*/context->colors);
  }

  GLsizei y_start = height * y / GBA_SCREEN_HEIGHT;
  GLsizei y_end = height * (y + 1) / GBA_SCREEN_HEIGHT;

  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glViewport(0, y_start, width, y_end);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4u);
}

void GbaPpuOpenGlBgBitmapMode3(
    GbaPpuOpenGlBgBitmap* context, const GbaPpuMemory* memory,
    const GbaPpuRegisters* registers,
    const GbaPpuInternalRegisters* internal_registers,
    GbaPpuDirtyBits* dirty_bits, GLuint fbo, GLsizei width, GLsizei height,
    uint8_t y) {
  if (!dirty_bits->vram.mode_3.overall && !dirty_bits->palette.large_palette) {
    return;
  }

  if (dirty_bits->vram.mode_3.overall || dirty_bits->palette.large_palette) {
    for (uint_fast8_t y = 0; y < GBA_SCREEN_HEIGHT; y++) {
      for (uint_fast8_t x = 0; x < GBA_SCREEN_WIDTH; x++) {
        context->colors[y * GBA_SCREEN_HEIGHT + x] =
            memory->vram.mode_3.bg.pixels[y][x] << 1u;
      }
    }
  }

  Render(context, memory, registers, internal_registers, dirty_bits, fbo, width,
         height, y, context->src_texture_mode34, GBA_SCREEN_WIDTH,
         GBA_SCREEN_HEIGHT);
}

void GbaPpuOpenGlBgBitmapMode4(
    GbaPpuOpenGlBgBitmap* context, const GbaPpuMemory* memory,
    const GbaPpuRegisters* registers,
    const GbaPpuInternalRegisters* internal_registers,
    GbaPpuDirtyBits* dirty_bits, GLuint fbo, GLsizei width, GLsizei height,
    uint8_t y) {
  if (!dirty_bits->vram.mode_3.overall && !dirty_bits->palette.large_palette) {
    return;
  }

  if (dirty_bits->vram.mode_3.overall || dirty_bits->palette.large_palette) {
    for (uint_fast8_t y = 0; y < GBA_SCREEN_HEIGHT; y++) {
      for (uint_fast8_t x = 0; x < GBA_SCREEN_WIDTH; x++) {
        uint8_t color_index =
            memory->vram.mode_4.bg.pages[registers->dispcnt.page_select]
                .pixels[y][x];
        context->colors[y * GBA_SCREEN_HEIGHT + x] =
            memory->palette.bg.large_palette[color_index] << 1u;
      }
    }
  }

  Render(context, memory, registers, internal_registers, dirty_bits, fbo, width,
         height, y, context->src_texture_mode34, GBA_SCREEN_WIDTH,
         GBA_SCREEN_HEIGHT);
}

void GbaPpuOpenGlBgBitmapMode5(
    GbaPpuOpenGlBgBitmap* context, const GbaPpuMemory* memory,
    const GbaPpuRegisters* registers,
    const GbaPpuInternalRegisters* internal_registers,
    GbaPpuDirtyBits* dirty_bits, GLuint fbo, GLsizei width, GLsizei height,
    uint8_t y) {
  if (!dirty_bits->vram.mode_3.overall && !dirty_bits->palette.large_palette) {
    return;
  }

  if (dirty_bits->vram.mode_3.overall || dirty_bits->palette.large_palette) {
    for (uint_fast8_t y = 0; y < GBA_REDUCED_FRAME_HEIGHT; y++) {
      for (uint_fast8_t x = 0; x < GBA_REDUCED_FRAME_WIDTH; x++) {
        context->colors[y * GBA_REDUCED_FRAME_HEIGHT + x] =
            memory->vram.mode_3.bg.pixels[y][x] << 1u;
      }
    }
  }

  Render(context, memory, registers, internal_registers, dirty_bits, fbo, width,
         height, y, context->src_texture_mode5, GBA_REDUCED_FRAME_WIDTH,
         GBA_REDUCED_FRAME_HEIGHT);
}

void GbaPpuOpenGlBgBitmapReloadContext(GbaPpuOpenGlBgBitmap* context) {
  CreateMode34SourceTexture(&context->src_texture_mode34);
  CreateMode5SourceTexture(&context->src_texture_mode5);
  CreateProgram(&context->program);
  CreateVertices(&context->vertices);
}

void GbaPpuOpenGlBgBitmapDestroy(GbaPpuOpenGlBgBitmap* context) {
  glDeleteTextures(1u, &context->src_texture_mode34);
  glDeleteTextures(1u, &context->src_texture_mode5);
  glDeleteProgram(context->program);
  glDeleteBuffers(1u, &context->vertices);
}