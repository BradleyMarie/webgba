#ifndef _WEBGBA_EMULATOR_INTERRUPTS_GBA_INTERRUPT_CONTROLLER_
#define _WEBGBA_EMULATOR_INTERRUPTS_GBA_INTERRUPT_CONTROLLER_

#include <stdint.h>

#include "emulator/interrupts/interrupt_line.h"

typedef struct _GbaInterruptController GbaInterruptController;

bool GbaInterruptControllerAllocate(
    GbaInterruptController **interrupt_controller, InterruptLine **rst_line,
    InterruptLine **fiq_line, InterruptLine **irq_line);

uint16_t GbaInterruptControllerReadInterruptMasterEnable(
    GbaInterruptController *interrupt_controller);

void GbaInterruptControllerWriteInterruptMasterEnable(
    GbaInterruptController *interrupt_controller, uint16_t value);

uint16_t GbaInterruptControllerReadInterruptEnable(
    GbaInterruptController *interrupt_controller);

void GbaInterruptControllerWriteInterruptEnable(
    GbaInterruptController *interrupt_controller, uint16_t value);

uint16_t GbaInterruptControllerReadInterruptRequestFlags(
    GbaInterruptController *interrupt_controller);

void GbaInterruptControllerInterruptAcknowledge(
    GbaInterruptController *interrupt_controller, uint16_t value);

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