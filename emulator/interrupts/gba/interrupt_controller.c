#include "emulator/interrupts/gba/interrupt_controller.h"

#include <assert.h>
#include <stdlib.h>
#include <strings.h>

#define INTERRUPT_CONTROLLER_REGISTERS_SIZE 12u

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
  GbaInterruptControllerRegisters interrupt_controller_registers;
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

  *rst_line = InterruptLineAllocate(NULL, GbaRstFiqLineIsRaisedFunction, NULL);
  if (*rst_line == NULL) {
    GbaInterruptControllerRelease(*interrupt_controller);
    return false;
  }

  *fiq_line = InterruptLineAllocate(NULL, GbaRstFiqLineIsRaisedFunction, NULL);
  if (*fiq_line == NULL) {
    InterruptLineFree(*rst_line);
    GbaInterruptControllerRelease(*interrupt_controller);
    return false;
  }

  *irq_line = InterruptLineAllocate(*interrupt_controller,
                                    GbaIrqLineIsRaisedFunction, GbaIrqLineFree);
  if (*irq_line == NULL) {
    InterruptLineFree(*fiq_line);
    InterruptLineFree(*rst_line);
    GbaInterruptControllerRelease(*interrupt_controller);
    return false;
  }

  GbaInterruptControllerRetain(*interrupt_controller);

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