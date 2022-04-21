#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_LOAD_STORE_REGISTER_BYTE_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_LOAD_STORE_REGISTER_BYTE_

#include "emulator/cpu/arm7tdmi/exceptions.h"
#include "emulator/cpu/arm7tdmi/memory.h"
#include "emulator/cpu/arm7tdmi/registers.h"

static inline void ArmLDR_DAW(ArmAllRegisters *registers, const Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint_fast16_t offset) {
  uint32_t value;
  bool success =
      ArmLoad32LE(memory, registers->current.user.gprs.gprs[Rn], &value);
  uint32_t writeback = registers->current.user.gprs.gprs[Rn] - offset;

  if (!success) {
    ArmLoadGPSR(registers, Rn, writeback);
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rn, writeback);
  ArmLoadGPSR(registers, Rd, value);
}

static inline void ArmLDR_DB(ArmAllRegisters *registers, const Memory *memory,
                             ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                             uint32_t offset) {
  uint32_t value;
  bool success = ArmLoad32LE(
      memory, registers->current.user.gprs.gprs[Rn] - offset, &value);

  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rd, value);
}

static inline void ArmLDR_DBW(ArmAllRegisters *registers, const Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint_fast16_t offset) {
  uint32_t writeback = registers->current.user.gprs.gprs[Rn] - offset;
  uint32_t value;
  bool success = ArmLoad32LE(memory, writeback, &value);

  if (!success) {
    ArmLoadGPSR(registers, Rn, writeback);
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rn, writeback);
  ArmLoadGPSR(registers, Rd, value);
}

static inline void ArmLDR_IAW(ArmAllRegisters *registers, const Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint_fast16_t offset) {
  uint32_t value;
  bool success =
      ArmLoad32LE(memory, registers->current.user.gprs.gprs[Rn], &value);
  uint32_t writeback = registers->current.user.gprs.gprs[Rn] + offset;

  if (!success) {
    ArmLoadGPSR(registers, Rn, writeback);
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rn, writeback);
  ArmLoadGPSR(registers, Rd, value);
}

static inline void ArmLDR_IB(ArmAllRegisters *registers, const Memory *memory,
                             ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                             uint32_t offset) {
  uint32_t value;
  bool success = ArmLoad32LE(
      memory, registers->current.user.gprs.gprs[Rn] + offset, &value);

  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rd, value);
}

static inline void ArmLDR_IBW(ArmAllRegisters *registers, const Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint_fast16_t offset) {
  uint32_t writeback = registers->current.user.gprs.gprs[Rn] + offset;
  uint32_t value;
  bool success = ArmLoad32LE(memory, writeback, &value);

  if (!success) {
    ArmLoadGPSR(registers, Rn, writeback);
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rn, writeback);
  ArmLoadGPSR(registers, Rd, value);
}

static inline void ArmLDRT_DAW(ArmAllRegisters *registers, const Memory *memory,
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

  uint32_t writeback = registers->current.user.gprs.gprs[Rn] - offset;

  if (!success) {
    ArmLoadGPSR(registers, Rn, writeback);
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rn, writeback);
  ArmLoadGPSR(registers, Rd, value);
}

static inline void ArmLDRT_DB(ArmAllRegisters *registers, const Memory *memory,
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
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rd, value);
}

static inline void ArmLDRT_IAW(ArmAllRegisters *registers, const Memory *memory,
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

  uint32_t writeback = registers->current.user.gprs.gprs[Rn] + offset;

  if (!success) {
    ArmLoadGPSR(registers, Rn, writeback);
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rn, writeback);
  ArmLoadGPSR(registers, Rd, value);
}

static inline void ArmLDRT_IB(ArmAllRegisters *registers, const Memory *memory,
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
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rd, value);
}

static inline void ArmLDRB_DAW(ArmAllRegisters *registers, const Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast16_t offset) {
  uint8_t temp;
  bool success = Load8(memory, registers->current.user.gprs.gprs[Rn], &temp);
  uint32_t writeback = registers->current.user.gprs.gprs[Rn] - offset;

  if (!success) {
    ArmLoadGPSR(registers, Rn, writeback);
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rn, writeback);
  ArmLoadGPSR(registers, Rd, temp);
}

static inline void ArmLDRB_DB(ArmAllRegisters *registers, const Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint32_t offset) {
  uint8_t temp;
  bool success =
      Load8(memory, registers->current.user.gprs.gprs[Rn] - offset, &temp);

  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rd, temp);
}

