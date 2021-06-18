#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_SINGLE_DATA_SWAP_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_SINGLE_DATA_SWAP_

#include "emulator/cpu/arm7tdmi/arm7tdmi.h"

static inline void ArmMRS(ArmUserRegisters *registers, ArmRegisterIndex Rd) {
  registers->gprs.gprs[Rd] = registers->cpsr.value;
}

static inline void ArmMRSR(ArmPrivilegedRegisters *registers,
                           ArmRegisterIndex Rd) {
  registers->user.gprs.gprs[Rd] = registers->spsr.value;
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_SINGLE_DATA_SWAP_