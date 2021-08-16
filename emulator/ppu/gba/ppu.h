#ifndef _WEBGBA_EMULATOR_PPU_GBA_PPU_
#define _WEBGBA_EMULATOR_PPU_GBA_PPU_

#include <GLES2/gl2.h>

#include "emulator/memory/memory.h"
#include "emulator/platform/gba/platform.h"

typedef struct _GbaPpu GbaPpu;

// General PPU routines
bool GbaPpuAllocate(GbaPlatform *platform, GbaPpu **ppu, Memory **pram,
                    Memory **vram, Memory **oam, Memory **registers);

void GbaPpuStep(GbaPpu *ppu);

void GbaPpuFree(GbaPpu *ppu);

// Render Output Configuration
typedef void (*PpuRenderDoneFunction)(uint32_t width, uint32_t height);
void GbaPpuSetRenderOutput(GbaPpu *ppu, GLuint framebuffer);
void GbaPpuSetRenderScale(GbaPpu *ppu, uint8_t scale_factor);
void GbaPpuSetRenderDoneCallback(GbaPpu *ppu, PpuRenderDoneFunction frame_done);

// Context Loss Recovery
void GbaPpuReloadContext(GbaPpu *ppu);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_PPU_