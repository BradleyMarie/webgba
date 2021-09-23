#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_ARM7TDMI_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_ARM7TDMI_

#include "emulator/cpu/interrupt_line.h"
#include "emulator/memory/memory.h"

typedef struct _Arm7Tdmi Arm7Tdmi;

bool Arm7TdmiAllocate(Arm7Tdmi** cpu, InterruptLine** rst, InterruptLine** fiq,
                      InterruptLine** irq);

void Arm7TdmiStep(Arm7Tdmi* cpu, Memory* memory);

void Arm7TdmiFree(Arm7Tdmi* cpu);

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_ARM7TDMI_