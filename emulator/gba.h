#ifndef _WEBGBA_EMULATOR_GBA_
#define _WEBGBA_EMULATOR_GBA_

#include <GLES2/gl2.h>
#include <stdint.h>

#include "emulator/peripherals/gamepad.h"

typedef struct _GbaEmulator GbaEmulator;

// TODO: Make ROMs reloadable without tearing down emulator.
bool GbaEmulatorAllocate(const char *rom_data, uint32_t rom_size,
                         GbaEmulator **emulator, GamePad **gamepad);
void GbaEmulatorFree(GbaEmulator *emulator);

void GbaEmulatorStep(GbaEmulator *emulator);

// Render Output Management
typedef void (*GbaEmulatorRenderDoneFunction)(uint32_t width, uint32_t height);
void GbaEmulatorSetRenderOutput(GbaEmulator *emulator, GLuint framebuffer);
void GbaEmulatorSetRenderScale(GbaEmulator *emulator, uint8_t scale_factor);
void GbaEmulatorSetRenderDoneCallback(GbaEmulator *emulator,
                                      GbaEmulatorRenderDoneFunction frame_done);

// Context Loss Recovery
void GbaEmulatorReloadContext(GbaEmulator *emulator);

#endif  // _WEBGBA_EMULATOR_GBA_