#include "emulator/ppu/gba/opengl/programs.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "emulator/ppu/gba/opengl/shader_fragment.h"
#include "emulator/ppu/gba/opengl/shader_vertex.h"

static char* GetFragmentShaderSource(bool objects, bool scrolling_bg0,
                                     bool scrolling_bg1, bool scrolling_bg2,
                                     bool scrolling_bg3, bool affine_bg2,
                                     bool affine_bg3, bool bitmap_bg,
                                     bool palette_bitmap_bg) {
  size_t source_len = strlen(fragment_shader_source);
  char* program = malloc(source_len + 1);
  memcpy(program, fragment_shader_source, source_len + 1);

  if (objects) {
    char* location = strstr(program, "OBJECTS 0");
    char* value = strchr(location, '0');
    *value = '1';
  }

  if (scrolling_bg0) {
    char* location = strstr(program, "SCROLLING_BACKGROUND_0 0");
    char* value = strchr(location, ' ') + 1;
    *value = '1';
  }

  if (scrolling_bg1) {
    char* location = strstr(program, "SCROLLING_BACKGROUND_1 0");
    char* value = strchr(location, '0');
    *value = '1';
  }

  if (scrolling_bg2) {
    char* location = strstr(program, "SCROLLING_BACKGROUND_2 0");
    char* value = strchr(location, '0');
    *value = '1';
  }

  if (scrolling_bg3) {
    char* location = strstr(program, "SCROLLING_BACKGROUND_3 0");
    char* value = strchr(location, '0');
    *value = '1';
  }

  if (affine_bg2) {
    char* location = strstr(program, "AFFINE_BACKGROUND_2 0");
    char* value = strchr(location, '0');
    *value = '1';
  }

  if (affine_bg3) {
    char* location = strstr(program, "AFFINE_BACKGROUND_3 0");
    char* value = strchr(location, '0');
    *value = '1';
  }

  if (bitmap_bg) {
    char* location = strstr(program, "BITMAP_BACKGROUND 0");
    char* value = strchr(location, '0');
    *value = '1';
  }

  if (palette_bitmap_bg) {
    char* location = strstr(program, "PALETTE_BITMAP_BACKGROUND 0");
    char* value = strchr(location, '0');
    *value = '1';
  }

  return program;
}

static void InitializeMode0(OpenGlPrograms* context, GLuint vertex_shader) {
  for (uint8_t obj = 0u; obj < 2u; obj++) {
    for (uint8_t bg0 = 0u; bg0 < 2u; bg0++) {
      for (uint8_t bg1 = 0u; bg1 < 2u; bg1++) {
        for (uint8_t bg2 = 0u; bg2 < 2u; bg2++) {
          for (uint8_t bg3 = 0u; bg3 < 2u; bg3++) {
            context->mode0[obj][bg0][bg1][bg2][bg3] = glCreateProgram();
            glAttachShader(context->mode0[obj][bg0][bg1][bg2][bg3],
                           vertex_shader);

            GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

            char* shader_source = GetFragmentShaderSource(
                /*objects=*/obj, /*scrolling_bg0=*/bg0, /*scrolling_bg1=*/bg1,
                /*scrolling_bg2=*/bg2, /*scrolling_bg3=*/bg3,
                /*affine_bg2=*/false, /*affine_bg3=*/false, /*bitmap_bg=*/false,
                /*palette_bitmap_bg=*/false);

            const char* sources[1] = {shader_source};
            glShaderSource(fragment_shader, 1u, sources, NULL);

            free(shader_source);

            glCompileShader(fragment_shader);
            glAttachShader(context->mode0[obj][bg0][bg1][bg2][bg3],
                           fragment_shader);
            glDeleteShader(fragment_shader);

            glLinkProgram(context->mode0[obj][bg0][bg1][bg2][bg3]);
          }
        }
      }
    }
  }

  for (uint8_t obj = 0u; obj < 2u; obj++) {
    for (uint8_t bg0 = 0u; bg0 < 2u; bg0++) {
      for (uint8_t bg1 = 0u; bg1 < 2u; bg1++) {
        for (uint8_t bg2 = 0u; bg2 < 2u; bg2++) {
          for (uint8_t bg3 = 0u; bg3 < 2u; bg3++) {
            context->programs[0u][obj][bg0][bg1][bg2][bg3] =
                context->mode0[obj][bg0][bg1][bg2][bg3];
          }
        }
      }
    }
  }
}

