#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_SWI_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_SWI_

#include "emulator/cpu/arm7tdmi/arm7tdmi.h"

static inline void ArmSWI(ArmAllRegisters *registers) {
  ArmExceptionSWI(registers);
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_SWI_