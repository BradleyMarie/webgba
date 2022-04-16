#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_SIGNED_DATA_TRANSFER_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_SIGNED_DATA_TRANSFER_

#include "emulator/cpu/arm7tdmi/exceptions.h"
#include "emulator/cpu/arm7tdmi/memory.h"
#include "emulator/cpu/arm7tdmi/registers.h"

static inline void ArmLDRH_DAW(ArmAllRegisters *registers, const Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast8_t offset) {
  uint16_t value;
  bool success =
      ArmLoad16LE(memory, registers->current.user.gprs.gprs[Rn], &value);
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

static inline void ArmLDRH_DB(ArmAllRegisters *registers, const Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint32_t offset) {
  uint16_t value;
  bool success = ArmLoad16LE(
      memory, registers->current.user.gprs.gprs[Rn] - offset, &value);

  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rd, value);
}

static inline void ArmLDRH_DBW(ArmAllRegisters *registers, const Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast8_t offset) {
  uint32_t writeback = registers->current.user.gprs.gprs[Rn] - offset;
  uint16_t value;
  bool success = ArmLoad16LE(memory, writeback, &value);

  if (!success) {
    ArmLoadGPSR(registers, Rn, writeback);
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rn, writeback);
  ArmLoadGPSR(registers, Rd, value);
}

static inline void ArmLDRH_IAW(ArmAllRegisters *registers, const Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast8_t offset) {
  uint16_t value;
  bool success =
      ArmLoad16LE(memory, registers->current.user.gprs.gprs[Rn], &value);
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

static inline void ArmLDRH_IB(ArmAllRegisters *registers, const Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint32_t offset) {
  uint16_t value;
  bool success = ArmLoad16LE(
      memory, registers->current.user.gprs.gprs[Rn] + offset, &value);

  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rd, value);
}

static inline void ArmLDRH_IBW(ArmAllRegisters *registers, const Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast8_t offset) {
  uint32_t writeback = registers->current.user.gprs.gprs[Rn] + offset;
  uint16_t value;
  bool success = ArmLoad16LE(memory, writeback, &value);

  if (!success) {
    ArmLoadGPSR(registers, Rn, writeback);
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rn, writeback);
  ArmLoadGPSR(registers, Rd, value);
}

static inline void ArmLDRSB_DAW(ArmAllRegisters *registers,
                                const Memory *memory, ArmRegisterIndex Rd,
                                ArmRegisterIndex Rn, uint_fast8_t offset) {
  int8_t value;
  bool success = Load8S(memory, registers->current.user.gprs.gprs[Rn], &value);
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

static inline void ArmLDRSB_DB(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint32_t offset) {
  int8_t value;
  bool success =
      Load8S(memory, registers->current.user.gprs.gprs[Rn] - offset, &value);

  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rd, value);
}

static inline void ArmLDRSB_DBW(ArmAllRegisters *registers,
                                const Memory *memory, ArmRegisterIndex Rd,
                                ArmRegisterIndex Rn, uint_fast8_t offset) {
  uint32_t writeback = registers->current.user.gprs.gprs[Rn] - offset;
  int8_t value;
  bool success = Load8S(memory, writeback, &value);

  if (!success) {
    ArmLoadGPSR(registers, Rn, writeback);
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rn, writeback);
  ArmLoadGPSR(registers, Rd, value);
}

static inline void ArmLDRSB_IAW(ArmAllRegisters *registers,
                                const Memory *memory, ArmRegisterIndex Rd,
                                ArmRegisterIndex Rn, uint_fast8_t offset) {
  int8_t value;
  bool success = Load8S(memory, registers->current.user.gprs.gprs[Rn], &value);
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

static inline void ArmLDRSB_IB(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint32_t offset) {
  int8_t value;
  bool success =
      Load8S(memory, registers->current.user.gprs.gprs[Rn] + offset, &value);

  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rd, value);
}

static inline void ArmLDRSB_IBW(ArmAllRegisters *registers,
                                const Memory *memory, ArmRegisterIndex Rd,
                                ArmRegisterIndex Rn, uint_fast8_t offset) {
  uint32_t writeback = registers->current.user.gprs.gprs[Rn] + offset;
  int8_t value;
  bool success = Load8S(memory, writeback, &value);

  if (!success) {
    ArmLoadGPSR(registers, Rn, writeback);
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rn, writeback);
  ArmLoadGPSR(registers, Rd, value);
}

