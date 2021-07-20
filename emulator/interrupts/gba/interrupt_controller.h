#ifndef _WEBGBA_EMULATOR_INTERRUPTS_GBA_INTERRUPT_CONTROLLER_
#define _WEBGBA_EMULATOR_INTERRUPTS_GBA_INTERRUPT_CONTROLLER_

#include <stdint.h>

#include "emulator/interrupts/interrupt_line.h"

typedef struct _GBAInterruptController GBAInterruptController;

bool GbaInterruptControllerAllocate(
    GBAInterruptController **interrupt_controller, InterruptLine **rst_line,
    InterruptLine **fiq_line, InterruptLine **irq_line);

uint16_t GbaInterruptControllerReadInterruptMasterEnable(
    GBAInterruptController *interrupt_controller);

void GbaInterruptControllerWriteInterruptMasterEnable(
    GBAInterruptController *interrupt_controller, uint16_t value);

uint16_t GbaInterruptControllerReadInterruptEnable(
    GBAInterruptController *interrupt_controller);

void GbaInterruptControllerWriteInterruptEnable(
    GBAInterruptController *interrupt_controller, uint16_t value);

uint16_t GbaInterruptControllerReadInterruptRequestFlags(
    GBAInterruptController *interrupt_controller);

void GbaInterruptControllerInterruptAcknowledge(
    GBAInterruptController *interrupt_controller, uint16_t value);

void GbaInterruptControllerRaiseVBlankInterrupt(
    GBAInterruptController *interrupt_controller);

void GbaInterruptControllerRaiseHBlankInterrupt(
    GBAInterruptController *interrupt_controller);

void GbaInterruptControllerRaiseVBlankCountInterrupt(
    GBAInterruptController *interrupt_controller);

void GbaInterruptControllerRaiseTimer0Interrupt(
    GBAInterruptController *interrupt_controller);

void GbaInterruptControllerRaiseTimer1Interrupt(
    GBAInterruptController *interrupt_controller);

void GbaInterruptControllerRaiseTimer2Interrupt(
    GBAInterruptController *interrupt_controller);

void GbaInterruptControllerRaiseTimer3Interrupt(
    GBAInterruptController *interrupt_controller);

void GbaInterruptControllerRaiseSerialInterrupt(
    GBAInterruptController *interrupt_controller);

void GbaInterruptControllerRaiseDma0Interrupt(
    GBAInterruptController *interrupt_controller);

void GbaInterruptControllerRaiseDma1Interrupt(
    GBAInterruptController *interrupt_controller);

void GbaInterruptControllerRaiseDma2Interrupt(
    GBAInterruptController *interrupt_controller);

void GbaInterruptControllerRaiseDma3Interrupt(
    GBAInterruptController *interrupt_controller);

void GbaInterruptControllerRaiseKeypadInterrupt(
    GBAInterruptController *interrupt_controller);

void GbaInterruptControllerRaiseCartridgeInterrupt(
    GBAInterruptController *interrupt_controller);

void GbaInterruptControllerRetain(GBAInterruptController *interrupt_controller);

void GbaInterruptControllerRelease(
    GBAInterruptController *interrupt_controller);

#endif  // _WEBGBA_EMULATOR_INTERRUPTS_GBA_INTERRUPT_CONTROLLER_