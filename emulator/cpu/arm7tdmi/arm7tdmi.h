#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_ARM7TDMI_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_ARM7TDMI_

#include "emulator/cpu/arm7tdmi/registers.h"
#include "emulator/memory/memory.h"

typedef struct {
  ArmAllRegisters registers;
  bool pending_irq;
  bool pending_fiq;
  bool pending_rst;
} ArmCpu;

void ArmCpuStep(ArmCpu* cpu, Memory* memory);
void ArmCpuRun(ArmCpu* cpu, Memory* memory, uint32_t num_steps);

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_ARM7TDMI_