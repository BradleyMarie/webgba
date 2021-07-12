#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_COPROCESSOR_REGISTER_TRANSFER_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_COPROCESSOR_REGISTER_TRANSFER_

#include "emulator/cpu/arm7tdmi/exceptions.h"

static inline void ArmMCR(ArmAllRegisters *registers) {
  ArmExceptionUND(registers);
}

static inline void ArmMRC(ArmAllRegisters *registers) {
  ArmExceptionUND(registers);
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_COPROCESSOR_REGISTER_TRANSFER_