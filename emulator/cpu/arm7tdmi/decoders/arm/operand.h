#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_ARM_OPERAND_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_ARM_OPERAND_

#include <assert.h>

#include "emulator/cpu/arm7tdmi/registers.h"
#include "util/macros.h"

static inline void ArmOperandMultiply(uint32_t instruction,
                                      ArmRegisterIndex *Rd,
                                      ArmRegisterIndex *Rm,
                                      ArmRegisterIndex *Rs) {
  *Rd = (ArmRegisterIndex)((instruction >> 16u) & 0xFu);
  *Rs = (ArmRegisterIndex)((instruction >> 8u) & 0xFu);
  *Rm = (ArmRegisterIndex)((instruction >> 0u) & 0xFu);
}

static inline void ArmOperandMultiplyAccumulate(uint32_t instruction,
                                                ArmRegisterIndex *Rd,
                                                ArmRegisterIndex *Rm,
                                                ArmRegisterIndex *Rs,
                                                ArmRegisterIndex *Rn) {
  *Rd = (ArmRegisterIndex)((instruction >> 16u) & 0xFu);
  *Rn = (ArmRegisterIndex)((instruction >> 12u) & 0xFu);
  *Rs = (ArmRegisterIndex)((instruction >> 8u) & 0xFu);
  *Rm = (ArmRegisterIndex)((instruction >> 0u) & 0xFu);
}

static inline void ArmOperandMultiplyLong(uint32_t instruction,
                                          ArmRegisterIndex *Rd_LSW,
                                          ArmRegisterIndex *Rd_MSW,
                                          ArmRegisterIndex *Rm,
                                          ArmRegisterIndex *Rs) {
  *Rd_MSW = (ArmRegisterIndex)((instruction >> 16u) & 0xFu);
  *Rd_LSW = (ArmRegisterIndex)((instruction >> 12u) & 0xFu);
  *Rs = (ArmRegisterIndex)((instruction >> 8u) & 0xFu);
  *Rm = (ArmRegisterIndex)((instruction >> 0u) & 0xFu);
}

static inline void ArmOperandBranchExchange(uint32_t instruction,
                                            ArmRegisterIndex *Rm) {
  *Rm = (ArmRegisterIndex)((instruction >> 0u) & 0xFu);
}

static inline void ArmOperandSingleDataSwap(uint32_t instruction,
                                            ArmRegisterIndex *Rd,
                                            ArmRegisterIndex *Rm,
                                            ArmRegisterIndex *Rn) {
  *Rn = (ArmRegisterIndex)((instruction >> 16u) & 0xFu);
  *Rd = (ArmRegisterIndex)((instruction >> 12u) & 0xFu);
  *Rm = (ArmRegisterIndex)((instruction >> 0u) & 0xFu);
}

static inline void ArmOperandHalfwordImmediate(uint32_t instruction,
                                               ArmRegisterIndex *Rd,
                                               ArmRegisterIndex *Rn,
                                               uint_fast8_t *offset) {
  *Rn = (ArmRegisterIndex)((instruction >> 16u) & 0xFu);
  *Rd = (ArmRegisterIndex)((instruction >> 12u) & 0xFu);
  *offset = (instruction & 0xFu) | ((instruction >> 4u) & 0xF0u);
  assert(0u <= *offset && *offset <= UINT8_MAX);
}

static inline void ArmOperandHalfwordAddressMode(
    uint32_t instruction, const ArmGeneralPurposeRegisters *registers,
    ArmRegisterIndex *Rd, ArmRegisterIndex *Rn, uint32_t *offset) {
  *Rn = (ArmRegisterIndex)((instruction >> 16u) & 0xFu);
  *Rd = (ArmRegisterIndex)((instruction >> 12u) & 0xFu);
  ArmRegisterIndex Rm = (ArmRegisterIndex)(instruction & 0xFu);
  *offset = registers->gprs[Rm];
}

static inline void ArmOperandDataProcessingImmediate(
    uint32_t instruction, const ArmUserRegisters *registers,
    ArmRegisterIndex *Rd, uint32_t *operand1, uint32_t *operand2,
    bool *shifter_carry_out) {
  ArmRegisterIndex Rn = (ArmRegisterIndex)((instruction >> 16u) & 0xFu);
  *operand1 = registers->gprs.gprs[Rn];

  *Rd = (ArmRegisterIndex)((instruction >> 12u) & 0xFu);

  uint_fast8_t shift_amount = (instruction >> 8u) & 0xFu;
  uint32_t immediate = (uint8_t)instruction;
  if (shift_amount == 0u) {
    *shifter_carry_out = registers->cpsr.carry;
    *operand2 = immediate;
    return;
  }

  shift_amount *= 2u;

  *operand2 = (immediate >> shift_amount) | (immediate << (32u - shift_amount));
  *shifter_carry_out = (*operand2 >> 31u) & 0x1u;
}

