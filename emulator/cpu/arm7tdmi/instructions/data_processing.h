#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_DATA_PROCESSING_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_DATA_PROCESSING_

#include "emulator/cpu/arm7tdmi/arm7tdmi.h"

static inline uint64_t ArmADC(ArmUserRegisters *registers, ArmRegisterIndex Rd,
                              ArmRegisterIndex Rn, uint32_t operand2) {
  uint64_t sum = (uint64_t)registers->gprs.gprs[Rn] + (uint64_t)operand2 +
                 (uint64_t)registers->cpsr.carry;
  registers->gprs.gprs[Rd] = (uint32_t)sum;
  return sum;
}

static inline void ArmADCS(ArmUserRegisters *registers, ArmRegisterIndex Rd,
                           ArmRegisterIndex Rn, uint32_t operand2) {
  int64_t sum_s = (int64_t)registers->gprs.gprs_s[Rn] +
                  (int64_t)(int32_t)operand2 + (int64_t)registers->cpsr.carry;
  uint64_t sum = ArmADC(registers, Rd, Rn, operand2);
  registers->cpsr.negative = ArmNegativeFlagUInt32(registers->gprs.gprs_s[Rd]);
  registers->cpsr.zero = ArmZeroFlagUInt32(registers->gprs.gprs_s[Rd]);
  registers->cpsr.carry = ArmCarryFlag(sum);
  registers->cpsr.overflow = ArmOverflowFlag(sum_s);
}

static inline uint64_t ArmADD(ArmGeneralPurposeRegisters *registers,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint32_t operand2) {
  uint64_t sum = (uint64_t)registers->gprs[Rn] + (uint64_t)operand2;
  registers->gprs[Rd] = (uint32_t)sum;
  return sum;
}

static inline void ArmADDS(ArmUserRegisters *registers, ArmRegisterIndex Rd,
                           ArmRegisterIndex Rn, uint32_t operand2) {
  int64_t sum_s =
      (int64_t)registers->gprs.gprs_s[Rn] + (int64_t)(int32_t)operand2;
  uint64_t sum = ArmADD(&registers->gprs, Rd, Rn, operand2);
  registers->cpsr.negative = ArmNegativeFlagUInt32(registers->gprs.gprs[Rd]);
  registers->cpsr.zero = ArmZeroFlagUInt32(registers->gprs.gprs[Rd]);
  registers->cpsr.carry = ArmCarryFlag(sum);
  registers->cpsr.overflow = ArmOverflowFlag(sum_s);
}

static inline void ArmAND(ArmGeneralPurposeRegisters *registers,
                          ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                          uint32_t operand2) {
  registers->gprs[Rd] = registers->gprs[Rn] & operand2;
}

static inline void ArmANDS(ArmUserRegisters *registers, ArmRegisterIndex Rd,
                           ArmRegisterIndex Rn, uint32_t operand2,
                           bool operand2_carry) {
  ArmAND(&registers->gprs, Rd, Rn, operand2);
  registers->cpsr.negative = ArmNegativeFlagUInt32(registers->gprs.gprs[Rd]);
  registers->cpsr.zero = ArmZeroFlagUInt32(registers->gprs.gprs[Rd]);
  registers->cpsr.carry = operand2_carry;
}

static inline void ArmBIC(ArmGeneralPurposeRegisters *registers,
                          ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                          uint32_t operand2) {
  registers->gprs[Rd] = registers->gprs[Rn] & ~operand2;
}

static inline void ArmBICS(ArmUserRegisters *registers, ArmRegisterIndex Rd,
                           ArmRegisterIndex Rn, uint32_t operand2) {
  ArmBIC(&registers->gprs, Rd, Rn, operand2);
  registers->cpsr.negative = ArmNegativeFlagUInt32(registers->gprs.gprs[Rd]);
  registers->cpsr.zero = ArmZeroFlagUInt32(registers->gprs.gprs[Rd]);
}

static inline void ArmCMN(ArmUserRegisters *registers, ArmRegisterIndex Rn,
                          uint32_t operand2) {
  uint64_t sum = (uint64_t)registers->gprs.gprs[Rn] + (uint64_t)operand2;
  int64_t sum_s =
      (int64_t)registers->gprs.gprs_s[Rn] + (int64_t)(int32_t)operand2;
  registers->cpsr.negative = ArmNegativeFlagUInt32((uint32_t)sum);
  registers->cpsr.zero = ArmZeroFlagUInt32((uint32_t)sum);
  registers->cpsr.carry = ArmCarryFlag(sum);
  registers->cpsr.overflow = ArmOverflowFlag(sum_s);
}

static inline void ArmCMP(ArmUserRegisters *registers, ArmRegisterIndex Rn,
                          uint32_t operand2) {
  uint64_t difference = (uint64_t)registers->gprs.gprs[Rn] - (uint64_t)operand2;
  int64_t difference_s =
      (int64_t)registers->gprs.gprs_s[Rn] - (int64_t)(int32_t)operand2;
  registers->cpsr.negative = ArmNegativeFlagUInt32((uint32_t)difference);
  registers->cpsr.zero = ArmZeroFlagUInt32((uint32_t)difference);
  registers->cpsr.carry = ArmCarryFlag(difference);
  registers->cpsr.overflow = ArmOverflowFlag(difference_s);
}

