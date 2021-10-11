#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "emulator/gba.h"
#include "third_party/libretro/libretro.h"

#define BASE_WIDTH 240u
#define BASE_HEIGHT 160u
#define MAX_WIDTH 3120u
#define MAX_HEIGHT 2160u

static struct retro_hw_render_callback hw_render;
static retro_video_refresh_t video_cb;
static retro_audio_sample_t audio_cb;
static retro_audio_sample_batch_t audio_batch_cb;
static retro_environment_t environ_cb;
static retro_input_poll_t input_poll_cb;
static retro_input_state_t input_state_cb;

static GbaEmulator *emulator;
static GamePad *gamepad;
static uint8_t render_scale = 1u;

static void UpdateVariables() {
  struct retro_variable var;
  var.key = "webgba_resolution";

  if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value != NULL) {
    char *pch;
    char str[100];
    strncpy(str, var.value, sizeof(str) - 1);
    str[99] = '\0';

    pch = strtok(str, "x");
    if (pch != NULL) {
      render_scale = strtoul(pch, NULL, 0) / BASE_WIDTH;
    }
  }
}

static void ContextReset() { GbaEmulatorReloadContext(emulator); }

static void PollInput() {
  input_poll_cb();

  GamePadToggleUp(gamepad, input_state_cb(0, RETRO_DEVICE_JOYPAD, 0,
                                          RETRO_DEVICE_ID_JOYPAD_UP));
  GamePadToggleDown(gamepad, input_state_cb(0, RETRO_DEVICE_JOYPAD, 0,
                                            RETRO_DEVICE_ID_JOYPAD_DOWN));
  GamePadToggleLeft(gamepad, input_state_cb(0, RETRO_DEVICE_JOYPAD, 0,
                                            RETRO_DEVICE_ID_JOYPAD_LEFT));
  GamePadToggleRight(gamepad, input_state_cb(0, RETRO_DEVICE_JOYPAD, 0,
                                             RETRO_DEVICE_ID_JOYPAD_RIGHT));
  GamePadToggleA(gamepad, input_state_cb(0, RETRO_DEVICE_JOYPAD, 0,
                                         RETRO_DEVICE_ID_JOYPAD_A));
  GamePadToggleB(gamepad, input_state_cb(0, RETRO_DEVICE_JOYPAD, 0,
                                         RETRO_DEVICE_ID_JOYPAD_B));
  GamePadToggleL(gamepad, input_state_cb(0, RETRO_DEVICE_JOYPAD, 0,
                                         RETRO_DEVICE_ID_JOYPAD_L));
  GamePadToggleR(gamepad, input_state_cb(0, RETRO_DEVICE_JOYPAD, 0,
                                         RETRO_DEVICE_ID_JOYPAD_R));
  GamePadToggleStart(gamepad, input_state_cb(0, RETRO_DEVICE_JOYPAD, 0,
                                             RETRO_DEVICE_ID_JOYPAD_START));
  GamePadToggleSelect(gamepad, input_state_cb(0, RETRO_DEVICE_JOYPAD, 0,
                                              RETRO_DEVICE_ID_JOYPAD_SELECT));
}

static void FrameDoneCallback(uint32_t width, uint32_t height) {
  video_cb(RETRO_HW_FRAME_BUFFER_VALID, width, height, 0);
}

void retro_init() {}

void retro_deinit() {}

unsigned retro_api_version() { return RETRO_API_VERSION; }

void retro_set_controller_port_device(unsigned port, unsigned device) {}

void retro_get_system_info(struct retro_system_info *info) {
  memset(info, 0, sizeof(struct retro_system_info));

  info->library_name = "WebGBA";
  info->library_version = "v0.1";
  info->need_fullpath = false;
  info->valid_extensions = "gba";
}

