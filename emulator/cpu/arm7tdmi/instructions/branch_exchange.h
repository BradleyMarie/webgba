#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_BRANCH_EXCHANGE_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_BRANCH_EXCHANGE_

#include "emulator/cpu/arm7tdmi/registers.h"

static inline void ArmBX(ArmAllRegisters *registers, ArmRegisterIndex Rm) {
  registers->current.user.cpsr.thumb =
      registers->current.user.gprs.gprs[Rm] & 1u;
  registers->current.user.gprs.pc =
      registers->current.user.gprs.gprs[Rm] & 0xFFFFFFFEu;
  registers->execution_control.thumb = registers->current.user.cpsr.thumb;
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_BRANCH_EXCHANGE_