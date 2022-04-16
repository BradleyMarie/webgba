#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_DATA_PROCESSING_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_DATA_PROCESSING_

#include "emulator/cpu/arm7tdmi/flags.h"
#include "emulator/cpu/arm7tdmi/registers.h"

static inline uint64_t ArmADC(ArmAllRegisters *registers, ArmRegisterIndex Rd,
                              ArmRegisterIndex Rn, uint32_t operand2) {
  uint64_t sum = (uint64_t)registers->current.user.gprs.gprs[Rn] +
                 (uint64_t)operand2 +
                 (uint64_t)registers->current.user.cpsr.carry;
  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rd, (uint32_t)sum);
  return sum;
}

static inline void ArmADCS(ArmAllRegisters *registers, ArmRegisterIndex Rd,
                           ArmRegisterIndex Rn, uint32_t operand2) {
  if (Rd != REGISTER_R15) {
    int64_t sum_s = (int64_t)registers->current.user.gprs.gprs_s[Rn] +
                    (int64_t)(int32_t)operand2 +
                    (int64_t)registers->current.user.cpsr.carry;
    uint64_t sum = ArmADC(registers, Rd, Rn, operand2);
    registers->current.user.cpsr.negative =
        ArmNegativeFlagUInt32((uint32_t)sum);
    registers->current.user.cpsr.zero = ArmZeroFlagUInt32((uint32_t)sum);
    registers->current.user.cpsr.carry = ArmAdditionCarryFlag(sum);
    registers->current.user.cpsr.overflow = ArmOverflowFlag(sum_s);
  } else {
    uint32_t sum = registers->current.user.gprs.gprs[Rn] + operand2 +
                   registers->current.user.cpsr.carry;
    ArmLoadCPSR(registers, registers->current.spsr);
    ArmLoadGPSR(registers, REGISTER_R15, sum);
  }
}

static inline uint64_t ArmADD(ArmAllRegisters *registers, ArmRegisterIndex Rd,
                              ArmRegisterIndex Rn, uint32_t operand2) {
  uint64_t sum =
      (uint64_t)registers->current.user.gprs.gprs[Rn] + (uint64_t)operand2;
  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rd, (uint32_t)sum);
  return sum;
}

static inline void ArmADDS(ArmAllRegisters *registers, ArmRegisterIndex Rd,
                           ArmRegisterIndex Rn, uint32_t operand2) {
  if (Rd != REGISTER_R15) {
    int64_t sum_s = (int64_t)registers->current.user.gprs.gprs_s[Rn] +
                    (int64_t)(int32_t)operand2;
    uint64_t sum = ArmADD(registers, Rd, Rn, operand2);
    registers->current.user.cpsr.negative =
        ArmNegativeFlagUInt32((uint32_t)sum);
    registers->current.user.cpsr.zero = ArmZeroFlagUInt32((uint32_t)sum);
    registers->current.user.cpsr.carry = ArmAdditionCarryFlag(sum);
    registers->current.user.cpsr.overflow = ArmOverflowFlag(sum_s);
  } else {
    uint32_t sum = registers->current.user.gprs.gprs[Rn] + operand2;
    ArmLoadCPSR(registers, registers->current.spsr);
    ArmLoadGPSR(registers, REGISTER_R15, (uint32_t)sum);
  }
}

static inline uint32_t ArmAND(ArmAllRegisters *registers, ArmRegisterIndex Rd,
                              ArmRegisterIndex Rn, uint32_t operand2) {
  uint32_t result = registers->current.user.gprs.gprs[Rn] & operand2;
  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rd, result);
  return result;
}

static inline void ArmANDS(ArmAllRegisters *registers, ArmRegisterIndex Rd,
                           ArmRegisterIndex Rn, uint32_t operand2,
                           bool operand2_carry) {
  if (Rd != REGISTER_R15) {
    uint32_t result = ArmAND(registers, Rd, Rn, operand2);
    registers->current.user.cpsr.negative = ArmNegativeFlagUInt32(result);
    registers->current.user.cpsr.zero = ArmZeroFlagUInt32(result);
    registers->current.user.cpsr.carry = operand2_carry;
  } else {
    uint32_t result = registers->current.user.gprs.gprs[Rn] & operand2;
    ArmLoadCPSR(registers, registers->current.spsr);
    ArmLoadGPSR(registers, REGISTER_R15, result);
  }
}

static inline uint32_t ArmBIC(ArmAllRegisters *registers, ArmRegisterIndex Rd,
                              ArmRegisterIndex Rn, uint32_t operand2) {
  uint32_t result = registers->current.user.gprs.gprs[Rn] & ~operand2;
  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rd, result);
  return result;
}

