#include "emulator/ppu/gba/opengl/bg_bitmap.h"

#include <string.h>

static void CreateMode3Texture(GLuint* result) {
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

static void CreateMode4Textures(GLuint results[2u]) {
  glGenTextures(2u, results);
  for (uint8_t i = 0; i < 2u; i++) {
    glBindTexture(GL_TEXTURE_2D, results[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, /*level=*/0, /*internal_format=*/GL_LUMINANCE,
                 /*width=*/GBA_SCREEN_WIDTH, /*height=*/GBA_SCREEN_HEIGHT,
                 /*border=*/0, /*format=*/GL_LUMINANCE,
                 /*type=*/GL_UNSIGNED_BYTE,
                 /*pixels=*/NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
  }
}

static void CreateMode5Textures(GLuint results[2u]) {
  glGenTextures(2u, results);
  for (uint8_t i = 0; i < 2u; i++) {
    glBindTexture(GL_TEXTURE_2D, results[i]);
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
}

static void CreateVertices(GLuint* result) {
  static const GLfloat vertices[8u] = {-1.0, -1.0, -1.0, 1.0,
                                       1.0,  1.0,  1.0,  -1.0};
  glGenBuffers(1, result);
  glBindBuffer(GL_ARRAY_BUFFER, *result);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 8, vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

static GLuint CreateVertexShader() {
  static const char* vertex_shader_source =
      "#version 100\n"
      "attribute highp vec2 vertex;\n"
      "attribute highp vec2 vertextexcoord;\n"
      "varying highp vec2 texcoord;\n"
      "void main() {\n"
      "  texcoord = vertextexcoord;\n"
      "  gl_Position = vec4(vertex, 0.0, 1.0);\n"
      "}\n";

  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
  glCompileShader(vertex_shader);

  return vertex_shader;
}

static GLuint CreateMode3FragmentShader() {
  static const char* fragment_shader_source =
      "#version 100\n"
      "uniform lowp sampler2D image;\n"
      "uniform highp vec2 mosaic;\n"
      "const highp vec2 screen = vec2(240.0, 160.0);\n"
      "varying highp vec2 texcoord;\n"
      "void main() {\n"
      "  texcoord -= mod(texcoord, mosaic);\n"
      "  lowp vec4 color = texture2D(image, texcoord / screen);\n"
      "  color *= step(texcoord.x, screen.x);\n"
      "  color *= step(texcoord.y, screen.y);\n"
      "  color *= step(-texcoord.x, 0.0);\n"
      "  color *= step(-texcoord.y, 0.0);\n"
      "  gl_FragColor = vec4(color.b, color.g, color.r, 0.0);\n"
      "}\n";

  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
  glCompileShader(fragment_shader);

  return fragment_shader;
}

static GLuint CreateMode4FragmentShader() {
  static const char* fragment_shader_source =
      "#version 100\n"
      "uniform mediump sampler2D image;\n"
      "uniform highp vec2 mosaic;\n"
      "uniform lowp sampler2D palette;\n"
      "const mediump float offset = 1.0 / 512.0;\n"
      "const highp vec2 screen = vec2(240.0, 160.0);\n"
      "varying highp vec2 texcoord;\n"
      "void main() {\n"
      "  texcoord -= mod(texcoord, mosaic);\n"
      "  mediump vec4 index = texture2D(image, texcoord / screen);\n"
      "  lowp vec4 color = texture2D(palette, vec2(index.r + offset, 0.5));\n"
      "  color *= step(texcoord.x, screen.x);\n"
      "  color *= step(texcoord.y, screen.y);\n"
      "  color *= step(-texcoord.x, 0.0);\n"
      "  color *= step(-texcoord.y, 0.0);\n"
      "  gl_FragColor = vec4(color.b, color.g, color.r, 0.0);\n"
      "}\n";

  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
  glCompileShader(fragment_shader);

  return fragment_shader;
}

static GLuint CreateMode5FragmentShader() {
  static const char* fragment_shader_source =
      "#version 100\n"
      "uniform lowp sampler2D image;\n"
      "uniform highp vec2 mosaic;\n"
      "const highp vec2 screen = vec2(160.0, 128.0);\n"
      "varying highp vec2 texcoord;\n"
      "void main() {\n"
      "  texcoord -= mod(texcoord, mosaic);\n"
      "  lowp vec4 color = texture2D(image, texcoord / screen);\n"
      "  color *= step(texcoord.x, screen.x);\n"
      "  color *= step(texcoord.y, screen.y);\n"
      "  color *= step(-texcoord.x, 0.0);\n"
      "  color *= step(-texcoord.y, 0.0);\n"
      "  gl_FragColor = vec4(color.b, color.g, color.r, 0.0);\n"
      "}\n";

  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
  glCompileShader(fragment_shader);

  return fragment_shader;
}

static void CreatePrograms(GLuint* mode3_program, GLuint* mode4_program,
                           GLuint* mode5_program) {
  GLuint vertex_shader = CreateVertexShader();
  GLuint mode3_fragment_shader = CreateMode3FragmentShader();
  GLuint mode4_fragment_shader = CreateMode4FragmentShader();
  GLuint mode5_fragment_shader = CreateMode5FragmentShader();

  *mode3_program = glCreateProgram();
  glAttachShader(mode3_program, vertex_shader);
  glAttachShader(mode3_program, mode3_fragment_shader);
  glLinkProgram(mode3_program);

  *mode4_program = glCreateProgram();
  glAttachShader(mode4_program, vertex_shader);
  glAttachShader(mode4_program, mode4_fragment_shader);
  glLinkProgram(mode4_program);

  *mode5_program = glCreateProgram();
  glAttachShader(mode5_program, vertex_shader);
  glAttachShader(mode5_program, mode5_fragment_shader);
  glLinkProgram(mode5_program);

  glDeleteShader(vertex_shader);
  glDeleteShader(mode3_fragment_shader);
  glDeleteShader(mode4_fragment_shader);
  glDeleteShader(mode5_fragment_shader);
}

void GbaPpuOpenGlBgBitmapMode3(
    GbaPpuOpenGlBgBitmap* context, const GbaPpuMemory* memory,
    const GbaPpuRegisters* registers,
    const GbaPpuInternalRegisters* internal_registers,
    GbaPpuDirtyBits* dirty_bits, GLuint fbo, GLsizei width, GLsizei height,
    uint8_t y) {
  if (dirty_bits->vram.mode_3.overall) {
    for (uint_fast8_t y = 0; y < GBA_SCREEN_HEIGHT; y++) {
      for (uint_fast8_t x = 0; x < GBA_SCREEN_WIDTH; x++) {
        context->staging.colors[y * GBA_SCREEN_WIDTH + x] =
            memory->vram.mode_3.bg.pixels[y][x] << 1u;
      }
    }

    glBindTexture(GL_TEXTURE_2D, context->mode3_texture);
    glTexSubImage2D(GL_TEXTURE_2D, /*level=*/0, /*xoffset=*/0, /*yoffset=*/0,
                    /*width=*/GBA_SCREEN_WIDTH, /*height=*/GBA_SCREEN_HEIGHT,
                    /*format=*/GL_RGBA, /*type=*/GL_UNSIGNED_SHORT_5_5_5_1,
                    /*pixels=*/context->staging.colors);
    glBindTexture(GL_TEXTURE_2D, 0);
    dirty_bits->vram.mode_3.overall = false;
  }

  // TODO: Call Render
}

void GbaPpuOpenGlBgBitmapMode4(
    GbaPpuOpenGlBgBitmap* context, const GbaPpuMemory* memory,
    const GbaPpuRegisters* registers,
    const GbaPpuInternalRegisters* internal_registers,
    GbaPpuDirtyBits* dirty_bits, GLuint fbo, GLsizei width, GLsizei height,
    uint8_t y) {
  if (dirty_bits->vram.mode_4.pages[registers->dispcnt.page_select]) {
    for (uint_fast8_t y = 0; y < GBA_SCREEN_HEIGHT; y++) {
      for (uint_fast8_t x = 0; x < GBA_SCREEN_WIDTH; x++) {
        context->staging.indices[y * GBA_SCREEN_WIDTH + x] =
            memory->vram.mode_4.bg.pages[registers->dispcnt.page_select]
                .pixels[y][x];
      }
    }

    glBindTexture(GL_TEXTURE_2D,
                  context->mode4_textures[registers->dispcnt.page_select]);
    glTexSubImage2D(GL_TEXTURE_2D, /*level=*/0, /*xoffset=*/0, /*yoffset=*/0,
                    /*width=*/GBA_SCREEN_WIDTH, /*height=*/GBA_SCREEN_HEIGHT,
                    /*format=*/GL_LUMINANCE, /*type=*/GL_UNSIGNED_BYTE,
                    /*pixels=*/context->staging.indices);
    glBindTexture(GL_TEXTURE_2D, 0);

    dirty_bits->vram.mode_4.pages[registers->dispcnt.page_select] = false;
  }

  // TODO: Call Render
}

void GbaPpuOpenGlBgBitmapMode5(
    GbaPpuOpenGlBgBitmap* context, const GbaPpuMemory* memory,
    const GbaPpuRegisters* registers,
    const GbaPpuInternalRegisters* internal_registers,
    GbaPpuDirtyBits* dirty_bits, GLuint fbo, GLsizei width, GLsizei height,
    uint8_t y) {
  if (dirty_bits->vram.mode_5.pages[registers->dispcnt.page_select]) {
    for (uint_fast8_t y = 0; y < GBA_REDUCED_FRAME_HEIGHT; y++) {
      for (uint_fast8_t x = 0; x < GBA_REDUCED_FRAME_WIDTH; x++) {
        context->staging.colors[y * GBA_REDUCED_FRAME_WIDTH + x] =
            memory->vram.mode_5.bg.pages[registers->dispcnt.page_select]
                .pixels[y][x];
      }
    }

    glBindTexture(GL_TEXTURE_2D,
                  context->mode5_textures[registers->dispcnt.page_select]);
    glTexSubImage2D(GL_TEXTURE_2D, /*level=*/0, /*xoffset=*/0, /*yoffset=*/0,
                    /*width=*/GBA_REDUCED_FRAME_WIDTH,
                    /*height=*/GBA_REDUCED_FRAME_HEIGHT,
                    /*format=*/GL_RGBA, /*type=*/GL_UNSIGNED_SHORT_5_5_5_1,
                    /*pixels=*/context->staging.colors);
    glBindTexture(GL_TEXTURE_2D, 0);

    dirty_bits->vram.mode_5.pages[registers->dispcnt.page_select] = false;
  }

  // TODO: Call Render
}

void GbaPpuOpenGlBgBitmapReloadContext(GbaPpuOpenGlBgBitmap* context) {
  CreateVertices(&context->vertices);
  CreateMode3Texture(&context->mode3_texture);
  CreateMode4Texture(context->mode4_textures);
  CreateMode5Texture(context->mode5_textures);
  CreatePrograms(&context->mode3_program, &context->mode4_program,
                 &context->mode5_program);
}

void GbaPpuOpenGlBgBitmapDestroy(GbaPpuOpenGlBgBitmap* context) {
  glDeleteBuffers(1u, &context->vertices);
  glDeleteTextures(1u, &context->mode3_texture);
  glDeleteTextures(2u, context->mode4_textures);
  glDeleteTextures(2u, context->mode5_textures);
  glDeleteProgram(context->mode3_program);
  glDeleteProgram(context->mode4_program);
  glDeleteProgram(context->mode5_program);
}