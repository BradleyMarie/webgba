#ifndef _WEBGBA_EMULATOR_INTERRUPTS_GBA_INTERRUPT_CONTROLLER_
#define _WEBGBA_EMULATOR_INTERRUPTS_GBA_INTERRUPT_CONTROLLER_

#include <stdint.h>

#include "emulator/interrupts/interrupt_line.h"
#include "emulator/memory/memory.h"

typedef struct _GbaInterruptController GbaInterruptController;

bool GbaInterruptControllerAllocate(
    GbaInterruptController **interrupt_controller, Memory **registers,
    InterruptLine **rst_line, InterruptLine **fiq_line,
    InterruptLine **irq_line);

void GbaInterruptControllerRaiseVBlankInterrupt(
    GbaInterruptController *interrupt_controller);

void GbaInterruptControllerRaiseHBlankInterrupt(
    GbaInterruptController *interrupt_controller);

void GbaInterruptControllerRaiseVBlankCountInterrupt(
    GbaInterruptController *interrupt_controller);

void GbaInterruptControllerRaiseTimer0Interrupt(
    GbaInterruptController *interrupt_controller);

void GbaInterruptControllerRaiseTimer1Interrupt(
    GbaInterruptController *interrupt_controller);

void GbaInterruptControllerRaiseTimer2Interrupt(
    GbaInterruptController *interrupt_controller);

void GbaInterruptControllerRaiseTimer3Interrupt(
    GbaInterruptController *interrupt_controller);

void GbaInterruptControllerRaiseSerialInterrupt(
    GbaInterruptController *interrupt_controller);

void GbaInterruptControllerRaiseDma0Interrupt(
    GbaInterruptController *interrupt_controller);

void GbaInterruptControllerRaiseDma1Interrupt(
    GbaInterruptController *interrupt_controller);

void GbaInterruptControllerRaiseDma2Interrupt(
    GbaInterruptController *interrupt_controller);

void GbaInterruptControllerRaiseDma3Interrupt(
    GbaInterruptController *interrupt_controller);

void GbaInterruptControllerRaiseKeypadInterrupt(
    GbaInterruptController *interrupt_controller);

void GbaInterruptControllerRaiseCartridgeInterrupt(
    GbaInterruptController *interrupt_controller);

void GbaInterruptControllerRetain(GbaInterruptController *interrupt_controller);

void GbaInterruptControllerRelease(
    GbaInterruptController *interrupt_controller);

#endif  // _WEBGBA_EMULATOR_INTERRUPTS_GBA_INTERRUPT_CONTROLLER_