static inline void ArmBICS(ArmAllRegisters *registers, ArmRegisterIndex Rd,
                           ArmRegisterIndex Rn, uint32_t operand2,
                           bool operand2_carry) {
  if (Rd != REGISTER_R15) {
    uint32_t result = ArmBIC(registers, Rd, Rn, operand2);
    registers->current.user.cpsr.negative = ArmNegativeFlagUInt32(result);
    registers->current.user.cpsr.zero = ArmZeroFlagUInt32(result);
    registers->current.user.cpsr.carry = operand2_carry;
  } else {
    uint32_t result = registers->current.user.gprs.gprs[Rn] & ~operand2;
    ArmLoadCPSR(registers, registers->current.spsr);
    ArmLoadGPSR(registers, REGISTER_R15, result);
  }
}

static inline void ArmCMN(ArmAllRegisters *registers, ArmRegisterIndex Rn,
                          uint32_t operand2) {
  uint64_t sum =
      (uint64_t)registers->current.user.gprs.gprs[Rn] + (uint64_t)operand2;
  int64_t sum_s = (int64_t)registers->current.user.gprs.gprs_s[Rn] +
                  (int64_t)(int32_t)operand2;
  registers->current.user.cpsr.negative = ArmNegativeFlagUInt32((uint32_t)sum);
  registers->current.user.cpsr.zero = ArmZeroFlagUInt32((uint32_t)sum);
  registers->current.user.cpsr.carry = ArmAdditionCarryFlag(sum);
  registers->current.user.cpsr.overflow = ArmOverflowFlag(sum_s);
  ArmAdvanceProgramCounter(registers);
}

static inline void ArmCMP(ArmAllRegisters *registers, ArmRegisterIndex Rn,
                          uint32_t operand2) {
  uint64_t difference =
      (uint64_t)registers->current.user.gprs.gprs[Rn] - (uint64_t)operand2;
  int64_t difference_s = (int64_t)registers->current.user.gprs.gprs_s[Rn] -
                         (int64_t)(int32_t)operand2;
  registers->current.user.cpsr.negative =
      ArmNegativeFlagUInt32((uint32_t)difference);
  registers->current.user.cpsr.zero = ArmZeroFlagUInt32((uint32_t)difference);
  registers->current.user.cpsr.carry = ArmSubtractionCarryFlag(difference);
  registers->current.user.cpsr.overflow = ArmOverflowFlag(difference_s);
  ArmAdvanceProgramCounter(registers);
}

static inline uint32_t ArmEOR(ArmAllRegisters *registers, ArmRegisterIndex Rd,
                              ArmRegisterIndex Rn, uint32_t operand2) {
  uint32_t result = registers->current.user.gprs.gprs[Rn] ^ operand2;
  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rd, result);
  return result;
}

static inline void ArmEORS(ArmAllRegisters *registers, ArmRegisterIndex Rd,
                           ArmRegisterIndex Rn, uint32_t operand2,
                           bool operand2_carry) {
  if (Rd != REGISTER_R15) {
    uint32_t result = ArmEOR(registers, Rd, Rn, operand2);
    registers->current.user.cpsr.negative = ArmNegativeFlagUInt32(result);
    registers->current.user.cpsr.zero = ArmZeroFlagUInt32(result);
    registers->current.user.cpsr.carry = operand2_carry;
  } else {
    uint32_t result = registers->current.user.gprs.gprs[Rn] ^ operand2;
    ArmLoadCPSR(registers, registers->current.spsr);
    ArmLoadGPSR(registers, REGISTER_R15, result);
  }
}

static inline uint32_t ArmMOV(ArmAllRegisters *registers, ArmRegisterIndex Rd,
                              uint32_t operand2) {
  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rd, operand2);
  return operand2;
}

static inline void ArmMOVS(ArmAllRegisters *registers, ArmRegisterIndex Rd,
                           uint32_t operand2, bool operand2_carry) {
  if (Rd != REGISTER_R15) {
    uint32_t result = ArmMOV(registers, Rd, operand2);
    registers->current.user.cpsr.negative = ArmNegativeFlagUInt32(result);
    registers->current.user.cpsr.zero = ArmZeroFlagUInt32(result);
    registers->current.user.cpsr.carry = operand2_carry;
  } else {
    ArmLoadCPSR(registers, registers->current.spsr);
    ArmLoadGPSR(registers, REGISTER_R15, operand2);
  }
}

