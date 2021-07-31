#include "emulator/platform/gba/platform.h"

#include <assert.h>
#include <stdlib.h>
#include <strings.h>

#define LOW_REGISTERS_SIZE 12u

#define IE_OFFSET 0u
#define IF_OFFSET 2u
#define WAITCNT_OFFSET 4u
#define IME_OFFSET 8u

typedef union {
  struct {
    bool vblank : 1;
    bool hblank : 1;
    bool vblank_count : 1;
    bool timer0 : 1;
    bool timer1 : 1;
    bool timer2 : 1;
    bool timer3 : 1;
    bool serial : 1;
    bool dma0 : 1;
    bool dma1 : 1;
    bool dma2 : 1;
    bool dma3 : 1;
    bool keypad : 1;
    bool cartridge : 1;
    unsigned char unused : 2;
  };
  uint16_t value;
} GbaInterruptRegister;

typedef union {
  struct {
    bool enabled : 1;
    unsigned short unused : 15;
  };
  uint16_t value;
} GbaInterruptMasterEnableRegister;

typedef union {
  struct {
    unsigned char sram : 2;
    unsigned char rom_0_first_access : 2;
    bool rom_0_second_access : 1;
    unsigned char rom_1_first_access : 2;
    bool rom_1_second_access : 1;
    unsigned char rom_2_first_access : 2;
    bool rom_2_second_access : 1;
    unsigned char phi_terminal_output : 2;  // Unimplemented
    bool unused : 1;
    bool gamepak_prefetch : 1;
    bool gamepak_type : 1;  // Read Only
  };
  uint16_t value;
} GbaWaitstateControlRegisters;

typedef struct {
  GbaInterruptRegister interrupt_enable;
  GbaInterruptRegister interrupt_flags;
  GbaWaitstateControlRegisters waitcnt;
  uint16_t unused0;
  GbaInterruptMasterEnableRegister interrupt_master_enable;
  uint16_t unused1;
} GbaPlatformLowRegisters;

#define POSTFLG_OFFSET 0u
#define HALTCNT_OFFSET 1u

typedef struct {
  uint8_t postflg;
  uint8_t memory_control_bytes[4];  // Unimplemented
} GbaPlatformHighRegisters;

#define STOP_MASK 0x3080u

struct _GbaPlatform {
  union {
    GbaPlatformLowRegisters low_registers;
    uint16_t low_register_half_words[6];
  };
  GbaPlatformHighRegisters high_registers;
  GbaPowerState power_state;
  uint16_t reference_count;
};

static bool GbaIrqLineIsRaisedFunction(const void *context) {
  const GbaPlatform *controller = (const GbaPlatform *)context;
  return controller->low_registers.interrupt_master_enable.enabled &&
         controller->low_registers.interrupt_enable.value &
             controller->low_registers.interrupt_flags.value;
}

static void GbaIrqLineFree(void *context) {
  GbaPlatform *controller = (GbaPlatform *)context;
  GbaPlatformRelease(controller);
}

static bool GbaRstFiqLineIsRaisedFunction(const void *context) { return false; }

static bool GbaPlatformLowRegistersLoad16LEFunction(const void *context,
                                                    uint32_t address,
                                                    uint16_t *value) {
  assert(address <= UINT32_MAX - 2u && address + 2u <= LOW_REGISTERS_SIZE);

  const GbaPlatform *platform = (const GbaPlatform *)context;

  assert((address & 0x1u) == 0u);
  switch (address) {
    case IE_OFFSET:
      *value = platform->low_registers.interrupt_enable.value;
      return true;
    case IF_OFFSET:
      *value = platform->low_registers.interrupt_flags.value;
      return true;
    case WAITCNT_OFFSET:
      *value = platform->low_registers.waitcnt.value;
      return true;
    case IME_OFFSET:
      *value = platform->low_registers.interrupt_master_enable.value;
      return true;
  }

  return false;
}

static bool GbaPlatformLowRegistersLoad32LEFunction(const void *context,
                                                    uint32_t address,
                                                    uint32_t *value) {
  assert(address <= UINT32_MAX - 4u && address + 4u <= LOW_REGISTERS_SIZE);

  uint16_t low_bits;
  bool low =
      GbaPlatformLowRegistersLoad16LEFunction(context, address, &low_bits);
  if (!low) {
    return false;
  }

  uint16_t high_bits;
  bool high = GbaPlatformLowRegistersLoad16LEFunction(context, address + 2u,
                                                      &high_bits);
  if (high) {
    *value |= (((uint32_t)high_bits) << 16u) | (uint32_t)low_bits;
  } else {
    *value = 0u;
  }

  return true;
}

