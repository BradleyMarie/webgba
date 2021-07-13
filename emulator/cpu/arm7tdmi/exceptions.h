#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_EXCEPTIONS_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_EXCEPTIONS_

#include "emulator/cpu/arm7tdmi/registers.h"

void ArmExceptionDataABT(ArmAllRegisters* registers);
void ArmExceptionPrefetchABT(ArmAllRegisters* registers);
void ArmExceptionFIQ(ArmAllRegisters* registers);
void ArmExceptionIRQ(ArmAllRegisters* registers);
void ArmExceptionRST(ArmAllRegisters* registers);
void ArmExceptionSWI(ArmAllRegisters* registers);
void ArmExceptionUND(ArmAllRegisters* registers);

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_EXCEPTIONS_