#include "emulator/ppu/gba/opengl/bg_control.h"

#include <assert.h>

#include "emulator/ppu/gba/opengl/texture_bindings.h"

bool OpenGlBgControlLoad(OpenGlBgControl* context,
                         const GbaPpuRegisters* registers,
                         GbaPpuDirtyBits* dirty_bits) {
  for (uint8_t i = 0u; i < GBA_PPU_NUM_BACKGROUNDS; i++) {
    if (i == 0u &&
        (!registers->dispcnt.bg0_enable || registers->dispcnt.mode > 1u)) {
      continue;
    }

    if (i == 1u &&
        (!registers->dispcnt.bg1_enable || registers->dispcnt.mode > 1u)) {
      continue;
    }

    if (i == 2u && !registers->dispcnt.bg2_enable) {
      continue;
    }

    if (i == 3u &&
        (!registers->dispcnt.bg3_enable ||
         (registers->dispcnt.mode != 0u && registers->dispcnt.mode != 2u))) {
      continue;
    }

    GLuint value = registers->bgcnt[i].value;

    if (registers->bgcnt[i].mosaic) {
      value |= (registers->mosaic.bg_horiz + 1u) << 16u;
      value |= (registers->mosaic.bg_vert + 1u) << 24u;
    } else {
      value |= 1u << 16u;
      value |= 1u << 24u;
    }

    if (value != context->staging[registers->vcount][i]) {
      context->dirty = true;
    }

    context->staging[registers->vcount][i] = value;
  }

  return context->dirty;
}

void OpenGlBgControlBind(OpenGlBgControl* context, GLint start, GLint end,
                         GLuint program) {
  GLint backgrounds = glGetUniformBlockIndex(program, "Backgrounds");
  glUniformBlockBinding(program, backgrounds, BACKGROUNDS_BUFFER);

  glBindBuffer(GL_UNIFORM_BUFFER, context->buffer);
  glBindBufferBase(GL_UNIFORM_BUFFER, BACKGROUNDS_BUFFER, context->buffer);

  if (context->dirty) {
    glBufferSubData(GL_UNIFORM_BUFFER, /*offset=*/sizeof(GLuint) * 4u * start,
                    /*size=*/sizeof(GLuint) * 4u * (end - start),
                    /*data=*/&context->staging[start]);
    context->dirty = false;
  }

  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void OpenGlBgControlReloadContext(OpenGlBgControl* context) {
  glGenBuffers(1, &context->buffer);
  glBindBuffer(GL_UNIFORM_BUFFER, context->buffer);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(context->staging), context->staging,
               GL_DYNAMIC_DRAW);

  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void OpenGlBgControlDestroy(OpenGlBgControl* context) {
  glDeleteBuffers(1, &context->buffer);
}