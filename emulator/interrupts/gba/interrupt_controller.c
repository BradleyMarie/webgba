#include "emulator/interrupts/gba/interrupt_controller.h"

#include <assert.h>
#include <stdlib.h>
#include <strings.h>

#define INTERRUPT_CONTROLLER_REGISTERS_SIZE 12u

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

typedef struct {
  GbaInterruptRegister interrupt_enable;
  GbaInterruptRegister interrupt_flags;
  uint16_t waitcnt;
  uint16_t unused0;
  GbaInterruptMasterEnableRegister interrupt_master_enable;
  uint16_t unused1;
} GbaInterruptControllerRegisters;

struct _GbaInterruptController {
  union {
    GbaInterruptControllerRegisters interrupt_controller_registers;
    uint16_t interrupt_controller_register_half_words[6];
  };
  uint16_t reference_count;
};

bool GbaIrqLineIsRaisedFunction(const void *context) {
  const GbaInterruptController *controller =
      (const GbaInterruptController *)context;
  return controller->interrupt_controller_registers.interrupt_master_enable
             .enabled &&
         controller->interrupt_controller_registers.interrupt_enable.value &
             controller->interrupt_controller_registers.interrupt_flags.value;
}

void GbaIrqLineFree(void *context) {
  GbaInterruptController *controller = (GbaInterruptController *)context;
  GbaInterruptControllerRelease(controller);
}

bool GbaRstFiqLineIsRaisedFunction(const void *context) { return false; }

static bool GbaInterruptControllerRegistersLoad16LEFunction(const void *context,
                                                            uint32_t address,
                                                            uint16_t *value) {
  assert(address <= UINT32_MAX - 2u &&
         address + 2u <= INTERRUPT_CONTROLLER_REGISTERS_SIZE);

  const GbaInterruptController *interrupt_controller =
      (const GbaInterruptController *)context;

  assert((address & 0x1u) == 0u);
  switch (address) {
    case IE_OFFSET:
      *value = interrupt_controller->interrupt_controller_registers
                   .interrupt_enable.value;
      return true;
    case IF_OFFSET:
      *value = interrupt_controller->interrupt_controller_registers
                   .interrupt_flags.value;
      return true;
    case WAITCNT_OFFSET:
      *value = interrupt_controller->interrupt_controller_registers.waitcnt;
      return true;
    case IME_OFFSET:
      *value = interrupt_controller->interrupt_controller_registers
                   .interrupt_master_enable.value;
      return true;
  }

  return false;
}

static bool GbaInterruptControllerRegistersLoad32LEFunction(const void *context,
                                                            uint32_t address,
                                                            uint32_t *value) {
  assert(address <= UINT32_MAX - 4u &&
         address + 4u <= INTERRUPT_CONTROLLER_REGISTERS_SIZE);

  uint16_t low_bits;
  bool low = GbaInterruptControllerRegistersLoad16LEFunction(context, address,
                                                             &low_bits);
  if (!low) {
    return false;
  }

  uint16_t high_bits;
  bool high = GbaInterruptControllerRegistersLoad16LEFunction(
      context, address + 2u, &high_bits);
  if (high) {
    *value |= (((uint32_t)high_bits) << 16u) | (uint32_t)low_bits;
  } else {
    *value = 0u;
  }

  return true;
}

static bool GbaInterruptControllerRegistersLoad8Function(const void *context,
                                                         uint32_t address,
                                                         uint8_t *value) {
  assert(address <= UINT32_MAX - 1u &&
         address + 1u <= INTERRUPT_CONTROLLER_REGISTERS_SIZE);

  uint32_t read_address = address & 0xFFFFFFFEu;

  uint16_t value16;
  bool success = GbaInterruptControllerRegistersLoad16LEFunction(
      context, read_address, &value16);
  if (success) {
    *value = (address == read_address) ? value16 : value16 >> 8u;
  }

  return success;
}

static bool GbaInterruptControllerRegistersStore16LEFunction(void *context,
                                                             uint32_t address,
                                                             uint16_t value) {
  assert(address <= UINT32_MAX - 2u &&
         address + 2u <= INTERRUPT_CONTROLLER_REGISTERS_SIZE);

  GbaInterruptController *interrupt_controller =
      (GbaInterruptController *)context;

  assert((address & 0x1u) == 0u);

  // If address equals IF_OFFSET, the write contains the bits to disable in
  // the register and uses different logic than writes to other registers.
  if (address == IF_OFFSET) {
    interrupt_controller
        ->interrupt_controller_register_half_words[address >> 1u] &= ~value;
  } else {
    interrupt_controller
        ->interrupt_controller_register_half_words[address >> 1u] = value;
  }

  return true;
}

