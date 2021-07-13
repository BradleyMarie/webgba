#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_LOAD_STORE_REGISTER_BYTE_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_LOAD_STORE_REGISTER_BYTE_

#include "emulator/cpu/arm7tdmi/exceptions.h"
#include "emulator/cpu/arm7tdmi/memory.h"
#include "emulator/cpu/arm7tdmi/registers.h"

static inline bool ArmLDR_DAW(ArmAllRegisters *registers, const Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint_fast16_t offset) {
  bool success = ArmLoad32LE(memory, registers->current.user.gprs.gprs[Rn],
                             &registers->current.user.gprs.gprs[Rd]);
  registers->current.user.gprs.gprs[Rn] -= offset;
  if (!success) {
    ArmExceptionDataABT(registers);
  }
  return success;
}

static inline bool ArmLDR_DB(ArmAllRegisters *registers, const Memory *memory,
                             ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                             uint32_t offset) {
  bool success =
      ArmLoad32LE(memory, registers->current.user.gprs.gprs[Rn] - offset,
                  &registers->current.user.gprs.gprs[Rd]);
  if (!success) {
    ArmExceptionDataABT(registers);
  }
  return success;
}

static inline bool ArmLDR_DBW(ArmAllRegisters *registers, const Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint_fast16_t offset) {
  registers->current.user.gprs.gprs[Rn] -= offset;
  bool success = ArmLoad32LE(memory, registers->current.user.gprs.gprs[Rn],
                             &registers->current.user.gprs.gprs[Rd]);
  if (!success) {
    ArmExceptionDataABT(registers);
  }
  return success;
}

static inline bool ArmLDR_IAW(ArmAllRegisters *registers, const Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint_fast16_t offset) {
  bool success = ArmLoad32LE(memory, registers->current.user.gprs.gprs[Rn],
                             &registers->current.user.gprs.gprs[Rd]);
  registers->current.user.gprs.gprs[Rn] += offset;
  if (!success) {
    ArmExceptionDataABT(registers);
  }
  return success;
}

static inline bool ArmLDR_IB(ArmAllRegisters *registers, const Memory *memory,
                             ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                             uint32_t offset) {
  bool success =
      ArmLoad32LE(memory, registers->current.user.gprs.gprs[Rn] + offset,
                  &registers->current.user.gprs.gprs[Rd]);
  if (!success) {
    ArmExceptionDataABT(registers);
  }
  return success;
}

static inline bool ArmLDR_IBW(ArmAllRegisters *registers, const Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint_fast16_t offset) {
  registers->current.user.gprs.gprs[Rn] += offset;
  bool success = ArmLoad32LE(memory, registers->current.user.gprs.gprs[Rn],
                             &registers->current.user.gprs.gprs[Rd]);
  if (!success) {
    ArmExceptionDataABT(registers);
  }
  return success;
}

static inline bool ArmLDRT_DAW(ArmAllRegisters *registers, const Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast16_t offset) {
  ArmProgramStatusRegister current_status = registers->current.user.cpsr;
  ArmProgramStatusRegister temporary_status = current_status;
  temporary_status.mode = MODE_USR;
  uint32_t address = registers->current.user.gprs.gprs[Rn];

  ArmLoadCPSR(registers, temporary_status);
  uint32_t value;
  bool success = ArmLoad32LE(memory, address, &value);
  ArmLoadCPSR(registers, current_status);

  registers->current.user.gprs.gprs[Rn] -= offset;

  if (!success) {
    ArmExceptionDataABT(registers);
  } else {
    registers->current.user.gprs.gprs[Rd] = value;
  }

  return success;
}

static inline bool ArmLDRT_DB(ArmAllRegisters *registers, const Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint32_t offset) {
  ArmProgramStatusRegister current_status = registers->current.user.cpsr;
  ArmProgramStatusRegister temporary_status = current_status;
  temporary_status.mode = MODE_USR;

  ArmLoadCPSR(registers, temporary_status);
  uint32_t value;
  bool success = ArmLoad32LE(
      memory, registers->current.user.gprs.gprs[Rn] - offset, &value);
  ArmLoadCPSR(registers, current_status);

  if (!success) {
    ArmExceptionDataABT(registers);
  } else {
    registers->current.user.gprs.gprs[Rd] = value;
  }

  return success;
}