static inline uint32_t ArmMVN(ArmAllRegisters *registers, ArmRegisterIndex Rd,
                              uint32_t operand2) {
  uint32_t result = ~operand2;
  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rd, result);
  return result;
}

static inline void ArmMVNS(ArmAllRegisters *registers, ArmRegisterIndex Rd,
                           uint32_t operand2, bool operand2_carry) {
  if (Rd != REGISTER_R15) {
    uint32_t result = ArmMVN(registers, Rd, operand2);
    registers->current.user.cpsr.negative = ArmNegativeFlagUInt32(result);
    registers->current.user.cpsr.zero = ArmZeroFlagUInt32(result);
    registers->current.user.cpsr.carry = operand2_carry;
  } else {
    uint32_t result = ~operand2;
    ArmLoadCPSR(registers, registers->current.spsr);
    ArmLoadGPSR(registers, REGISTER_R15, result);
  }
}

static inline uint32_t ArmORR(ArmAllRegisters *registers, ArmRegisterIndex Rd,
                              ArmRegisterIndex Rn, uint32_t operand2) {
  uint32_t result = registers->current.user.gprs.gprs[Rn] | operand2;
  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rd, result);
  return result;
}

static inline void ArmORRS(ArmAllRegisters *registers, ArmRegisterIndex Rd,
                           ArmRegisterIndex Rn, uint32_t operand2,
                           bool operand2_carry) {
  if (Rd != REGISTER_R15) {
    uint32_t result = ArmORR(registers, Rd, Rn, operand2);
    registers->current.user.cpsr.negative = ArmNegativeFlagUInt32(result);
    registers->current.user.cpsr.zero = ArmZeroFlagUInt32(result);
    registers->current.user.cpsr.carry = operand2_carry;
  } else {
    uint32_t result = registers->current.user.gprs.gprs[Rn] | operand2;
    ArmLoadCPSR(registers, registers->current.spsr);
    ArmLoadGPSR(registers, REGISTER_R15, result);
  }
}

static inline uint64_t ArmRSB(ArmAllRegisters *registers, ArmRegisterIndex Rd,
                              ArmRegisterIndex Rn, uint32_t operand2) {
  uint64_t difference =
      (uint64_t)operand2 - (uint64_t)registers->current.user.gprs.gprs[Rn];
  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rd, (uint32_t)difference);
  return difference;
}

static inline void ArmRSBS(ArmAllRegisters *registers, ArmRegisterIndex Rd,
                           ArmRegisterIndex Rn, uint32_t operand2) {
  if (Rd != REGISTER_R15) {
    int64_t difference_s = (int64_t)(int32_t)operand2 -
                           (int64_t)registers->current.user.gprs.gprs_s[Rn];
    uint64_t difference = ArmRSB(registers, Rd, Rn, operand2);
    registers->current.user.cpsr.negative =
        ArmNegativeFlagUInt32((uint32_t)difference);
    registers->current.user.cpsr.zero = ArmZeroFlagUInt32((uint32_t)difference);
    registers->current.user.cpsr.carry = ArmSubtractionCarryFlag(difference);
    registers->current.user.cpsr.overflow = ArmOverflowFlag(difference_s);
  } else {
    uint32_t difference = operand2 - registers->current.user.gprs.gprs[Rn];
    ArmLoadCPSR(registers, registers->current.spsr);
    ArmLoadGPSR(registers, REGISTER_R15, difference);
  }
}

static inline uint64_t ArmRSC(ArmAllRegisters *registers, ArmRegisterIndex Rd,
                              ArmRegisterIndex Rn, uint32_t operand2) {
  uint64_t difference = (uint64_t)operand2 -
                        (uint64_t)registers->current.user.gprs.gprs[Rn] -
                        (uint64_t)!registers->current.user.cpsr.carry;
  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rd, (uint32_t)difference);
  return difference;
}

static inline void ArmRSCS(ArmAllRegisters *registers, ArmRegisterIndex Rd,
                           ArmRegisterIndex Rn, uint32_t operand2) {
  if (Rd != REGISTER_R15) {
    int64_t difference_s = (int64_t)(int32_t)operand2 -
                           (int64_t)registers->current.user.gprs.gprs_s[Rn] -
                           (int64_t)!registers->current.user.cpsr.carry;
    uint64_t difference = ArmRSC(registers, Rd, Rn, operand2);
    registers->current.user.cpsr.negative =
        ArmNegativeFlagUInt32((uint32_t)difference);
    registers->current.user.cpsr.zero = ArmZeroFlagUInt32((uint32_t)difference);
    registers->current.user.cpsr.carry = ArmSubtractionCarryFlag(difference);
    registers->current.user.cpsr.overflow = ArmOverflowFlag(difference_s);
  } else {
    uint32_t difference = operand2 - registers->current.user.gprs.gprs[Rn] -
                          !registers->current.user.cpsr.carry;
    ArmLoadCPSR(registers, registers->current.spsr);
    ArmLoadGPSR(registers, REGISTER_R15, difference);
  }
}

