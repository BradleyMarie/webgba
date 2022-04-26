#if __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif  // __EMSCRIPTEN__

#include <SDL/SDL.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "emulator/gba.h"
#include "third_party/libsamplerate/samplerate.h"

#if __EMSCRIPTEN__

#else

#endif  // __EMSCRIPTEN__

#if __EMSCRIPTEN__
#define WEB_INPUT_A 0
#define WEB_INPUT_B 1
#define WEB_INPUT_L 4
#define WEB_INPUT_R 5
#define WEB_INPUT_START 9
#define WEB_INPUT_SELECT 8
#define WEB_INPUT_UP 12
#define WEB_INPUT_DOWN 13
#define WEB_INPUT_LEFT 14
#define WEB_INPUT_RIGHT 15
#else
#define XINPUT_A 0
#define XINPUT_B 1
#define XINPUT_L 4
#define XINPUT_R 5
#define XINPUT_START 7
#define XINPUT_SELECT 6
#endif  // __EMSCRIPTEN__

static SDL_Joystick *g_joystick = NULL;
static SRC_STATE *g_src_state = NULL;
static double g_src_ratio = 1.0;
static GLuint g_fbo_texture = 0;
static GLuint g_fbo = 0;
static GLuint g_program = 0;
static GLuint g_vertices = 0;
static SDL_Surface *g_screen = NULL;
static GbaEmulator *g_emulator = NULL;
static GamePad *g_gamepad = NULL;

#define AUDIO_BUFFER_SIZE 32768
static float g_audio_buffer[AUDIO_BUFFER_SIZE];
static size_t g_audio_buffer_start = 0;
static size_t g_audio_buffer_end = 0;
static bool g_buffer_full = false;

static void AddSample(float sample) {
  g_audio_buffer[g_audio_buffer_end] = sample;

  size_t new_end = (g_audio_buffer_end + 1) % AUDIO_BUFFER_SIZE;
  if (g_audio_buffer_start == g_audio_buffer_end && g_buffer_full) {
    g_audio_buffer_start = new_end;
    g_audio_buffer_end = new_end;
  } else {
    g_audio_buffer_end = new_end;
  }

  if (g_audio_buffer_start == g_audio_buffer_end) {
    g_buffer_full = true;
  }
}

static float RemoveSample() {
  if (g_audio_buffer_start == g_audio_buffer_end && !g_buffer_full) {
    return 0;
  }

  float result = g_audio_buffer[g_audio_buffer_start];
  g_audio_buffer_start = (g_audio_buffer_start + 1) % AUDIO_BUFFER_SIZE;

  if (g_audio_buffer_start == g_audio_buffer_end) {
    g_buffer_full = false;
  }

  return result;
}

static void RenderAudioSample(int16_t left, int16_t right) {
  float input_samples[2];
  src_short_to_float_array(&left, &input_samples[0], 1);
  src_short_to_float_array(&right, &input_samples[1], 1);

  float output_samples[128];
  SRC_DATA data;
  data.data_in = input_samples;
  data.input_frames = 1;
  data.data_out = output_samples;
  data.output_frames = 64;
  data.end_of_input = false;
  data.src_ratio = g_src_ratio;

  if (src_process(g_src_state, &data) != 0) {
    return;
  }

  SDL_LockAudio();

  for (long i = 0; i < data.output_frames_gen * 2; i++) {
    AddSample(output_samples[i]);
  }

  SDL_UnlockAudio();
}

static void AudioCallback(void *userdata, Uint8 *stream, int len) {
  while (len != 0) {
    float sample = RemoveSample();

#if __EMSCRIPTEN__
    int bytes_to_copy = sizeof(float);
    assert(bytes_to_copy <= len);

    memcpy(stream, &sample, bytes_to_copy);
#else
    int bytes_to_copy = sizeof(int16_t);
    assert(bytes_to_copy <= len);

    src_float_to_short_array(&sample, (void *)stream, 1);
#endif  // __EMSCRIPTEN__

    stream += bytes_to_copy;
    len -= bytes_to_copy;
  }
}

#if __EMSCRIPTEN__
typedef void ReturnType;
#else
typedef bool ReturnType;
#endif  // __EMSCRIPTEN__

static bool SetVideoMode(int width, int height) {
  SDL_Surface *new_surface = SDL_SetVideoMode(
      /*width=*/width, /*height=*/height, 0, SDL_RESIZABLE | SDL_OPENGL);
  if (new_surface == NULL) {
    return false;
  }

  g_screen = new_surface;

  return true;
}