static inline bool ArmLDRT_IAW(ArmAllRegisters *registers, const Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast16_t offset) {
  ArmProgramStatusRegister current_status = registers->current.user.cpsr;
  ArmProgramStatusRegister temporary_status = current_status;
  temporary_status.mode = MODE_USR;
  uint32_t address = registers->current.user.gprs.gprs[Rn];

  ArmLoadCPSR(registers, temporary_status);
  uint32_t value;
  bool success = ArmLoad32LE(memory, address, &value);
  ArmLoadCPSR(registers, current_status);

  registers->current.user.gprs.gprs[Rn] += offset;

  if (!success) {
    ArmExceptionDataABT(registers);
  } else {
    registers->current.user.gprs.gprs[Rd] = value;
  }

  return success;
}

static inline bool ArmLDRT_IB(ArmAllRegisters *registers, const Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint32_t offset) {
  ArmProgramStatusRegister current_status = registers->current.user.cpsr;
  ArmProgramStatusRegister temporary_status = current_status;
  temporary_status.mode = MODE_USR;

  ArmLoadCPSR(registers, temporary_status);
  uint32_t value;
  bool success = ArmLoad32LE(
      memory, registers->current.user.gprs.gprs[Rn] + offset, &value);
  ArmLoadCPSR(registers, current_status);

  if (!success) {
    ArmExceptionDataABT(registers);
  } else {
    registers->current.user.gprs.gprs[Rd] = value;
  }

  return success;
}

static inline bool ArmLDRB_DAW(ArmAllRegisters *registers, const Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast16_t offset) {
  uint8_t temp;
  bool success = Load8(memory, registers->current.user.gprs.gprs[Rn], &temp);
  registers->current.user.gprs.gprs[Rn] -= offset;

  if (!success) {
    ArmExceptionDataABT(registers);
  } else {
    registers->current.user.gprs.gprs[Rd] = temp;
  }

  return success;
}

static inline bool ArmLDRB_DB(ArmAllRegisters *registers, const Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint32_t offset) {
  uint8_t temp;
  bool success =
      Load8(memory, registers->current.user.gprs.gprs[Rn] - offset, &temp);

  if (!success) {
    ArmExceptionDataABT(registers);
  } else {
    registers->current.user.gprs.gprs[Rd] = temp;
  }

  return success;
}

static inline bool ArmLDRB_DBW(ArmAllRegisters *registers, const Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast16_t offset) {
  registers->current.user.gprs.gprs[Rn] -= offset;
  uint8_t temp;
  bool success = Load8(memory, registers->current.user.gprs.gprs[Rn], &temp);

  if (!success) {
    ArmExceptionDataABT(registers);
  } else {
    registers->current.user.gprs.gprs[Rd] = temp;
  }

  return success;
}

static inline bool ArmLDRB_IAW(ArmAllRegisters *registers, const Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast16_t offset) {
  uint8_t temp;
  bool success = Load8(memory, registers->current.user.gprs.gprs[Rn], &temp);
  registers->current.user.gprs.gprs[Rn] += offset;

  if (!success) {
    ArmExceptionDataABT(registers);
  } else {
    registers->current.user.gprs.gprs[Rd] = temp;
  }

  return success;
}

static inline bool ArmLDRB_IB(ArmAllRegisters *registers, const Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint32_t offset) {
  uint8_t temp;
  bool success =
      Load8(memory, registers->current.user.gprs.gprs[Rn] + offset, &temp);

  if (!success) {
    ArmExceptionDataABT(registers);
  } else {
    registers->current.user.gprs.gprs[Rd] = temp;
  }

  return success;
}

static inline bool ArmLDRB_IBW(ArmAllRegisters *registers, const Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast16_t offset) {
  registers->current.user.gprs.gprs[Rn] += offset;
  uint8_t temp;
  bool success = Load8(memory, registers->current.user.gprs.gprs[Rn], &temp);

  if (!success) {
    ArmExceptionDataABT(registers);
  } else {
    registers->current.user.gprs.gprs[Rd] = temp;
  }

  return success;
}