static inline void ArmLDRB_DBW(ArmAllRegisters *registers, const Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast16_t offset) {
  uint32_t writeback = registers->current.user.gprs.gprs[Rn] - offset;
  uint8_t temp;
  bool success = Load8(memory, writeback, &temp);

  if (!success) {
    ArmLoadGPSR(registers, Rn, writeback);
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rn, writeback);
  ArmLoadGPSR(registers, Rd, temp);
}

static inline void ArmLDRB_IAW(ArmAllRegisters *registers, const Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast16_t offset) {
  uint8_t temp;
  bool success = Load8(memory, registers->current.user.gprs.gprs[Rn], &temp);
  uint32_t writeback = registers->current.user.gprs.gprs[Rn] + offset;

  if (!success) {
    ArmLoadGPSR(registers, Rn, writeback);
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rn, writeback);
  ArmLoadGPSR(registers, Rd, temp);
}

static inline void ArmLDRB_IB(ArmAllRegisters *registers, const Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint32_t offset) {
  uint8_t temp;
  bool success =
      Load8(memory, registers->current.user.gprs.gprs[Rn] + offset, &temp);

  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rd, temp);
}

static inline void ArmLDRB_IBW(ArmAllRegisters *registers, const Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast16_t offset) {
  uint32_t writeback = registers->current.user.gprs.gprs[Rn] + offset;
  uint8_t temp;
  bool success = Load8(memory, writeback, &temp);

  if (!success) {
    ArmLoadGPSR(registers, Rn, writeback);
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rn, writeback);
  ArmLoadGPSR(registers, Rd, temp);
}

static inline void ArmLDRBT_DAW(ArmAllRegisters *registers,
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

  uint32_t writeback = registers->current.user.gprs.gprs[Rn] - offset;

  if (!success) {
    ArmLoadGPSR(registers, Rn, writeback);
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rn, writeback);
  ArmLoadGPSR(registers, Rd, value);
}

static inline void ArmLDRBT_DB(ArmAllRegisters *registers, const Memory *memory,
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
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rd, value);
}

static inline void ArmLDRBT_IAW(ArmAllRegisters *registers,
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

  uint32_t writeback = registers->current.user.gprs.gprs[Rn] + offset;

  if (!success) {
    ArmLoadGPSR(registers, Rn, writeback);
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rn, writeback);
  ArmLoadGPSR(registers, Rd, value);
}

static inline void ArmLDRBT_IB(ArmAllRegisters *registers, const Memory *memory,
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
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rd, value);
}

static inline void ArmSTR_DAW(ArmAllRegisters *registers, Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint_fast16_t offset) {
  uint32_t value = registers->current.user.gprs.gprs[Rd];
  if (Rd == REGISTER_R15) {
    value += 4u;
  }

  bool success =
      ArmStore32LE(memory, registers->current.user.gprs.gprs[Rn], value);
  uint32_t writeback = registers->current.user.gprs.gprs[Rn] - offset;

  if (!success) {
    ArmLoadGPSR(registers, Rn, writeback);
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rn, writeback);
}

static inline void ArmSTR_DB(ArmAllRegisters *registers, Memory *memory,
                             ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                             uint32_t offset) {
  uint32_t value = registers->current.user.gprs.gprs[Rd];
  if (Rd == REGISTER_R15) {
    value += 4u;
  }

  bool success = ArmStore32LE(
      memory, registers->current.user.gprs.gprs[Rn] - offset, value);
  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
}

static inline void ArmSTR_DBW(ArmAllRegisters *registers, Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint_fast16_t offset) {
  uint32_t value = registers->current.user.gprs.gprs[Rd];
  if (Rd == REGISTER_R15) {
    value += 4u;
  }

  uint32_t writeback = registers->current.user.gprs.gprs[Rn] - offset;
  bool success = ArmStore32LE(memory, writeback, value);
  if (!success) {
    ArmLoadGPSR(registers, Rn, writeback);
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rn, writeback);
}

static inline void ArmSTR_IAW(ArmAllRegisters *registers, Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint_fast16_t offset) {
  uint32_t value = registers->current.user.gprs.gprs[Rd];
  if (Rd == REGISTER_R15) {
    value += 4u;
  }

  bool success =
      ArmStore32LE(memory, registers->current.user.gprs.gprs[Rn], value);
  uint32_t writeback = registers->current.user.gprs.gprs[Rn] + offset;

  if (!success) {
    ArmLoadGPSR(registers, Rn, writeback);
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rn, writeback);
}

static inline void ArmSTR_IB(ArmAllRegisters *registers, Memory *memory,
                             ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                             uint32_t offset) {
  uint32_t value = registers->current.user.gprs.gprs[Rd];
  if (Rd == REGISTER_R15) {
    value += 4u;
  }

  bool success = ArmStore32LE(
      memory, registers->current.user.gprs.gprs[Rn] + offset, value);
  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
}