static ReturnType RenderNextFrame() {
  //
  // Check for events
  //

  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT:
#if __EMSCRIPTEN__
        emscripten_cancel_main_loop();
        return;
#else
        return false;
#endif  // __EMSCRIPTEN__
      case SDL_VIDEORESIZE:
        if (!SetVideoMode(/*width=*/event.resize.w,
                          /*height=*/event.resize.h)) {
#if __EMSCRIPTEN__
          emscripten_cancel_main_loop();
          return;
#else
          return false;
#endif  // __EMSCRIPTEN__
        }
        break;
    }
  }

  //
  // Update gamepad
  //

#if __EMSCRIPTEN__
  const Uint8 *key_state = SDL_GetKeyboardState(NULL);
#else
  const Uint8 *key_state = SDL_GetKeyState(NULL);
#endif  // __EMSCRIPTEN__

  bool a_pressed = key_state[SDLK_x] != 0;
  bool b_pressed = key_state[SDLK_z] != 0;
  bool l_pressed = key_state[SDLK_a] != 0;
  bool r_pressed = key_state[SDLK_s] != 0;
  bool start_pressed = key_state[SDLK_RETURN] != 0;
  bool select_pressed = key_state[SDLK_BACKSPACE] != 0;
  bool up_pressed = key_state[SDLK_UP] != 0;
  bool down_pressed = key_state[SDLK_DOWN] != 0;
  bool left_pressed = key_state[SDLK_LEFT] != 0;
  bool right_pressed = key_state[SDLK_RIGHT] != 0;

  if (!g_joystick && SDL_NumJoysticks() > 0) {
    g_joystick = SDL_JoystickOpen(0);
  }

  if (g_joystick) {
#if __EMSCRIPTEN__
    if (SDL_JoystickGetButton(g_joystick, WEB_INPUT_A)) {
      a_pressed = true;
    }

    if (SDL_JoystickGetButton(g_joystick, WEB_INPUT_B)) {
      b_pressed = true;
    }

    if (SDL_JoystickGetButton(g_joystick, WEB_INPUT_L)) {
      l_pressed = true;
    }

    if (SDL_JoystickGetButton(g_joystick, WEB_INPUT_R)) {
      r_pressed = true;
    }

    if (SDL_JoystickGetButton(g_joystick, WEB_INPUT_START)) {
      start_pressed = true;
    }

    if (SDL_JoystickGetButton(g_joystick, WEB_INPUT_SELECT)) {
      select_pressed = true;
    }

    bool update_dpad = false;
    if (SDL_JoystickGetButton(g_joystick, WEB_INPUT_UP)) {
      up_pressed = true;
      if (!update_dpad) {
        down_pressed = false;
        left_pressed = false;
        right_pressed = false;
        update_dpad = true;
      }
    }

    if (SDL_JoystickGetButton(g_joystick, WEB_INPUT_DOWN)) {
      down_pressed = true;
      if (!update_dpad) {
        up_pressed = false;
        left_pressed = false;
        right_pressed = false;
        update_dpad = true;
      }
    }

    if (SDL_JoystickGetButton(g_joystick, WEB_INPUT_LEFT)) {
      left_pressed = true;
      if (!update_dpad) {
        up_pressed = false;
        down_pressed = false;
        right_pressed = false;
        update_dpad = true;
      }
    }

    if (SDL_JoystickGetButton(g_joystick, WEB_INPUT_RIGHT)) {
      right_pressed = true;
      if (!update_dpad) {
        up_pressed = false;
        down_pressed = false;
        left_pressed = false;
        update_dpad = true;
      }
    }
#else
    if (SDL_JoystickGetButton(g_joystick, XINPUT_A)) {
      a_pressed = true;
    }

    if (SDL_JoystickGetButton(g_joystick, XINPUT_B)) {
      b_pressed = true;
    }

    if (SDL_JoystickGetButton(g_joystick, XINPUT_L)) {
      l_pressed = true;
    }

    if (SDL_JoystickGetButton(g_joystick, XINPUT_R)) {
      r_pressed = true;
    }

    if (SDL_JoystickGetButton(g_joystick, XINPUT_START)) {
      start_pressed = true;
    }

    if (SDL_JoystickGetButton(g_joystick, XINPUT_SELECT)) {
      select_pressed = true;
    }

    Uint8 hat_state = SDL_JoystickGetHat(g_joystick, 0);
    switch (hat_state) {
      case SDL_HAT_UP:
        up_pressed = true;
        down_pressed = false;
        left_pressed = false;
        right_pressed = false;
        break;
      case SDL_HAT_DOWN:
        up_pressed = false;
        down_pressed = true;
        left_pressed = false;
        right_pressed = false;
        break;
      case SDL_HAT_LEFT:
        up_pressed = false;
        down_pressed = false;
        left_pressed = true;
        right_pressed = false;
        break;
      case SDL_HAT_RIGHT:
        up_pressed = false;
        down_pressed = false;
        left_pressed = false;
        right_pressed = true;
        break;
      case SDL_HAT_RIGHTUP:
        up_pressed = true;
        down_pressed = false;
        left_pressed = false;
        right_pressed = true;
        break;
      case SDL_HAT_RIGHTDOWN:
        up_pressed = false;
        down_pressed = true;
        left_pressed = false;
        right_pressed = true;
        break;
      case SDL_HAT_LEFTUP:
        up_pressed = true;
        down_pressed = false;
        left_pressed = true;
        right_pressed = false;
        break;
      case SDL_HAT_LEFTDOWN:
        up_pressed = false;
        down_pressed = true;
        left_pressed = true;
        right_pressed = false;
        break;
    }
#endif  // __EMSCRIPTEN__
  }

  GamePadToggleA(g_gamepad, a_pressed);
  GamePadToggleB(g_gamepad, b_pressed);
  GamePadToggleL(g_gamepad, l_pressed);
  GamePadToggleR(g_gamepad, r_pressed);
  GamePadToggleStart(g_gamepad, start_pressed);
  GamePadToggleSelect(g_gamepad, select_pressed);
  GamePadToggleUp(g_gamepad, up_pressed);
  GamePadToggleDown(g_gamepad, down_pressed);
  GamePadToggleLeft(g_gamepad, left_pressed);
  GamePadToggleRight(g_gamepad, right_pressed);

  //
  // Run emulation
  //

  GbaEmulatorStep(g_emulator, /*fbo=*/g_fbo, /*scale_factor=*/1,
                  RenderAudioSample);

  //
  // Render result
  //

  glUseProgram(g_program);

  GLuint coord_attrib = glGetAttribLocation(g_program, "coord");
  glBindBuffer(GL_ARRAY_BUFFER, g_vertices);
  glVertexAttribPointer(coord_attrib, /*size=*/2, /*type=*/GL_FLOAT,
                        /*normalized=*/false, /*stride=*/0, /*pointer=*/NULL);
  glEnableVertexAttribArray(coord_attrib);

  GLint texture_location = glGetUniformLocation(g_program, "image");
  glUniform1i(texture_location, 0);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, g_fbo_texture);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, g_screen->w, g_screen->h);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4u);

  //
  // Flip framebuffer
  //

  SDL_GL_SwapBuffers();