static inline void ArmLDRSH_DAW(ArmAllRegisters *registers,
                                const Memory *memory, ArmRegisterIndex Rd,
                                ArmRegisterIndex Rn, uint_fast8_t offset) {
  int16_t value;
  bool success =
      ArmLoad16SLE(memory, registers->current.user.gprs.gprs[Rn], &value);
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

static inline void ArmLDRSH_DB(ArmAllRegisters *registers, const Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint32_t offset) {
  int16_t value;
  bool success = ArmLoad16SLE(
      memory, registers->current.user.gprs.gprs[Rn] - offset, &value);

  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rd, value);
}

static inline void ArmLDRSH_DBW(ArmAllRegisters *registers,
                                const Memory *memory, ArmRegisterIndex Rd,
                                ArmRegisterIndex Rn, uint_fast8_t offset) {
  uint32_t writeback = registers->current.user.gprs.gprs[Rn] - offset;
  int16_t value;
  bool success = ArmLoad16SLE(memory, writeback, &value);

  if (!success) {
    ArmLoadGPSR(registers, Rn, writeback);
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rn, writeback);
  ArmLoadGPSR(registers, Rd, value);
}

static inline void ArmLDRSH_IAW(ArmAllRegisters *registers,
                                const Memory *memory, ArmRegisterIndex Rd,
                                ArmRegisterIndex Rn, uint_fast8_t offset) {
  int16_t value;
  bool success =
      ArmLoad16SLE(memory, registers->current.user.gprs.gprs[Rn], &value);
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

static inline void ArmLDRSH_IB(ArmAllRegisters *registers, const Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint32_t offset) {
  int16_t value;
  bool success = ArmLoad16SLE(
      memory, registers->current.user.gprs.gprs[Rn] + offset, &value);

  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rd, value);
}

static inline void ArmLDRSH_IBW(ArmAllRegisters *registers,
                                const Memory *memory, ArmRegisterIndex Rd,
                                ArmRegisterIndex Rn, uint_fast8_t offset) {
  uint32_t writeback = registers->current.user.gprs.gprs[Rn] + offset;
  int16_t value;
  bool success = ArmLoad16SLE(memory, writeback, &value);

  if (!success) {
    ArmLoadGPSR(registers, Rn, writeback);
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rn, writeback);
  ArmLoadGPSR(registers, Rd, value);
}