static inline void ArmEOR(ArmGeneralPurposeRegisters *registers,
                          ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                          uint32_t operand2) {
  registers->gprs[Rd] = registers->gprs[Rn] ^ operand2;
}

static inline void ArmEORS(ArmUserRegisters *registers, ArmRegisterIndex Rd,
                           ArmRegisterIndex Rn, uint32_t operand2,
                           bool operand2_carry) {
  ArmEOR(&registers->gprs, Rd, Rn, operand2);
  registers->cpsr.negative = ArmNegativeFlagUInt32(registers->gprs.gprs[Rd]);
  registers->cpsr.zero = ArmZeroFlagUInt32(registers->gprs.gprs[Rd]);
  registers->cpsr.carry = operand2_carry;
}

static inline void ArmMOV(ArmGeneralPurposeRegisters *registers,
                          ArmRegisterIndex Rd, uint32_t operand2) {
  registers->gprs[Rd] = operand2;
}

static inline void ArmMOVS(ArmAllRegisters *registers, ArmRegisterIndex Rd,
                           uint32_t operand2, bool operand2_carry) {
  ArmMOV(&registers->current.user.gprs, Rd, operand2);
  if (Rd != REGISTER_R15) {
    registers->current.user.cpsr.negative =
        ArmNegativeFlagUInt32(registers->current.user.gprs.gprs[Rd]);
    registers->current.user.cpsr.zero =
        ArmZeroFlagUInt32(registers->current.user.gprs.gprs[Rd]);
    registers->current.user.cpsr.carry = operand2_carry;
  } else {
    uint_fast8_t current_bank_index =
        ArmModeToBankIndex(registers->current.user.cpsr.mode);
    uint_fast8_t current_bank_size = ArmBankIndexToBankSize(current_bank_index);
    for (uint_fast8_t i = 0; i < current_bank_size; i++) {
      registers->banked_gprs[current_bank_index][i] =
          registers->current.user.gprs.gprs[REGISTER_R14 - i];
    }
    registers->banked_spsrs[current_bank_index] = registers->current.spsr;

    registers->current.user.cpsr = registers->current.spsr;

    uint_fast8_t next_bank_index =
        ArmModeToBankIndex(registers->current.spsr.mode);
    uint_fast8_t next_bank_size = ArmBankIndexToBankSize(next_bank_index);
    for (uint_fast8_t i = 0; i < next_bank_size; i++) {
      registers->current.user.gprs.gprs[REGISTER_R14 - i] =
          registers->banked_gprs[next_bank_index][i];
    }
    registers->current.spsr = registers->banked_spsrs[next_bank_index];
  }
}

static inline void ArmMVN(ArmGeneralPurposeRegisters *registers,
                          ArmRegisterIndex Rd, uint32_t operand2) {
  registers->gprs[Rd] = ~operand2;
}

static inline void ArmMVNS(ArmAllRegisters *registers, ArmRegisterIndex Rd,
                           uint32_t operand2, bool operand2_carry) {
  ArmMVN(&registers->current.user.gprs, Rd, operand2);
  if (Rd != REGISTER_R15) {
    registers->current.user.cpsr.negative =
        ArmNegativeFlagUInt32(registers->current.user.gprs.gprs[Rd]);
    registers->current.user.cpsr.zero =
        ArmZeroFlagUInt32(registers->current.user.gprs.gprs[Rd]);
    registers->current.user.cpsr.carry = operand2_carry;
  } else {
    uint_fast8_t current_bank_index =
        ArmModeToBankIndex(registers->current.user.cpsr.mode);
    uint_fast8_t current_bank_size = ArmBankIndexToBankSize(current_bank_index);
    for (uint_fast8_t i = 0; i < current_bank_size; i++) {
      registers->banked_gprs[current_bank_index][i] =
          registers->current.user.gprs.gprs[REGISTER_R14 - i];
    }
    registers->banked_spsrs[current_bank_index] = registers->current.spsr;

    registers->current.user.cpsr = registers->current.spsr;

    uint_fast8_t next_bank_index =
        ArmModeToBankIndex(registers->current.spsr.mode);
    uint_fast8_t next_bank_size = ArmBankIndexToBankSize(next_bank_index);
    for (uint_fast8_t i = 0; i < next_bank_size; i++) {
      registers->current.user.gprs.gprs[REGISTER_R14 - i] =
          registers->banked_gprs[next_bank_index][i];
    }
    registers->current.spsr = registers->banked_spsrs[next_bank_index];
  }
}

static inline void ArmOOR(ArmGeneralPurposeRegisters *registers,
                          ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                          uint32_t operand2) {
  registers->gprs[Rd] = registers->gprs[Rn] | operand2;
}

static inline void ArmOORS(ArmUserRegisters *registers, ArmRegisterIndex Rd,
                           ArmRegisterIndex Rn, uint32_t operand2,
                           bool operand2_carry) {
  ArmOOR(&registers->gprs, Rd, Rn, operand2);
  registers->cpsr.negative = ArmNegativeFlagUInt32(registers->gprs.gprs[Rd]);
  registers->cpsr.zero = ArmZeroFlagUInt32(registers->gprs.gprs[Rd]);
  registers->cpsr.carry = operand2_carry;
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_DATA_PROCESSING_