#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_SIGNED_DATA_TRANSFER_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_SIGNED_DATA_TRANSFER_

#include <assert.h>

#include "emulator/cpu/arm7tdmi/memory.h"
#include "emulator/cpu/arm7tdmi/registers.h"

static inline void ArmLDRH_DAW(ArmAllRegisters *registers, const Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast8_t offset) {
  uint16_t temp;
  bool success =
      ArmLoad16LE(memory, registers->current.user.gprs.gprs[Rn], &temp);
  assert(success);
  registers->current.user.gprs.gprs[Rd] = temp;
  registers->current.user.gprs.gprs[Rn] -= offset;
}

static inline void ArmLDRH_DB(ArmAllRegisters *registers, const Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint32_t offset) {
  uint16_t temp;
  bool success = ArmLoad16LE(
      memory, registers->current.user.gprs.gprs[Rn] - offset, &temp);
  assert(success);
  registers->current.user.gprs.gprs[Rd] = temp;
}

static inline void ArmLDRH_DBW(ArmAllRegisters *registers, const Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast8_t offset) {
  registers->current.user.gprs.gprs[Rn] -= offset;
  uint16_t temp;
  bool success =
      ArmLoad16LE(memory, registers->current.user.gprs.gprs[Rn], &temp);
  assert(success);
  registers->current.user.gprs.gprs[Rd] = temp;
}

static inline void ArmLDRH_IAW(ArmAllRegisters *registers, const Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast8_t offset) {
  uint16_t temp;
  bool success =
      ArmLoad16LE(memory, registers->current.user.gprs.gprs[Rn], &temp);
  assert(success);
  registers->current.user.gprs.gprs[Rd] = temp;
  registers->current.user.gprs.gprs[Rn] += offset;
}

static inline void ArmLDRH_IB(ArmAllRegisters *registers, const Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint32_t offset) {
  uint16_t temp;
  bool success = ArmLoad16LE(
      memory, registers->current.user.gprs.gprs[Rn] + offset, &temp);
  assert(success);
  registers->current.user.gprs.gprs[Rd] = temp;
}

static inline void ArmLDRH_IBW(ArmAllRegisters *registers, const Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast8_t offset) {
  registers->current.user.gprs.gprs[Rn] += offset;
  uint16_t temp;
  bool success =
      ArmLoad16LE(memory, registers->current.user.gprs.gprs[Rn], &temp);
  assert(success);
  registers->current.user.gprs.gprs[Rd] = temp;
}

static inline void ArmLDRSB_DAW(ArmAllRegisters *registers,
                                const Memory *memory, ArmRegisterIndex Rd,
                                ArmRegisterIndex Rn, uint_fast8_t offset) {
  int8_t temp;
  bool success = Load8S(memory, registers->current.user.gprs.gprs[Rn], &temp);
  assert(success);
  registers->current.user.gprs.gprs_s[Rd] = temp;
  registers->current.user.gprs.gprs[Rn] -= offset;
}

static inline void ArmLDRSB_DB(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint32_t offset) {
  int8_t temp;
  bool success =
      Load8S(memory, registers->current.user.gprs.gprs[Rn] - offset, &temp);
  assert(success);
  registers->current.user.gprs.gprs_s[Rd] = temp;
}

static inline void ArmLDRSB_DBW(ArmAllRegisters *registers,
                                const Memory *memory, ArmRegisterIndex Rd,
                                ArmRegisterIndex Rn, uint_fast8_t offset) {
  registers->current.user.gprs.gprs[Rn] -= offset;
  int8_t temp;
  bool success = Load8S(memory, registers->current.user.gprs.gprs[Rn], &temp);
  assert(success);
  registers->current.user.gprs.gprs_s[Rd] = temp;
}

static inline void ArmLDRSB_IAW(ArmAllRegisters *registers,
                                const Memory *memory, ArmRegisterIndex Rd,
                                ArmRegisterIndex Rn, uint_fast8_t offset) {
  int8_t temp;
  bool success = Load8S(memory, registers->current.user.gprs.gprs[Rn], &temp);
  assert(success);
  registers->current.user.gprs.gprs_s[Rd] = temp;
  registers->current.user.gprs.gprs[Rn] += offset;
}

static inline void ArmLDRSB_IB(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint32_t offset) {
  int8_t temp;
  bool success =
      Load8S(memory, registers->current.user.gprs.gprs[Rn] + offset, &temp);
  assert(success);
  registers->current.user.gprs.gprs_s[Rd] = temp;
}