static inline void ArmOperandDataProcessingOperand2(
    uint32_t instruction, const ArmUserRegisters *registers,
    ArmRegisterIndex *Rd, uint32_t *operand1, uint32_t *operand2,
    bool *shifter_carry_out) {
  *Rd = (ArmRegisterIndex)((instruction >> 12u) & 0xFu);

  ArmRegisterIndex Rn = (ArmRegisterIndex)((instruction >> 16u) & 0xFu);
  *operand1 = registers->gprs.gprs[Rn];

  ArmRegisterIndex Rm = (ArmRegisterIndex)(instruction & 0xFu);
  instruction = (instruction >> 4u) & 0xFFu;

  uint32_t shifted_value = registers->gprs.gprs[Rm];

  bool immediate;
  uint_fast8_t shift_amount;
  if (instruction & 0x1u) {
    // ARM defines specifying R15 for Rn to be unpredictable
    if (Rn == REGISTER_R15) {
      *operand1 += 4u;
    }
    // ARM defines specifying R15 for Rn to be unpredictable
    if (Rm == REGISTER_R15) {
      shifted_value += 4u;
    }
    ArmRegisterIndex Rs = (ArmRegisterIndex)(instruction >> 0x4u);
    assert(REGISTER_R0 <= Rm && Rm <= REGISTER_R15);
    shift_amount = (uint8_t)registers->gprs.gprs[Rs];
    immediate = false;
  } else {
    shift_amount = instruction >> 3u;
    immediate = true;
  }

  uint_fast8_t shift = (instruction >> 1u) & 0x3u;
  switch (shift) {
    case 0u:  // LSL
      if (shift_amount == 0u) {
        *shifter_carry_out = registers->cpsr.carry;
        *operand2 = shifted_value;
        break;
      }

      if (shift_amount == 32u) {
        *shifter_carry_out = shifted_value & 0x1u;
        *operand2 = 0u;
        break;
      }

      if (shift_amount > 32u) {
        *shifter_carry_out = false;
        *operand2 = 0u;
        break;
      }

      *shifter_carry_out = (shifted_value >> (32u - shift_amount)) & 0x1u;
      *operand2 = shifted_value << shift_amount;
      break;
    case 1u:  // LSR
      if (shift_amount == 0u) {
        if (!immediate) {
          *shifter_carry_out = registers->cpsr.carry;
          *operand2 = shifted_value;
          break;
        }

        shift_amount = 32u;
      }

      if (shift_amount == 32u) {
        *shifter_carry_out = (shifted_value >> 31u) & 0x1u;
        *operand2 = 0u;
        break;
      }

      if (shift_amount > 32u) {
        *shifter_carry_out = false;
        *operand2 = 0u;
        break;
      }

      *shifter_carry_out = (shifted_value >> (shift_amount - 1u)) & 0x1u;
      *operand2 = shifted_value >> shift_amount;
      break;
    case 2u:  // ASR
      if (shift_amount == 0u) {
        if (!immediate) {
          *shifter_carry_out = registers->cpsr.carry;
          *operand2 = shifted_value;
          break;
        }

        shift_amount = 32u;
      }

      if (shift_amount >= 32u) {
        if (shifted_value >> 31u) {
          *shifter_carry_out = true;
          *operand2 = 0xFFFFFFFFu;
        } else {
          *shifter_carry_out = false;
          *operand2 = 0u;
        }
        break;
      }

      *shifter_carry_out = (shifted_value >> (shift_amount - 1u)) & 0x1u;

      // Technically this triggers implementation defined behavior in C
      *operand2 = ((int32_t)shifted_value) >> shift_amount;
      break;
    case 3u:
      if (shift_amount == 0u) {
        if (immediate) {
          // RRX
          *shifter_carry_out = shifted_value & 0x1u;
          *operand2 = (registers->cpsr.carry << 31u) | (shifted_value >> 1u);
        } else {
          *shifter_carry_out = registers->cpsr.carry;
          *operand2 = shifted_value;
        }
        break;
      }

      shift_amount &= 0x1Fu;

      if (shift_amount == 0u) {
        *shifter_carry_out = (shifted_value >> 31u) & 0x1u;
        *operand2 = shifted_value;
        break;
      }

      // ROR
      *shifter_carry_out = (shifted_value >> (shift_amount - 1u)) & 0x1u;
      *operand2 = (shifted_value >> shift_amount) |
                  (shifted_value << (32u - shift_amount));
      break;
    default:
      codegen_assert(false);
  }
}