static inline bool ArmLDRBT_DAW(ArmAllRegisters *registers,
                                const Memory *memory, ArmRegisterIndex Rd,
                                ArmRegisterIndex Rn, uint_fast16_t offset) {
  ArmProgramStatusRegister current_status = registers->current.user.cpsr;
  ArmProgramStatusRegister temporary_status = current_status;
  temporary_status.mode = MODE_USR;
  uint32_t address = registers->current.user.gprs.gprs[Rn];

  ArmLoadCPSR(registers, temporary_status);
  uint8_t value;
  bool success = Load8(memory, address, &value);
  ArmLoadCPSR(registers, current_status);

  registers->current.user.gprs.gprs[Rn] -= offset;

  if (!success) {
    ArmExceptionDataABT(registers);
  } else {
    registers->current.user.gprs.gprs[Rd] = value;
  }

  return success;
}

static inline bool ArmLDRBT_DB(ArmAllRegisters *registers, const Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint32_t offset) {
  ArmProgramStatusRegister current_status = registers->current.user.cpsr;
  ArmProgramStatusRegister temporary_status = current_status;
  temporary_status.mode = MODE_USR;

  ArmLoadCPSR(registers, temporary_status);
  uint8_t value;
  bool success =
      Load8(memory, registers->current.user.gprs.gprs[Rn] - offset, &value);
  ArmLoadCPSR(registers, current_status);

  if (!success) {
    ArmExceptionDataABT(registers);
  } else {
    registers->current.user.gprs.gprs[Rd] = value;
  }

  return success;
}

static inline bool ArmLDRBT_IAW(ArmAllRegisters *registers,
                                const Memory *memory, ArmRegisterIndex Rd,
                                ArmRegisterIndex Rn, uint_fast16_t offset) {
  ArmProgramStatusRegister current_status = registers->current.user.cpsr;
  ArmProgramStatusRegister temporary_status = current_status;
  temporary_status.mode = MODE_USR;
  uint32_t address = registers->current.user.gprs.gprs[Rn];

  ArmLoadCPSR(registers, temporary_status);
  uint8_t value;
  bool success = Load8(memory, address, &value);
  ArmLoadCPSR(registers, current_status);

  registers->current.user.gprs.gprs[Rn] += offset;

  if (!success) {
    ArmExceptionDataABT(registers);
  } else {
    registers->current.user.gprs.gprs[Rd] = value;
  }

  return success;
}

static inline bool ArmLDRBT_IB(ArmAllRegisters *registers, const Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint32_t offset) {
  ArmProgramStatusRegister current_status = registers->current.user.cpsr;
  ArmProgramStatusRegister temporary_status = current_status;
  temporary_status.mode = MODE_USR;

  ArmLoadCPSR(registers, temporary_status);
  uint8_t value;
  bool success =
      Load8(memory, registers->current.user.gprs.gprs[Rn] + offset, &value);
  ArmLoadCPSR(registers, current_status);

  if (!success) {
    ArmExceptionDataABT(registers);
  } else {
    registers->current.user.gprs.gprs[Rd] = value;
  }

  return success;
}

static inline bool ArmSTR_DAW(ArmAllRegisters *registers, Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint_fast16_t offset) {
  bool success = ArmStore32LE(memory, registers->current.user.gprs.gprs[Rn],
                              registers->current.user.gprs.gprs[Rd]);
  registers->current.user.gprs.gprs[Rn] -= offset;

  if (!success) {
    ArmExceptionDataABT(registers);
  }

  return success;
}

static inline bool ArmSTR_DB(ArmAllRegisters *registers, Memory *memory,
                             ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                             uint32_t offset) {
  bool success =
      ArmStore32LE(memory, registers->current.user.gprs.gprs[Rn] - offset,
                   registers->current.user.gprs.gprs[Rd]);
  if (!success) {
    ArmExceptionDataABT(registers);
  }

  return success;
}

static inline bool ArmSTR_DBW(ArmAllRegisters *registers, Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint_fast16_t offset) {
  registers->current.user.gprs.gprs[Rn] -= offset;
  bool success = ArmStore32LE(memory, registers->current.user.gprs.gprs[Rn],
                              registers->current.user.gprs.gprs[Rd]);
  if (!success) {
    ArmExceptionDataABT(registers);
  }

  return success;
}

static inline bool ArmSTR_IAW(ArmAllRegisters *registers, Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint_fast16_t offset) {
  bool success = ArmStore32LE(memory, registers->current.user.gprs.gprs[Rn],
                              registers->current.user.gprs.gprs[Rd]);
  registers->current.user.gprs.gprs[Rn] += offset;

  if (!success) {
    ArmExceptionDataABT(registers);
  }

  return success;
}

