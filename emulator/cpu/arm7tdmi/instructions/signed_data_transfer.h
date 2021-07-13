#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_SIGNED_DATA_TRANSFER_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_SIGNED_DATA_TRANSFER_

#include "emulator/cpu/arm7tdmi/exceptions.h"
#include "emulator/cpu/arm7tdmi/memory.h"
#include "emulator/cpu/arm7tdmi/registers.h"

static inline bool ArmLDRH_DAW(ArmAllRegisters *registers, const Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast8_t offset) {
  uint16_t temp;
  bool success =
      ArmLoad16LE(memory, registers->current.user.gprs.gprs[Rn], &temp);
  registers->current.user.gprs.gprs[Rn] -= offset;

  if (!success) {
    ArmExceptionDataABT(registers);
  } else {
    registers->current.user.gprs.gprs[Rd] = temp;
  }

  return success;
}

static inline bool ArmLDRH_DB(ArmAllRegisters *registers, const Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint32_t offset) {
  uint16_t temp;
  bool success = ArmLoad16LE(
      memory, registers->current.user.gprs.gprs[Rn] - offset, &temp);

  if (!success) {
    ArmExceptionDataABT(registers);
  } else {
    registers->current.user.gprs.gprs[Rd] = temp;
  }

  return success;
}

static inline bool ArmLDRH_DBW(ArmAllRegisters *registers, const Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast8_t offset) {
  registers->current.user.gprs.gprs[Rn] -= offset;
  uint16_t temp;
  bool success =
      ArmLoad16LE(memory, registers->current.user.gprs.gprs[Rn], &temp);

  if (!success) {
    ArmExceptionDataABT(registers);
  } else {
    registers->current.user.gprs.gprs[Rd] = temp;
  }

  return success;
}

static inline bool ArmLDRH_IAW(ArmAllRegisters *registers, const Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast8_t offset) {
  uint16_t temp;
  bool success =
      ArmLoad16LE(memory, registers->current.user.gprs.gprs[Rn], &temp);
  registers->current.user.gprs.gprs[Rn] += offset;

  if (!success) {
    ArmExceptionDataABT(registers);
  } else {
    registers->current.user.gprs.gprs[Rd] = temp;
  }

  return success;
}

static inline bool ArmLDRH_IB(ArmAllRegisters *registers, const Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint32_t offset) {
  uint16_t temp;
  bool success = ArmLoad16LE(
      memory, registers->current.user.gprs.gprs[Rn] + offset, &temp);

  if (!success) {
    ArmExceptionDataABT(registers);
  } else {
    registers->current.user.gprs.gprs[Rd] = temp;
  }

  return success;
}

static inline bool ArmLDRH_IBW(ArmAllRegisters *registers, const Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast8_t offset) {
  registers->current.user.gprs.gprs[Rn] += offset;
  uint16_t temp;
  bool success =
      ArmLoad16LE(memory, registers->current.user.gprs.gprs[Rn], &temp);

  if (!success) {
    ArmExceptionDataABT(registers);
  } else {
    registers->current.user.gprs.gprs[Rd] = temp;
  }

  return success;
}

static inline bool ArmLDRSB_DAW(ArmAllRegisters *registers,
                                const Memory *memory, ArmRegisterIndex Rd,
                                ArmRegisterIndex Rn, uint_fast8_t offset) {
  int8_t temp;
  bool success = Load8S(memory, registers->current.user.gprs.gprs[Rn], &temp);
  registers->current.user.gprs.gprs[Rn] -= offset;

  if (!success) {
    ArmExceptionDataABT(registers);
  } else {
    registers->current.user.gprs.gprs[Rd] = temp;
  }

  return success;
}

static inline bool ArmLDRSB_DB(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint32_t offset) {
  int8_t temp;
  bool success =
      Load8S(memory, registers->current.user.gprs.gprs[Rn] - offset, &temp);

  if (!success) {
    ArmExceptionDataABT(registers);
  } else {
    registers->current.user.gprs.gprs[Rd] = temp;
  }

  return success;
}

