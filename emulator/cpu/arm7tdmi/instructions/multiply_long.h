#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_MULTIPLY_LONG_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_MULTIPLY_LONG_

#include "emulator/cpu/arm7tdmi/flags.h"
#include "emulator/cpu/arm7tdmi/registers.h"

static inline uint64_t ArmUMULL(ArmAllRegisters *registers,
                                ArmRegisterIndex RdLo, ArmRegisterIndex RdHi,
                                ArmRegisterIndex Rm, ArmRegisterIndex Rs) {
  uint64_t product = (uint64_t)registers->current.user.gprs.gprs[Rm] *
                     (uint64_t)registers->current.user.gprs.gprs[Rs];
  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, RdLo, product);
  ArmLoadGPSR(registers, RdHi, product >> 32u);
  return product;
}

static inline void ArmUMULLS(ArmAllRegisters *registers, ArmRegisterIndex RdLo,
                             ArmRegisterIndex RdHi, ArmRegisterIndex Rm,
                             ArmRegisterIndex Rs) {
  uint64_t product = ArmUMULL(registers, RdLo, RdHi, Rm, Rs);
  registers->current.user.cpsr.zero = ArmZeroFlagUInt64(product);
  registers->current.user.cpsr.negative = ArmNegativeFlagUInt64(product);
}

static inline uint64_t ArmUMLAL(ArmAllRegisters *registers,
                                ArmRegisterIndex RdLo, ArmRegisterIndex RdHi,
                                ArmRegisterIndex Rm, ArmRegisterIndex Rs) {
  uint64_t result = ((uint64_t)registers->current.user.gprs.gprs[RdHi] << 32) +
                    (uint64_t)registers->current.user.gprs.gprs[RdLo];
  result += (uint64_t)registers->current.user.gprs.gprs[Rm] *
            (uint64_t)registers->current.user.gprs.gprs[Rs];
  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, RdLo, result);
  ArmLoadGPSR(registers, RdHi, result >> 32u);
  return result;
}

static inline void ArmUMLALS(ArmAllRegisters *registers, ArmRegisterIndex RdLo,
                             ArmRegisterIndex RdHi, ArmRegisterIndex Rm,
                             ArmRegisterIndex Rs) {
  uint64_t result = ArmUMLAL(registers, RdLo, RdHi, Rm, Rs);
  registers->current.user.cpsr.zero = ArmZeroFlagUInt64(result);
  registers->current.user.cpsr.negative = ArmNegativeFlagUInt64(result);
}

static inline int64_t ArmSMULL(ArmAllRegisters *registers,
                               ArmRegisterIndex RdLo, ArmRegisterIndex RdHi,
                               ArmRegisterIndex Rm, ArmRegisterIndex Rs) {
  int64_t product = (int64_t)registers->current.user.gprs.gprs_s[Rm] *
                    (int64_t)registers->current.user.gprs.gprs_s[Rs];
  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, RdLo, (uint64_t)product);
  ArmLoadGPSR(registers, RdHi, (uint64_t)product >> 32u);
  return product;
}

static inline void ArmSMULLS(ArmAllRegisters *registers, ArmRegisterIndex RdLo,
                             ArmRegisterIndex RdHi, ArmRegisterIndex Rm,
                             ArmRegisterIndex Rs) {
  int64_t product = ArmSMULL(registers, RdLo, RdHi, Rm, Rs);
  registers->current.user.cpsr.zero = ArmZeroFlagInt64(product);
  registers->current.user.cpsr.negative = ArmNegativeFlagInt64(product);
}

static inline int64_t ArmSMLAL(ArmAllRegisters *registers,
                               ArmRegisterIndex RdLo, ArmRegisterIndex RdHi,
                               ArmRegisterIndex Rm, ArmRegisterIndex Rs) {
  int64_t result =
      (int64_t)(((uint64_t)registers->current.user.gprs.gprs[RdHi] << 32) +
                (uint64_t)registers->current.user.gprs.gprs[RdLo]);
  result += (int64_t)registers->current.user.gprs.gprs_s[Rm] *
            (int64_t)registers->current.user.gprs.gprs_s[Rs];
  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, RdLo, (uint64_t)result);
  ArmLoadGPSR(registers, RdHi, (uint64_t)result >> 32u);
  return result;
}

static inline void ArmSMLALS(ArmAllRegisters *registers, ArmRegisterIndex RdLo,
                             ArmRegisterIndex RdHi, ArmRegisterIndex Rm,
                             ArmRegisterIndex Rs) {
  int64_t result = ArmSMLAL(registers, RdLo, RdHi, Rm, Rs);
  registers->current.user.cpsr.zero = ArmZeroFlagInt64(result);
  registers->current.user.cpsr.negative = ArmNegativeFlagInt64(result);
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_MULTIPLY_LONG_