#ifndef __EMSCRIPTEN__
  return true;
#endif  // __EMSCRIPTEN__
}

int main(int argc, char *argv[]) {
#ifndef __EMSCRIPTEN__
  if (argc < 2) {
    printf("usage: webgba <game>");
    return EXIT_SUCCESS;
  }
#endif  // __EMSCRIPTEN__

  //
  // Initialize SDL
  //

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) < 0) {
    fprintf(stderr, "ERROR: Failed to initialize SDL\n");
    return EXIT_FAILURE;
  }

  //
  // Load Game
  //

#ifdef __EMSCRIPTEN__
  FILE *file = fopen("/game.gba", "rb");
#else
  FILE *file = fopen(argv[1], "rb");
#endif  // __EMSCRIPTEN__

  if (file == NULL) {
    fprintf(stderr, "ERROR: Failed to load game file\n");
    SDL_Quit();
    return EXIT_FAILURE;
  }

  if (fseek(file, 0, SEEK_END) != 0) {
    fprintf(stderr, "ERROR: Failed to read game file\n");
    fclose(file);
    SDL_Quit();
    return EXIT_FAILURE;
  }

  long int game_size = ftell(file);
  if (game_size < 0) {
    fprintf(stderr, "ERROR: Failed to read game file\n");
    fclose(file);
    SDL_Quit();
    return EXIT_FAILURE;
  }

  if (fseek(file, 0, SEEK_SET) != 0) {
    fprintf(stderr, "ERROR: Failed to read game file\n");
    fclose(file);
    SDL_Quit();
    return EXIT_FAILURE;
  }

  void *game_data = malloc((size_t)game_size);
  if (game_data == NULL) {
    fprintf(stderr, "ERROR: Out of memory\n");
    fclose(file);
    SDL_Quit();
    return EXIT_FAILURE;
  }

  size_t objects_read = fread(game_data, game_size, /*count=*/1, file);
  fclose(file);

  if (objects_read != 1) {
    fprintf(stderr, "ERROR: Failed to read game file\n");
    free(game_data);
    SDL_Quit();
    return EXIT_FAILURE;
  }

  //
  // Create Emulator
  //

  bool success =
      GbaEmulatorAllocate(game_data, game_size, &g_emulator, &g_gamepad);

  free(game_data);

  if (!success) {
    fprintf(stderr, "ERROR: Out of memory\n");
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

#ifndef __EMSCRIPTEN__
  SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);