static inline void ArmLDRSB_IBW(ArmAllRegisters *registers,
                                const Memory *memory, ArmRegisterIndex Rd,
                                ArmRegisterIndex Rn, uint_fast8_t offset) {
  registers->current.user.gprs.gprs[Rn] += offset;
  int8_t temp;
  bool success = Load8S(memory, registers->current.user.gprs.gprs[Rn], &temp);
  assert(success);
  registers->current.user.gprs.gprs_s[Rd] = temp;
}

static inline void ArmLDRSH_DAW(ArmAllRegisters *registers,
                                const Memory *memory, ArmRegisterIndex Rd,
                                ArmRegisterIndex Rn, uint_fast8_t offset) {
  int16_t temp;
  bool success =
      ArmLoad16SLE(memory, registers->current.user.gprs.gprs[Rn], &temp);
  assert(success);
  registers->current.user.gprs.gprs_s[Rd] = temp;
  registers->current.user.gprs.gprs[Rn] -= offset;
}

static inline void ArmLDRSH_DB(ArmAllRegisters *registers, const Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint32_t offset) {
  int16_t temp;
  bool success = ArmLoad16SLE(
      memory, registers->current.user.gprs.gprs[Rn] - offset, &temp);
  assert(success);
  registers->current.user.gprs.gprs_s[Rd] = temp;
}

static inline void ArmLDRSH_DBW(ArmAllRegisters *registers,
                                const Memory *memory, ArmRegisterIndex Rd,
                                ArmRegisterIndex Rn, uint_fast8_t offset) {
  registers->current.user.gprs.gprs[Rn] -= offset;
  int16_t temp;
  bool success =
      ArmLoad16SLE(memory, registers->current.user.gprs.gprs[Rn], &temp);
  assert(success);
  registers->current.user.gprs.gprs_s[Rd] = temp;
}

static inline void ArmLDRSH_IAW(ArmAllRegisters *registers,
                                const Memory *memory, ArmRegisterIndex Rd,
                                ArmRegisterIndex Rn, uint_fast8_t offset) {
  int16_t temp;
  bool success =
      ArmLoad16SLE(memory, registers->current.user.gprs.gprs[Rn], &temp);
  assert(success);
  registers->current.user.gprs.gprs_s[Rd] = temp;
  registers->current.user.gprs.gprs[Rn] += offset;
}

static inline void ArmLDRSH_IB(ArmAllRegisters *registers, const Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint32_t offset) {
  int16_t temp;
  bool success = ArmLoad16SLE(
      memory, registers->current.user.gprs.gprs[Rn] + offset, &temp);
  assert(success);
  registers->current.user.gprs.gprs_s[Rd] = temp;
}

static inline void ArmLDRSH_IBW(ArmAllRegisters *registers,
                                const Memory *memory, ArmRegisterIndex Rd,
                                ArmRegisterIndex Rn, uint_fast8_t offset) {
  registers->current.user.gprs.gprs[Rn] += offset;
  int16_t temp;
  bool success =
      ArmLoad16SLE(memory, registers->current.user.gprs.gprs[Rn], &temp);
  assert(success);
  registers->current.user.gprs.gprs_s[Rd] = temp;
}

static inline void ArmSTRH_DAW(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast8_t offset) {
  uint16_t temp = registers->current.user.gprs.gprs[Rd];
  bool success =
      ArmStore16LE(memory, registers->current.user.gprs.gprs[Rn], temp);
  assert(success);
  registers->current.user.gprs.gprs[Rn] -= offset;
}

static inline void ArmSTRH_DB(const ArmAllRegisters *registers, Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint32_t offset) {
  uint16_t temp = registers->current.user.gprs.gprs[Rd];
  bool success = ArmStore16LE(
      memory, registers->current.user.gprs.gprs[Rn] - offset, temp);
  assert(success);
}

static inline void ArmSTRH_DBW(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast8_t offset) {
  registers->current.user.gprs.gprs[Rn] -= offset;
  uint16_t temp = registers->current.user.gprs.gprs[Rd];
  bool success =
      ArmStore16LE(memory, registers->current.user.gprs.gprs[Rn], temp);
  assert(success);
}

static inline void ArmSTRH_IAW(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast8_t offset) {
  uint16_t temp = registers->current.user.gprs.gprs[Rd];
  bool success =
      ArmStore16LE(memory, registers->current.user.gprs.gprs[Rn], temp);
  assert(success);
  registers->current.user.gprs.gprs[Rn] += offset;
}

static inline void ArmSTRH_IB(const ArmAllRegisters *registers, Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint32_t offset) {
  uint16_t temp = registers->current.user.gprs.gprs[Rd];
  bool success = ArmStore16LE(
      memory, registers->current.user.gprs.gprs[Rn] + offset, temp);
  assert(success);
}

