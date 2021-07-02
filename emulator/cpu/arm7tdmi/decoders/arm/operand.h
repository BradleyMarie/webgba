#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_ARM_OPERAND_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_ARM_OPERAND_

#include "emulator/cpu/arm7tdmi/arm7tdmi.h"

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
    ArmRegisterIndex *Rd, ArmRegisterIndex *Rn, bool *shifter_carry_out,
    uint32_t *value) {
  *Rn = (ArmRegisterIndex)((instruction >> 16u) & 0xFu);
  *Rd = (ArmRegisterIndex)((instruction >> 12u) & 0xFu);

  uint_fast8_t shift_amount = (instruction >> 8u) & 0xFu;
  uint32_t immediate = (uint8_t)instruction;
  if (shift_amount == 0u) {
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
    ArmRegisterIndex *Rd, ArmRegisterIndex *Rn, bool *shifter_carry_out,
    uint32_t *value) {
  *Rn = (ArmRegisterIndex)((instruction >> 16u) & 0xFu);
  *Rd = (ArmRegisterIndex)((instruction >> 12u) & 0xFu);
  ArmRegisterIndex Rm = (ArmRegisterIndex)(instruction & 0xFu);

  instruction = (instruction >> 4u) & 0xFFu;

  bool immediate;
  uint_fast8_t shift_amount;
  if (instruction & 0x1u) {
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
        *value = registers->gprs.gprs[Rm];
        break;
      }

      if (shift_amount == 32u) {
        *shifter_carry_out = registers->gprs.gprs[Rm] & 0x1u;
        *value = 0u;
        break;
      }

      if (shift_amount > 32u) {
        *shifter_carry_out = false;
        *value = 0u;
        break;
      }

      *shifter_carry_out =
          (registers->gprs.gprs[Rm] >> (32u - shift_amount)) & 0x1u;
      *value = registers->gprs.gprs[Rm] << shift_amount;
      break;
    case 1u:  // LSR
      if (shift_amount == 0u) {
        if (!immediate) {
          *shifter_carry_out = registers->cpsr.carry;
          *value = registers->gprs.gprs[Rm];
          break;
        }

        shift_amount = 32u;
      }

      if (shift_amount == 32u) {
        *shifter_carry_out = (registers->gprs.gprs[Rm] >> 31u) & 0x1u;
        *value = 0u;
        break;
      }

      if (shift_amount > 32u) {
        *shifter_carry_out = false;
        *value = 0u;
        break;
      }

      *shifter_carry_out =
          (registers->gprs.gprs[Rm] >> (shift_amount - 1u)) & 0x1u;
      *value = registers->gprs.gprs[Rm] >> shift_amount;
      break;
    case 2u:  // ASR
      if (shift_amount == 0u) {
        if (!immediate) {
          *shifter_carry_out = registers->cpsr.carry;
          *value = registers->gprs.gprs[Rm];
          break;
        }

        shift_amount = 32u;
      }

      if (shift_amount >= 32u) {
        if (registers->gprs.gprs[Rm] >> 31u) {
          *shifter_carry_out = true;
          *value = 0xFFFFFFFFu;
        } else {
          *shifter_carry_out = false;
          *value = 0u;
        }
        break;
      }

      *shifter_carry_out =
          (registers->gprs.gprs[Rm] >> (shift_amount - 1u)) & 0x1u;
      // Technically this triggers implementation defined behavior
      *value = registers->gprs.gprs_s[Rm] >> shift_amount;
      break;
    case 3u:
      if (shift_amount == 0u) {
        if (immediate) {
          // RRX
          *shifter_carry_out = registers->gprs.gprs[Rm] & 0x1u;
          *value =
              (registers->cpsr.carry << 31u) | (registers->gprs.gprs[Rm] >> 1u);
        } else {
          *shifter_carry_out = registers->cpsr.carry;
          *value = registers->gprs.gprs[Rm];
        }
        break;
      }

      shift_amount &= 0x1Fu;

      if (shift_amount == 0u) {
        *shifter_carry_out = (registers->gprs.gprs[Rm] >> 31u) & 0x1u;
        *value = registers->gprs.gprs[Rm];
        break;
      }

      // ROR
      *shifter_carry_out =
          (registers->gprs.gprs[Rm] >> (shift_amount - 1u)) & 0x1u;
      *value = (registers->gprs.gprs[Rm] >> shift_amount) |
               (registers->gprs.gprs[Rm] << (32u - shift_amount));
      break;
    default:
      assert(false);
  }
}

static inline void ArmOperandLoadStoreImmediate(uint32_t instruction,
                                                ArmRegisterIndex *Rd,
                                                ArmRegisterIndex *Rn,
                                                uint_fast16_t *value) {
  *Rn = (ArmRegisterIndex)((instruction >> 16u) & 0xFu);
  *Rd = (ArmRegisterIndex)((instruction >> 12u) & 0xFu);
  *value = instruction & 0xFFFu;
  assert(0u <= *value && *value <= 4095u);
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

      // Technically this triggers implementation defined behavior
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
  instruction &= 0x00FFFFFFu;
  *offset = (int32_t)((instruction ^ sign_bit) - sign_bit);
  assert(-8388608 <= *offset && *offset <= 8388607);
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_ARM_OPERAND_