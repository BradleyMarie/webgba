#include "emulator/interrupts/gba/interrupt_controller.h"

#include <assert.h>
#include <stdlib.h>
#include <strings.h>

typedef union {
  struct {
    bool enabled : 1;
    unsigned unused : 15;
  };
  uint16_t value;
} GbaInterruptMasterEnableRegister;

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
    unsigned unused : 2;
  };
  uint16_t value;
} GbaInterruptRegister;

struct _GBAInterruptController {
  GbaInterruptMasterEnableRegister master_enabled;
  GbaInterruptRegister enabled;
  GbaInterruptRegister flags;
  uint16_t reference_count;
};

bool GbaIrqLineIsRaisedFunction(const void *context) {
  const GBAInterruptController *controller =
      (const GBAInterruptController *)context;
  return controller->master_enabled.enabled &&
         controller->enabled.value & controller->flags.value;
}

void GbaIrqLineFree(void *context) {
  GBAInterruptController *controller = (GBAInterruptController *)context;
  GbaInterruptControllerRelease(controller);
}

bool GbaRstFiqLineIsRaisedFunction(const void *context) { return false; }

bool GbaInterruptControllerAllocate(
    GBAInterruptController **interrupt_controller, InterruptLine **rst_line,
    InterruptLine **fiq_line, InterruptLine **irq_line) {
  *interrupt_controller =
      (GBAInterruptController *)calloc(1, sizeof(GBAInterruptController));
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
    GBAInterruptController *interrupt_controller) {
  return interrupt_controller->master_enabled.value;
}

void GbaInterruptControllerWriteInterruptMasterEnable(
    GBAInterruptController *interrupt_controller, uint16_t value) {
  interrupt_controller->master_enabled.value = value & 0x1u;
}

uint16_t GbaInterruptControllerReadInterruptEnable(
    GBAInterruptController *interrupt_controller) {
  return interrupt_controller->enabled.value;
}

void GbaInterruptControllerWriteInterruptEnable(
    GBAInterruptController *interrupt_controller, uint16_t value) {
  interrupt_controller->enabled.value = value;
}

uint16_t GbaInterruptControllerReadInterruptRequestFlags(
    GBAInterruptController *interrupt_controller) {
  return interrupt_controller->flags.value;
}

void GbaInterruptControllerInterruptAcknowledge(
    GBAInterruptController *interrupt_controller, uint16_t value) {
  interrupt_controller->flags.value &= ~value;
}

void GbaInterruptControllerRaiseVBlankInterrupt(
    GBAInterruptController *interrupt_controller) {
  interrupt_controller->flags.vblank = true;
}

void GbaInterruptControllerRaiseHBlankInterrupt(
    GBAInterruptController *interrupt_controller) {
  interrupt_controller->flags.hblank = true;
}

void GbaInterruptControllerRaiseVBlankCountInterrupt(
    GBAInterruptController *interrupt_controller) {
  interrupt_controller->flags.vblank_count = true;
}

void GbaInterruptControllerRaiseTimer0Interrupt(
    GBAInterruptController *interrupt_controller) {
  interrupt_controller->flags.timer0 = true;
}

void GbaInterruptControllerRaiseTimer1Interrupt(
    GBAInterruptController *interrupt_controller) {
  interrupt_controller->flags.timer1 = true;
}

void GbaInterruptControllerRaiseTimer2Interrupt(
    GBAInterruptController *interrupt_controller) {
  interrupt_controller->flags.timer2 = true;
}

void GbaInterruptControllerRaiseTimer3Interrupt(
    GBAInterruptController *interrupt_controller) {
  interrupt_controller->flags.timer3 = true;
}

void GbaInterruptControllerRaiseSerialInterrupt(
    GBAInterruptController *interrupt_controller) {
  interrupt_controller->flags.serial = true;
}

void GbaInterruptControllerRaiseDma0Interrupt(
    GBAInterruptController *interrupt_controller) {
  interrupt_controller->flags.dma0 = true;
}

void GbaInterruptControllerRaiseDma1Interrupt(
    GBAInterruptController *interrupt_controller) {
  interrupt_controller->flags.dma1 = true;
}

void GbaInterruptControllerRaiseDma2Interrupt(
    GBAInterruptController *interrupt_controller) {
  interrupt_controller->flags.dma2 = true;
}

void GbaInterruptControllerRaiseDma3Interrupt(
    GBAInterruptController *interrupt_controller) {
  interrupt_controller->flags.dma3 = true;
}

void GbaInterruptControllerRaiseKeypadInterrupt(
    GBAInterruptController *interrupt_controller) {
  interrupt_controller->flags.keypad = true;
}

void GbaInterruptControllerRaiseCartridgeInterrupt(
    GBAInterruptController *interrupt_controller) {
  interrupt_controller->flags.cartridge = true;
}

void GbaInterruptControllerRetain(
    GBAInterruptController *interrupt_controller) {
  assert(interrupt_controller->reference_count != UINT16_MAX);
  interrupt_controller->reference_count += 1u;
}

void GbaInterruptControllerRelease(
    GBAInterruptController *interrupt_controller) {
  assert(interrupt_controller->reference_count != 0u);
  interrupt_controller->reference_count -= 1u;
  if (interrupt_controller->reference_count == 0u) {
    free(interrupt_controller);
  }
}