void retro_get_system_av_info(struct retro_system_av_info *info) {
  memset(info, 0, sizeof(struct retro_system_av_info));

  info->timing.fps = 16777216.0 / 280896.0;
  info->timing.sample_rate = 32768.0;
  info->geometry.base_width = BASE_WIDTH;
  info->geometry.base_height = BASE_HEIGHT;
  info->geometry.max_width = MAX_WIDTH;
  info->geometry.max_height = MAX_HEIGHT;
  info->geometry.aspect_ratio = (double)BASE_WIDTH / (double)BASE_HEIGHT;
}

void retro_set_environment(retro_environment_t cb) {
  environ_cb = cb;

  struct retro_variable variables[] = {
      {"webgba_resolution",
       "Output Resolution; "
       "240x160|"
       "480x320|"
       "720x480|"
       "960x800|"
       "1200x960|"
       "1440x1120|"
       "1920x1280|"
       "2160x1440|"
       "2400x1600|"
       "2640x1760|"
       "2880x1920|"
       "3120x2080|"
       "3360x2240|"
       "3600x2400"},
      {NULL, NULL},
  };

  cb(RETRO_ENVIRONMENT_SET_VARIABLES, variables);
}

void retro_set_audio_sample(retro_audio_sample_t cb) { audio_cb = cb; }

void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb) {
  audio_batch_cb = cb;
}

void retro_set_input_poll(retro_input_poll_t cb) { input_poll_cb = cb; }

void retro_set_input_state(retro_input_state_t cb) { input_state_cb = cb; }

void retro_set_video_refresh(retro_video_refresh_t cb) { video_cb = cb; }

void retro_run() {
  PollInput();

  bool updated = false;
  if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE, &updated) && updated) {
    UpdateVariables();
  }

  GbaEmulatorSetRenderAudioSample(emulator, audio_cb);
  GbaEmulatorSetRenderOutput(emulator, hw_render.get_current_framebuffer());
  GbaEmulatorSetRenderScale(emulator, render_scale);
  GbaEmulatorSetRenderDoneCallback(emulator, FrameDoneCallback);
  for (uint32_t i = 0u; i < 280896u; i++) {
    GbaEmulatorStep(emulator);
  }
}

static bool retro_init_hw_context() {
  memset(&hw_render, 0, sizeof(struct retro_hw_render_callback));

  hw_render.context_type = RETRO_HW_CONTEXT_OPENGL_CORE;
  hw_render.version_major = 3;
  hw_render.version_minor = 1;
  hw_render.context_reset = ContextReset;
  hw_render.context_destroy = NULL;
  hw_render.depth = true;
  hw_render.stencil = true;
  hw_render.bottom_left_origin = true;

  if (!environ_cb(RETRO_ENVIRONMENT_SET_HW_RENDER, &hw_render)) {
    return false;
  }

  return true;
}

bool retro_load_game(const struct retro_game_info *info) {
  bool success =
      GbaEmulatorAllocate(info->data, info->size, &emulator, &gamepad);
  if (!success) {
    return false;
  }

  UpdateVariables();

  enum retro_pixel_format fmt = RETRO_PIXEL_FORMAT_RGB565;
  if (!environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &fmt)) {
    return false;
  }

  if (!retro_init_hw_context()) {
    return false;
  }

  return true;
}

void retro_unload_game() {
  GamePadFree(gamepad);
  GbaEmulatorFree(emulator);
}

unsigned retro_get_region() { return RETRO_REGION_NTSC; }

bool retro_load_game_special(unsigned type, const struct retro_game_info *info,
                             size_t num) {
  return false;
}

size_t retro_serialize_size() { return 0; }

bool retro_serialize(void *data, size_t size) { return false; }

bool retro_unserialize(const void *data, size_t size) { return false; }

void *retro_get_memory_data(unsigned id) { return NULL; }

size_t retro_get_memory_size(unsigned id) { return 0; }

void retro_reset() {}

void retro_cheat_reset() {}

void retro_cheat_set(unsigned index, bool enabled, const char *code) {}