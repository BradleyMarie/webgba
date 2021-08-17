#include "emulator/ppu/gba/ppu.h"

#include <assert.h>
#include <stdlib.h>

#include "emulator/ppu/gba/oam/oam.h"
#include "emulator/ppu/gba/palette/palette.h"
#include "emulator/ppu/gba/types.h"
#include "emulator/ppu/gba/vram/vram.h"

struct _GbaPpu {
  GbaPlatform *platform;
  GbaPpuMemory memory;
  GbaPpuFrameBuffer framebuffer;
  union {
    GbaPpuRegisters registers;
    uint16_t register_half_words[44];
  };
  GbaPpuInternalRegisters internal_registers;

  PpuRenderDoneFunction frame_done;
  uint32_t width;
  uint32_t height;

  // Stand ins for a real implementation
  GLuint fbo;
  GLuint program;
  GLuint vbo;
  uint64_t frame_count;
};

static bool GbaPpuRegistersLoad16(const void *context, uint32_t address,
                                  uint16_t *value) {
  assert((address & 0x1u) == 0u);

  const GbaPpu *ppu = (const GbaPpu *)context;

  switch (address) {
    case DISPCNT_OFFSET:
      *value = ppu->registers.dispcnt.value;
      return true;
    case GREENSWP_OFFSET:
      *value = ppu->registers.greenswp;
      return true;
    case DISPSTAT_OFFSET:
      *value = ppu->registers.dispstat.value;
      return true;
    case VCOUNT_OFFSET:
      *value = ppu->registers.vcount;
      return true;
    case BG0CNT_OFFSET:
      *value = ppu->registers.bg0cnt.value;
      return true;
    case BG1CNT_OFFSET:
      *value = ppu->registers.bg1cnt.value;
      return true;
    case BG2CNT_OFFSET:
      *value = ppu->registers.bg2cnt.value;
      return true;
    case BG3CNT_OFFSET:
      *value = ppu->registers.bg3cnt.value;
      return true;
    case WININ_OFFSET:
      *value = ppu->registers.winin;
      return true;
    case WINOUT_OFFSET:
      *value = ppu->registers.winout;
      return true;
    case BLDCNT_OFFSET:
      *value = ppu->registers.bldcnt;
      return true;
  }

  return false;
}

static bool GbaPpuRegistersLoad32(const void *context, uint32_t address,
                                  uint32_t *value) {
  assert((address & 0x3u) == 0u);

  uint16_t low_bits;
  bool low = GbaPpuRegistersLoad16(context, address, &low_bits);
  if (!low) {
    return false;
  }

  uint16_t high_bits;
  bool high = GbaPpuRegistersLoad16(context, address + 2u, &high_bits);
  if (high) {
    *value = (((uint32_t)high_bits) << 16u) | (uint32_t)low_bits;
  } else {
    *value = 0u;
  }

  return true;
}

static bool GbaPpuRegistersLoad8(const void *context, uint32_t address,
                                 uint8_t *value) {
  uint32_t read_address = address & 0xFFFFFFFEu;

  uint16_t value16;
  bool success = GbaPpuRegistersLoad16(context, read_address, &value16);
  if (success) {
    *value = (address == read_address) ? value16 : value16 >> 8u;
  }

  return success;
}

static bool GbaPpuRegistersStore16(void *context, uint32_t address,
                                   uint16_t value) {
  assert((address & 0x1u) == 0u);

  if (address >= GBA_PPU_REGISTERS_SIZE) {
    return false;
  }

  GbaPpu *ppu = (GbaPpu *)context;

  // If address equals VCOUNT_OFFSET, we are attempting to write to a read-only
  // register. In this case, ignore the write and leave the register unmodified.
  if (address == VCOUNT_OFFSET) {
    return true;
  }

  // If address equals DISPSTAT_OFFSET, and any of the lower 3 bits of value are
  // set, we are attempting to modify read-only bits in the DISPSTAT register.
  // In this case, ignore mask out those bits in value so they are not modified.
  if (address == DISPSTAT_OFFSET) {
    value &= 0xFFF8u;
  }

  ppu->register_half_words[address >> 1u] = value;

  switch (address) {
    case BG2X_OFFSET:
    case BG2X_OFFSET_HI:
      ppu->internal_registers.bg2_x_row_start = ppu->registers.bg2x;
      ppu->internal_registers.bg2_x = ppu->registers.bg2x;
      break;
    case BG2Y_OFFSET:
    case BG2Y_OFFSET_HI:
      ppu->internal_registers.bg2_y_row_start = ppu->registers.bg2y;
      ppu->internal_registers.bg2_y = ppu->registers.bg2y;
      break;
  }

  return true;
}

