#include "emulator/peripherals/gba/peripherals.h"

#include <assert.h>
#include <stdlib.h>
#include <threads.h>

#define SIODATA32S_OFFSET 0x00u
#define SIOMULTI0_OFFSET 0x00u
#define SIOMULTI1_OFFSET 0x02u
#define SIOMULTI2_OFFSET 0x04u
#define SIOMULTI3_OFFSET 0x06u
#define SIOCNT_OFFSET 0x08u
#define SIOMLT_SEND_OFFSET 0x0Au
#define SIODATA8_OFFSET 0x0Au
#define KEYINPUT_OFFSET 0x10u
#define KEYCNT_OFFSET 0x12u
#define RCNT_OFFSET 0x14u
#define IR_OFFSET 0x16u
#define JOYCNT_OFFSET 0x20u
#define JOY_RECV_L_OFFSET 0x30u
#define JOY_RECV_H_OFFSET 0x32u
#define JOY_TRANS_L_OFFSET 0x34u
#define JOY_TRANS_H_OFFSET 0x36u
#define JOYSTAT_OFFSET 0x38u

#define GBA_PERIPHERALS_REGISTERS_SIZE 0x3Cu

typedef union {
  struct {
    bool a : 1;
    bool b : 1;
    bool select : 1;
    bool start : 1;
    bool right : 1;
    bool left : 1;
    bool up : 1;
    bool down : 1;
    bool r : 1;
    bool l : 1;
    unsigned char unused : 6;
  };
  uint16_t value;
} KeyInput;

typedef union {
  struct {
    bool a : 1;
    bool b : 1;
    bool select : 1;
    bool start : 1;
    bool right : 1;
    bool left : 1;
    bool up : 1;
    bool down : 1;
    bool r : 1;
    bool l : 1;
    unsigned char unused : 4;
    bool irq_enable : 1;
    bool irq_condition : 1;
  };
  uint16_t value;
} KeyControl;

typedef union {
  struct {
    uint16_t siomulti0;
    uint16_t siomulti1;
    uint16_t siomulti2;
    uint16_t siomulti3;
    uint16_t siocnt;
    uint16_t siomlt_send;
    uint32_t unused0;
    KeyInput keyinput;
    KeyControl keycnt;
    uint16_t rcnt;
    uint16_t ir;
    uint16_t unused1[4];
    uint16_t joycnt;
    uint16_t unused3[7];
    union {
      uint16_t joy_recv_half[2];
      uint32_t joy_recv;
    };
    union {
      uint16_t joy_trans_half[2];
      uint32_t joy_trans;
    };
    uint16_t joystat;
    uint16_t unused4;
  };
  uint16_t half_words[30];
  uint32_t words[15];
  uint8_t bytes[60];
} GbaPeripheralRegisters;

struct _GbaPeripherals {
  mtx_t mutex;
  GbaPeripheralRegisters registers;
  GbaPlatform *platform;
  uint16_t reference_count;
};

static bool GbaPeripheralsRegistersLoad16LEImpl(const void *context,
                                                uint32_t address,
                                                uint16_t *value) {
  assert((address & 0x1u) == 0u);

  const GbaPeripherals *peripherals = (const GbaPeripherals *)context;

  switch (address) {
    case SIOMULTI0_OFFSET:
      *value = 0u;
      return true;
    case SIOMULTI1_OFFSET:
      *value = 0u;
      return true;
    case SIOMULTI2_OFFSET:
      *value = 0u;
      return true;
    case SIOMULTI3_OFFSET:
      *value = 0u;
      return true;
    case SIOCNT_OFFSET:
      *value = peripherals->registers.siocnt;
      return true;
    case SIOMLT_SEND_OFFSET:
      *value = peripherals->registers.siomlt_send;
      return true;
    case KEYINPUT_OFFSET:
      *value = peripherals->registers.keyinput.value;
      return true;
    case KEYCNT_OFFSET:
      *value = peripherals->registers.keycnt.value;
      return true;
    case RCNT_OFFSET:
      *value = peripherals->registers.rcnt;
      return true;
    case IR_OFFSET:
      *value = 0u;
      return true;
    case JOYCNT_OFFSET:
      *value = peripherals->registers.joycnt;
      return true;
    case JOY_RECV_L_OFFSET:
      *value = 0u;
      return true;
    case JOY_RECV_H_OFFSET:
      *value = 0u;
      return true;
    case JOY_TRANS_L_OFFSET:
      *value = peripherals->registers.joy_trans_half[0];
      return true;
    case JOY_TRANS_H_OFFSET:
      *value = peripherals->registers.joy_trans_half[1];
      return true;
    case JOYSTAT_OFFSET:
      *value = 0u;
      return true;
  }

  return false;
}

