#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_ARM_OPERAND_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_ARM_OPERAND_

#include "emulator/cpu/arm7tdmi/arm7tdmi.h"

static inline void ArmOperandMultiply(uint32_t instruction,
                                      ArmRegisterIndex *Rd,
                                      ArmRegisterIndex *Rn,
                                      ArmRegisterIndex *Rs,
                                      ArmRegisterIndex *Rm) {
  *Rd = (ArmRegisterIndex)((instruction >> 16u) & 0xFu);
  *Rn = (ArmRegisterIndex)((instruction >> 12u) & 0xFu);
  *Rs = (ArmRegisterIndex)((instruction >> 8u) & 0xFu);
  *Rm = (ArmRegisterIndex)((instruction >> 0u) & 0xFu);
}

static inline void ArmOperandMultiplyLong(uint32_t instruction,
                                          ArmRegisterIndex *Rn) {
  *Rn = (ArmRegisterIndex)(instruction & 0xFu);
}

static inline void ArmOperandBranchExchange(uint32_t instruction,
                                            ArmRegisterIndex *Rd,
                                            ArmRegisterIndex *Rn,
                                            ArmRegisterIndex *Rs,
                                            ArmRegisterIndex *Rm) {
  *Rd = (ArmRegisterIndex)((instruction >> 16u) & 0xFu);
  *Rn = (ArmRegisterIndex)((instruction >> 12u) & 0xFu);
  *Rs = (ArmRegisterIndex)((instruction >> 8u) & 0xFu);
  *Rm = (ArmRegisterIndex)((instruction >> 0u) & 0xFu);
}

static inline void ArmOperandSingleDataSwap(uint32_t instruction,
                                            ArmRegisterIndex *Rn,
                                            ArmRegisterIndex *Rd,
                                            ArmRegisterIndex *Rm) {
  *Rn = (ArmRegisterIndex)((instruction >> 16u) & 0xFu);
  *Rd = (ArmRegisterIndex)((instruction >> 12u) & 0xFu);
  *Rm = (ArmRegisterIndex)((instruction >> 0u) & 0xFu);
}

static inline void ArmOperandHalfwordImmediate(uint32_t instruction,
                                               ArmRegisterIndex *Rn,
                                               ArmRegisterIndex *Rd,
                                               uint_fast8_t *offset) {
  *Rn = (ArmRegisterIndex)((instruction >> 16u) & 0xFu);
  *Rd = (ArmRegisterIndex)((instruction >> 12u) & 0xFu);
  *offset = (instruction & 0xFu) | ((instruction >> 4u) & 0xF0u);
  assert(0 <= *offset <= UINT8_MAX);
}

static inline void ArmOperandHalfwordAddressMode(
    uint32_t instruction, const ArmGeneralPurposeRegisters registers,
    ArmRegisterIndex *Rn, ArmRegisterIndex *Rd, uint32_t *offset) {
  *Rn = (ArmRegisterIndex)((instruction >> 16u) & 0xFu);
  *Rd = (ArmRegisterIndex)((instruction >> 12u) & 0xFu);
  ArmRegisterIndex Rm = (ArmRegisterIndex)(instruction & 0xFu);
  *offset = registers.gprs[Rm];
}

static inline void ArmOperandDataProcessingImmediate(
    uint32_t instruction, const ArmUserRegisters *registers,
    ArmRegisterIndex *Rn, ArmRegisterIndex *Rd, bool *shifter_carry_out,
    uint32_t *value) {
  *Rn = (ArmRegisterIndex)((instruction >> 16u) & 0xFu);
  *Rd = (ArmRegisterIndex)((instruction >> 12u) & 0xFu);

  uint_fast8_t shift_amount = (instruction >> 8u) & 0xFu;
  uint32_t immediate = (uint8_t)instruction;
  if (shift_amount == 0) {
    *shifter_carry_out = registers->cpsr.carry;
    *value = immediate;
    return;
  }

  shift_amount *= 2u;

  *value = (immediate >> shift_amount) | (immediate << (32u - shift_amount));
  *shifter_carry_out = (*value >> 31u) & 0x1u;
}