static inline void ArmSTR_IBW(ArmAllRegisters *registers, Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint_fast16_t offset) {
  uint32_t value = registers->current.user.gprs.gprs[Rd];
  if (Rd == REGISTER_R15) {
    value += 4u;
  }

  uint32_t writeback = registers->current.user.gprs.gprs[Rn] + offset;
  bool success = ArmStore32LE(memory, writeback, value);
  if (!success) {
    ArmLoadGPSR(registers, Rn, writeback);
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rn, writeback);
}

static inline void ArmSTRT_DAW(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast16_t offset) {
  ArmProgramStatusRegister current_status = registers->current.user.cpsr;
  ArmProgramStatusRegister temporary_status = current_status;
  temporary_status.mode = MODE_USR;
  uint32_t address = registers->current.user.gprs.gprs[Rn];
  uint32_t value = registers->current.user.gprs.gprs[Rd];
  if (Rd == REGISTER_R15) {
    value += 4u;
  }

  ArmLoadCPSR(registers, temporary_status);
  bool success = ArmStore32LE(memory, address, value);
  ArmLoadCPSR(registers, current_status);

  uint32_t writeback = registers->current.user.gprs.gprs[Rn] - offset;

  if (!success) {
    ArmLoadGPSR(registers, Rn, writeback);
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rn, writeback);
}

static inline void ArmSTRT_DB(ArmAllRegisters *registers, Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint32_t offset) {
  ArmProgramStatusRegister current_status = registers->current.user.cpsr;
  ArmProgramStatusRegister temporary_status = current_status;
  temporary_status.mode = MODE_USR;
  uint32_t value = registers->current.user.gprs.gprs[Rd];
  if (Rd == REGISTER_R15) {
    value += 4u;
  }

  ArmLoadCPSR(registers, temporary_status);
  bool success = ArmStore32LE(
      memory, registers->current.user.gprs.gprs[Rn] - offset, value);
  ArmLoadCPSR(registers, current_status);

  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
}

static inline void ArmSTRT_IB(ArmAllRegisters *registers, Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint32_t offset) {
  ArmProgramStatusRegister current_status = registers->current.user.cpsr;
  ArmProgramStatusRegister temporary_status = current_status;
  temporary_status.mode = MODE_USR;
  uint32_t value = registers->current.user.gprs.gprs[Rd];
  if (Rd == REGISTER_R15) {
    value += 4u;
  }

  ArmLoadCPSR(registers, temporary_status);
  bool success = ArmStore32LE(
      memory, registers->current.user.gprs.gprs[Rn] + offset, value);
  ArmLoadCPSR(registers, current_status);

  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
}

static inline void ArmSTRT_IAW(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast16_t offset) {
  ArmProgramStatusRegister current_status = registers->current.user.cpsr;
  ArmProgramStatusRegister temporary_status = current_status;
  uint32_t address = registers->current.user.gprs.gprs[Rn];
  uint32_t value = registers->current.user.gprs.gprs[Rd];
  if (Rd == REGISTER_R15) {
    value += 4u;
  }

  ArmLoadCPSR(registers, temporary_status);
  bool success = ArmStore32LE(memory, address, value);
  ArmLoadCPSR(registers, current_status);

  uint32_t writeback = registers->current.user.gprs.gprs[Rn] + offset;

  if (!success) {
    ArmLoadGPSR(registers, Rn, writeback);
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rn, writeback);
}

static inline void ArmSTRB_DAW(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast16_t offset) {
  uint8_t temp = registers->current.user.gprs.gprs[Rd];
  if (Rd == REGISTER_R15) {
    temp += 4u;
  }

  bool success = Store8(memory, registers->current.user.gprs.gprs[Rn], temp);
  uint32_t writeback = registers->current.user.gprs.gprs[Rn] - offset;

  if (!success) {
    ArmLoadGPSR(registers, Rn, writeback);
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rn, writeback);
}

static inline void ArmSTRB_DB(ArmAllRegisters *registers, Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint32_t offset) {
  uint8_t temp = registers->current.user.gprs.gprs[Rd];
  if (Rd == REGISTER_R15) {
    temp += 4u;
  }

  bool success =
      Store8(memory, registers->current.user.gprs.gprs[Rn] - offset, temp);

  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
}