static void InitializeMode1(OpenGlPrograms* context, GLuint vertex_shader) {
  for (uint8_t obj = 0u; obj < 2u; obj++) {
    for (uint8_t bg0 = 0u; bg0 < 2u; bg0++) {
      for (uint8_t bg1 = 0u; bg1 < 2u; bg1++) {
        for (uint8_t bg2 = 0u; bg2 < 2u; bg2++) {
          context->mode1[obj][bg0][bg1][bg2] = glCreateProgram();
          glAttachShader(context->mode1[obj][bg0][bg1][bg2], vertex_shader);

          GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

          char* shader_source = GetFragmentShaderSource(
              /*objects=*/obj, /*scrolling_bg0=*/bg0, /*scrolling_bg1=*/bg1,
              /*scrolling_bg2=*/false, /*scrolling_bg3=*/false,
              /*affine_bg2=*/bg2, /*affine_bg3=*/false, /*bitmap_bg=*/false,
              /*palette_bitmap_bg=*/false);

          const char* sources[1] = {shader_source};
          glShaderSource(fragment_shader, 1u, sources, NULL);

          free(shader_source);

          glCompileShader(fragment_shader);
          glAttachShader(context->mode1[obj][bg0][bg1][bg2], fragment_shader);
          glDeleteShader(fragment_shader);

          glLinkProgram(context->mode1[obj][bg0][bg1][bg2]);
        }
      }
    }
  }

  for (uint8_t obj = 0u; obj < 2u; obj++) {
    for (uint8_t bg0 = 0u; bg0 < 2u; bg0++) {
      for (uint8_t bg1 = 0u; bg1 < 2u; bg1++) {
        for (uint8_t bg2 = 0u; bg2 < 2u; bg2++) {
          for (uint8_t bg3 = 0u; bg3 < 2u; bg3++) {
            context->programs[1u][obj][bg0][bg1][bg2][bg3] =
                context->mode1[obj][bg0][bg1][bg2];
          }
        }
      }
    }
  }
}

static void InitializeMode2(OpenGlPrograms* context, GLuint vertex_shader) {
  for (uint8_t obj = 0u; obj < 2u; obj++) {
    for (uint8_t bg2 = 0u; bg2 < 2u; bg2++) {
      for (uint8_t bg3 = 0u; bg3 < 2u; bg3++) {
        context->mode2[obj][bg2][bg3] = glCreateProgram();
        glAttachShader(context->mode2[obj][bg2][bg3], vertex_shader);

        GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

        char* shader_source = GetFragmentShaderSource(
            /*objects=*/obj, /*scrolling_bg0=*/false, /*scrolling_bg1=*/false,
            /*scrolling_bg2=*/false, /*scrolling_bg3=*/false,
            /*affine_bg2=*/bg2, /*affine_bg3=*/bg3, /*bitmap_bg=*/false,
            /*palette_bitmap_bg=*/false);

        const char* sources[1] = {shader_source};
        glShaderSource(fragment_shader, 1u, sources, NULL);

        free(shader_source);

        glCompileShader(fragment_shader);
        glAttachShader(context->mode2[obj][bg2][bg3], fragment_shader);
        glDeleteShader(fragment_shader);

        glLinkProgram(context->mode2[obj][bg2][bg3]);
      }
    }
  }

  for (uint8_t obj = 0u; obj < 2u; obj++) {
    for (uint8_t bg0 = 0u; bg0 < 2u; bg0++) {
      for (uint8_t bg1 = 0u; bg1 < 2u; bg1++) {
        for (uint8_t bg2 = 0u; bg2 < 2u; bg2++) {
          for (uint8_t bg3 = 0u; bg3 < 2u; bg3++) {
            context->programs[2u][obj][bg0][bg1][bg2][bg3] =
                context->mode2[obj][bg2][bg3];
          }
        }
      }
    }
  }
}

static void InitializeMode35(OpenGlPrograms* context, GLuint vertex_shader) {
  for (uint8_t obj = 0u; obj < 2u; obj++) {
    for (uint8_t bg2 = 0u; bg2 < 2u; bg2++) {
      context->mode35[obj][bg2] = glCreateProgram();
      glAttachShader(context->mode35[obj][bg2], vertex_shader);

      GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

      char* shader_source = GetFragmentShaderSource(
          /*objects=*/obj, /*scrolling_bg0=*/false, /*scrolling_bg1=*/false,
          /*scrolling_bg2=*/false, /*scrolling_bg3=*/false,
          /*affine_bg2=*/false, /*affine_bg3=*/false, /*bitmap_bg=*/bg2,
          /*palette_bitmap_bg=*/false);

      const char* sources[1] = {shader_source};
      glShaderSource(fragment_shader, 1u, sources, NULL);

      free(shader_source);

      glCompileShader(fragment_shader);
      glAttachShader(context->mode35[obj][bg2], fragment_shader);
      glDeleteShader(fragment_shader);

      glLinkProgram(context->mode35[obj][bg2]);
    }
  }

  for (uint8_t obj = 0u; obj < 2u; obj++) {
    for (uint8_t bg0 = 0u; bg0 < 2u; bg0++) {
      for (uint8_t bg1 = 0u; bg1 < 2u; bg1++) {
        for (uint8_t bg2 = 0u; bg2 < 2u; bg2++) {
          for (uint8_t bg3 = 0u; bg3 < 2u; bg3++) {
            context->programs[3u][obj][bg0][bg1][bg2][bg3] =
                context->mode35[obj][bg2];
            context->programs[5u][obj][bg0][bg1][bg2][bg3] =
                context->mode35[obj][bg2];
          }
        }
      }
    }
  }
}

