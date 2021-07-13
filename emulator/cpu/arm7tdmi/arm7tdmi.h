#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_ARM7TDMI_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_ARM7TDMI_

#include "emulator/cpu/arm7tdmi/registers.h"
#include "emulator/memory/memory.h"

typedef struct {
  ArmAllRegisters registers;
  bool pending_irq;
  bool pending_fiq;
  bool pending_rst;
} Arm7Tdmi;

Arm7Tdmi* Arm7TdmiAllocate();
void Arm7TdmiStep(Arm7Tdmi* cpu, Memory* memory);
void Arm7TdmiRun(Arm7Tdmi* cpu, Memory* memory, uint32_t num_steps);
void Arm7TdmiFree(Arm7Tdmi* cpu);

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_ARM7TDMI_