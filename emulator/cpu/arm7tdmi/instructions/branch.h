#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_BRANCH_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_BRANCH_

#include "emulator/cpu/arm7tdmi/registers.h"

static inline void ArmB(ArmAllRegisters *registers, uint_fast32_t offset) {
  ArmLoadProgramCounter(registers, registers->current.user.gprs.pc + offset);
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_BRANCH_