#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_ARM7TDMI_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_ARM7TDMI_

#include "emulator/cpu/arm7tdmi/registers.h"
#include "emulator/memory/memory.h"
#include "emulator/platform/interrupt_line.h"

typedef struct {
  unsigned int interrupts_raised;
  ArmAllRegisters registers;
  InterruptLine* rst;
  InterruptLine* fiq;
  InterruptLine* irq;
} Arm7Tdmi;

Arm7Tdmi* Arm7TdmiAllocate(InterruptLine* rst, InterruptLine* fiq,
                           InterruptLine* irq);
void Arm7TdmiStep(Arm7Tdmi* cpu, Memory* memory);
void Arm7TdmiFree(Arm7Tdmi* cpu);

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_ARM7TDMI_