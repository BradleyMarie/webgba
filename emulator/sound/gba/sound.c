#include "emulator/sound/gba/sound.h"

#include <assert.h>
#include <stdlib.h>

#define SOUND1CNT_L_OFFSET 0x00u
#define SOUND1CNT_H_OFFSET 0x02u
#define SOUND1CNT_X_OFFSET 0x04u
#define SOUND2CNT_L_OFFSET 0x08u
#define SOUND2CNT_H_OFFSET 0x0Cu
#define SOUND3CNT_L_OFFSET 0x10u
#define SOUND3CNT_H_OFFSET 0x12u
#define SOUND3CNT_X_OFFSET 0x14u
#define SOUND4CNT_L_OFFSET 0x18u
#define SOUND4CNT_H_OFFSET 0x1Cu
#define SOUNDCNT_L_OFFSET 0x20u
#define SOUNDCNT_H_OFFSET 0x22u
#define SOUNDCNT_X_OFFSET 0x24u
#define SOUNDBIAS_OFFSET 0x28u
#define WAVE_RAM0_L_OFFSET 0x30u
#define WAVE_RAM0_H_OFFSET 0x32u
#define WAVE_RAM1_L_OFFSET 0x34u
#define WAVE_RAM1_H_OFFSET 0x36u
#define WAVE_RAM2_L_OFFSET 0x38u
#define WAVE_RAM2_H_OFFSET 0x3Au
#define WAVE_RAM3_L_OFFSET 0x3Cu
#define WAVE_RAM3_H_OFFSET 0x3Eu
#define FIFO_A_OFFSET 0x40u
#define FIFO_B_OFFSET 0x44u

#define GBA_SPU_REGISTERS_SIZE 0x40u

typedef union {
  struct {
    uint16_t sound1cnt_l;
    uint16_t sound1cnt_h;
    uint16_t sound1cnt_x;
    uint16_t unused0;
    uint16_t sound2cnt_l;
    uint16_t unused1;
    uint16_t sound2cnt_h;
    uint16_t unused2;
    uint16_t sound3cnt_l;
    uint16_t sound3cnt_h;
    uint16_t sound3cnt_x;
    uint16_t unused3;
    uint16_t sound4cnt_l;
    uint16_t unused4;
    uint16_t sound4cnt_h;
    uint16_t unused5;
    uint16_t soundcnt_l;
    uint16_t soundcnt_h;
    uint16_t soundcnt_x;
    uint16_t unused6;
    uint16_t soundbias;
    uint16_t unused7;
    uint16_t unused8;
    uint16_t unused9;
    uint16_t wave_ram0_l;
    uint16_t wave_ram0_h;
    uint16_t wave_ram1_l;
    uint16_t wave_ram1_h;
    uint16_t wave_ram2_l;
    uint16_t wave_ram2_h;
    uint16_t wave_ram3_l;
    uint16_t wave_ram3_h;
  };
  uint16_t half_words[32];
} GbaSpuRegisters;

struct _GbaSpu {
  GbaSpuRegisters registers;
  uint16_t reference_count;
};

static bool GbaSpuRegistersLoad16LE(const void *context, uint32_t address,
                                    uint16_t *value) {
  assert((address & 0x1u) == 0u);

  const GbaSpu *spu = (const GbaSpu *)context;

  switch (address) {
    case SOUND1CNT_L_OFFSET:
      *value = spu->registers.sound1cnt_l;
      return true;
    case SOUND1CNT_H_OFFSET:
      *value = spu->registers.sound1cnt_h;
      return true;
    case SOUND1CNT_X_OFFSET:
      *value = spu->registers.sound1cnt_x;
      return true;
    case SOUND2CNT_L_OFFSET:
      *value = spu->registers.sound2cnt_l;
      return true;
    case SOUND2CNT_H_OFFSET:
      *value = spu->registers.sound2cnt_h;
      return true;
    case SOUND3CNT_L_OFFSET:
      *value = spu->registers.sound3cnt_l;
      return true;
    case SOUND3CNT_H_OFFSET:
      *value = spu->registers.sound3cnt_h;
      return true;
    case SOUND3CNT_X_OFFSET:
      *value = spu->registers.sound3cnt_x;
      return true;
    case SOUND4CNT_L_OFFSET:
      *value = spu->registers.sound4cnt_l;
      return true;
    case SOUND4CNT_H_OFFSET:
      *value = spu->registers.sound4cnt_h;
      return true;
    case SOUNDCNT_L_OFFSET:
      *value = spu->registers.soundcnt_l;
      return true;
    case SOUNDCNT_H_OFFSET:
      *value = spu->registers.soundcnt_h;
      return true;
    case SOUNDCNT_X_OFFSET:
      *value = spu->registers.soundcnt_x;
      return true;
    case SOUNDBIAS_OFFSET:
      *value = spu->registers.soundbias;
      return true;
    case WAVE_RAM0_L_OFFSET:
      *value = spu->registers.wave_ram0_l;
      return true;
    case WAVE_RAM0_H_OFFSET:
      *value = spu->registers.wave_ram0_h;
      return true;
    case WAVE_RAM1_L_OFFSET:
      *value = spu->registers.wave_ram1_l;
      return true;
    case WAVE_RAM1_H_OFFSET:
      *value = spu->registers.wave_ram1_h;
      return true;
    case WAVE_RAM2_L_OFFSET:
      *value = spu->registers.wave_ram2_l;
      return true;
    case WAVE_RAM2_H_OFFSET:
      *value = spu->registers.wave_ram2_h;
      return true;
    case WAVE_RAM3_L_OFFSET:
      *value = spu->registers.wave_ram3_l;
      return true;
    case WAVE_RAM3_H_OFFSET:
      *value = spu->registers.wave_ram3_h;
      return true;
  }

  return false;
}