static bool GbaPeripheralsRegistersLoad32LEImpl(const void *context,
                                                uint32_t address,
                                                uint32_t *value) {
  assert((address & 0x3u) == 0u);

  uint16_t low_bits;
  bool low = GbaPeripheralsRegistersLoad16LEImpl(context, address, &low_bits);
  if (!low) {
    return false;
  }

  uint16_t high_bits;
  bool high =
      GbaPeripheralsRegistersLoad16LEImpl(context, address + 2u, &high_bits);
  if (high) {
    *value = (((uint32_t)high_bits) << 16u) | (uint32_t)low_bits;
  } else {
    *value = low_bits;
  }

  return true;
}

static bool GbaPeripheralsRegistersLoad8Impl(const void *context,
                                             uint32_t address, uint8_t *value) {
  uint32_t read_address = address & 0xFFFFFFFEu;

  uint16_t value16;
  bool success =
      GbaPeripheralsRegistersLoad16LEImpl(context, read_address, &value16);
  if (success) {
    *value = (address == read_address) ? value16 : value16 >> 8u;
  }

  return success;
}

static bool GbaPeripheralsRegistersStore16LEImpl(void *context,
                                                 uint32_t address,
                                                 uint16_t value) {
  assert((address & 0x1u) == 0u);

  GbaPeripherals *peripherals = (GbaPeripherals *)context;

  if (address >= GBA_PERIPHERALS_REGISTERS_SIZE || address == KEYINPUT_OFFSET) {
    return true;
  }

  peripherals->registers.half_words[address >> 1u] = value;

  return true;
}

static bool GbaPeripheralsRegistersStore32LEImpl(void *context,
                                                 uint32_t address,
                                                 uint32_t value) {
  assert((address & 0x3u) == 0u);

  GbaPeripheralsRegistersStore16LEImpl(context, address, value);
  GbaPeripheralsRegistersStore16LEImpl(context, address + 2u, value >> 16u);

  return true;
}

static bool GbaPeripheralsRegistersStore8Impl(void *context, uint32_t address,
                                              uint8_t value) {
  GbaPeripherals *peripherals = (GbaPeripherals *)context;

  if (address >= GBA_PERIPHERALS_REGISTERS_SIZE || address == KEYINPUT_OFFSET ||
      address == KEYINPUT_OFFSET + 1u) {
    return true;
  }

  peripherals->registers.bytes[address] = value;

  return true;
}

static bool GbaPeripheralsRegistersLoad32LE(const void *context,
                                            uint32_t address, uint32_t *value) {
  GbaPeripherals *peripherals = (GbaPeripherals *)context;

  mtx_lock(&peripherals->mutex);
  bool result = GbaPeripheralsRegistersLoad32LEImpl(context, address, value);
  mtx_unlock(&peripherals->mutex);

  return result;
}

static bool GbaPeripheralsRegistersLoad16LE(const void *context,
                                            uint32_t address, uint16_t *value) {
  GbaPeripherals *peripherals = (GbaPeripherals *)context;

  mtx_lock(&peripherals->mutex);
  bool result = GbaPeripheralsRegistersLoad16LEImpl(context, address, value);
  mtx_unlock(&peripherals->mutex);

  return result;
}

static bool GbaPeripheralsRegistersLoad8(const void *context, uint32_t address,
                                         uint8_t *value) {
  GbaPeripherals *peripherals = (GbaPeripherals *)context;

  mtx_lock(&peripherals->mutex);
  bool result = GbaPeripheralsRegistersLoad8Impl(context, address, value);
  mtx_unlock(&peripherals->mutex);

  return result;
}

static bool GbaPeripheralsRegistersStore32LE(void *context, uint32_t address,
                                             uint32_t value) {
  GbaPeripherals *peripherals = (GbaPeripherals *)context;

  mtx_lock(&peripherals->mutex);
  bool result = GbaPeripheralsRegistersStore32LEImpl(context, address, value);
  mtx_unlock(&peripherals->mutex);

  return result;
}

static bool GbaPeripheralsRegistersStore16LE(void *context, uint32_t address,
                                             uint16_t value) {
  GbaPeripherals *peripherals = (GbaPeripherals *)context;

  mtx_lock(&peripherals->mutex);
  bool result = GbaPeripheralsRegistersStore16LEImpl(context, address, value);
  mtx_unlock(&peripherals->mutex);

  return result;
}