static inline uint64_t ArmSBC(ArmAllRegisters *registers, ArmRegisterIndex Rd,
                              ArmRegisterIndex Rn, uint32_t operand2) {
  uint64_t difference = (uint64_t)registers->current.user.gprs.gprs[Rn] -
                        (uint64_t)operand2 -
                        (uint64_t)!registers->current.user.cpsr.carry;
  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rd, (uint32_t)difference);
  return difference;
}

static inline void ArmSBCS(ArmAllRegisters *registers, ArmRegisterIndex Rd,
                           ArmRegisterIndex Rn, uint32_t operand2) {
  if (Rd != REGISTER_R15) {
    int64_t difference_s = (int64_t)registers->current.user.gprs.gprs_s[Rn] -
                           (int64_t)(int32_t)operand2 -
                           (int64_t)!registers->current.user.cpsr.carry;
    uint64_t difference = ArmSBC(registers, Rd, Rn, operand2);
    registers->current.user.cpsr.negative =
        ArmNegativeFlagUInt32((uint32_t)difference);
    registers->current.user.cpsr.zero = ArmZeroFlagUInt32((uint32_t)difference);
    registers->current.user.cpsr.carry = ArmSubtractionCarryFlag(difference);
    registers->current.user.cpsr.overflow = ArmOverflowFlag(difference_s);
  } else {
    uint32_t difference = registers->current.user.gprs.gprs[Rn] - operand2 -
                          !registers->current.user.cpsr.carry;
    ArmLoadCPSR(registers, registers->current.spsr);
    ArmLoadGPSR(registers, REGISTER_R15, difference);
  }
}

static inline uint64_t ArmSUB(ArmAllRegisters *registers, ArmRegisterIndex Rd,
                              ArmRegisterIndex Rn, uint32_t operand2) {
  uint64_t difference =
      (uint64_t)registers->current.user.gprs.gprs[Rn] - (uint64_t)operand2;
  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rd, (uint32_t)difference);
  return difference;
}

static inline void ArmSUBS(ArmAllRegisters *registers, ArmRegisterIndex Rd,
                           ArmRegisterIndex Rn, uint32_t operand2) {
  if (Rd != REGISTER_R15) {
    int64_t difference_s = (int64_t)registers->current.user.gprs.gprs_s[Rn] -
                           (int64_t)(int32_t)operand2;
    uint64_t difference = ArmSUB(registers, Rd, Rn, operand2);
    registers->current.user.cpsr.negative =
        ArmNegativeFlagUInt32((uint32_t)difference);
    registers->current.user.cpsr.zero = ArmZeroFlagUInt32((uint32_t)difference);
    registers->current.user.cpsr.carry = ArmSubtractionCarryFlag(difference);
    registers->current.user.cpsr.overflow = ArmOverflowFlag(difference_s);
  } else {
    uint32_t difference = registers->current.user.gprs.gprs[Rn] - operand2;
    ArmLoadCPSR(registers, registers->current.spsr);
    ArmLoadGPSR(registers, REGISTER_R15, difference);
  }
}

static inline void ArmTEQ(ArmAllRegisters *registers, ArmRegisterIndex Rn,
                          uint32_t operand2, bool operand2_carry) {
  uint32_t result = registers->current.user.gprs.gprs[Rn] ^ operand2;
  registers->current.user.cpsr.negative = ArmNegativeFlagUInt32(result);
  registers->current.user.cpsr.zero = ArmZeroFlagUInt32(result);
  registers->current.user.cpsr.carry = operand2_carry;
  ArmAdvanceProgramCounter(registers);
}

static inline void ArmTST(ArmAllRegisters *registers, ArmRegisterIndex Rn,
                          uint32_t operand2, bool operand2_carry) {
  uint32_t result = registers->current.user.gprs.gprs[Rn] & operand2;
  registers->current.user.cpsr.negative = ArmNegativeFlagUInt32(result);
  registers->current.user.cpsr.zero = ArmZeroFlagUInt32(result);
  registers->current.user.cpsr.carry = operand2_carry;
  ArmAdvanceProgramCounter(registers);
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_DATA_PROCESSING_