static bool GbaInterruptControllerRegistersStore32LEFunction(void *context,
                                                             uint32_t address,
                                                             uint32_t value) {
  assert(address <= UINT32_MAX - 4u &&
         address + 4u <= INTERRUPT_CONTROLLER_REGISTERS_SIZE);

  GbaInterruptControllerRegistersStore16LEFunction(context, address, value);
  GbaInterruptControllerRegistersStore16LEFunction(context, address + 2u,
                                                   value >> 16u);

  return true;
}

static bool GbaInterruptControllerRegistersStore8Function(void *context,
                                                          uint32_t address,
                                                          uint8_t value) {
  assert(address <= UINT32_MAX - 1u &&
         address + 1u <= INTERRUPT_CONTROLLER_REGISTERS_SIZE);

  GbaInterruptController *interrupt_controller =
      (GbaInterruptController *)context;

  uint32_t read_address = address & 0xFFFFFFFEu;

  uint16_t value16;
  if (read_address == IF_OFFSET) {
    value16 = (address == read_address) ? value : (uint16_t)value << 8u;
  } else {
    value16 =
        interrupt_controller
            ->interrupt_controller_register_half_words[read_address >> 1u];
    if (address == read_address) {
      value16 &= 0xFF00;
      value16 |= value;
    } else {
      value16 &= 0x00FF;
      value16 |= (uint16_t)value << 8u;
    }
  }

  GbaInterruptControllerRegistersStore16LEFunction(context, read_address,
                                                   value16);

  return true;
}

void GbaInterruptControllerRegistersFree(void *context) {
  GbaInterruptController *interrupt_controller =
      (GbaInterruptController *)context;
  GbaInterruptControllerRelease(interrupt_controller);
}

bool GbaInterruptControllerAllocate(
    GbaInterruptController **interrupt_controller, Memory **registers,
    InterruptLine **rst_line, InterruptLine **fiq_line,
    InterruptLine **irq_line) {
  *interrupt_controller =
      (GbaInterruptController *)calloc(1, sizeof(GbaInterruptController));
  if (*interrupt_controller == NULL) {
    return false;
  }

  (*interrupt_controller)->reference_count = 1;

  *registers = MemoryAllocate(*interrupt_controller,
                              GbaInterruptControllerRegistersLoad32LEFunction,
                              GbaInterruptControllerRegistersLoad16LEFunction,
                              GbaInterruptControllerRegistersLoad8Function,
                              GbaInterruptControllerRegistersStore32LEFunction,
                              GbaInterruptControllerRegistersStore16LEFunction,
                              GbaInterruptControllerRegistersStore8Function,
                              GbaInterruptControllerRegistersFree);

  if (*registers == NULL) {
    GbaInterruptControllerRelease(*interrupt_controller);
    return false;
  }

  (*interrupt_controller)->reference_count += 1;

  *rst_line = InterruptLineAllocate(NULL, GbaRstFiqLineIsRaisedFunction, NULL);
  if (*rst_line == NULL) {
    MemoryFree(*registers);
    GbaInterruptControllerRelease(*interrupt_controller);
    return false;
  }

  *fiq_line = InterruptLineAllocate(NULL, GbaRstFiqLineIsRaisedFunction, NULL);
  if (*fiq_line == NULL) {
    InterruptLineFree(*rst_line);
    MemoryFree(*registers);
    GbaInterruptControllerRelease(*interrupt_controller);
    return false;
  }

  *irq_line = InterruptLineAllocate(*interrupt_controller,
                                    GbaIrqLineIsRaisedFunction, GbaIrqLineFree);
  if (*irq_line == NULL) {
    InterruptLineFree(*fiq_line);
    InterruptLineFree(*rst_line);
    MemoryFree(*registers);
    GbaInterruptControllerRelease(*interrupt_controller);
    return false;
  }

  (*interrupt_controller)->reference_count += 1;

  return true;
}

uint16_t GbaInterruptControllerReadInterruptMasterEnable(
    GbaInterruptController *interrupt_controller) {
  return interrupt_controller->interrupt_controller_registers
      .interrupt_master_enable.value;
}

void GbaInterruptControllerWriteInterruptMasterEnable(
    GbaInterruptController *interrupt_controller, uint16_t value) {
  interrupt_controller->interrupt_controller_registers.interrupt_master_enable
      .value = value & 0x1u;
}