static bool GbaPpuRegistersStore32(void *context, uint32_t address,
                                   uint32_t value) {
  GbaPpuRegistersStore16(context, address, value);
  GbaPpuRegistersStore16(context, address + 2u, value >> 16u);
  return true;
}

static bool GbaPpuRegistersStore8(void *context, uint32_t address,
                                  uint8_t value) {
  GbaPpu *ppu = (GbaPpu *)context;

  uint32_t read_address = address & 0xFFFFFFFEu;
  uint16_t value16 = ppu->register_half_words[read_address >> 1u];
  if (address == read_address) {
    value16 &= 0xFF00;
    value16 |= value;
  } else {
    value16 &= 0x00FF;
    value16 |= (uint16_t)value << 8u;
  }

  GbaPpuRegistersStore16(context, read_address, value16);

  return true;
}

void GbaPpuRegistersFree(void *context) {
  GbaPpu *ppu = (GbaPpu *)context;
  GbaPpuFree(ppu);
}

bool GbaPpuAllocate(GbaPlatform *platform, GbaPpu **ppu, Memory **palette,
                    Memory **vram, Memory **oam, Memory **registers) {
  *ppu = (GbaPpu *)calloc(1, sizeof(GbaPpu));
  if (*ppu == NULL) {
    return false;
  }

  (*ppu)->memory.free_address = *ppu;
  (*ppu)->memory.reference_count = 1u;

  *palette = PaletteAllocate(&(*ppu)->memory);
  if (*palette == NULL) {
    free(*ppu);
    return false;
  }

  *vram = VRamAllocate(&(*ppu)->memory);
  if (*vram == NULL) {
    MemoryFree(*palette);
    free(*ppu);
    return false;
  }

  *oam = PaletteAllocate(&(*ppu)->memory);
  if (*oam == NULL) {
    MemoryFree(*vram);
    MemoryFree(*palette);
    free(*ppu);
    return false;
  }

  *registers = MemoryAllocate(*ppu, GbaPpuRegistersLoad32,
                              GbaPpuRegistersLoad16, GbaPpuRegistersLoad8,
                              GbaPpuRegistersStore32, GbaPpuRegistersStore16,
                              GbaPpuRegistersStore8, GbaPpuRegistersFree);
  if (*registers == NULL) {
    MemoryFree(*oam);
    MemoryFree(*vram);
    MemoryFree(*palette);
    free(*ppu);
    return false;
  }

  (*ppu)->platform = platform;
  (*ppu)->registers.dispcnt.forced_blank = true;
  (*ppu)->width = GBA_SCREEN_WIDTH;
  (*ppu)->height = GBA_SCREEN_HEIGHT;
  (*ppu)->memory.reference_count += 1u;

  GbaPlatformRetain(platform);

  return true;
}

void GbaPpuFree(GbaPpu *ppu) {
  assert(ppu->memory.reference_count != 0u);
  ppu->memory.reference_count -= 1u;
  if (ppu->memory.reference_count == 0u) {
    GbaPlatformRelease(ppu->platform);
    free(ppu);
  }
}