static inline void ArmSTRB_DBW(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast16_t offset) {
  uint32_t writeback = registers->current.user.gprs.gprs[Rn] - offset;
  uint8_t temp = registers->current.user.gprs.gprs[Rd];
  if (Rd == REGISTER_R15) {
    temp += 4u;
  }

  bool success = Store8(memory, writeback, temp);

  if (!success) {
    ArmLoadGPSR(registers, Rn, writeback);
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rn, writeback);
}

static inline void ArmSTRB_IAW(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast16_t offset) {
  uint8_t temp = registers->current.user.gprs.gprs[Rd];
  if (Rd == REGISTER_R15) {
    temp += 4u;
  }

  bool success = Store8(memory, registers->current.user.gprs.gprs[Rn], temp);
  uint32_t writeback = registers->current.user.gprs.gprs[Rn] + offset;

  if (!success) {
    ArmLoadGPSR(registers, Rn, writeback);
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rn, writeback);
}

static inline void ArmSTRB_IB(ArmAllRegisters *registers, Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint32_t offset) {
  uint8_t temp = registers->current.user.gprs.gprs[Rd];
  if (Rd == REGISTER_R15) {
    temp += 4u;
  }

  bool success =
      Store8(memory, registers->current.user.gprs.gprs[Rn] + offset, temp);

  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
}

static inline void ArmSTRB_IBW(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast16_t offset) {
  uint32_t writeback = registers->current.user.gprs.gprs[Rn] + offset;
  uint8_t temp = registers->current.user.gprs.gprs[Rd];
  if (Rd == REGISTER_R15) {
    temp += 4u;
  }

  bool success = Store8(memory, writeback, temp);

  if (!success) {
    ArmLoadGPSR(registers, Rn, writeback);
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rn, writeback);
}

static inline void ArmSTRBT_DAW(ArmAllRegisters *registers, Memory *memory,
                                ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                                uint_fast16_t offset) {
  ArmProgramStatusRegister current_status = registers->current.user.cpsr;
  ArmProgramStatusRegister temporary_status = current_status;
  temporary_status.mode = MODE_USR;
  uint32_t address = registers->current.user.gprs.gprs[Rn];
  uint8_t value = registers->current.user.gprs.gprs[Rd];
  if (Rd == REGISTER_R15) {
    value += 4u;
  }

  ArmLoadCPSR(registers, temporary_status);
  bool success = Store8(memory, address, value);
  ArmLoadCPSR(registers, current_status);

  uint32_t writeback = registers->current.user.gprs.gprs[Rn] - offset;

  if (!success) {
    ArmLoadGPSR(registers, Rn, writeback);
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rn, writeback);
}

static inline void ArmSTRBT_DB(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint32_t offset) {
  ArmProgramStatusRegister current_status = registers->current.user.cpsr;
  ArmProgramStatusRegister temporary_status = current_status;
  temporary_status.mode = MODE_USR;
  uint8_t temp = registers->current.user.gprs.gprs[Rd];
  if (Rd == REGISTER_R15) {
    temp += 4u;
  }

  ArmLoadCPSR(registers, temporary_status);
  bool success =
      Store8(memory, registers->current.user.gprs.gprs[Rn] - offset, temp);
  ArmLoadCPSR(registers, current_status);

  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
}

static inline void ArmSTRBT_IAW(ArmAllRegisters *registers, Memory *memory,
                                ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                                uint_fast16_t offset) {
  ArmProgramStatusRegister current_status = registers->current.user.cpsr;
  ArmProgramStatusRegister temporary_status = current_status;
  temporary_status.mode = MODE_USR;
  uint32_t address = registers->current.user.gprs.gprs[Rn];
  uint8_t value = registers->current.user.gprs.gprs[Rd];
  if (Rd == REGISTER_R15) {
    value += 4u;
  }

  ArmLoadCPSR(registers, temporary_status);
  bool success = Store8(memory, address, value);
  ArmLoadCPSR(registers, current_status);

  uint32_t writeback = registers->current.user.gprs.gprs[Rn] + offset;

  if (!success) {
    ArmLoadGPSR(registers, Rn, writeback);
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rn, writeback);
}

static inline void ArmSTRBT_IB(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint32_t offset) {
  ArmProgramStatusRegister current_status = registers->current.user.cpsr;
  ArmProgramStatusRegister temporary_status = current_status;
  temporary_status.mode = MODE_USR;
  uint8_t temp = registers->current.user.gprs.gprs[Rd];
  if (Rd == REGISTER_R15) {
    temp += 4u;
  }

  ArmLoadCPSR(registers, temporary_status);
  bool success =
      Store8(memory, registers->current.user.gprs.gprs[Rn] + offset, temp);
  ArmLoadCPSR(registers, current_status);

  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_LOAD_STORE_REGISTER_BYTE_