static bool GbaPlatformLowRegistersLoad8Function(const void *context,
                                                 uint32_t address,
                                                 uint8_t *value) {
  assert(address <= UINT32_MAX - 1u && address + 1u <= LOW_REGISTERS_SIZE);

  uint32_t read_address = address & 0xFFFFFFFEu;

  uint16_t value16;
  bool success =
      GbaPlatformLowRegistersLoad16LEFunction(context, read_address, &value16);
  if (success) {
    *value = (address == read_address) ? value16 : value16 >> 8u;
  }

  return success;
}

static bool GbaPlatformLowRegistersStore16LEFunction(void *context,
                                                     uint32_t address,
                                                     uint16_t value) {
  assert(address <= UINT32_MAX - 2u && address + 2u <= LOW_REGISTERS_SIZE);

  GbaPlatform *platform = (GbaPlatform *)context;

  assert((address & 0x1u) == 0u);

  switch (address) {
    case IF_OFFSET:
      platform->low_register_half_words[address >> 1u] &= ~value;
      return true;
    case WAITCNT_OFFSET:
      platform->low_register_half_words[address >> 1u] = value & 0x7FFFu;
      return true;
    case IME_OFFSET:
      platform->low_register_half_words[address >> 1u] = value & 1u;
      return true;
  }

  platform->low_register_half_words[address >> 1u] = value;

  return true;
}

static bool GbaPlatformLowRegistersStore32LEFunction(void *context,
                                                     uint32_t address,
                                                     uint32_t value) {
  assert(address <= UINT32_MAX - 4u && address + 4u <= LOW_REGISTERS_SIZE);

  GbaPlatformLowRegistersStore16LEFunction(context, address, value);
  GbaPlatformLowRegistersStore16LEFunction(context, address + 2u, value >> 16u);

  return true;
}

static bool GbaPlatformLowRegistersStore8Function(void *context,
                                                  uint32_t address,
                                                  uint8_t value) {
  assert(address <= UINT32_MAX - 1u && address + 1u <= LOW_REGISTERS_SIZE);

  GbaPlatform *platform = (GbaPlatform *)context;

  uint32_t read_address = address & 0xFFFFFFFEu;

  uint16_t value16;
  if (read_address == IF_OFFSET) {
    value16 = (address == read_address) ? value : (uint16_t)value << 8u;
  } else {
    value16 = platform->low_register_half_words[read_address >> 1u];
    if (address == read_address) {
      value16 &= 0xFF00;
      value16 |= value;
    } else {
      value16 &= 0x00FF;
      value16 |= (uint16_t)value << 8u;
    }
  }

  GbaPlatformLowRegistersStore16LEFunction(context, read_address, value16);

  return true;
}

static bool GbaPlatformHighRegistersLoad8Function(const void *context,
                                                  uint32_t address,
                                                  uint8_t *value) {
  const GbaPlatform *platform = (const GbaPlatform *)context;

  if (address == POSTFLG_OFFSET) {
    *value = platform->high_registers.postflg;
    return true;
  }

  switch ((address + 0x500) % 0x800) {
    case 0u:
      *value = platform->high_registers.memory_control_bytes[0];
      return true;
    case 1u:
      *value = platform->high_registers.memory_control_bytes[1];
      return true;
    case 2u:
      *value = platform->high_registers.memory_control_bytes[2];
      return true;
    case 3u:
      *value = platform->high_registers.memory_control_bytes[3];
      return true;
  };

  return false;
}

static bool GbaPlatformHighRegistersLoad16LEFunction(const void *context,
                                                     uint32_t address,
                                                     uint16_t *value) {
  assert((address & 0x1u) == 0u);

  uint8_t low_bits;
  bool low = GbaPlatformHighRegistersLoad8Function(context, address, &low_bits);
  if (!low) {
    return false;
  }

  uint8_t high_bits;
  bool high =
      GbaPlatformHighRegistersLoad8Function(context, address + 1u, &high_bits);
  if (high) {
    *value |= (((uint16_t)high_bits) << 8u) | (uint16_t)low_bits;
  } else {
    assert(false);
  }

  return false;
}