#include <math.h>
void GbaPpuStep(GbaPpu *ppu) {
  glBindFramebuffer(GL_FRAMEBUFFER, ppu->fbo);

  glClearColor(0.3, 0.4, 0.5, 1.0);
  glViewport(0, 0, ppu->width, ppu->height);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  glUseProgram(ppu->program);

  glEnable(GL_DEPTH_TEST);

  glBindBuffer(GL_ARRAY_BUFFER, ppu->vbo);
  int vloc = glGetAttribLocation(ppu->program, "aVertex");
  glVertexAttribPointer(vloc, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);
  glEnableVertexAttribArray(vloc);
  int cloc = glGetAttribLocation(ppu->program, "aColor");
  glVertexAttribPointer(cloc, 4, GL_FLOAT, GL_FALSE, 0,
                        (void *)(8 * sizeof(GLfloat)));
  glEnableVertexAttribArray(cloc);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  int loc = glGetUniformLocation(ppu->program, "uMVP");

  float angle = ppu->frame_count / 100.0;
  float cos_angle = cos(angle);
  float sin_angle = sin(angle);

  const GLfloat mvp[] = {
      cos_angle, -sin_angle, 0, 0, sin_angle, cos_angle, 0, 0,
      0,         0,          1, 0, 0,         0,         0, 1,
  };
  glUniformMatrix4fv(loc, 1, GL_FALSE, mvp);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  cos_angle *= 0.5;
  sin_angle *= 0.5;
  const GLfloat mvp2[] = {
      cos_angle, -sin_angle, 0, 0.0, sin_angle, cos_angle, 0,   0.0,
      0,         0,          1, 0,   0.4,       0.4,       0.2, 1,
  };

  glUniformMatrix4fv(loc, 1, GL_FALSE, mvp2);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glDisableVertexAttribArray(vloc);
  glDisableVertexAttribArray(cloc);

  glUseProgram(0);

  ppu->frame_count++;

  if (ppu->frame_done != NULL) {
    ppu->frame_done(ppu->width, ppu->height);
  }
}

void GbaPpuSetRenderOutput(GbaPpu *ppu, GLuint framebuffer) {
  ppu->fbo = framebuffer;
}

void GbaPpuSetRenderScale(GbaPpu *ppu, uint8_t scale_factor) {
  assert(scale_factor != 0);
  ppu->width = GBA_SCREEN_WIDTH * scale_factor;
  ppu->height = GBA_SCREEN_HEIGHT * scale_factor;
}

void GbaPpuSetRenderDoneCallback(GbaPpu *ppu,
                                 PpuRenderDoneFunction frame_done) {
  ppu->frame_done = frame_done;
}

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
void GbaPpuReloadContext(GbaPpu *ppu) {
  static const char *vertex_shader[] = {
      "uniform mat4 uMVP;",
      "attribute vec2 aVertex;",
      "attribute vec4 aColor;",
      "varying vec4 color;",
      "void main() {",
      "  gl_Position = uMVP * vec4(aVertex, 0.0, 1.0);",
      "  color = aColor;",
      "}",
  };

  static const char *fragment_shader[] = {
      "#ifdef GL_ES\n",
      "precision mediump float;\n",
      "#endif\n",
      "varying vec4 color;",
      "void main() {",
      "  gl_FragColor = color;",
      "}",
  };

  // Compile Program
  ppu->program = glCreateProgram();
  GLuint vert = glCreateShader(GL_VERTEX_SHADER);
  GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);

  glShaderSource(vert, ARRAY_SIZE(vertex_shader), vertex_shader, 0);
  glShaderSource(frag, ARRAY_SIZE(fragment_shader), fragment_shader, 0);
  glCompileShader(vert);
  glCompileShader(frag);

  glAttachShader(ppu->program, vert);
  glAttachShader(ppu->program, frag);
  glLinkProgram(ppu->program);
  glDeleteShader(vert);
  glDeleteShader(frag);

  // Setup VAO
  static const GLfloat vertex_data[] = {
      -0.5, -0.5, 0.5, -0.5, -0.5, 0.5, 0.5, 0.5, 1.0, 1.0, 1.0, 1.0,
      1.0,  1.0,  0.0, 1.0,  0.0,  1.0, 1.0, 1.0, 1.0, 0.0, 1.0, 1.0,
  };

  glUseProgram(ppu->program);

  glGenBuffers(1, &ppu->vbo);
  glBindBuffer(GL_ARRAY_BUFFER, ppu->vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data,
               GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glUseProgram(0);
}