static inline bool ArmLDRSB_DBW(ArmAllRegisters *registers,
                                const Memory *memory, ArmRegisterIndex Rd,
                                ArmRegisterIndex Rn, uint_fast8_t offset) {
  registers->current.user.gprs.gprs[Rn] -= offset;
  int8_t temp;
  bool success = Load8S(memory, registers->current.user.gprs.gprs[Rn], &temp);

  if (!success) {
    ArmExceptionDataABT(registers);
  } else {
    registers->current.user.gprs.gprs[Rd] = temp;
  }

  return success;
}

static inline bool ArmLDRSB_IAW(ArmAllRegisters *registers,
                                const Memory *memory, ArmRegisterIndex Rd,
                                ArmRegisterIndex Rn, uint_fast8_t offset) {
  int8_t temp;
  bool success = Load8S(memory, registers->current.user.gprs.gprs[Rn], &temp);
  registers->current.user.gprs.gprs[Rn] += offset;

  if (!success) {
    ArmExceptionDataABT(registers);
  } else {
    registers->current.user.gprs.gprs[Rd] = temp;
  }

  return success;
}

static inline bool ArmLDRSB_IB(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint32_t offset) {
  int8_t temp;
  bool success =
      Load8S(memory, registers->current.user.gprs.gprs[Rn] + offset, &temp);

  if (!success) {
    ArmExceptionDataABT(registers);
  } else {
    registers->current.user.gprs.gprs[Rd] = temp;
  }

  return success;
}

static inline bool ArmLDRSB_IBW(ArmAllRegisters *registers,
                                const Memory *memory, ArmRegisterIndex Rd,
                                ArmRegisterIndex Rn, uint_fast8_t offset) {
  registers->current.user.gprs.gprs[Rn] += offset;
  int8_t temp;
  bool success = Load8S(memory, registers->current.user.gprs.gprs[Rn], &temp);

  if (!success) {
    ArmExceptionDataABT(registers);
  } else {
    registers->current.user.gprs.gprs[Rd] = temp;
  }

  return success;
}

static inline bool ArmLDRSH_DAW(ArmAllRegisters *registers,
                                const Memory *memory, ArmRegisterIndex Rd,
                                ArmRegisterIndex Rn, uint_fast8_t offset) {
  int16_t temp;
  bool success =
      ArmLoad16SLE(memory, registers->current.user.gprs.gprs[Rn], &temp);
  registers->current.user.gprs.gprs[Rn] -= offset;

  if (!success) {
    ArmExceptionDataABT(registers);
  } else {
    registers->current.user.gprs.gprs[Rd] = temp;
  }

  return success;
}

static inline bool ArmLDRSH_DB(ArmAllRegisters *registers, const Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint32_t offset) {
  int16_t temp;
  bool success = ArmLoad16SLE(
      memory, registers->current.user.gprs.gprs[Rn] - offset, &temp);

  if (!success) {
    ArmExceptionDataABT(registers);
  } else {
    registers->current.user.gprs.gprs[Rd] = temp;
  }

  return success;
}

static inline bool ArmLDRSH_DBW(ArmAllRegisters *registers,
                                const Memory *memory, ArmRegisterIndex Rd,
                                ArmRegisterIndex Rn, uint_fast8_t offset) {
  registers->current.user.gprs.gprs[Rn] -= offset;
  int16_t temp;
  bool success =
      ArmLoad16SLE(memory, registers->current.user.gprs.gprs[Rn], &temp);

  if (!success) {
    ArmExceptionDataABT(registers);
  } else {
    registers->current.user.gprs.gprs[Rd] = temp;
  }

  return success;
}

static inline bool ArmLDRSH_IAW(ArmAllRegisters *registers,
                                const Memory *memory, ArmRegisterIndex Rd,
                                ArmRegisterIndex Rn, uint_fast8_t offset) {
  int16_t temp;
  bool success =
      ArmLoad16SLE(memory, registers->current.user.gprs.gprs[Rn], &temp);
  registers->current.user.gprs.gprs[Rn] += offset;

  if (!success) {
    ArmExceptionDataABT(registers);
  } else {
    registers->current.user.gprs.gprs[Rd] = temp;
  }

  return success;
}

