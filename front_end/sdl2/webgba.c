#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

#include "emulator/gba.h"

static GLuint g_fbo_texture = 0;
static GLuint g_fbo = 0;
static GLuint g_program = 0;
static SDL_Window *g_window = NULL;
static SDL_GLContext *g_glcontext = NULL;
static SDL_AudioDeviceID g_audiodevice = 0;
static GbaEmulator *g_emulator = NULL;
static GamePad *g_gamepad = NULL;

static void RenderAudioSample(int16_t left, int16_t right) {
  int16_t buffer[2] = {left, right};
  SDL_QueueAudio(g_audiodevice, buffer, sizeof(buffer));
}

static bool RenderNextFrame() {
  //
  // Check for events
  //

  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT:
        return false;
      case SDL_WINDOWEVENT:
        switch (event.window.event) {
          case SDL_WINDOWEVENT_CLOSE:
            return false;
          case SDL_WINDOWEVENT_RESIZED:
            // TODO
            break;
        }
    }
  }

  //
  // Update gamepad
  //

  const Uint8 *keyboard_state = SDL_GetKeyboardState(NULL);
  GamePadToggleA(g_gamepad, keyboard_state[SDL_SCANCODE_X] != 0);
  GamePadToggleB(g_gamepad, keyboard_state[SDL_SCANCODE_Z] != 0);
  GamePadToggleL(g_gamepad, keyboard_state[SDL_SCANCODE_A] != 0);
  GamePadToggleR(g_gamepad, keyboard_state[SDL_SCANCODE_S] != 0);
  GamePadToggleStart(g_gamepad, keyboard_state[SDL_SCANCODE_RETURN] != 0);
  GamePadToggleSelect(g_gamepad, keyboard_state[SDL_SCANCODE_BACKSPACE] != 0);
  GamePadToggleUp(g_gamepad, keyboard_state[SDL_SCANCODE_UP] != 0);
  GamePadToggleDown(g_gamepad, keyboard_state[SDL_SCANCODE_DOWN] != 0);
  GamePadToggleLeft(g_gamepad, keyboard_state[SDL_SCANCODE_LEFT] != 0);
  GamePadToggleRight(g_gamepad, keyboard_state[SDL_SCANCODE_RIGHT] != 0);

  //
  // Run emulation
  //

  GbaEmulatorStep(g_emulator, /*fbo=*/g_fbo, /*scale_factor=*/1,
                  RenderAudioSample);

  //
  // Render result
  //

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, g_fbo_texture);

  int width, height;
  SDL_GetWindowSize(g_window, &width, &height);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, width, height);
  glUseProgram(g_program);
  glDrawArrays(GL_TRIANGLES, 0, 3u);

  //
  // Flip framebuffer
  //

  SDL_GL_SwapWindow(g_window);

  return true;
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("usage: webgba <game>");
    return EXIT_SUCCESS;
  }

  //
  // Initialize SDL
  //

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS) < 0) {
    fprintf(stderr, "ERROR: Failed to initialize SDL\n");
    return EXIT_FAILURE;
  }

  //
  // Load Game
  //

  SDL_RWops *file = SDL_RWFromFile(argv[1], "rb");
  if (file == NULL) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load game file\n");
    SDL_Quit();
    return EXIT_FAILURE;
  }

  Sint64 size = SDL_RWsize(file);
  if (size < 0) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to get game file size\n");
    SDL_RWclose(file);
    SDL_Quit();
    return EXIT_FAILURE;
  }

  if (size > SIZE_MAX) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Out of memory\n");
    SDL_RWclose(file);
    SDL_Quit();
    return EXIT_FAILURE;
  }

  void *game = SDL_malloc((size_t)size);
  if (!game) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Out of memory\n");
    SDL_RWclose(file);
    SDL_Quit();
    return EXIT_FAILURE;
  }

  size_t objects_read = SDL_RWread(file, game, size, /*maxnum=*/1);
  SDL_RWclose(file);

  if (objects_read != 1) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to read game file\n");
    SDL_free(game);
    SDL_Quit();
    return EXIT_FAILURE;
  }

  //
  // Create Emulator
  //

  bool success = GbaEmulatorAllocate(game, size, &g_emulator, &g_gamepad);
  SDL_free(game);

  if (!success) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Out of memory\n");
    SDL_Quit();
    return EXIT_FAILURE;
  }

  //
  // Create Window
  //

  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_RESET_NOTIFICATION,
                      SDL_GL_CONTEXT_RESET_LOSE_CONTEXT);

  g_window = SDL_CreateWindow(
      "WebGBA", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      /*width=*/240, /*height=*/160, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

  if (g_window == NULL) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create window\n");
    GbaEmulatorFree(g_emulator);
    GamePadFree(g_gamepad);
    SDL_Quit();
    return EXIT_FAILURE;
  }

  g_glcontext = SDL_GL_CreateContext(g_window);
  if (g_glcontext == NULL) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create GL context\n");
    SDL_DestroyWindow(g_window);
    GbaEmulatorFree(g_emulator);
    GamePadFree(g_gamepad);
    SDL_Quit();
    return EXIT_FAILURE;
  }

  SDL_GL_MakeCurrent(g_window, g_glcontext);
  SDL_GL_SetSwapInterval(1);

  //
  // Configure Audio Device
  //

  SDL_AudioSpec want;

  SDL_memset(&want, 0, sizeof(want));
  want.format = AUDIO_S16;
  want.freq = 131072.0 * 60.0 / (16777216.0 / 280896.0);
  want.channels = 2;
  want.samples = 4096;

  SDL_AudioSpec have;
  g_audiodevice =
      SDL_OpenAudioDevice(/*device=*/NULL, /*iscapture=*/0, &want, &have,
                          /*allowed_changes=*/0);
  if (g_audiodevice == 0) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to open audio device\n");
    SDL_GL_DeleteContext(g_glcontext);
    SDL_DestroyWindow(g_window);
    GbaEmulatorFree(g_emulator);
    GamePadFree(g_gamepad);
    SDL_Quit();
    return EXIT_FAILURE;
  }

  SDL_PauseAudioDevice(g_audiodevice, /*pause_on=*/0);

  //
  // Create Emulator Framebuffer
  //

  glGenTextures(/*n=*/1, &g_fbo_texture);
  glBindTexture(GL_TEXTURE_2D, g_fbo_texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, /*level=*/0, /*internal_format=*/GL_RGB,
               /*width=*/240, /*height=*/160, /*border=*/0,
               /*format=*/GL_RGBA, /*type=*/GL_UNSIGNED_SHORT_5_5_5_1,
               /*pixels=*/NULL);
  glBindTexture(GL_TEXTURE_2D, 0);

  glGenFramebuffers(/*n=*/1, &g_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, g_fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         g_fbo_texture, /*level=*/0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  //
  // Create Shader Program
  //

  g_program = glCreateProgram();

  static const char *vertex_shader_source[9u] = {
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
  glAttachShader(g_program, vertex_shader);
  glDeleteShader(vertex_shader);

  static const char *fragment_shader_source[7u] = {
      "#version 130\n",
      "uniform sampler2D image;\n",
      "in vec2 texCoord;\n",
      "void main() {\n",
      "  vec4 color = textureLod(image, texCoord, 0.0);\n",
      "  gl_FragColor = vec4(color.r, color.g, color.b, 0.0);\n",
      "}\n",
  };

  GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 7u, fragment_shader_source, NULL);
  glCompileShader(fragment_shader);
  glAttachShader(g_program, fragment_shader);
  glDeleteShader(fragment_shader);

  glLinkProgram(g_program);

  //
  // Load emulator OpenGL context
  //

  GbaEmulatorReloadContext(g_emulator);

  //
  // Run
  //

  bool running = true;
  while (running) {
    running = RenderNextFrame();
  }

  //
  // Cleanup
  //

  SDL_CloseAudioDevice(g_audiodevice);
  SDL_DestroyWindow(g_window);
  SDL_GL_DeleteContext(g_glcontext);

  GbaEmulatorFree(g_emulator);
  GamePadFree(g_gamepad);

  SDL_Quit();

  return EXIT_SUCCESS;
}