static bool GbaPeripheralsRegistersStore8(void *context, uint32_t address,
                                          uint8_t value) {
  GbaPeripherals *peripherals = (GbaPeripherals *)context;

  mtx_lock(&peripherals->mutex);
  bool result = GbaPeripheralsRegistersStore8Impl(context, address, value);
  mtx_unlock(&peripherals->mutex);

  return result;
}

static void GbaPeripheralsMaybeSendKeypadInterrupt(
    const GbaPeripherals *peripherals) {
  if (!peripherals->registers.keycnt.irq_enable) {
    return;
  }

  static const uint16_t field_mask = 0x01FF;
  uint16_t input = (~peripherals->registers.keyinput.value) & field_mask;
  uint16_t control = peripherals->registers.keycnt.value & field_mask;
  if (!control || !input) {
    return;
  }

  uint16_t anded = input & control;
  if (peripherals->registers.keycnt.irq_condition) {
    if (anded == control) {
      GbaPlatformRaiseKeypadInterrupt(peripherals->platform);
    }
  } else {
    if (anded != 0) {
      GbaPlatformRaiseKeypadInterrupt(peripherals->platform);
    }
  }
}

static void GbaGamePadToggleUp(void *context, bool pressed) {
  GbaPeripherals *peripherals = (GbaPeripherals *)context;

  mtx_lock(&peripherals->mutex);

  pressed = !pressed;
  if (peripherals->registers.keyinput.up != pressed) {
    peripherals->registers.keyinput.up = pressed;
    GbaPeripheralsMaybeSendKeypadInterrupt(peripherals);
  }

  mtx_unlock(&peripherals->mutex);
}

static void GbaGamePadToggleDown(void *context, bool pressed) {
  GbaPeripherals *peripherals = (GbaPeripherals *)context;

  mtx_lock(&peripherals->mutex);

  pressed = !pressed;
  if (peripherals->registers.keyinput.down != pressed) {
    peripherals->registers.keyinput.down = pressed;
    GbaPeripheralsMaybeSendKeypadInterrupt(peripherals);
  }

  mtx_unlock(&peripherals->mutex);
}

static void GbaGamePadToggleLeft(void *context, bool pressed) {
  GbaPeripherals *peripherals = (GbaPeripherals *)context;

  mtx_lock(&peripherals->mutex);

  pressed = !pressed;
  if (peripherals->registers.keyinput.left != pressed) {
    peripherals->registers.keyinput.left = pressed;
    GbaPeripheralsMaybeSendKeypadInterrupt(peripherals);
  }

  mtx_unlock(&peripherals->mutex);
}

static void GbaGamePadToggleRight(void *context, bool pressed) {
  GbaPeripherals *peripherals = (GbaPeripherals *)context;

  mtx_lock(&peripherals->mutex);

  pressed = !pressed;
  if (peripherals->registers.keyinput.right != pressed) {
    peripherals->registers.keyinput.right = pressed;
    GbaPeripheralsMaybeSendKeypadInterrupt(peripherals);
  }

  mtx_unlock(&peripherals->mutex);
}

static void GbaGamePadToggleA(void *context, bool pressed) {
  GbaPeripherals *peripherals = (GbaPeripherals *)context;

  mtx_lock(&peripherals->mutex);

  pressed = !pressed;
  if (peripherals->registers.keyinput.a != pressed) {
    peripherals->registers.keyinput.a = pressed;
    GbaPeripheralsMaybeSendKeypadInterrupt(peripherals);
  }

  mtx_unlock(&peripherals->mutex);
}

static void GbaGamePadToggleB(void *context, bool pressed) {
  GbaPeripherals *peripherals = (GbaPeripherals *)context;

  mtx_lock(&peripherals->mutex);

  pressed = !pressed;
  if (peripherals->registers.keyinput.b != pressed) {
    peripherals->registers.keyinput.b = pressed;
    GbaPeripheralsMaybeSendKeypadInterrupt(peripherals);
  }

  mtx_unlock(&peripherals->mutex);
}

static void GbaGamePadToggleL(void *context, bool pressed) {
  GbaPeripherals *peripherals = (GbaPeripherals *)context;

  mtx_lock(&peripherals->mutex);

  pressed = !pressed;
  if (peripherals->registers.keyinput.l != pressed) {
    peripherals->registers.keyinput.l = pressed;
    GbaPeripheralsMaybeSendKeypadInterrupt(peripherals);
  }

  mtx_unlock(&peripherals->mutex);
}

