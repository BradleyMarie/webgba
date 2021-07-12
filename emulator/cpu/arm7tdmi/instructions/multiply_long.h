#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_MULTIPLY_LONG_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_MULTIPLY_LONG_

#include "emulator/cpu/arm7tdmi/arm7tdmi.h"
#include "emulator/cpu/arm7tdmi/flags.h"

static inline uint64_t ArmUMULL(ArmGeneralPurposeRegisters *registers,
                                ArmRegisterIndex RdLo, ArmRegisterIndex RdHi,
                                ArmRegisterIndex Rm, ArmRegisterIndex Rs) {
  uint64_t product =
      (uint64_t)registers->gprs[Rm] * (uint64_t)registers->gprs[Rs];
  registers->gprs[RdLo] = (uint32_t)product;
  registers->gprs[RdHi] = (uint32_t)(product >> 32);
  return product;
}

static inline void ArmUMULLS(ArmUserRegisters *registers, ArmRegisterIndex RdLo,
                             ArmRegisterIndex RdHi, ArmRegisterIndex Rm,
                             ArmRegisterIndex Rs) {
  uint64_t product = ArmUMULL(&registers->gprs, RdLo, RdHi, Rm, Rs);
  registers->cpsr.zero = ArmZeroFlagUInt64(product);
  registers->cpsr.negative = ArmNegativeFlag(registers->gprs.gprs[RdHi]);
}

static inline uint64_t ArmUMLAL(ArmGeneralPurposeRegisters *registers,
                                ArmRegisterIndex RdLo, ArmRegisterIndex RdHi,
                                ArmRegisterIndex Rm, ArmRegisterIndex Rs) {
  uint64_t result =
      ((uint64_t)registers->gprs[RdHi] << 32) + (uint64_t)registers->gprs[RdLo];
  result += (uint64_t)registers->gprs[Rm] * (uint64_t)registers->gprs[Rs];
  registers->gprs[RdLo] = (uint32_t)result;
  registers->gprs[RdHi] = (uint32_t)(result >> 32);
  return result;
}

static inline void ArmUMLALS(ArmUserRegisters *registers, ArmRegisterIndex RdLo,
                             ArmRegisterIndex RdHi, ArmRegisterIndex Rm,
                             ArmRegisterIndex Rs) {
  uint64_t result = ArmUMLAL(&registers->gprs, RdLo, RdHi, Rm, Rs);
  registers->cpsr.zero = ArmZeroFlagUInt64(result);
  registers->cpsr.negative = ArmNegativeFlag(registers->gprs.gprs[RdHi]);
}

static inline int64_t ArmSMULL(ArmGeneralPurposeRegisters *registers,
                               ArmRegisterIndex RdLo, ArmRegisterIndex RdHi,
                               ArmRegisterIndex Rm, ArmRegisterIndex Rs) {
  int64_t product =
      (int64_t)registers->gprs_s[Rm] * (int64_t)registers->gprs_s[Rs];
  registers->gprs[RdLo] = (uint32_t)(uint64_t)product;
  registers->gprs[RdHi] = (uint32_t)((uint64_t)product >> 32);
  return product;
}

static inline void ArmSMULLS(ArmUserRegisters *registers, ArmRegisterIndex RdLo,
                             ArmRegisterIndex RdHi, ArmRegisterIndex Rm,
                             ArmRegisterIndex Rs) {
  int64_t product = ArmSMULL(&registers->gprs, RdLo, RdHi, Rm, Rs);
  registers->cpsr.zero = ArmZeroFlagInt64(product);
  registers->cpsr.negative = ArmNegativeFlag(registers->gprs.gprs[RdHi]);
}

static inline int64_t ArmSMLAL(ArmGeneralPurposeRegisters *registers,
                               ArmRegisterIndex RdLo, ArmRegisterIndex RdHi,
                               ArmRegisterIndex Rm, ArmRegisterIndex Rs) {
  int64_t result = (int64_t)(((uint64_t)registers->gprs[RdHi] << 32) +
                             (uint64_t)registers->gprs[RdLo]);
  result += (int64_t)registers->gprs_s[Rm] * (int64_t)registers->gprs_s[Rs];
  registers->gprs[RdLo] = (uint32_t)(uint64_t)result;
  registers->gprs[RdHi] = (uint32_t)((uint64_t)result >> 32);
  return result;
}

static inline void ArmSMLALS(ArmUserRegisters *registers, ArmRegisterIndex RdLo,
                             ArmRegisterIndex RdHi, ArmRegisterIndex Rm,
                             ArmRegisterIndex Rs) {
  int64_t result = ArmSMLAL(&registers->gprs, RdLo, RdHi, Rm, Rs);
  registers->cpsr.zero = ArmZeroFlagInt64(result);
  registers->cpsr.negative = ArmNegativeFlag(registers->gprs.gprs[RdHi]);
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_MULTIPLY_LONG_