static inline void ArmOperandLoadStoreImmediate(uint32_t instruction,
                                                ArmRegisterIndex *Rd,
                                                ArmRegisterIndex *Rn,
                                                uint_fast16_t *value) {
  *Rn = (ArmRegisterIndex)((instruction >> 16u) & 0xFu);
  *Rd = (ArmRegisterIndex)((instruction >> 12u) & 0xFu);
  *value = instruction & 0xFFFu;
  assert(*value <= 4095u);
}

static inline void ArmOperandLoadStoreAddressMode(
    uint32_t instruction, const ArmUserRegisters *registers,
    ArmRegisterIndex *Rd, ArmRegisterIndex *Rn, uint32_t *offset) {
  *Rn = (ArmRegisterIndex)((instruction >> 16u) & 0xFu);
  *Rd = (ArmRegisterIndex)((instruction >> 12u) & 0xFu);
  ArmRegisterIndex Rm = (ArmRegisterIndex)(instruction & 0xFu);

  instruction = (instruction >> 4u) & 0xFFu;
  uint_fast8_t shift = (instruction >> 1u) & 0x3u;
  uint_fast8_t shift_amount = instruction >> 3u;
  switch (shift) {
    case 0u:  // LSL
      *offset = registers->gprs.gprs[Rm] << shift_amount;
      break;
    case 1u:  // LSR
      if (shift_amount == 0u) {
        *offset = 0u;
        break;
      }

      *offset = registers->gprs.gprs[Rm] >> shift_amount;
      break;
    case 2u:  // ASR
      if (shift_amount == 0u) {
        if (registers->gprs.gprs[Rm] >> 31u) {
          *offset = 0xFFFFFFFFu;
        } else {
          *offset = 0u;
        }
        break;
      }

      // Technically this triggers implementation defined behavior in C
      *offset = registers->gprs.gprs_s[Rm] >> shift_amount;
      break;
    case 3u:
      if (shift_amount == 0u) {
        // RRX
        *offset =
            (registers->cpsr.carry << 31u) | (registers->gprs.gprs[Rm] >> 1u);
        break;
      }

      // ROR
      *offset = (registers->gprs.gprs[Rm] >> shift_amount) |
                (registers->gprs.gprs[Rm] << (32u - shift_amount));
      break;
    default:
      codegen_assert(false);
  }
}

static inline void ArmOperandRegisterAndRegisterList(
    uint32_t instruction, ArmRegisterIndex *Rn, uint_fast16_t *register_list) {
  *Rn = (ArmRegisterIndex)((instruction >> 16u) & 0xFu);
  *register_list = (uint16_t)instruction;
}

static inline void ArmOperandBranchForward(uint32_t instruction,
                                           uint_fast32_t *offset) {
  instruction <<= 8u;
  *offset = instruction >> 6u;
}

static inline void ArmOperandBranchReverse(uint32_t instruction,
                                           uint_fast32_t *offset) {
  *offset = 0xFC000000u | (instruction << 2u);
}

static inline void ArmOperandMoveFromStatusRegister(uint32_t instruction,
                                                    ArmRegisterIndex *Rd) {
  *Rd = (ArmRegisterIndex)((instruction >> 12u) & 0xFu);
}

static inline void ArmOperandMoveToStatusRegisterImmediate(uint32_t instruction,
                                                           bool *control,
                                                           bool *flags,
                                                           uint32_t *value) {
  *control = (instruction >> 16u) & 0x1;
  *flags = (instruction >> 19u) & 0x1u;

  uint_fast8_t shift_amount = (instruction >> 8u) & 0xFu;
  shift_amount *= 2u;

  uint32_t immediate = (uint8_t)instruction;
  *value = (immediate >> shift_amount) | (immediate << (32u - shift_amount));
}

static inline void ArmOperandMoveToStatusRegisterRegister(
    uint32_t instruction, const ArmGeneralPurposeRegisters *registers,
    bool *control, bool *flags, uint32_t *value) {
  *control = (instruction >> 16u) & 0x1;
  *flags = (instruction >> 19u) & 0x1;

  ArmRegisterIndex rm = (ArmRegisterIndex)(instruction & 0xFu);
  *value = registers->gprs[rm];
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_ARM_OPERAND_