static bool GbaPlatformHighRegistersLoad32LEFunction(const void *context,
                                                     uint32_t address,
                                                     uint32_t *value) {
  assert((address & 0x3u) == 0u);

  uint16_t low_bits;
  bool low =
      GbaPlatformHighRegistersLoad16LEFunction(context, address, &low_bits);
  if (!low) {
    return false;
  }

  uint16_t high_bits;
  bool high = GbaPlatformHighRegistersLoad16LEFunction(context, address + 2u,
                                                       &high_bits);
  if (high) {
    *value |= (((uint32_t)high_bits) << 16u) | (uint32_t)low_bits;
  } else {
    assert(false);
  }

  return true;
}

static bool GbaPlatformHighRegistersStore8Function(void *context,
                                                   uint32_t address,
                                                   uint8_t value) {
  GbaPlatform *platform = (GbaPlatform *)context;

  switch (address) {
    case POSTFLG_OFFSET:
      platform->high_registers.postflg = value;
      return true;
    case HALTCNT_OFFSET:
      value &= 0x80;
      if (value) {  // Stop
        if ((platform->low_registers.interrupt_enable.value &
             platform->low_registers.interrupt_flags.value & STOP_MASK) == 0) {
          platform->power_state = GBA_POWER_STATE_STOP;
        }
      } else {  // Halt
        if ((platform->low_registers.interrupt_enable.value &
             platform->low_registers.interrupt_flags.value) == 0) {
          platform->power_state = GBA_POWER_STATE_HALT;
        }
      }
      return true;
  }

  switch ((address + 0x500) % 0x800) {
    case 0u:
      platform->high_registers.memory_control_bytes[0] = value & 0x2F;
      return true;
    case 3u:
      platform->high_registers.memory_control_bytes[3] = value;
      return true;
  };

  return true;
}

static bool GbaPlatformHighRegistersStore16LEFunction(void *context,
                                                      uint32_t address,
                                                      uint16_t value) {
  assert((address & 0x1u) == 0u);

  GbaPlatformHighRegistersStore8Function(context, address, value);
  GbaPlatformHighRegistersStore8Function(context, address + 1, value >> 8u);

  return true;
}

static bool GbaPlatformHighRegistersStore32LEFunction(void *context,
                                                      uint32_t address,
                                                      uint32_t value) {
  assert((address & 0x3u) == 0u);

  GbaPlatformHighRegistersStore16LEFunction(context, address, value);
  GbaPlatformHighRegistersStore16LEFunction(context, address + 2u,
                                            value >> 16u);

  return true;
}

void GbaPlatformRegistersFree(void *context) {
  GbaPlatform *platform = (GbaPlatform *)context;
  GbaPlatformRelease(platform);
}

bool GbaPlatformAllocate(GbaPlatform **platform, Memory **low_registers,
                         Memory **high_registers, InterruptLine **rst_line,
                         InterruptLine **fiq_line, InterruptLine **irq_line) {
  *platform = (GbaPlatform *)calloc(1, sizeof(GbaPlatform));
  if (*platform == NULL) {
    return false;
  }

  (*platform)->reference_count = 1;

  *low_registers = MemoryAllocate(
      *platform, GbaPlatformLowRegistersLoad32LEFunction,
      GbaPlatformLowRegistersLoad16LEFunction,
      GbaPlatformLowRegistersLoad8Function,
      GbaPlatformLowRegistersStore32LEFunction,
      GbaPlatformLowRegistersStore16LEFunction,
      GbaPlatformLowRegistersStore8Function, GbaPlatformRegistersFree);

  if (*low_registers == NULL) {
    GbaPlatformRelease(*platform);
    return false;
  }

  (*platform)->reference_count += 1;

  *high_registers = MemoryAllocate(
      *platform, GbaPlatformHighRegistersLoad32LEFunction,
      GbaPlatformHighRegistersLoad16LEFunction,
      GbaPlatformHighRegistersLoad8Function,
      GbaPlatformHighRegistersStore32LEFunction,
      GbaPlatformHighRegistersStore16LEFunction,
      GbaPlatformHighRegistersStore8Function, GbaPlatformRegistersFree);

  if (*low_registers == NULL) {
    MemoryFree(*low_registers);
    GbaPlatformRelease(*platform);
    return false;
  }

  (*platform)->reference_count += 1;

  *rst_line = InterruptLineAllocate(NULL, GbaRstFiqLineIsRaisedFunction, NULL);
  if (*rst_line == NULL) {
    MemoryFree(*high_registers);
    MemoryFree(*low_registers);
    GbaPlatformRelease(*platform);
    return false;
  }

  *fiq_line = InterruptLineAllocate(NULL, GbaRstFiqLineIsRaisedFunction, NULL);
  if (*fiq_line == NULL) {
    InterruptLineFree(*rst_line);
    MemoryFree(*high_registers);
    MemoryFree(*low_registers);
    GbaPlatformRelease(*platform);
    return false;
  }

  *irq_line = InterruptLineAllocate(*platform, GbaIrqLineIsRaisedFunction,
                                    GbaIrqLineFree);
  if (*irq_line == NULL) {
    InterruptLineFree(*fiq_line);
    InterruptLineFree(*rst_line);
    MemoryFree(*high_registers);
    MemoryFree(*low_registers);
    GbaPlatformRelease(*platform);
    return false;
  }

  (*platform)->reference_count += 1;

  return true;
}