static inline bool ArmSTR_IB(ArmAllRegisters *registers, Memory *memory,
                             ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                             uint32_t offset) {
  bool success =
      ArmStore32LE(memory, registers->current.user.gprs.gprs[Rn] + offset,
                   registers->current.user.gprs.gprs[Rd]);
  if (!success) {
    ArmExceptionDataABT(registers);
  }

  return success;
}

static inline bool ArmSTR_IBW(ArmAllRegisters *registers, Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint_fast16_t offset) {
  registers->current.user.gprs.gprs[Rn] += offset;
  bool success = ArmStore32LE(memory, registers->current.user.gprs.gprs[Rn],
                              registers->current.user.gprs.gprs[Rd]);
  if (!success) {
    ArmExceptionDataABT(registers);
  }

  return success;
}

static inline bool ArmSTRT_DAW(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast16_t offset) {
  ArmProgramStatusRegister current_status = registers->current.user.cpsr;
  ArmProgramStatusRegister temporary_status = current_status;
  temporary_status.mode = MODE_USR;
  uint32_t address = registers->current.user.gprs.gprs[Rn];
  uint32_t value = registers->current.user.gprs.gprs[Rd];

  ArmLoadCPSR(registers, temporary_status);
  bool success = ArmStore32LE(memory, address, value);
  ArmLoadCPSR(registers, current_status);

  registers->current.user.gprs.gprs[Rn] -= offset;

  if (!success) {
    ArmExceptionDataABT(registers);
  }

  return success;
}

static inline bool ArmSTRT_DB(ArmAllRegisters *registers, Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint32_t offset) {
  ArmProgramStatusRegister current_status = registers->current.user.cpsr;
  ArmProgramStatusRegister temporary_status = current_status;
  temporary_status.mode = MODE_USR;
  uint32_t value = registers->current.user.gprs.gprs[Rd];

  ArmLoadCPSR(registers, temporary_status);
  bool success = ArmStore32LE(
      memory, registers->current.user.gprs.gprs[Rn] - offset, value);
  ArmLoadCPSR(registers, current_status);

  if (!success) {
    ArmExceptionDataABT(registers);
  }

  return success;
}

static inline bool ArmSTRT_IB(ArmAllRegisters *registers, Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint32_t offset) {
  ArmProgramStatusRegister current_status = registers->current.user.cpsr;
  ArmProgramStatusRegister temporary_status = current_status;
  temporary_status.mode = MODE_USR;
  uint32_t value = registers->current.user.gprs.gprs[Rd];

  ArmLoadCPSR(registers, temporary_status);
  bool success = ArmStore32LE(
      memory, registers->current.user.gprs.gprs[Rn] + offset, value);
  ArmLoadCPSR(registers, current_status);

  if (!success) {
    ArmExceptionDataABT(registers);
  }

  return success;
}

static inline bool ArmSTRT_IAW(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast16_t offset) {
  ArmProgramStatusRegister current_status = registers->current.user.cpsr;
  ArmProgramStatusRegister temporary_status = current_status;
  uint32_t address = registers->current.user.gprs.gprs[Rn];
  uint32_t value = registers->current.user.gprs.gprs[Rd];

  ArmLoadCPSR(registers, temporary_status);
  bool success = ArmStore32LE(memory, address, value);
  ArmLoadCPSR(registers, current_status);

  registers->current.user.gprs.gprs[Rn] += offset;

  if (!success) {
    ArmExceptionDataABT(registers);
  }

  return success;
}

static inline bool ArmSTRB_DAW(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast16_t offset) {
  uint8_t temp = registers->current.user.gprs.gprs[Rd];
  bool success = Store8(memory, registers->current.user.gprs.gprs[Rn], temp);
  registers->current.user.gprs.gprs[Rn] -= offset;

  if (!success) {
    ArmExceptionDataABT(registers);
  }

  return success;
}

static inline bool ArmSTRB_DB(ArmAllRegisters *registers, Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint32_t offset) {
  uint8_t temp = registers->current.user.gprs.gprs[Rd];
  bool success =
      Store8(memory, registers->current.user.gprs.gprs[Rn] - offset, temp);

  if (!success) {
    ArmExceptionDataABT(registers);
  }

  return success;
}

