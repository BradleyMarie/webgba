#ifndef _WEBGBA_EMULATOR_GBA_
#define _WEBGBA_EMULATOR_GBA_

#include <GLES3/gl3.h>
#include <stdint.h>

#include "emulator/peripherals/gamepad.h"
#include "emulator/screen.h"

typedef struct _GbaEmulator GbaEmulator;

// TODO: Make ROMs reloadable without tearing down emulator.
bool GbaEmulatorAllocate(const unsigned char *rom_data, uint32_t rom_size,
                         GbaEmulator **emulator, GamePad **gamepad);

// Callback type for one sample's worth of audio data
typedef void (*GbaEmulatorRenderAudioSample)(int16_t left, int16_t right);

typedef enum {
  GBA_RENDERER_SCANLINES_SOFTWARE,
  GBA_RENDERER_SCANLINES_OPENGL,
  GBA_RENDERER_PIXELS_SOFTWARE,
} GbaGraphicsRenderer;

typedef struct {
  GbaGraphicsRenderer renderer;
  uint8_t opengl_render_scale;
} GbaGraphicsRenderOptions;

// Advance emulation by one frame
void GbaEmulatorStep(GbaEmulator *emulator, Screen *screen,
                     const GbaGraphicsRenderOptions *graphics_renderer,
                     GbaEmulatorRenderAudioSample audio_sample_callback);

// Context Loss Recovery
void GbaEmulatorReloadContext(GbaEmulator *emulator);

void GbaEmulatorFree(GbaEmulator *emulator);

#endif  // _WEBGBA_EMULATOR_GBA_