void GbaPlatformRaiseVBlankInterrupt(GbaPlatform *platform) {
  platform->low_registers.interrupt_flags.vblank = true;
  if (platform->power_state == GBA_POWER_STATE_HALT &&
      GbaIrqLineIsRaisedFunction(platform)) {
    platform->power_state = GBA_POWER_STATE_RUN;
  }
}

void GbaPlatformRaiseHBlankInterrupt(GbaPlatform *platform) {
  platform->low_registers.interrupt_flags.hblank = true;
  if (platform->power_state == GBA_POWER_STATE_HALT &&
      GbaIrqLineIsRaisedFunction(platform)) {
    platform->power_state = GBA_POWER_STATE_RUN;
  }
}

void GbaPlatformRaiseVBlankCountInterrupt(GbaPlatform *platform) {
  platform->low_registers.interrupt_flags.vblank_count = true;
  if (platform->power_state == GBA_POWER_STATE_HALT &&
      GbaIrqLineIsRaisedFunction(platform)) {
    platform->power_state = GBA_POWER_STATE_RUN;
  }
}

void GbaPlatformRaiseTimer0Interrupt(GbaPlatform *platform) {
  platform->low_registers.interrupt_flags.timer0 = true;
  if (platform->power_state == GBA_POWER_STATE_HALT &&
      GbaIrqLineIsRaisedFunction(platform)) {
    platform->power_state = GBA_POWER_STATE_RUN;
  }
}

void GbaPlatformRaiseTimer1Interrupt(GbaPlatform *platform) {
  platform->low_registers.interrupt_flags.timer1 = true;
  if (platform->power_state == GBA_POWER_STATE_HALT &&
      GbaIrqLineIsRaisedFunction(platform)) {
    platform->power_state = GBA_POWER_STATE_RUN;
  }
}

void GbaPlatformRaiseTimer2Interrupt(GbaPlatform *platform) {
  platform->low_registers.interrupt_flags.timer2 = true;
  if (platform->power_state == GBA_POWER_STATE_HALT &&
      GbaIrqLineIsRaisedFunction(platform)) {
    platform->power_state = GBA_POWER_STATE_RUN;
  }
}

void GbaPlatformRaiseTimer3Interrupt(GbaPlatform *platform) {
  platform->low_registers.interrupt_flags.timer3 = true;
  if (platform->power_state == GBA_POWER_STATE_HALT &&
      GbaIrqLineIsRaisedFunction(platform)) {
    platform->power_state = GBA_POWER_STATE_RUN;
  }
}

void GbaPlatformRaiseSerialInterrupt(GbaPlatform *platform) {
  platform->low_registers.interrupt_flags.serial = true;
  if (platform->power_state != GBA_POWER_STATE_RUN) {
    const static uint16_t masks[3] = {0xFFu, 0xFFu, STOP_MASK};
    assert(platform->power_state < 3u);
    if (platform->low_registers.interrupt_enable.value &
        platform->low_registers.interrupt_flags.value &
        masks[platform->power_state]) {
      platform->power_state = GBA_POWER_STATE_RUN;
    }
  }
}

void GbaPlatformRaiseDma0Interrupt(GbaPlatform *platform) {
  platform->low_registers.interrupt_flags.dma0 = true;
  if (platform->power_state == GBA_POWER_STATE_HALT &&
      GbaIrqLineIsRaisedFunction(platform)) {
    platform->power_state = GBA_POWER_STATE_RUN;
  }
}