uint16_t GbaInterruptControllerReadInterruptEnable(
    GbaInterruptController *interrupt_controller) {
  return interrupt_controller->interrupt_controller_registers.interrupt_enable
      .value;
}

void GbaInterruptControllerWriteInterruptEnable(
    GbaInterruptController *interrupt_controller, uint16_t value) {
  interrupt_controller->interrupt_controller_registers.interrupt_enable.value =
      value;
}

uint16_t GbaInterruptControllerReadInterruptRequestFlags(
    GbaInterruptController *interrupt_controller) {
  return interrupt_controller->interrupt_controller_registers.interrupt_flags
      .value;
}

void GbaInterruptControllerInterruptAcknowledge(
    GbaInterruptController *interrupt_controller, uint16_t value) {
  interrupt_controller->interrupt_controller_registers.interrupt_flags.value &=
      ~value;
}

void GbaInterruptControllerRaiseVBlankInterrupt(
    GbaInterruptController *interrupt_controller) {
  interrupt_controller->interrupt_controller_registers.interrupt_flags.vblank =
      true;
}

void GbaInterruptControllerRaiseHBlankInterrupt(
    GbaInterruptController *interrupt_controller) {
  interrupt_controller->interrupt_controller_registers.interrupt_flags.hblank =
      true;
}

void GbaInterruptControllerRaiseVBlankCountInterrupt(
    GbaInterruptController *interrupt_controller) {
  interrupt_controller->interrupt_controller_registers.interrupt_flags
      .vblank_count = true;
}

void GbaInterruptControllerRaiseTimer0Interrupt(
    GbaInterruptController *interrupt_controller) {
  interrupt_controller->interrupt_controller_registers.interrupt_flags.timer0 =
      true;
}

void GbaInterruptControllerRaiseTimer1Interrupt(
    GbaInterruptController *interrupt_controller) {
  interrupt_controller->interrupt_controller_registers.interrupt_flags.timer1 =
      true;
}

void GbaInterruptControllerRaiseTimer2Interrupt(
    GbaInterruptController *interrupt_controller) {
  interrupt_controller->interrupt_controller_registers.interrupt_flags.timer2 =
      true;
}

void GbaInterruptControllerRaiseTimer3Interrupt(
    GbaInterruptController *interrupt_controller) {
  interrupt_controller->interrupt_controller_registers.interrupt_flags.timer3 =
      true;
}

void GbaInterruptControllerRaiseSerialInterrupt(
    GbaInterruptController *interrupt_controller) {
  interrupt_controller->interrupt_controller_registers.interrupt_flags.serial =
      true;
}

void GbaInterruptControllerRaiseDma0Interrupt(
    GbaInterruptController *interrupt_controller) {
  interrupt_controller->interrupt_controller_registers.interrupt_flags.dma0 =
      true;
}

void GbaInterruptControllerRaiseDma1Interrupt(
    GbaInterruptController *interrupt_controller) {
  interrupt_controller->interrupt_controller_registers.interrupt_flags.dma1 =
      true;
}

void GbaInterruptControllerRaiseDma2Interrupt(
    GbaInterruptController *interrupt_controller) {
  interrupt_controller->interrupt_controller_registers.interrupt_flags.dma2 =
      true;
}

void GbaInterruptControllerRaiseDma3Interrupt(
    GbaInterruptController *interrupt_controller) {
  interrupt_controller->interrupt_controller_registers.interrupt_flags.dma3 =
      true;
}

void GbaInterruptControllerRaiseKeypadInterrupt(
    GbaInterruptController *interrupt_controller) {
  interrupt_controller->interrupt_controller_registers.interrupt_flags.keypad =
      true;
}

void GbaInterruptControllerRaiseCartridgeInterrupt(
    GbaInterruptController *interrupt_controller) {
  interrupt_controller->interrupt_controller_registers.interrupt_flags
      .cartridge = true;
}

void GbaInterruptControllerRetain(
    GbaInterruptController *interrupt_controller) {
  assert(interrupt_controller->reference_count != UINT16_MAX);
  interrupt_controller->reference_count += 1u;
}

void GbaInterruptControllerRelease(
    GbaInterruptController *interrupt_controller) {
  assert(interrupt_controller->reference_count != 0u);
  interrupt_controller->reference_count -= 1u;
  if (interrupt_controller->reference_count == 0u) {
    free(interrupt_controller);
  }
}

static_assert(sizeof(GbaInterruptControllerRegisters) ==
                  INTERRUPT_CONTROLLER_REGISTERS_SIZE,
              "sizeof(GbaInterruptControllerRegisters) != "
              "INTERRUPT_CONTROLLER_REGISTERS_SIZE");