static void GbaGamePadToggleR(void *context, bool pressed) {
  GbaPeripherals *peripherals = (GbaPeripherals *)context;

  mtx_lock(&peripherals->mutex);

  pressed = !pressed;
  if (peripherals->registers.keyinput.r != pressed) {
    peripherals->registers.keyinput.r = pressed;
    GbaPeripheralsMaybeSendKeypadInterrupt(peripherals);
  }

  mtx_unlock(&peripherals->mutex);
}

static void GbaGamePadToggleStart(void *context, bool pressed) {
  GbaPeripherals *peripherals = (GbaPeripherals *)context;

  mtx_lock(&peripherals->mutex);

  pressed = !pressed;
  if (peripherals->registers.keyinput.start != pressed) {
    peripherals->registers.keyinput.start = pressed;
    GbaPeripheralsMaybeSendKeypadInterrupt(peripherals);
  }

  mtx_unlock(&peripherals->mutex);
}

static void GbaGamePadToggleSelect(void *context, bool pressed) {
  GbaPeripherals *peripherals = (GbaPeripherals *)context;

  mtx_lock(&peripherals->mutex);

  pressed = !pressed;
  if (peripherals->registers.keyinput.select != pressed) {
    peripherals->registers.keyinput.select = pressed;
    GbaPeripheralsMaybeSendKeypadInterrupt(peripherals);
  }

  mtx_unlock(&peripherals->mutex);
}

static void GbaPeripheralsMemoryFree(void *context) {
  GbaPeripherals *peripherals = (GbaPeripherals *)context;
  GbaPeripheralsFree(peripherals);
}

bool GbaPeripheralsAllocate(GbaPlatform *platform, GbaPeripherals **peripherals,
                            GamePad **gamepad, Memory **registers) {
  *peripherals = (GbaPeripherals *)calloc(1, sizeof(GbaPeripherals));
  if (*peripherals == NULL) {
    return false;
  }

  if (mtx_init(&(*peripherals)->mutex, mtx_plain) != thrd_success) {
    free(peripherals);
    return false;
  }

  *gamepad = GamePadAllocate(
      *peripherals, GbaGamePadToggleUp, GbaGamePadToggleDown,
      GbaGamePadToggleLeft, GbaGamePadToggleRight, GbaGamePadToggleA,
      GbaGamePadToggleB, GbaGamePadToggleL, GbaGamePadToggleR,
      GbaGamePadToggleStart, GbaGamePadToggleSelect, GbaPeripheralsMemoryFree);
  if (*gamepad == NULL) {
    mtx_destroy(&(*peripherals)->mutex);
    free(*peripherals);
    return false;
  }

  (*peripherals)->reference_count = 3u;

  *registers = MemoryAllocate(
      *peripherals, GbaPeripheralsRegistersLoad32LE,
      GbaPeripheralsRegistersLoad16LE, GbaPeripheralsRegistersLoad8,
      GbaPeripheralsRegistersStore32LE, GbaPeripheralsRegistersStore16LE,
      GbaPeripheralsRegistersStore8, GbaPeripheralsMemoryFree);
  if (*registers == NULL) {
    GamePadFree(*gamepad);
    mtx_destroy(&(*peripherals)->mutex);
    free(*peripherals);
    return false;
  }

  (*peripherals)->registers.keyinput.up = true;
  (*peripherals)->registers.keyinput.down = true;
  (*peripherals)->registers.keyinput.left = true;
  (*peripherals)->registers.keyinput.right = true;
  (*peripherals)->registers.keyinput.a = true;
  (*peripherals)->registers.keyinput.b = true;
  (*peripherals)->registers.keyinput.l = true;
  (*peripherals)->registers.keyinput.r = true;
  (*peripherals)->registers.keyinput.start = true;
  (*peripherals)->registers.keyinput.select = true;
  (*peripherals)->registers.rcnt = 0x8000u;
  (*peripherals)->platform = platform;

  GbaPlatformRetain(platform);

  return true;
}

void GbaPeripheralsFree(GbaPeripherals *peripherals) {
  assert(peripherals->reference_count != 0u);
  mtx_lock(&peripherals->mutex);
  peripherals->reference_count -= 1u;
  if (peripherals->reference_count == 0u) {
    mtx_destroy(&peripherals->mutex);
    free(peripherals);
  } else {
    mtx_unlock(&peripherals->mutex);
  }
}

static_assert(
    sizeof(GbaPeripheralRegisters) == GBA_PERIPHERALS_REGISTERS_SIZE,
    "sizeof(GbaPeripheralRegisters) != GBA_PERIPHERALS_REGISTERS_SIZE");