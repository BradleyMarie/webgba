#ifndef _WEBGBA_EMULATOR_PPU_GBA_PPU_COPY_TO_FBO_
#define _WEBGBA_EMULATOR_PPU_GBA_PPU_COPY_TO_FBO_

#include <GLES2/gl2.h>

#include "emulator/ppu/gba/types.h"

typedef struct {
  GLuint program;
  GLuint texture;
} GbaPpuCopyToFboResources;

void GbaPpuCopyFramebufferToFbo(const GbaPpuCopyToFboResources* resources,
                                const GbaPpuFrameBuffer* framebuffer,
                                GLuint fbo);

void GbaPpuCopyToFboReloadContext(GbaPpuCopyToFboResources* resources);

void GbaPpuCopyToFboDestroy(GbaPpuCopyToFboResources* resources);

#endif  // _WEBGBA_EMULATOR_PPU_GBA_PPU_COPY_TO_FBO_