static inline bool ArmLDRSH_IB(ArmAllRegisters *registers, const Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint32_t offset) {
  int16_t temp;
  bool success = ArmLoad16SLE(
      memory, registers->current.user.gprs.gprs[Rn] + offset, &temp);

  if (!success) {
    ArmExceptionDataABT(registers);
  } else {
    registers->current.user.gprs.gprs[Rd] = temp;
  }

  return success;
}

static inline bool ArmLDRSH_IBW(ArmAllRegisters *registers,
                                const Memory *memory, ArmRegisterIndex Rd,
                                ArmRegisterIndex Rn, uint_fast8_t offset) {
  registers->current.user.gprs.gprs[Rn] += offset;
  int16_t temp;
  bool success =
      ArmLoad16SLE(memory, registers->current.user.gprs.gprs[Rn], &temp);

  if (!success) {
    ArmExceptionDataABT(registers);
  } else {
    registers->current.user.gprs.gprs[Rd] = temp;
  }

  return success;
}

static inline bool ArmSTRH_DAW(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast8_t offset) {
  uint16_t temp = registers->current.user.gprs.gprs[Rd];
  bool success =
      ArmStore16LE(memory, registers->current.user.gprs.gprs[Rn], temp);
  registers->current.user.gprs.gprs[Rn] -= offset;

  if (!success) {
    ArmExceptionDataABT(registers);
  } else {
    registers->current.user.gprs.gprs[Rd] = temp;
  }

  return success;
}

static inline bool ArmSTRH_DB(ArmAllRegisters *registers, Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint32_t offset) {
  uint16_t temp = registers->current.user.gprs.gprs[Rd];
  bool success = ArmStore16LE(
      memory, registers->current.user.gprs.gprs[Rn] - offset, temp);

  if (!success) {
    ArmExceptionDataABT(registers);
  }

  return success;
}

static inline bool ArmSTRH_DBW(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast8_t offset) {
  registers->current.user.gprs.gprs[Rn] -= offset;
  uint16_t temp = registers->current.user.gprs.gprs[Rd];
  bool success =
      ArmStore16LE(memory, registers->current.user.gprs.gprs[Rn], temp);

  if (!success) {
    ArmExceptionDataABT(registers);
  }

  return success;
}

static inline bool ArmSTRH_IAW(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast8_t offset) {
  uint16_t temp = registers->current.user.gprs.gprs[Rd];
  bool success =
      ArmStore16LE(memory, registers->current.user.gprs.gprs[Rn], temp);
  registers->current.user.gprs.gprs[Rn] += offset;

  if (!success) {
    ArmExceptionDataABT(registers);
  }

  return success;
}

static inline bool ArmSTRH_IB(ArmAllRegisters *registers, Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint32_t offset) {
  uint16_t temp = registers->current.user.gprs.gprs[Rd];
  bool success = ArmStore16LE(
      memory, registers->current.user.gprs.gprs[Rn] + offset, temp);

  if (!success) {
    ArmExceptionDataABT(registers);
  }

  return success;
}

static inline bool ArmSTRH_IBW(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast8_t offset) {
  registers->current.user.gprs.gprs[Rn] += offset;
  uint16_t temp = registers->current.user.gprs.gprs[Rd];
  bool success =
      ArmStore16LE(memory, registers->current.user.gprs.gprs[Rn], temp);

  if (!success) {
    ArmExceptionDataABT(registers);
  }

  return success;
}

static inline bool ArmSTRSB_DAW(ArmAllRegisters *registers, Memory *memory,
                                ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                                uint_fast8_t offset) {
  int8_t temp = registers->current.user.gprs.gprs_s[Rd];
  bool success = Store8S(memory, registers->current.user.gprs.gprs[Rn], temp);
  registers->current.user.gprs.gprs[Rn] -= offset;

  if (!success) {
    ArmExceptionDataABT(registers);
  }

  return success;
}

static inline bool ArmSTRSB_DB(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint32_t offset) {
  int8_t temp = registers->current.user.gprs.gprs_s[Rd];
  bool success =
      Store8S(memory, registers->current.user.gprs.gprs[Rn] - offset, temp);

  if (!success) {
    ArmExceptionDataABT(registers);
  }

  return success;
}