static inline void ArmSTRH_DAW(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast8_t offset) {
  uint16_t value = registers->current.user.gprs.gprs[Rd];
  bool success =
      ArmStore16LE(memory, registers->current.user.gprs.gprs[Rn], value);
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

static inline void ArmSTRH_DB(ArmAllRegisters *registers, Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint32_t offset) {
  uint16_t value = registers->current.user.gprs.gprs[Rd];
  bool success = ArmStore16LE(
      memory, registers->current.user.gprs.gprs[Rn] - offset, value);

  if (!success) {
    ArmExceptionDataABT(registers);
  }

  ArmAdvanceProgramCounter(registers);
}

static inline void ArmSTRH_DBW(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast8_t offset) {
  uint32_t writeback = registers->current.user.gprs.gprs[Rn] - offset;
  uint16_t value = registers->current.user.gprs.gprs[Rd];
  bool success = ArmStore16LE(memory, writeback, value);

  if (!success) {
    ArmLoadGPSR(registers, Rn, writeback);
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rn, writeback);
}

static inline void ArmSTRH_IAW(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast8_t offset) {
  uint16_t value = registers->current.user.gprs.gprs[Rd];
  bool success =
      ArmStore16LE(memory, registers->current.user.gprs.gprs[Rn], value);
  uint32_t writeback = registers->current.user.gprs.gprs[Rn] + offset;

  if (!success) {
    ArmLoadGPSR(registers, Rn, writeback);
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rn, writeback);
}

static inline void ArmSTRH_IB(ArmAllRegisters *registers, Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint32_t offset) {
  uint16_t value = registers->current.user.gprs.gprs[Rd];
  bool success = ArmStore16LE(
      memory, registers->current.user.gprs.gprs[Rn] + offset, value);

  if (!success) {
    ArmExceptionDataABT(registers);
  }

  ArmAdvanceProgramCounter(registers);
}

static inline void ArmSTRH_IBW(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast8_t offset) {
  uint32_t writeback = registers->current.user.gprs.gprs[Rn] + offset;
  uint16_t value = registers->current.user.gprs.gprs[Rd];
  bool success = ArmStore16LE(memory, writeback, value);

  if (!success) {
    ArmLoadGPSR(registers, Rn, writeback);
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rn, writeback);
}

static inline void ArmSTRSB_DAW(ArmAllRegisters *registers, Memory *memory,
                                ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                                uint_fast8_t offset) {
  int8_t value = registers->current.user.gprs.gprs_s[Rd];
  bool success = Store8S(memory, registers->current.user.gprs.gprs[Rn], value);
  uint32_t writeback = registers->current.user.gprs.gprs[Rn] - offset;

  if (!success) {
    ArmLoadGPSR(registers, Rn, writeback);
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rn, writeback);
}

static inline void ArmSTRSB_DB(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint32_t offset) {
  int8_t value = registers->current.user.gprs.gprs_s[Rd];
  bool success =
      Store8S(memory, registers->current.user.gprs.gprs[Rn] - offset, value);

  if (!success) {
    ArmExceptionDataABT(registers);
  }

  ArmAdvanceProgramCounter(registers);
}

static inline void ArmSTRSB_DBW(ArmAllRegisters *registers, Memory *memory,
                                ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                                uint_fast8_t offset) {
  uint32_t writeback = registers->current.user.gprs.gprs[Rn] - offset;
  int8_t value = registers->current.user.gprs.gprs_s[Rd];
  bool success = Store8S(memory, writeback, value);

  if (!success) {
    ArmLoadGPSR(registers, Rn, writeback);
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rn, writeback);
}

static inline void ArmSTRSB_IAW(ArmAllRegisters *registers, Memory *memory,
                                ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                                uint_fast8_t offset) {
  int8_t value = registers->current.user.gprs.gprs_s[Rd];
  bool success = Store8S(memory, registers->current.user.gprs.gprs[Rn], value);
  uint32_t writeback = registers->current.user.gprs.gprs[Rn] + offset;

  if (!success) {
    ArmLoadGPSR(registers, Rn, writeback);
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rn, writeback);
}

static inline void ArmSTRSB_IB(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint32_t offset) {
  int8_t value = registers->current.user.gprs.gprs_s[Rd];
  bool success =
      Store8S(memory, registers->current.user.gprs.gprs[Rn] + offset, value);

  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
}

static inline void ArmSTRSB_IBW(ArmAllRegisters *registers, Memory *memory,
                                ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                                uint_fast8_t offset) {
  uint32_t writeback = registers->current.user.gprs.gprs[Rn] + offset;
  int8_t value = registers->current.user.gprs.gprs_s[Rd];
  bool success = Store8S(memory, writeback, value);

  if (!success) {
    ArmLoadGPSR(registers, Rn, writeback);
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rn, writeback);
}

static inline void ArmSTRSH_DAW(ArmAllRegisters *registers, Memory *memory,
                                ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                                uint_fast8_t offset) {
  int16_t value = registers->current.user.gprs.gprs_s[Rd];
  bool success =
      ArmStore16SLE(memory, registers->current.user.gprs.gprs[Rn], value);
  uint32_t writeback = registers->current.user.gprs.gprs[Rn] - offset;

  if (!success) {
    ArmLoadGPSR(registers, Rn, writeback);
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rn, writeback);
}

static inline void ArmSTRSH_DB(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint32_t offset) {
  int16_t value = registers->current.user.gprs.gprs_s[Rd];
  bool success = ArmStore16SLE(
      memory, registers->current.user.gprs.gprs[Rn] - offset, value);

  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
}

static inline void ArmSTRSH_DBW(ArmAllRegisters *registers, Memory *memory,
                                ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                                uint_fast8_t offset) {
  uint32_t writeback = registers->current.user.gprs.gprs[Rn] - offset;
  int16_t value = registers->current.user.gprs.gprs_s[Rd];
  bool success = ArmStore16SLE(memory, writeback, value);

  if (!success) {
    ArmLoadGPSR(registers, Rn, writeback);
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rn, writeback);
}

static inline void ArmSTRSH_IAW(ArmAllRegisters *registers, Memory *memory,
                                ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                                uint_fast8_t offset) {
  int16_t value = registers->current.user.gprs.gprs_s[Rd];
  bool success =
      ArmStore16SLE(memory, registers->current.user.gprs.gprs[Rn], value);
  uint32_t writeback = registers->current.user.gprs.gprs[Rn] + offset;

  if (!success) {
    ArmLoadGPSR(registers, Rn, writeback);
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rn, writeback);
}

static inline void ArmSTRSH_IB(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint32_t offset) {
  int16_t value = registers->current.user.gprs.gprs_s[Rd];
  bool success = ArmStore16SLE(
      memory, registers->current.user.gprs.gprs[Rn] + offset, value);

  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
}

static inline void ArmSTRSH_IBW(ArmAllRegisters *registers, Memory *memory,
                                ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                                uint_fast8_t offset) {
  uint32_t writeback = registers->current.user.gprs.gprs[Rn] + offset;
  int16_t value = registers->current.user.gprs.gprs_s[Rd];
  bool success = ArmStore16SLE(memory, writeback, value);

  if (!success) {
    ArmLoadGPSR(registers, Rn, writeback);
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rn, writeback);
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_SIGNED_DATA_TRANSFER_