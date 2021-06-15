#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_MULTIPLY_LONG_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_MULTIPLY_LONG_

#include "emulator/cpu/arm7tdmi/arm7tdmi.h"

static inline void ArmUMULL(ArmGeneralPurposeRegisters *registers,
                            ArmRegisterIndex RdLo, ArmRegisterIndex RdHi,
                            ArmRegisterIndex Rm, ArmRegisterIndex Rs) {
  uint64_t product =
      (uint64_t)registers->gprs[Rm] * (uint64_t)registers->gprs[Rs];
  registers->gprs[RdLo] = (uint32_t)product;
  registers->gprs[RdHi] = (uint32_t)(product >> 32);
}

static inline void ArmUMULLS(ArmUserRegisters *registers, ArmRegisterIndex RdLo,
                             ArmRegisterIndex RdHi, ArmRegisterIndex Rm,
                             ArmRegisterIndex Rs) {
  uint64_t product =
      (uint64_t)registers->gprs.gprs[Rm] * (uint64_t)registers->gprs.gprs[Rs];
  registers->gprs.gprs[RdLo] = (uint32_t)product;
  registers->gprs.gprs[RdHi] = (uint32_t)(product >> 32);
  registers->cpsr.zero =
      ((registers->gprs.gprs[RdLo] | registers->gprs.gprs[RdHi]) == 0);
  registers->cpsr.negative = !!(registers->gprs.gprs[RdHi] & (1 << 31));
}

static inline void ArmUMLAL(ArmGeneralPurposeRegisters *registers,
                            ArmRegisterIndex RdLo, ArmRegisterIndex RdHi,
                            ArmRegisterIndex Rm, ArmRegisterIndex Rs) {
  uint64_t result =
      ((uint64_t)registers->gprs[RdHi] << 32) + (uint64_t)registers->gprs[RdLo];
  result += (uint64_t)registers->gprs[Rm] * (uint64_t)registers->gprs[Rs];
  registers->gprs[RdLo] = (uint32_t)result;
  registers->gprs[RdHi] = (uint32_t)(result >> 32);
}

static inline void ArmUMLALS(ArmUserRegisters *registers, ArmRegisterIndex RdLo,
                             ArmRegisterIndex RdHi, ArmRegisterIndex Rm,
                             ArmRegisterIndex Rs) {
  uint64_t result = ((uint64_t)registers->gprs.gprs[RdHi] << 32) +
                    (uint64_t)registers->gprs.gprs[RdLo];
  result +=
      (uint64_t)registers->gprs.gprs[Rm] * (uint64_t)registers->gprs.gprs[Rs];
  registers->gprs.gprs[RdLo] = (uint32_t)result;
  registers->gprs.gprs[RdHi] = (uint32_t)(result >> 32);
  registers->cpsr.zero =
      ((registers->gprs.gprs[RdLo] | registers->gprs.gprs[RdHi]) == 0);
  registers->cpsr.negative = !!(registers->gprs.gprs[RdHi] & (1 << 31));
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_MULTIPLY_LONG_