static inline void ArmSTRH_IBW(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast8_t offset) {
  registers->current.user.gprs.gprs[Rn] += offset;
  uint16_t temp = registers->current.user.gprs.gprs[Rd];
  bool success =
      ArmStore16LE(memory, registers->current.user.gprs.gprs[Rn], temp);
  assert(success);
}

static inline void ArmSTRSB_DAW(ArmAllRegisters *registers, Memory *memory,
                                ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                                uint_fast8_t offset) {
  int8_t temp = registers->current.user.gprs.gprs_s[Rd];
  bool success = Store8S(memory, registers->current.user.gprs.gprs[Rn], temp);
  assert(success);
  registers->current.user.gprs.gprs[Rn] -= offset;
}

static inline void ArmSTRSB_DB(const ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint32_t offset) {
  int8_t temp = registers->current.user.gprs.gprs_s[Rd];
  bool success =
      Store8S(memory, registers->current.user.gprs.gprs[Rn] - offset, temp);
  assert(success);
}

static inline void ArmSTRSB_DBW(ArmAllRegisters *registers, Memory *memory,
                                ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                                uint_fast8_t offset) {
  registers->current.user.gprs.gprs[Rn] -= offset;
  int8_t temp = registers->current.user.gprs.gprs_s[Rd];
  bool success = Store8S(memory, registers->current.user.gprs.gprs[Rn], temp);
  assert(success);
}

static inline void ArmSTRSB_IAW(ArmAllRegisters *registers, Memory *memory,
                                ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                                uint_fast8_t offset) {
  int8_t temp = registers->current.user.gprs.gprs_s[Rd];
  bool success = Store8S(memory, registers->current.user.gprs.gprs[Rn], temp);
  assert(success);
  registers->current.user.gprs.gprs[Rn] += offset;
}

static inline void ArmSTRSB_IB(const ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint32_t offset) {
  int8_t temp = registers->current.user.gprs.gprs_s[Rd];
  bool success =
      Store8S(memory, registers->current.user.gprs.gprs[Rn] + offset, temp);
  assert(success);
}

static inline void ArmSTRSB_IBW(ArmAllRegisters *registers, Memory *memory,
                                ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                                uint_fast8_t offset) {
  registers->current.user.gprs.gprs[Rn] += offset;
  int8_t temp = registers->current.user.gprs.gprs_s[Rd];
  bool success = Store8S(memory, registers->current.user.gprs.gprs[Rn], temp);
  assert(success);
}

static inline void ArmSTRSH_DAW(ArmAllRegisters *registers, Memory *memory,
                                ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                                uint_fast8_t offset) {
  int16_t temp = registers->current.user.gprs.gprs_s[Rd];
  bool success =
      ArmStore16SLE(memory, registers->current.user.gprs.gprs[Rn], temp);
  assert(success);
  registers->current.user.gprs.gprs[Rn] -= offset;
}

static inline void ArmSTRSH_DB(const ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint32_t offset) {
  int16_t temp = registers->current.user.gprs.gprs_s[Rd];
  bool success = ArmStore16SLE(
      memory, registers->current.user.gprs.gprs[Rn] - offset, temp);
  assert(success);
}

static inline void ArmSTRSH_DBW(ArmAllRegisters *registers, Memory *memory,
                                ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                                uint_fast8_t offset) {
  registers->current.user.gprs.gprs[Rn] -= offset;
  int16_t temp = registers->current.user.gprs.gprs_s[Rd];
  bool success =
      ArmStore16SLE(memory, registers->current.user.gprs.gprs[Rn], temp);
  assert(success);
}

static inline void ArmSTRSH_IAW(ArmAllRegisters *registers, Memory *memory,
                                ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                                uint_fast8_t offset) {
  int16_t temp = registers->current.user.gprs.gprs_s[Rd];
  bool success =
      ArmStore16SLE(memory, registers->current.user.gprs.gprs[Rn], temp);
  assert(success);
  registers->current.user.gprs.gprs[Rn] += offset;
}

static inline void ArmSTRSH_IB(const ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint32_t offset) {
  int16_t temp = registers->current.user.gprs.gprs_s[Rd];
  bool success = ArmStore16SLE(
      memory, registers->current.user.gprs.gprs[Rn] + offset, temp);
  assert(success);
}

static inline void ArmSTRSH_IBW(ArmAllRegisters *registers, Memory *memory,
                                ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                                uint_fast8_t offset) {
  registers->current.user.gprs.gprs[Rn] += offset;
  int16_t temp = registers->current.user.gprs.gprs_s[Rd];
  bool success =
      ArmStore16SLE(memory, registers->current.user.gprs.gprs[Rn], temp);
  assert(success);
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_SIGNED_DATA_TRANSFER_