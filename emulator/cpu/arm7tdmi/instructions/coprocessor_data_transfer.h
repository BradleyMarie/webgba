#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_COPROCESSOR_DATA_TRANSFER_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_COPROCESSOR_DATA_TRANSFER_

#include <assert.h>

#include "emulator/cpu/arm7tdmi/arm7tdmi.h"

static inline void ArmLDC(ArmAllRegisters *registers) {
  ArmExceptionUND(registers);
}

static inline void ArmSTC(ArmAllRegisters *registers) {
  ArmExceptionUND(registers);
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_COPROCESSOR_DATA_TRANSFER_