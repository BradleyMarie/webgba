#ifndef _WEBGBA_EMULATOR_GBA_
#define _WEBGBA_EMULATOR_GBA_

#include <GLES2/gl2.h>
#include <stdint.h>

#include "emulator/peripherals/gamepad.h"

typedef struct _GbaEmulator GbaEmulator;

// TODO: Make ROMs reloadable without tearing down emulator.
bool GbaEmulatorAllocate(const unsigned char *rom_data, uint32_t rom_size,
                         GbaEmulator **emulator, GamePad **gamepad);

// Callback type for one sample's worth of audio data
typedef void (*GbaEmulatorRenderAudioSample)(int16_t left, int16_t right);

// Advance emulation by one frame
void GbaEmulatorStep(GbaEmulator *emulator, GLuint fbo, GLsizei width,
                     GLsizei height,
                     GbaEmulatorRenderAudioSample audio_sample_callback);

// Context Loss Recovery
void GbaEmulatorReloadContext(GbaEmulator *emulator);

void GbaEmulatorFree(GbaEmulator *emulator);

#endif  // _WEBGBA_EMULATOR_GBA_