static bool GbaSpuRegistersLoad32LE(const void *context, uint32_t address,
                                    uint32_t *value) {
  assert((address & 0x3u) == 0u);

  uint16_t low_bits;
  bool low = GbaSpuRegistersLoad16LE(context, address, &low_bits);
  if (!low) {
    return false;
  }

  uint16_t high_bits;
  bool high = GbaSpuRegistersLoad16LE(context, address + 2u, &high_bits);
  if (high) {
    *value |= (((uint32_t)high_bits) << 16u) | (uint32_t)low_bits;
  } else {
    *value = 0u;
  }

  return true;
}

static bool GbaSpuRegistersLoad8(const void *context, uint32_t address,
                                 uint8_t *value) {
  uint32_t read_address = address & 0xFFFFFFFEu;

  uint16_t value16;
  bool success = GbaSpuRegistersLoad16LE(context, read_address, &value16);
  if (success) {
    *value = (address == read_address) ? value16 : value16 >> 8u;
  }

  return success;
}

static bool GbaSpuRegistersStore16LE(void *context, uint32_t address,
                                     uint16_t value) {
  assert((address & 0x1u) == 0u);

  GbaSpu *spu = (GbaSpu *)context;

  if (address >= GBA_SPU_REGISTERS_SIZE) {
    // Possibly FIFO registers
    return true;
  }

  spu->registers.half_words[address >> 1u] = value;

  return true;
}

static bool GbaSpuRegistersStore32LE(void *context, uint32_t address,
                                     uint32_t value) {
  assert((address & 0x3u) == 0u);

  GbaSpuRegistersStore16LE(context, address, value);
  GbaSpuRegistersStore16LE(context, address + 2u, value >> 16u);

  return true;
}

static bool GbaSpuRegistersStore8(void *context, uint32_t address,
                                  uint8_t value) {
  GbaSpu *spu = (GbaSpu *)context;

  uint32_t read_address = address & 0xFFFFFFFEu;
  uint16_t value16 = spu->registers.half_words[read_address >> 1u];
  if (address == read_address) {
    value16 &= 0xFF00;
    value16 |= value;
  } else {
    value16 &= 0x00FF;
    value16 |= (uint16_t)value << 8u;
  }

  GbaSpuRegistersStore16LE(context, read_address, value16);

  return true;
}

void GbaSpuMemoryFree(void *context) {
  GbaSpu *spu = (GbaSpu *)context;
  GbaSpuFree(spu);
}

bool GbaSpuAllocate(GbaSpu **spu, Memory **registers) {
  *spu = (GbaSpu *)calloc(1, sizeof(GbaSpu));
  if (*spu == NULL) {
    return false;
  }

  *registers = MemoryAllocate(
      *spu, GbaSpuRegistersLoad32LE, GbaSpuRegistersLoad16LE,
      GbaSpuRegistersLoad8, GbaSpuRegistersStore32LE, GbaSpuRegistersStore16LE,
      GbaSpuRegistersStore8, GbaSpuMemoryFree);
  if (*registers == NULL) {
    free(*spu);
    return false;
  }

  (*spu)->reference_count = 2u;

  return true;
}

void GbaSpuStep(GbaSpu *spu) {}

void GbaSpuFree(GbaSpu *spu) {
  assert(spu->reference_count != 0u);
  spu->reference_count -= 1u;
  if (spu->reference_count == 0u) {
    free(spu);
  }
}

static_assert(sizeof(GbaSpuRegisters) == GBA_SPU_REGISTERS_SIZE,
              "sizeof(GbaSpuRegisters) != GBA_SPU_REGISTERS_SIZE");