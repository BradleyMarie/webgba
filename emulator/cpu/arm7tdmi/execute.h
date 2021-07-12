#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_EXECUTE_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_EXECUTE_

#include "emulator/cpu/arm7tdmi/arm7tdmi.h"
#include "emulator/memory/memory.h"

void ArmCpuStep(ArmCpu* cpu, Memory* memory);
void ArmCpuRun(ArmCpu* cpu, Memory* memory, uint32_t num_steps);

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_EXECUTE_