void GbaPlatformRaiseDma1Interrupt(GbaPlatform *platform) {
  platform->low_registers.interrupt_flags.dma1 = true;
  if (platform->power_state == GBA_POWER_STATE_HALT &&
      GbaIrqLineIsRaisedFunction(platform)) {
    platform->power_state = GBA_POWER_STATE_RUN;
  }
}

void GbaPlatformRaiseDma2Interrupt(GbaPlatform *platform) {
  platform->low_registers.interrupt_flags.dma2 = true;
  if (platform->power_state == GBA_POWER_STATE_HALT &&
      GbaIrqLineIsRaisedFunction(platform)) {
    platform->power_state = GBA_POWER_STATE_RUN;
  }
}

void GbaPlatformRaiseDma3Interrupt(GbaPlatform *platform) {
  platform->low_registers.interrupt_flags.dma3 = true;
  if (platform->power_state == GBA_POWER_STATE_HALT &&
      GbaIrqLineIsRaisedFunction(platform)) {
    platform->power_state = GBA_POWER_STATE_RUN;
  }
}

void GbaPlatformRaiseKeypadInterrupt(GbaPlatform *platform) {
  platform->low_registers.interrupt_flags.keypad = true;
}

void GbaPlatformRaiseCartridgeInterrupt(GbaPlatform *platform) {
  platform->low_registers.interrupt_flags.cartridge = true;
}

uint_fast8_t GbaPlatformSramWaitStateCycles(const GbaPlatform *platform) {
  assert(platform->low_registers.waitcnt.sram < 4u);
  static const uint_fast8_t values[4] = {4u, 3u, 2u, 8u};
  return values[platform->low_registers.waitcnt.sram];
}

uint_fast8_t GbaPlatformRom0FirstAccessWaitCycles(const GbaPlatform *platform) {
  assert(platform->low_registers.waitcnt.rom_0_first_access < 4u);
  static const uint_fast8_t values[4] = {4u, 3u, 2u, 8u};
  return values[platform->low_registers.waitcnt.rom_0_first_access];
}

uint_fast8_t GbaPlatformRom0SecondAccessWaitCycles(
    const GbaPlatform *platform) {
  static const uint_fast8_t values[2] = {2u, 1u};
  return values[platform->low_registers.waitcnt.rom_0_second_access];
}

uint_fast8_t GbaPlatformRom1FirstAccessWaitCycles(const GbaPlatform *platform) {
  assert(platform->low_registers.waitcnt.rom_1_first_access < 4u);
  static const uint_fast8_t values[4] = {4u, 3u, 2u, 8u};
  return values[platform->low_registers.waitcnt.rom_1_first_access];
}

uint_fast8_t GbaPlatformRom1SecondAccessWaitCycles(
    const GbaPlatform *platform) {
  static const uint_fast8_t values[2] = {4u, 1u};
  return values[platform->low_registers.waitcnt.rom_1_second_access];
}

uint_fast8_t GbaPlatformRom2FirstAccessWaitCycles(const GbaPlatform *platform) {
  assert(platform->low_registers.waitcnt.rom_2_first_access < 4u);
  static const uint_fast8_t values[4] = {4u, 3u, 2u, 8u};
  return values[platform->low_registers.waitcnt.rom_2_first_access];
}

uint_fast8_t GbaPlatformRom2SecondAccessWaitCycles(
    const GbaPlatform *platform) {
  static const uint_fast8_t values[2] = {8u, 1u};
  return values[platform->low_registers.waitcnt.rom_2_second_access];
}

bool GbaPlatformRomPrefetch(const GbaPlatform *platform) {
  return platform->low_registers.waitcnt.gamepak_prefetch;
}

GbaPowerState GbaPlatformPowerState(const GbaPlatform *platform) {
  return platform->power_state;
}

void GbaPlatformRetain(GbaPlatform *platform) {
  assert(platform->reference_count != UINT16_MAX);
  platform->reference_count += 1u;
}

void GbaPlatformRelease(GbaPlatform *platform) {
  assert(platform->reference_count);
  platform->reference_count -= 1u;
  if (platform->reference_count == 0u) {
    free(platform);
  }
}

static_assert(sizeof(GbaPlatformLowRegisters) == LOW_REGISTERS_SIZE,
              "sizeof(GbaPlatformLowRegisters) != LOW_REGISTERS_SIZE");