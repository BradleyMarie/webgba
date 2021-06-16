#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_BRANCH_EXCHANGE_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_BRANCH_EXCHANGE_

#include "emulator/cpu/arm7tdmi/arm7tdmi.h"

static inline void ArmBX(ArmUserRegisters *registers, ArmRegisterIndex Rm) {
  registers->cpsr.thumb = registers->gprs.gprs[Rm] & 1u;
  registers->gprs.pc = registers->gprs.gprs[Rm] & (~1u);
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_BRANCH_EXCHANGE_