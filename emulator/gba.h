#ifndef _WEBGBA_EMULATOR_GBA_
#define _WEBGBA_EMULATOR_GBA_

#include <GLES2/gl2.h>
#include <stdint.h>

#include "emulator/peripherals/gamepad.h"

typedef struct _GbaEmulator GbaEmulator;

// TODO: Make ROMs reloadable without tearing down emulator.
bool GbaEmulatorAllocate(const char *rom_data, uint32_t rom_size,
                         GbaEmulator **emulator, GamePad **gamepad);

typedef void (*GbaFrameDoneFunction)(unsigned width, unsigned height);
void GbaEmulatorStep(GbaEmulator *emulator, GLuint framebuffer,
                     GbaFrameDoneFunction done_function);

void GbaEmulatorFree(GbaEmulator *timers);

#endif  // _WEBGBA_EMULATOR_GBA_