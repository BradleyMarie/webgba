#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_THUMB_SHIFT_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_THUMB_SHIFT_

#include <assert.h>

#include "emulator/cpu/arm7tdmi/arm7tdmi.h"
#include "emulator/cpu/arm7tdmi/flags.h"

static inline void ThumbASRS_I(ArmUserRegisters *registers, ArmRegisterIndex Rd,
                               ArmRegisterIndex Rm, uint_fast8_t shift_amount) {
  assert(shift_amount < 32u);
  if (shift_amount == 0u) {
    if (registers->gprs.gprs[Rm] >> 31u) {
      registers->cpsr.carry = true;
      registers->gprs.gprs_s[Rd] = 0xFFFFFFFFu;
    } else {
      registers->cpsr.carry = false;
      registers->gprs.gprs_s[Rd] = 0u;
    }
  } else {
    registers->cpsr.carry =
        (registers->gprs.gprs[Rm] >> (shift_amount - 1u)) & 0x1u;
    // Technically this triggers implementation defined behavior
    registers->gprs.gprs_s[Rd] = registers->gprs.gprs_s[Rm] >> shift_amount;
  }

  registers->cpsr.negative = ArmNegativeFlag(registers->gprs.gprs[Rd]);
  registers->cpsr.zero = ArmZeroFlagUInt32(registers->gprs.gprs[Rd]);
}

static inline void ThumbASRS_R(ArmUserRegisters *registers, ArmRegisterIndex Rd,
                               ArmRegisterIndex Rs) {
  uint8_t shift_amount = registers->gprs.gprs_s[Rs];
  if (shift_amount != 0u && shift_amount < 32u) {
    registers->cpsr.carry =
        (registers->gprs.gprs[Rd] >> (shift_amount - 1u)) & 0x1u;
    // Technically this triggers implementation defined behavior
    registers->gprs.gprs_s[Rd] >>= shift_amount;
  } else if (shift_amount >= 32u) {
    if (registers->gprs.gprs[Rd] >> 31u) {
      registers->cpsr.carry = true;
      registers->gprs.gprs_s[Rd] = 0xFFFFFFFFu;
    } else {
      registers->cpsr.carry = false;
      registers->gprs.gprs_s[Rd] = 0u;
    }
  }

  registers->cpsr.negative = ArmNegativeFlag(registers->gprs.gprs[Rd]);
  registers->cpsr.zero = ArmZeroFlagUInt32(registers->gprs.gprs[Rd]);
}

static inline void ThumbLSLS_I(ArmUserRegisters *registers, ArmRegisterIndex Rd,
                               ArmRegisterIndex Rm, uint_fast8_t shift_amount) {
  assert(shift_amount < 32u);
  if (shift_amount == 0u) {
    registers->gprs.gprs[Rd] = registers->gprs.gprs[Rm];
  } else {
    registers->cpsr.carry =
        (registers->gprs.gprs[Rm] >> (32u - shift_amount)) & 0x1u;
    registers->gprs.gprs[Rd] = registers->gprs.gprs[Rm] << shift_amount;
  }

  registers->cpsr.negative = ArmNegativeFlag(registers->gprs.gprs[Rd]);
  registers->cpsr.zero = ArmZeroFlagUInt32(registers->gprs.gprs[Rd]);
}

static inline void ThumbLSLS_R(ArmUserRegisters *registers, ArmRegisterIndex Rd,
                               ArmRegisterIndex Rs) {
  uint8_t shift_amount = registers->gprs.gprs_s[Rs];
  if (shift_amount != 0u && shift_amount < 32u) {
    registers->cpsr.carry =
        (registers->gprs.gprs[Rd] >> (32u - shift_amount)) & 0x1u;
    registers->gprs.gprs[Rd] <<= shift_amount;
  } else if (shift_amount == 32u) {
    registers->cpsr.carry = registers->gprs.gprs[Rd] & 0x1u;
    registers->gprs.gprs[Rd] = 0u;
  } else if (shift_amount > 32u) {
    registers->cpsr.carry = false;
    registers->gprs.gprs[Rd] = 0u;
  }

  registers->cpsr.negative = ArmNegativeFlag(registers->gprs.gprs[Rd]);
  registers->cpsr.zero = ArmZeroFlagUInt32(registers->gprs.gprs[Rd]);
}

static inline void ThumbLSRS_I(ArmUserRegisters *registers, ArmRegisterIndex Rd,
                               ArmRegisterIndex Rm, uint_fast8_t shift_amount) {
  assert(shift_amount < 32u);
  if (shift_amount == 0u) {
    registers->cpsr.carry = (registers->gprs.gprs[Rm] >> 31u) & 0x1u;
    registers->gprs.gprs[Rd] = 0u;
  } else {
    registers->cpsr.carry =
        (registers->gprs.gprs[Rm] >> (shift_amount - 1u)) & 0x1u;
    registers->gprs.gprs[Rd] = registers->gprs.gprs[Rm] >> shift_amount;
  }

  registers->cpsr.negative = ArmNegativeFlag(registers->gprs.gprs[Rd]);
  registers->cpsr.zero = ArmZeroFlagUInt32(registers->gprs.gprs[Rd]);
}

static inline void ThumbLSRS_R(ArmUserRegisters *registers, ArmRegisterIndex Rd,
                               ArmRegisterIndex Rm) {
  uint8_t shift_amount = registers->gprs.gprs_s[Rm];
  if (shift_amount != 0u && shift_amount < 32u) {
    registers->cpsr.carry =
        (registers->gprs.gprs[Rd] >> (shift_amount - 1u)) & 0x1u;
    registers->gprs.gprs[Rd] >>= shift_amount;
  } else if (shift_amount == 32u) {
    registers->cpsr.carry = (registers->gprs.gprs[Rd] >> 31u) & 0x1u;
    registers->gprs.gprs[Rd] = 0u;
  } else if (shift_amount > 32u) {
    registers->cpsr.carry = false;
    registers->gprs.gprs[Rd] = 0u;
  }

  registers->cpsr.negative = ArmNegativeFlag(registers->gprs.gprs[Rd]);
  registers->cpsr.zero = ArmZeroFlagUInt32(registers->gprs.gprs[Rd]);
}

static inline void ThumbRORS(ArmUserRegisters *registers, ArmRegisterIndex Rd,
                             ArmRegisterIndex Rm) {
  uint8_t shift_amount = registers->gprs.gprs_s[Rm];
  if (shift_amount == 0) {
    return;
  }

  shift_amount &= 0x1Fu;
  if (shift_amount == 0) {
    registers->cpsr.carry = (registers->gprs.gprs[Rd] >> 31) & 0x1u;
  } else {
    registers->cpsr.carry =
        (registers->gprs.gprs[Rd] >> (shift_amount - 1u)) & 0x1u;
    registers->gprs.gprs[Rd] =
        (registers->gprs.gprs[Rd] >> shift_amount) |
        (registers->gprs.gprs[Rd] << (32u - shift_amount));
  }

  registers->cpsr.negative = ArmNegativeFlag(registers->gprs.gprs[Rd]);
  registers->cpsr.zero = ArmZeroFlagUInt32(registers->gprs.gprs[Rd]);
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_THUMB_SHIFT_