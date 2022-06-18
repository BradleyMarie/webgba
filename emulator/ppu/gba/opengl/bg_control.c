#include "emulator/ppu/gba/opengl/bg_control.h"

#include <assert.h>

#include "emulator/ppu/gba/opengl/texture_bindings.h"

bool OpenGlBgControlLoad(OpenGlBgControl* context,
                         const GbaPpuRegisters* registers,
                         GbaPpuDirtyBits* dirty_bits) {
  if (!context->dirty) {
    context->dirty_start = registers->vcount;
  }

  bool row_dirty = false;
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

    GLuint new_value = registers->bgcnt[i].value;

    if (registers->bgcnt[i].mosaic) {
      new_value |= (registers->mosaic.bg_horiz + 1u) << 16u;
      new_value |= (registers->mosaic.bg_vert + 1u) << 24u;
    } else {
      new_value |= 1u << 16u;
      new_value |= 1u << 24u;
    }

    if (new_value != context->staging[registers->vcount][i]) {
      context->staging[registers->vcount][i] = new_value;
      row_dirty = true;
    }
  }

  if (row_dirty) {
    context->dirty_end = registers->vcount;
    context->dirty = true;
  }

  return context->dirty;
}

void OpenGlBgControlBind(OpenGlBgControl* context, GLuint program) {
  GLint backgrounds = glGetUniformBlockIndex(program, "Backgrounds");
  glUniformBlockBinding(program, backgrounds, BACKGROUNDS_BUFFER);

  glBindBuffer(GL_UNIFORM_BUFFER, context->buffer);
  glBindBufferBase(GL_UNIFORM_BUFFER, BACKGROUNDS_BUFFER, context->buffer);

  if (context->dirty) {
    glBufferSubData(GL_UNIFORM_BUFFER,
                    /*offset=*/sizeof(GLuint) * 4u * context->dirty_start,
                    /*size=*/sizeof(GLuint) * 4u *
                        (context->dirty_end - context->dirty_start + 1u),
                    /*data=*/&context->staging[context->dirty_start]);
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