static inline bool ArmSTRSB_DBW(ArmAllRegisters *registers, Memory *memory,
                                ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                                uint_fast8_t offset) {
  registers->current.user.gprs.gprs[Rn] -= offset;
  int8_t temp = registers->current.user.gprs.gprs_s[Rd];
  bool success = Store8S(memory, registers->current.user.gprs.gprs[Rn], temp);

  if (!success) {
    ArmExceptionDataABT(registers);
  }

  return success;
}

static inline bool ArmSTRSB_IAW(ArmAllRegisters *registers, Memory *memory,
                                ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                                uint_fast8_t offset) {
  int8_t temp = registers->current.user.gprs.gprs_s[Rd];
  bool success = Store8S(memory, registers->current.user.gprs.gprs[Rn], temp);
  registers->current.user.gprs.gprs[Rn] += offset;

  if (!success) {
    ArmExceptionDataABT(registers);
  }

  return success;
}

static inline bool ArmSTRSB_IB(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint32_t offset) {
  int8_t temp = registers->current.user.gprs.gprs_s[Rd];
  bool success =
      Store8S(memory, registers->current.user.gprs.gprs[Rn] + offset, temp);

  if (!success) {
    ArmExceptionDataABT(registers);
  }

  return success;
}

static inline bool ArmSTRSB_IBW(ArmAllRegisters *registers, Memory *memory,
                                ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                                uint_fast8_t offset) {
  registers->current.user.gprs.gprs[Rn] += offset;
  int8_t temp = registers->current.user.gprs.gprs_s[Rd];
  bool success = Store8S(memory, registers->current.user.gprs.gprs[Rn], temp);

  if (!success) {
    ArmExceptionDataABT(registers);
  }

  return success;
}

static inline bool ArmSTRSH_DAW(ArmAllRegisters *registers, Memory *memory,
                                ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                                uint_fast8_t offset) {
  int16_t temp = registers->current.user.gprs.gprs_s[Rd];
  bool success =
      ArmStore16SLE(memory, registers->current.user.gprs.gprs[Rn], temp);
  registers->current.user.gprs.gprs[Rn] -= offset;

  if (!success) {
    ArmExceptionDataABT(registers);
  }

  return success;
}

static inline bool ArmSTRSH_DB(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint32_t offset) {
  int16_t temp = registers->current.user.gprs.gprs_s[Rd];
  bool success = ArmStore16SLE(
      memory, registers->current.user.gprs.gprs[Rn] - offset, temp);

  if (!success) {
    ArmExceptionDataABT(registers);
  }

  return success;
}

static inline bool ArmSTRSH_DBW(ArmAllRegisters *registers, Memory *memory,
                                ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                                uint_fast8_t offset) {
  registers->current.user.gprs.gprs[Rn] -= offset;
  int16_t temp = registers->current.user.gprs.gprs_s[Rd];
  bool success =
      ArmStore16SLE(memory, registers->current.user.gprs.gprs[Rn], temp);

  if (!success) {
    ArmExceptionDataABT(registers);
  }

  return success;
}

static inline bool ArmSTRSH_IAW(ArmAllRegisters *registers, Memory *memory,
                                ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                                uint_fast8_t offset) {
  int16_t temp = registers->current.user.gprs.gprs_s[Rd];
  bool success =
      ArmStore16SLE(memory, registers->current.user.gprs.gprs[Rn], temp);
  registers->current.user.gprs.gprs[Rn] += offset;

  if (!success) {
    ArmExceptionDataABT(registers);
  }

  return success;
}

static inline bool ArmSTRSH_IB(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint32_t offset) {
  int16_t temp = registers->current.user.gprs.gprs_s[Rd];
  bool success = ArmStore16SLE(
      memory, registers->current.user.gprs.gprs[Rn] + offset, temp);

  if (!success) {
    ArmExceptionDataABT(registers);
  }

  return success;
}

static inline bool ArmSTRSH_IBW(ArmAllRegisters *registers, Memory *memory,
                                ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                                uint_fast8_t offset) {
  registers->current.user.gprs.gprs[Rn] += offset;
  int16_t temp = registers->current.user.gprs.gprs_s[Rd];
  bool success =
      ArmStore16SLE(memory, registers->current.user.gprs.gprs[Rn], temp);

  if (!success) {
    ArmExceptionDataABT(registers);
  }

  return success;
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_SIGNED_DATA_TRANSFER_