static void InitializeMode4(OpenGlPrograms* context, GLuint vertex_shader) {
  for (uint8_t obj = 0u; obj < 2u; obj++) {
    for (uint8_t bg2 = 0u; bg2 < 2u; bg2++) {
      context->mode4[obj][bg2] = glCreateProgram();
      glAttachShader(context->mode4[obj][bg2], vertex_shader);

      GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

      char* shader_source = GetFragmentShaderSource(
          /*objects=*/obj, /*scrolling_bg0=*/false, /*scrolling_bg1=*/false,
          /*scrolling_bg2=*/false, /*scrolling_bg3=*/false,
          /*affine_bg2=*/false, /*affine_bg3=*/false, /*bitmap_bg=*/false,
          /*palette_bitmap_bg=*/bg2);
      const char* sources[1] = {shader_source};
      glShaderSource(fragment_shader, 1u, sources, NULL);
      free(shader_source);

      glCompileShader(fragment_shader);
      glAttachShader(context->mode4[obj][bg2], fragment_shader);
      glDeleteShader(fragment_shader);

      glLinkProgram(context->mode4[obj][bg2]);
    }
  }

  for (uint8_t obj = 0u; obj < 2u; obj++) {
    for (uint8_t bg0 = 0u; bg0 < 2u; bg0++) {
      for (uint8_t bg1 = 0u; bg1 < 2u; bg1++) {
        for (uint8_t bg2 = 0u; bg2 < 2u; bg2++) {
          for (uint8_t bg3 = 0u; bg3 < 2u; bg3++) {
            context->programs[4u][obj][bg0][bg1][bg2][bg3] =
                context->mode4[obj][bg2];
          }
        }
      }
    }
  }
}

GLuint OpenGlProgramsGet(const OpenGlPrograms* context, uint8_t mode,
                         bool objects, bool bg0, bool bg1, bool bg2, bool bg3) {
  assert(mode < 6);
  return context->programs[mode][objects][bg0][bg1][bg2][bg3];
}

void OpenGlProgramsReloadContext(OpenGlPrograms* context) {
  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1u, &vertex_shader_source, NULL);
  glCompileShader(vertex_shader);

  InitializeMode0(context, vertex_shader);
  InitializeMode1(context, vertex_shader);
  InitializeMode2(context, vertex_shader);
  InitializeMode35(context, vertex_shader);
  InitializeMode4(context, vertex_shader);

  glDeleteShader(vertex_shader);
}

void OpenGlProgramsDestroy(OpenGlPrograms* context) {
  // Mode 0
  for (uint8_t obj = 0u; obj < 2u; obj++) {
    for (uint8_t bg0 = 0u; bg0 < 2u; bg0++) {
      for (uint8_t bg1 = 0u; bg1 < 2u; bg1++) {
        for (uint8_t bg2 = 0u; bg2 < 2u; bg2++) {
          for (uint8_t bg3 = 0u; bg3 < 2u; bg3++) {
            glDeleteProgram(context->mode0[obj][bg0][bg1][bg2][bg3]);
          }
        }
      }
    }
  }

  // Mode 1
  for (uint8_t obj = 0u; obj < 2u; obj++) {
    for (uint8_t bg0 = 0u; bg0 < 2u; bg0++) {
      for (uint8_t bg1 = 0u; bg1 < 2u; bg1++) {
        for (uint8_t bg2 = 0u; bg2 < 2u; bg2++) {
          glDeleteProgram(context->mode1[obj][bg0][bg1][bg2]);
        }
      }
    }
  }

  // Mode 2
  for (uint8_t obj = 0u; obj < 2u; obj++) {
    for (uint8_t bg2 = 0u; bg2 < 2u; bg2++) {
      for (uint8_t bg3 = 0u; bg3 < 2u; bg3++) {
        glDeleteProgram(context->mode2[obj][bg2][bg3]);
      }
    }
  }

  // Mode 3/5
  for (uint8_t obj = 0u; obj < 2u; obj++) {
    for (uint8_t bg2 = 0u; bg2 < 2u; bg2++) {
      glDeleteProgram(context->mode35[obj][bg2]);
    }
  }

  // Mode 4
  for (uint8_t obj = 0u; obj < 2u; obj++) {
    for (uint8_t bg2 = 0u; bg2 < 2u; bg2++) {
      glDeleteProgram(context->mode4[obj][bg2]);
    }
  }
}