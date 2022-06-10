#ifndef _WEBGBA_EMULATOR_SCREEN_
#define _WEBGBA_EMULATOR_SCREEN_

#include <GLES3/gl3.h>
#include <stdint.h>

typedef struct _Screen Screen;

Screen *ScreenAllocate();

void ScreenAttachFramebuffer(Screen *screen, GLuint framebuffer, GLsizei width,
                             GLsizei height);

uint16_t *ScreenGetPixelBuffer(Screen *screen, GLsizei width, GLsizei height);

GLuint ScreenGetRenderBuffer(Screen *screen, GLsizei width, GLsizei height);

void ScreenClear(const Screen *screen);

void ScreenRenderToFramebuffer(const Screen *screen);

void ScreenReloadContext(Screen *screen);

void ScreenFree(Screen *screen);

#endif  // _WEBGBA_EMULATOR_SCREEN_