static inline void ArmOperandDataProcessingOperand2(
    uint32_t instruction, const ArmUserRegisters *registers,
    ArmRegisterIndex *Rn, ArmRegisterIndex *Rd, bool *shifter_carry_out,
    uint32_t *value) {
  *Rn = (ArmRegisterIndex)((instruction >> 16u) & 0xFu);
  *Rd = (ArmRegisterIndex)((instruction >> 12u) & 0xFu);
  ArmRegisterIndex Rm = (ArmRegisterIndex)(instruction & 0xFu);

  // No Shift
  instruction = (instruction >> 4u) & 0xFFu;
  if (!instruction) {
    *shifter_carry_out = registers->cpsr.carry;
    *value = registers->gprs.gprs[Rm];
    return;
  }

  uint_fast8_t shift_amount = instruction >> 3u;
  uint_fast8_t shift = (instruction >> 1u) & 0x3u;
  if (shift_amount == 0u) {
    if (shift != 4u)  // ROR
    {
      *shifter_carry_out = registers->cpsr.carry;
      *value = registers->gprs.gprs[Rm];
      return;
    }

    // RRX
    *shifter_carry_out = registers->gprs.gprs[Rm] & 0x1u;
    *value = (registers->cpsr.carry << 31u) | (registers->gprs.gprs[Rm] >> 1u);
    return;
  }

  switch (shift) {
    case 1u:  // LSL
      *shifter_carry_out = (((uint64_t)*value << shift_amount) >> 32u) & 0x1u;
      *value = registers->gprs.gprs[Rm] << shift_amount;
      break;
    case 2u:  // LSR
      *shifter_carry_out =
          ((((uint64_t)*value << 32u) >> shift_amount) >> 31u) & 0x1u;
      *value = registers->gprs.gprs[Rm] >> shift_amount;
      break;
    case 3u:  // ASR
      // Technically this behavaior is undefined.
      *shifter_carry_out =
          ((((int64_t)*value << 32u) >> shift_amount) >> 31u) & 0x1u;
      *value = registers->gprs.gprs_s[Rm] >> shift_amount;
      break;
    case 4u:  // ROR
      *shifter_carry_out = (*value >> (shift_amount - 1u)) & 0x1u;
      *value = (registers->gprs.gprs[Rm] >> shift_amount) |
               (registers->gprs.gprs[Rm] << (32u - shift_amount));
      break;
    default:
      assert(false);
  }
}

static inline void ArmOperandLoadStoreImmediate(uint32_t instruction,
                                                ArmRegisterIndex *Rn,
                                                ArmRegisterIndex *Rd,
                                                uint_fast16_t *value) {
  *Rn = (ArmRegisterIndex)((instruction >> 16u) & 0xFu);
  *Rd = (ArmRegisterIndex)((instruction >> 12u) & 0xFu);
  *value = instruction & 0xFFFu;
  assert(0u <= *value <= 4095u);
}

static inline void ArmOperandLoadStoreAddressMode(
    uint32_t instruction, const ArmUserRegisters *registers,
    ArmRegisterIndex *Rn, ArmRegisterIndex *Rd, uint32_t *offset) {
  *Rn = (ArmRegisterIndex)((instruction >> 16u) & 0xFu);
  *Rd = (ArmRegisterIndex)((instruction >> 12u) & 0xFu);
  ArmRegisterIndex Rm = (ArmRegisterIndex)(instruction & 0xFu);

  // No Shift
  instruction = (instruction >> 4u) & 0xFFu;
  if (!instruction) {
    *offset = registers->gprs.gprs[Rm];
    return;
  }

  uint_fast8_t shift_amount;
  if (instruction & 0x1u) {
    ArmRegisterIndex Rs = (ArmRegisterIndex)(instruction >> 0x4u);
    assert(REGISTER_R0 <= Rm && Rm <= REGISTER_R15);
    shift_amount = (uint8_t)registers->gprs.gprs[Rs];
  } else {
    shift_amount = instruction >> 3u;
  }

  uint_fast8_t shift = (instruction >> 1u) & 0x3u;
  if (shift_amount == 0u) {
    if (shift != 4u)  // ROR
    {
      *offset = registers->gprs.gprs[Rm];
      return;
    }

    // RRX
    *offset = (registers->cpsr.carry << 31u) | (registers->gprs.gprs[Rm] >> 1u);
    return;
  }

  switch (shift) {
    case 1u:  // LSL
      *offset = registers->gprs.gprs[Rm] << shift_amount;
      break;
    case 2u:  // LSR
      *offset = registers->gprs.gprs[Rm] >> shift_amount;
      break;
    case 3u:  // ASR
      // Technically this behavaior is undefined.
      *offset = registers->gprs.gprs_s[Rm] >> shift_amount;
      break;
    case 4u:  // ROR
      *offset = (registers->gprs.gprs[Rm] >> shift_amount) |
                (registers->gprs.gprs[Rm] << (32u - shift_amount));
      break;
    default:
      assert(false);
  }
}

static inline void ArmOperandRegisterAndRegisterList(
    uint32_t instruction, ArmRegisterIndex *Rn, uint_fast16_t *register_list) {
  *Rn = (ArmRegisterIndex)((instruction >> 16u) & 0xFu);
  *register_list = (uint16_t)instruction;
}

static inline void ArmOperandBranch(uint32_t instruction,
                                    int_fast32_t *offset) {
  uint32_t sign_bit = 1u << 23u;
  instruction &= 0x0FFFu;
  *offset = (int32_t)((instruction ^ sign_bit) - sign_bit);
  assert(-8388608 <= *offset && *offset <= 8388607);
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_ARM_OPERAND_