#endif  // __EMSCRIPTEN__

  SDL_WM_SetCaption("WebGBA", "game");
  if (!SetVideoMode(/*width=*/240, /*height=*/160)) {
    fprintf(stderr, "ERROR: Failed to create window\n");
    GbaEmulatorFree(g_emulator);
    GamePadFree(g_gamepad);
    SDL_Quit();
    return EXIT_FAILURE;
  }

  //
  // Configure Audio Device
  //

  SDL_AudioSpec want;

  memset(&want, 0, sizeof(want));
  want.channels = 2;
#if __EMSCRIPTEN__
  want.format = AUDIO_F32;
  want.freq = EM_ASM_INT({
    var AudioContext = window.AudioContext || window.webkitAudioContext;
    var ctx = new AudioContext();
    var sr = ctx.sampleRate;
    ctx.close();
    return sr;
  });
  want.samples = 256;
#else
  want.format = AUDIO_S16;
  want.freq = 131072.0 * 60.0 / (16777216.0 / 280896.0);
  want.samples = 512;
#endif  // __EMSCRIPTEN__
  want.callback = AudioCallback;

  SDL_AudioSpec have;
  if (SDL_OpenAudio(&want, &have) != 0) {
    fprintf(stderr, "ERROR: Failed to open audio device\n");
    GbaEmulatorFree(g_emulator);
    GamePadFree(g_gamepad);
    SDL_Quit();
    return EXIT_FAILURE;
  }

  g_src_ratio = (double)have.freq / (131072.0 * 60.0 / (16777216.0 / 280896.0));

  g_src_state = src_new(SRC_ZERO_ORDER_HOLD, /*channels=*/2, /*error=*/NULL);
  if (g_src_state == NULL) {
    fprintf(stderr, "ERROR: Failed to open allocate resampler\n");
    SDL_CloseAudio();
    GbaEmulatorFree(g_emulator);
    GamePadFree(g_gamepad);
    SDL_Quit();
    return EXIT_FAILURE;
  }

  SDL_PauseAudio(/*pause_on=*/0);

  //
  // Enable Joystick If Available
  //

  SDL_JoystickEventState(SDL_ENABLE);
  if (SDL_NumJoysticks() > 0) {
    g_joystick = SDL_JoystickOpen(0);
  }

  //
  // Create Emulator Framebuffer
  //

  glGenTextures(/*n=*/1, &g_fbo_texture);
  glBindTexture(GL_TEXTURE_2D, g_fbo_texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, /*level=*/0, /*internal_format=*/GL_RGBA,
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

  static const char *vertex_shader_source[8u] = {
      "#version 100\n",
      "attribute highp vec2 coord;\n",
      "varying mediump vec2 texcoord;\n",
      "void main() {\n",
      "  texcoord.x = (coord.x + 1.0) * 0.5;\n",
      "  texcoord.y = (coord.y + 1.0) * 0.5;\n",
      "  gl_Position = vec4(coord, 0.0, 1.0);\n",
      "}\n",
  };

  GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 8u, vertex_shader_source, NULL);
  glCompileShader(vertex_shader);
  glAttachShader(g_program, vertex_shader);
  glDeleteShader(vertex_shader);

  static const char *fragment_shader_source[7u] = {
      "#version 100\n",
      "uniform lowp sampler2D image;\n",
      "varying mediump vec2 texcoord;\n",
      "void main() {\n",
      "  lowp vec4 color = texture2D(image, texcoord);\n",
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
  // Create Vertices
  //

  static const GLfloat vertices[8u] = {-1.0, -1.0, -1.0, 1.0,
                                       1.0,  1.0,  1.0,  -1.0};

  glGenBuffers(1, &g_vertices);
  glBindBuffer(GL_ARRAY_BUFFER, g_vertices);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  //
  // Load Emulator OpenGL Context
  //

  GbaEmulatorReloadContext(g_emulator);

  //
  // Run
  //

#if __EMSCRIPTEN__
  emscripten_set_main_loop(RenderNextFrame, /*fps=*/0,
                           /*simulate_infinite_loop=*/true);
#else
  bool running = true;
  while (running) {
    running = RenderNextFrame();
  }
#endif  // __EMSCRIPTEN__

  //
  // Cleanup
  //

  if (g_joystick) {
    SDL_JoystickClose(g_joystick);
  }

  src_delete(g_src_state);
  SDL_CloseAudio();

  GbaEmulatorFree(g_emulator);
  GamePadFree(g_gamepad);

  SDL_Quit();

  return EXIT_SUCCESS;
}
