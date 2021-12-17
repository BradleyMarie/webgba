#include "emulator/platform/gba/platform.h"

#include <assert.h>
#include <stdlib.h>
#include <strings.h>

#define STOP_MASK 0x3080u

#define IE_OFFSET 0x0u
#define IF_OFFSET 0x2u
#define WAITCNT_OFFSET 0x4u
#define IME_OFFSET 0x8u
#define POSTFLG_OFFSET 0x100u
#define HALTCNT_OFFSET 0x101u

typedef union {
  struct {
    bool vblank : 1;
    bool hblank : 1;
    bool vblank_count : 1;
    unsigned char timers : 4;
    bool serial : 1;
    unsigned char dmas : 4;
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

typedef union {
  uint8_t bytes[4];
  uint16_t half_words[2];
  uint32_t value;
} GbaInternalMemoryControlRegister;

typedef struct {
  GbaInterruptRegister interrupt_enable;
  GbaInterruptRegister interrupt_flags;
  GbaInterruptMasterEnableRegister interrupt_master_enable;
  GbaWaitstateControlRegisters waitcnt;
  uint8_t postflg;
  GbaInternalMemoryControlRegister memory_control;
} GbaPlatformRegisters;

struct _GbaPlatform {
  GbaPlatformRegisters registers;
  GbaPowerState power_state;
  InterruptLine *interrupt_line;
  uint16_t reference_count;
};

static inline bool GbaIrqLineIsRaisedFunction(const GbaPlatform *controller) {
  return (controller->registers.interrupt_enable.value &
          controller->registers.interrupt_flags.value) &&
         controller->registers.interrupt_master_enable.enabled;
}

static bool GbaPlatformRegisterIsMemoryControl(uint32_t address) {
  address = (address + 0x200u) % 0x10000u;
  return 0x800u <= address && address < 0x804u;
}

static void GbaPlatformRegisterWriteMemoryControlByte(GbaPlatform *platform,
                                                      uint32_t address,
                                                      uint8_t value) {
  switch ((address + 0x200u) % 0x10000u) {
    case 0x800u:
      platform->registers.memory_control.bytes[0] = value;
      break;
    case 0x801u:
      platform->registers.memory_control.bytes[1] = value;
      break;
    case 0x802u:
      platform->registers.memory_control.bytes[2] = value;
      break;
    case 0x803u:
      platform->registers.memory_control.bytes[3] = value;
      break;
    default:
      assert(false);
  }
}

static void GbaPlatformRegisterWriteMemoryControlHalfWord(GbaPlatform *platform,
                                                          uint32_t address,
                                                          uint16_t value) {
  assert((address & 0x1u) == 0u);
  GbaPlatformRegisterWriteMemoryControlByte(platform, address, value);
  GbaPlatformRegisterWriteMemoryControlByte(platform, address + 1u,
                                            value >> 8u);
}

static void GbaPlatformRegisterWriteHaltCnt(GbaPlatform *platform,
                                            uint8_t value) {
  value &= 0x80;
  if (value) {  // Stop
    if ((platform->registers.interrupt_enable.value &
         platform->registers.interrupt_flags.value & STOP_MASK) == 0) {
      platform->power_state = GBA_POWER_STATE_STOP;
    }
  } else {  // Halt
    if ((platform->registers.interrupt_enable.value &
         platform->registers.interrupt_flags.value) == 0) {
      platform->power_state = GBA_POWER_STATE_HALT;
    }
  }
}

static bool GbaPlatformRegistersLoad16LEFunction(const void *context,
                                                 uint32_t address,
                                                 uint16_t *value) {
  assert((address & 0x1u) == 0u);

  const GbaPlatform *platform = (const GbaPlatform *)context;

  switch (address) {
    case IE_OFFSET:
      *value = platform->registers.interrupt_enable.value;
      return true;
    case IF_OFFSET:
      *value = platform->registers.interrupt_flags.value;
      return true;
    case WAITCNT_OFFSET:
      *value = platform->registers.waitcnt.value;
      return true;
    case IME_OFFSET:
      *value = platform->registers.interrupt_master_enable.value;
      return true;
    case POSTFLG_OFFSET:
      *value = 0u;
      return true;
  }

  if (GbaPlatformRegisterIsMemoryControl(address)) {
    *value =
        platform->registers.memory_control.half_words[(address >> 1u) & 1u];
    return true;
  }

  return false;
}

static bool GbaPlatformRegistersLoad32LEFunction(const void *context,
                                                 uint32_t address,
                                                 uint32_t *value) {
  assert((address & 0x3u) == 0u);

  uint16_t low_bits;
  bool low = GbaPlatformRegistersLoad16LEFunction(context, address, &low_bits);
  if (!low) {
    return false;
  }

  uint16_t high_bits;
  bool high =
      GbaPlatformRegistersLoad16LEFunction(context, address + 2u, &high_bits);
  if (high) {
    *value = (((uint32_t)high_bits) << 16u) | (uint32_t)low_bits;
  } else {
    *value = low_bits;
  }

  return true;
}

static bool GbaPlatformRegistersLoad8Function(const void *context,
                                              uint32_t address,
                                              uint8_t *value) {
  const GbaPlatform *platform = (const GbaPlatform *)context;

  if (address == POSTFLG_OFFSET) {
    *value = platform->registers.postflg;
    return true;
  }

  uint32_t read_address = address & 0xFFFFFFFEu;

  uint16_t value16;
  bool success =
      GbaPlatformRegistersLoad16LEFunction(context, read_address, &value16);
  if (success) {
    *value = (address == read_address) ? value16 : value16 >> 8u;
  }

  return success;
}

static bool GbaPlatformRegistersStore16LEFunction(void *context,
                                                  uint32_t address,
                                                  uint16_t value) {
  assert((address & 0x1u) == 0u);

  GbaPlatform *platform = (GbaPlatform *)context;

  bool raised;
  switch (address) {
    case IE_OFFSET:
      platform->registers.interrupt_enable.value = value;
      raised = GbaIrqLineIsRaisedFunction(platform);
      InterruptLineSetLevel(platform->interrupt_line, raised);
      return true;
    case IF_OFFSET:
      platform->registers.interrupt_flags.value &= ~value;
      raised = GbaIrqLineIsRaisedFunction(platform);
      InterruptLineSetLevel(platform->interrupt_line, raised);
      return true;
    case WAITCNT_OFFSET:
      platform->registers.waitcnt.value = value & 0x7FFFu;
      return true;
    case IME_OFFSET:
      platform->registers.interrupt_master_enable.value = value & 1u;
      raised = GbaIrqLineIsRaisedFunction(platform);
      InterruptLineSetLevel(platform->interrupt_line, raised);
      return true;
    case POSTFLG_OFFSET:
      platform->registers.postflg = value;
      GbaPlatformRegisterWriteHaltCnt(platform, value >> 8u);
      return true;
  }

  if (GbaPlatformRegisterIsMemoryControl(address)) {
    GbaPlatformRegisterWriteMemoryControlHalfWord(platform, address, value);
    return true;
  }

  return true;
}

static bool GbaPlatformRegistersStore32LEFunction(void *context,
                                                  uint32_t address,
                                                  uint32_t value) {
  assert((address & 0x3u) == 0u);

  GbaPlatformRegistersStore16LEFunction(context, address, value);
  GbaPlatformRegistersStore16LEFunction(context, address + 2u, value >> 16u);

  return true;
}

static bool GbaPlatformRegistersStore8Function(void *context, uint32_t address,
                                               uint8_t value) {
  GbaPlatform *platform = (GbaPlatform *)context;

  switch (address) {
    case POSTFLG_OFFSET:
      platform->registers.postflg = value;
      return true;
    case HALTCNT_OFFSET:
      GbaPlatformRegisterWriteHaltCnt(platform, value);
      return true;
  }

  if (GbaPlatformRegisterIsMemoryControl(address)) {
    GbaPlatformRegisterWriteMemoryControlByte(platform, address, value);
    return true;
  }

  uint32_t read_address = address & 0xFFFFFFFEu;

  uint16_t value16;
  switch (address) {
    case IE_OFFSET:
      value16 = platform->registers.interrupt_enable.value;
      break;
    case IF_OFFSET:
      value16 = 0u;
      break;
    case WAITCNT_OFFSET:
      value16 = platform->registers.waitcnt.value;
      break;
    case IME_OFFSET:
      value16 = platform->registers.interrupt_master_enable.value;
      break;
  }

  if (address == read_address) {
    value16 &= 0xFF00;
    value16 |= value;
  } else {
    value16 &= 0x00FF;
    value16 |= (uint16_t)value << 8u;
  }

  GbaPlatformRegistersStore16LEFunction(context, read_address, value16);

  return true;
}

void GbaPlatformRegistersFree(void *context) {
  GbaPlatform *platform = (GbaPlatform *)context;
  GbaPlatformRelease(platform);
}

bool GbaPlatformAllocate(InterruptLine *irq_line, GbaPlatform **platform,
                         Memory **registers) {
  *platform = (GbaPlatform *)calloc(1, sizeof(GbaPlatform));
  if (*platform == NULL) {
    return false;
  }

  (*platform)->interrupt_line = irq_line;
  (*platform)->reference_count = 2u;

  *registers = MemoryAllocate(
      *platform, GbaPlatformRegistersLoad32LEFunction,
      GbaPlatformRegistersLoad16LEFunction, GbaPlatformRegistersLoad8Function,
      GbaPlatformRegistersStore32LEFunction,
      GbaPlatformRegistersStore16LEFunction, GbaPlatformRegistersStore8Function,
      GbaPlatformRegistersFree);

  if (*registers == NULL) {
    free(*platform);
    return false;
  }

  return true;
}

void GbaPlatformRaiseVBlankInterrupt(GbaPlatform *platform) {
  platform->registers.interrupt_flags.vblank = true;

  bool raised = GbaIrqLineIsRaisedFunction(platform);
  InterruptLineSetLevel(platform->interrupt_line, raised);

  if (platform->power_state == GBA_POWER_STATE_HALT && raised) {
    platform->power_state = GBA_POWER_STATE_RUN;
  }
}

void GbaPlatformRaiseHBlankInterrupt(GbaPlatform *platform) {
  platform->registers.interrupt_flags.hblank = true;

  bool raised = GbaIrqLineIsRaisedFunction(platform);
  InterruptLineSetLevel(platform->interrupt_line, raised);

  if (platform->power_state == GBA_POWER_STATE_HALT && raised) {
    platform->power_state = GBA_POWER_STATE_RUN;
  }
}

void GbaPlatformRaiseVBlankCountInterrupt(GbaPlatform *platform) {
  platform->registers.interrupt_flags.vblank_count = true;

  bool raised = GbaIrqLineIsRaisedFunction(platform);
  InterruptLineSetLevel(platform->interrupt_line, raised);

  if (platform->power_state == GBA_POWER_STATE_HALT && raised) {
    platform->power_state = GBA_POWER_STATE_RUN;
  }
}

void GbaPlatformRaiseTimerInterrupt(GbaPlatform *platform, GbaTimerId timer) {
  assert(GBA_TIMER_0 <= timer && timer <= GBA_TIMER_3);

  platform->registers.interrupt_flags.timers |= 1u << timer;

  bool raised = GbaIrqLineIsRaisedFunction(platform);
  InterruptLineSetLevel(platform->interrupt_line, raised);

  if (platform->power_state == GBA_POWER_STATE_HALT && raised) {
    platform->power_state = GBA_POWER_STATE_RUN;
  }
}

void GbaPlatformRaiseSerialInterrupt(GbaPlatform *platform) {
  platform->registers.interrupt_flags.serial = true;

  bool raised = GbaIrqLineIsRaisedFunction(platform);
  InterruptLineSetLevel(platform->interrupt_line, raised);

  if (platform->power_state != GBA_POWER_STATE_RUN) {
    const static uint16_t masks[3] = {0xFFu, 0xFFu, STOP_MASK};
    assert(platform->power_state < 3u);
    if (platform->registers.interrupt_enable.value &
        platform->registers.interrupt_flags.value &
        masks[platform->power_state]) {
      platform->power_state = GBA_POWER_STATE_RUN;
    }
  }
}

void GbaPlatformRaiseDmaInterrupt(GbaPlatform *platform, GbaDmaId dma) {
  assert(GBA_DMA_0 <= dma && dma <= GBA_DMA_3);

  platform->registers.interrupt_flags.dmas |= 1u << dma;

  bool raised = GbaIrqLineIsRaisedFunction(platform);
  InterruptLineSetLevel(platform->interrupt_line, raised);

  if (platform->power_state == GBA_POWER_STATE_HALT && raised) {
    platform->power_state = GBA_POWER_STATE_RUN;
  }
}

void GbaPlatformRaiseKeypadInterrupt(GbaPlatform *platform) {
  platform->registers.interrupt_flags.keypad = true;

  bool raised = GbaIrqLineIsRaisedFunction(platform);
  InterruptLineSetLevel(platform->interrupt_line, raised);

  if (platform->power_state != GBA_POWER_STATE_RUN) {
    const static uint16_t masks[3] = {0xFFu, 0xFFu, STOP_MASK};
    assert(platform->power_state < 3u);
    if (platform->registers.interrupt_enable.value &
        platform->registers.interrupt_flags.value &
        masks[platform->power_state]) {
      platform->power_state = GBA_POWER_STATE_RUN;
    }
  }
}

void GbaPlatformRaiseCartridgeInterrupt(GbaPlatform *platform) {
  platform->registers.interrupt_flags.cartridge = true;

  bool raised = GbaIrqLineIsRaisedFunction(platform);
  InterruptLineSetLevel(platform->interrupt_line, raised);

  if (platform->power_state != GBA_POWER_STATE_RUN) {
    const static uint16_t masks[3] = {0xFFu, 0xFFu, STOP_MASK};
    assert(platform->power_state < 3u);
    if (platform->registers.interrupt_enable.value &
        platform->registers.interrupt_flags.value &
        masks[platform->power_state]) {
      platform->power_state = GBA_POWER_STATE_RUN;
    }
  }
}

uint_fast8_t GbaPlatformSramWaitStateCycles(const GbaPlatform *platform) {
  assert(platform->registers.waitcnt.sram < 4u);
  static const uint_fast8_t values[4] = {4u, 3u, 2u, 8u};
  return values[platform->registers.waitcnt.sram];
}

uint_fast8_t GbaPlatformRom0FirstAccessWaitCycles(const GbaPlatform *platform) {
  assert(platform->registers.waitcnt.rom_0_first_access < 4u);
  static const uint_fast8_t values[4] = {4u, 3u, 2u, 8u};
  return values[platform->registers.waitcnt.rom_0_first_access];
}

uint_fast8_t GbaPlatformRom0SecondAccessWaitCycles(
    const GbaPlatform *platform) {
  static const uint_fast8_t values[2] = {2u, 1u};
  return values[platform->registers.waitcnt.rom_0_second_access];
}

uint_fast8_t GbaPlatformRom1FirstAccessWaitCycles(const GbaPlatform *platform) {
  assert(platform->registers.waitcnt.rom_1_first_access < 4u);
  static const uint_fast8_t values[4] = {4u, 3u, 2u, 8u};
  return values[platform->registers.waitcnt.rom_1_first_access];
}

uint_fast8_t GbaPlatformRom1SecondAccessWaitCycles(
    const GbaPlatform *platform) {
  static const uint_fast8_t values[2] = {4u, 1u};
  return values[platform->registers.waitcnt.rom_1_second_access];
}

uint_fast8_t GbaPlatformRom2FirstAccessWaitCycles(const GbaPlatform *platform) {
  assert(platform->registers.waitcnt.rom_2_first_access < 4u);
  static const uint_fast8_t values[4] = {4u, 3u, 2u, 8u};
  return values[platform->registers.waitcnt.rom_2_first_access];
}

uint_fast8_t GbaPlatformRom2SecondAccessWaitCycles(
    const GbaPlatform *platform) {
  static const uint_fast8_t values[2] = {8u, 1u};
  return values[platform->registers.waitcnt.rom_2_second_access];
}

bool GbaPlatformRomPrefetch(const GbaPlatform *platform) {
  return platform->registers.waitcnt.gamepak_prefetch;
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
    InterruptLineFree(platform->interrupt_line);
    free(platform);
  }
}