static inline bool ArmSTRB_DBW(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast16_t offset) {
  registers->current.user.gprs.gprs[Rn] -= offset;
  uint8_t temp = registers->current.user.gprs.gprs[Rd];
  bool success = Store8(memory, registers->current.user.gprs.gprs[Rn], temp);

  if (!success) {
    ArmExceptionDataABT(registers);
  }

  return success;
}

static inline bool ArmSTRB_IAW(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast16_t offset) {
  uint8_t temp = registers->current.user.gprs.gprs[Rd];
  bool success = Store8(memory, registers->current.user.gprs.gprs[Rn], temp);
  registers->current.user.gprs.gprs[Rn] += offset;

  if (!success) {
    ArmExceptionDataABT(registers);
  }

  return success;
}

static inline bool ArmSTRB_IB(ArmAllRegisters *registers, Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint32_t offset) {
  uint8_t temp = registers->current.user.gprs.gprs[Rd];
  bool success =
      Store8(memory, registers->current.user.gprs.gprs[Rn] + offset, temp);

  if (!success) {
    ArmExceptionDataABT(registers);
  }

  return success;
}

static inline bool ArmSTRB_IBW(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast16_t offset) {
  registers->current.user.gprs.gprs[Rn] += offset;
  uint8_t temp = registers->current.user.gprs.gprs[Rd];
  bool success = Store8(memory, registers->current.user.gprs.gprs[Rn], temp);

  if (!success) {
    ArmExceptionDataABT(registers);
  }

  return success;
}

static inline bool ArmSTRBT_DAW(ArmAllRegisters *registers, Memory *memory,
                                ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                                uint_fast16_t offset) {
  ArmProgramStatusRegister current_status = registers->current.user.cpsr;
  ArmProgramStatusRegister temporary_status = current_status;
  temporary_status.mode = MODE_USR;
  uint32_t address = registers->current.user.gprs.gprs[Rn];
  uint8_t value = registers->current.user.gprs.gprs[Rd];

  ArmLoadCPSR(registers, temporary_status);
  bool success = Store8(memory, address, value);
  ArmLoadCPSR(registers, current_status);

  registers->current.user.gprs.gprs[Rn] -= offset;

  if (!success) {
    ArmExceptionDataABT(registers);
  }

  return success;
}

static inline bool ArmSTRBT_DB(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint32_t offset) {
  ArmProgramStatusRegister current_status = registers->current.user.cpsr;
  ArmProgramStatusRegister temporary_status = current_status;
  temporary_status.mode = MODE_USR;
  uint8_t temp = registers->current.user.gprs.gprs[Rd];

  ArmLoadCPSR(registers, temporary_status);
  bool success =
      Store8(memory, registers->current.user.gprs.gprs[Rn] - offset, temp);
  ArmLoadCPSR(registers, current_status);

  if (!success) {
    ArmExceptionDataABT(registers);
  }

  return success;
}

static inline bool ArmSTRBT_IAW(ArmAllRegisters *registers, Memory *memory,
                                ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                                uint_fast16_t offset) {
  ArmProgramStatusRegister current_status = registers->current.user.cpsr;
  ArmProgramStatusRegister temporary_status = current_status;
  temporary_status.mode = MODE_USR;
  uint32_t address = registers->current.user.gprs.gprs[Rn];
  uint8_t value = registers->current.user.gprs.gprs[Rd];

  ArmLoadCPSR(registers, temporary_status);
  bool success = Store8(memory, address, value);
  ArmLoadCPSR(registers, current_status);

  registers->current.user.gprs.gprs[Rn] += offset;

  if (!success) {
    ArmExceptionDataABT(registers);
  }

  return success;
}

static inline bool ArmSTRBT_IB(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint32_t offset) {
  ArmProgramStatusRegister current_status = registers->current.user.cpsr;
  ArmProgramStatusRegister temporary_status = current_status;
  temporary_status.mode = MODE_USR;
  uint8_t temp = registers->current.user.gprs.gprs[Rd];

  ArmLoadCPSR(registers, temporary_status);
  bool success =
      Store8(memory, registers->current.user.gprs.gprs[Rn] + offset, temp);
  ArmLoadCPSR(registers, current_status);

  if (!success) {
    ArmExceptionDataABT(registers);
  }

  return success;
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_LOAD_STORE_REGISTER_BYTE_