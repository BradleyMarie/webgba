#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

#include "emulator/gba.h"

static SDL_GameController *g_gamecontroller = NULL;
static SDL_Window *g_window = NULL;
static SDL_GLContext *g_glcontext = NULL;
static SDL_AudioDeviceID g_audiodevice = 0;
static GbaEmulator *g_emulator = NULL;
static Screen *g_screen = NULL;
static GamePad *g_gamepad = NULL;
static GbaGraphicsRenderOptions g_render_options = {
    GBA_RENDERER_SCANLINES_SOFTWARE, 1u};

bool g_accept_equals = true;
bool g_accept_minus = true;
bool g_accept_g = true;
bool g_accept_zero = true;

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
  bool a_pressed = keyboard_state[SDL_SCANCODE_X] != 0;
  bool b_pressed = keyboard_state[SDL_SCANCODE_Z] != 0;
  bool l_pressed = keyboard_state[SDL_SCANCODE_A] != 0;
  bool r_pressed = keyboard_state[SDL_SCANCODE_S] != 0;
  bool start_pressed = keyboard_state[SDL_SCANCODE_RETURN] != 0;
  bool select_pressed = keyboard_state[SDL_SCANCODE_BACKSPACE] != 0;
  bool up_pressed = keyboard_state[SDL_SCANCODE_UP] != 0;
  bool down_pressed = keyboard_state[SDL_SCANCODE_DOWN] != 0;
  bool left_pressed = keyboard_state[SDL_SCANCODE_LEFT] != 0;
  bool right_pressed = keyboard_state[SDL_SCANCODE_RIGHT] != 0;

  // Render Control
  bool equals_pressed = keyboard_state[SDL_SCANCODE_EQUALS] != 0;
  bool minus_pressed = keyboard_state[SDL_SCANCODE_MINUS] != 0;
  bool g_pressed = keyboard_state[SDL_SCANCODE_G] != 0;
  bool zero_pressed = keyboard_state[SDL_SCANCODE_0] != 0;

  if (!equals_pressed) {
    g_accept_equals = true;
  } else if (g_accept_equals) {
    if (g_render_options.renderer == GBA_RENDERER_SCANLINES_OPENGL) {
      g_render_options.opengl_render_scale += 1u;
      if (g_render_options.opengl_render_scale > 16u) {
        g_render_options.opengl_render_scale = 16u;
      }
      g_accept_equals = false;
    }
  }

  if (!minus_pressed) {
    g_accept_minus = true;
  } else if (g_accept_minus) {
    if (g_render_options.renderer == GBA_RENDERER_SCANLINES_OPENGL) {
      g_render_options.opengl_render_scale -= 1u;
      if (g_render_options.opengl_render_scale == 0u) {
        g_render_options.opengl_render_scale = 1u;
      }
      g_accept_minus = false;
    }
  }

  if (!g_pressed) {
    g_accept_g = true;
  } else if (g_accept_g) {
    switch (g_render_options.renderer) {
      case GBA_RENDERER_PIXELS_SOFTWARE:
        g_render_options.renderer = GBA_RENDERER_SCANLINES_SOFTWARE;
        break;
      case GBA_RENDERER_SCANLINES_SOFTWARE:
        g_render_options.renderer = GBA_RENDERER_SCANLINES_OPENGL;
        break;
      case GBA_RENDERER_SCANLINES_OPENGL:
        g_render_options.renderer = GBA_RENDERER_PIXELS_SOFTWARE;
        break;
    }
    g_accept_g = false;
  }

  if (!zero_pressed) {
    g_accept_zero = true;
  } else if (g_accept_zero) {
    g_render_options.renderer = GBA_RENDERER_SCANLINES_SOFTWARE;
    g_render_options.opengl_render_scale = 1u;
    g_accept_zero = false;
  }

  if (!g_gamecontroller && SDL_NumJoysticks() > 0) {
    for (int i = 0; i < SDL_NumJoysticks(); ++i) {
      if (SDL_IsGameController(i)) {
        g_gamecontroller = SDL_GameControllerOpen(i);
        break;
      }
    }
  }

  if (g_gamecontroller) {
    if (SDL_GameControllerGetButton(g_gamecontroller,
                                    SDL_CONTROLLER_BUTTON_A)) {
      a_pressed = true;
    }

    if (SDL_GameControllerGetButton(g_gamecontroller,
                                    SDL_CONTROLLER_BUTTON_B)) {
      b_pressed = true;
    }

    if (SDL_GameControllerGetButton(g_gamecontroller,
                                    SDL_CONTROLLER_BUTTON_LEFTSHOULDER)) {
      l_pressed = true;
    }

    if (SDL_GameControllerGetButton(g_gamecontroller,
                                    SDL_CONTROLLER_BUTTON_RIGHTSHOULDER)) {
      r_pressed = true;
    }

    if (SDL_GameControllerGetButton(g_gamecontroller,
                                    SDL_CONTROLLER_BUTTON_START)) {
      start_pressed = true;
    }

    if (SDL_GameControllerGetButton(g_gamecontroller,
                                    SDL_CONTROLLER_BUTTON_BACK)) {
      select_pressed = true;
    }

    bool update_dpad = false;
    if (SDL_GameControllerGetButton(g_gamecontroller,
                                    SDL_CONTROLLER_BUTTON_DPAD_UP)) {
      up_pressed = true;
      if (!update_dpad) {
        down_pressed = false;
        left_pressed = false;
        right_pressed = false;
        update_dpad = true;
      }
    }

    if (SDL_GameControllerGetButton(g_gamecontroller,
                                    SDL_CONTROLLER_BUTTON_DPAD_DOWN)) {
      down_pressed = true;
      if (!update_dpad) {
        up_pressed = false;
        left_pressed = false;
        right_pressed = false;
        update_dpad = true;
      }
    }

    if (SDL_GameControllerGetButton(g_gamecontroller,
                                    SDL_CONTROLLER_BUTTON_DPAD_LEFT)) {
      left_pressed = true;
      if (!update_dpad) {
        up_pressed = false;
        down_pressed = false;
        right_pressed = false;
        update_dpad = true;
      }
    }

    if (SDL_GameControllerGetButton(g_gamecontroller,
                                    SDL_CONTROLLER_BUTTON_DPAD_RIGHT)) {
      right_pressed = true;
      if (!update_dpad) {
        up_pressed = false;
        down_pressed = false;
        left_pressed = false;
        update_dpad = true;
      }
    }
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
  // Get screen dimensions
  //

  int width, height;
  SDL_GetWindowSize(g_window, &width, &height);

  //
  // Run emulation
  //

  ScreenAttachFramebuffer(g_screen, /*fbo=*/0u, /*width=*/width,
                          /*height=*/height);

  GbaEmulatorStep(g_emulator, g_screen, &g_render_options, RenderAudioSample);

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

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER |
               SDL_INIT_EVENTS) < 0) {
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

  if ((uint64_t)size > SIZE_MAX) {
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
  // Create Screen
  //

  g_screen = ScreenAllocate();
  if (!g_screen) {
    GbaEmulatorFree(g_emulator);
    GamePadFree(g_gamepad);
    fprintf(stderr, "ERROR: Out of memory\n");
    SDL_Quit();
    return false;
  }

  //
  // Create Window
  //

  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
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
  // Enable Joystick If Available
  //

  SDL_JoystickEventState(SDL_ENABLE);
  if (SDL_NumJoysticks() > 0) {
    for (int i = 0; i < SDL_NumJoysticks(); ++i) {
      if (SDL_IsGameController(i)) {
        g_gamecontroller = SDL_GameControllerOpen(i);
        break;
      }
    }
  }

  //
  // Load emulator OpenGL context
  //

  GbaEmulatorReloadContext(g_emulator);
  ScreenReloadContext(g_screen);

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

  if (g_gamecontroller) {
    SDL_GameControllerClose(g_gamecontroller);
  }

  SDL_CloseAudioDevice(g_audiodevice);
  SDL_DestroyWindow(g_window);
  SDL_GL_DeleteContext(g_glcontext);

  GbaEmulatorFree(g_emulator);
  GamePadFree(g_gamepad);
  ScreenFree(g_screen);

  SDL_Quit();

  return EXIT_SUCCESS;
}
