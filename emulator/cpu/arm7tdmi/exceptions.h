#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_EXCEPTIONS_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_EXCEPTIONS_

#include "emulator/cpu/arm7tdmi/arm7tdmi.h"

void ArmExceptionUND(ArmAllRegisters* registers);
void ArmExceptionSWI(ArmAllRegisters* registers);

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_EXCEPTIONS_