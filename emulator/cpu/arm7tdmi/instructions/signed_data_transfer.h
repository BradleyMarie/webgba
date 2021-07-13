#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_SIGNED_DATA_TRANSFER_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_SIGNED_DATA_TRANSFER_

#include <assert.h>

#include "emulator/cpu/arm7tdmi/memory.h"
#include "emulator/cpu/arm7tdmi/registers.h"

static inline void ArmLDRH_DAW(ArmGeneralPurposeRegisters *registers,
                               const Memory *memory, ArmRegisterIndex Rd,
                               ArmRegisterIndex Rn, uint_fast8_t offset) {
  uint16_t temp;
  bool success = ArmLoad16LE(memory, registers->gprs[Rn], &temp);
  assert(success);
  registers->gprs[Rd] = temp;
  registers->gprs[Rn] -= offset;
}

static inline void ArmLDRH_DB(ArmGeneralPurposeRegisters *registers,
                              const Memory *memory, ArmRegisterIndex Rd,
                              ArmRegisterIndex Rn, uint32_t offset) {
  uint16_t temp;
  bool success = ArmLoad16LE(memory, registers->gprs[Rn] - offset, &temp);
  assert(success);
  registers->gprs[Rd] = temp;
}

static inline void ArmLDRH_DBW(ArmGeneralPurposeRegisters *registers,
                               const Memory *memory, ArmRegisterIndex Rd,
                               ArmRegisterIndex Rn, uint_fast8_t offset) {
  registers->gprs[Rn] -= offset;
  uint16_t temp;
  bool success = ArmLoad16LE(memory, registers->gprs[Rn], &temp);
  assert(success);
  registers->gprs[Rd] = temp;
}

static inline void ArmLDRH_IAW(ArmGeneralPurposeRegisters *registers,
                               const Memory *memory, ArmRegisterIndex Rd,
                               ArmRegisterIndex Rn, uint_fast8_t offset) {
  uint16_t temp;
  bool success = ArmLoad16LE(memory, registers->gprs[Rn], &temp);
  assert(success);
  registers->gprs[Rd] = temp;
  registers->gprs[Rn] += offset;
}

static inline void ArmLDRH_IB(ArmGeneralPurposeRegisters *registers,
                              const Memory *memory, ArmRegisterIndex Rd,
                              ArmRegisterIndex Rn, uint32_t offset) {
  uint16_t temp;
  bool success = ArmLoad16LE(memory, registers->gprs[Rn] + offset, &temp);
  assert(success);
  registers->gprs[Rd] = temp;
}

static inline void ArmLDRH_IBW(ArmGeneralPurposeRegisters *registers,
                               const Memory *memory, ArmRegisterIndex Rd,
                               ArmRegisterIndex Rn, uint_fast8_t offset) {
  registers->gprs[Rn] += offset;
  uint16_t temp;
  bool success = ArmLoad16LE(memory, registers->gprs[Rn], &temp);
  assert(success);
  registers->gprs[Rd] = temp;
}

static inline void ArmLDRSB_DAW(ArmGeneralPurposeRegisters *registers,
                                const Memory *memory, ArmRegisterIndex Rd,
                                ArmRegisterIndex Rn, uint_fast8_t offset) {
  int8_t temp;
  bool success = Load8S(memory, registers->gprs[Rn], &temp);
  assert(success);
  registers->gprs_s[Rd] = temp;
  registers->gprs[Rn] -= offset;
}

static inline void ArmLDRSB_DB(ArmGeneralPurposeRegisters *registers,
                               Memory *memory, ArmRegisterIndex Rd,
                               ArmRegisterIndex Rn, uint32_t offset) {
  int8_t temp;
  bool success = Load8S(memory, registers->gprs[Rn] - offset, &temp);
  assert(success);
  registers->gprs_s[Rd] = temp;
}

static inline void ArmLDRSB_DBW(ArmGeneralPurposeRegisters *registers,
                                const Memory *memory, ArmRegisterIndex Rd,
                                ArmRegisterIndex Rn, uint_fast8_t offset) {
  registers->gprs[Rn] -= offset;
  int8_t temp;
  bool success = Load8S(memory, registers->gprs[Rn], &temp);
  assert(success);
  registers->gprs_s[Rd] = temp;
}

static inline void ArmLDRSB_IAW(ArmGeneralPurposeRegisters *registers,
                                const Memory *memory, ArmRegisterIndex Rd,
                                ArmRegisterIndex Rn, uint_fast8_t offset) {
  int8_t temp;
  bool success = Load8S(memory, registers->gprs[Rn], &temp);
  assert(success);
  registers->gprs_s[Rd] = temp;
  registers->gprs[Rn] += offset;
}

static inline void ArmLDRSB_IB(ArmGeneralPurposeRegisters *registers,
                               Memory *memory, ArmRegisterIndex Rd,
                               ArmRegisterIndex Rn, uint32_t offset) {
  int8_t temp;
  bool success = Load8S(memory, registers->gprs[Rn] + offset, &temp);
  assert(success);
  registers->gprs_s[Rd] = temp;
}

static inline void ArmLDRSB_IBW(ArmGeneralPurposeRegisters *registers,
                                const Memory *memory, ArmRegisterIndex Rd,
                                ArmRegisterIndex Rn, uint_fast8_t offset) {
  registers->gprs[Rn] += offset;
  int8_t temp;
  bool success = Load8S(memory, registers->gprs[Rn], &temp);
  assert(success);
  registers->gprs_s[Rd] = temp;
}

static inline void ArmLDRSH_DAW(ArmGeneralPurposeRegisters *registers,
                                const Memory *memory, ArmRegisterIndex Rd,
                                ArmRegisterIndex Rn, uint_fast8_t offset) {
  int16_t temp;
  bool success = ArmLoad16SLE(memory, registers->gprs[Rn], &temp);
  assert(success);
  registers->gprs_s[Rd] = temp;
  registers->gprs[Rn] -= offset;
}

static inline void ArmLDRSH_DB(ArmGeneralPurposeRegisters *registers,
                               const Memory *memory, ArmRegisterIndex Rd,
                               ArmRegisterIndex Rn, uint32_t offset) {
  int16_t temp;
  bool success = ArmLoad16SLE(memory, registers->gprs[Rn] - offset, &temp);
  assert(success);
  registers->gprs_s[Rd] = temp;
}

static inline void ArmLDRSH_DBW(ArmGeneralPurposeRegisters *registers,
                                const Memory *memory, ArmRegisterIndex Rd,
                                ArmRegisterIndex Rn, uint_fast8_t offset) {
  registers->gprs[Rn] -= offset;
  int16_t temp;
  bool success = ArmLoad16SLE(memory, registers->gprs[Rn], &temp);
  assert(success);
  registers->gprs_s[Rd] = temp;
}

static inline void ArmLDRSH_IAW(ArmGeneralPurposeRegisters *registers,
                                const Memory *memory, ArmRegisterIndex Rd,
                                ArmRegisterIndex Rn, uint_fast8_t offset) {
  int16_t temp;
  bool success = ArmLoad16SLE(memory, registers->gprs[Rn], &temp);
  assert(success);
  registers->gprs_s[Rd] = temp;
  registers->gprs[Rn] += offset;
}

static inline void ArmLDRSH_IB(ArmGeneralPurposeRegisters *registers,
                               const Memory *memory, ArmRegisterIndex Rd,
                               ArmRegisterIndex Rn, uint32_t offset) {
  int16_t temp;
  bool success = ArmLoad16SLE(memory, registers->gprs[Rn] + offset, &temp);
  assert(success);
  registers->gprs_s[Rd] = temp;
}

static inline void ArmLDRSH_IBW(ArmGeneralPurposeRegisters *registers,
                                const Memory *memory, ArmRegisterIndex Rd,
                                ArmRegisterIndex Rn, uint_fast8_t offset) {
  registers->gprs[Rn] += offset;
  int16_t temp;
  bool success = ArmLoad16SLE(memory, registers->gprs[Rn], &temp);
  assert(success);
  registers->gprs_s[Rd] = temp;
}

static inline void ArmSTRH_DAW(ArmGeneralPurposeRegisters *registers,
                               Memory *memory, ArmRegisterIndex Rd,
                               ArmRegisterIndex Rn, uint_fast8_t offset) {
  uint16_t temp = registers->gprs[Rd];
  bool success = ArmStore16LE(memory, registers->gprs[Rn], temp);
  assert(success);
  registers->gprs[Rn] -= offset;
}

static inline void ArmSTRH_DB(const ArmGeneralPurposeRegisters *registers,
                              Memory *memory, ArmRegisterIndex Rd,
                              ArmRegisterIndex Rn, uint32_t offset) {
  uint16_t temp = registers->gprs[Rd];
  bool success = ArmStore16LE(memory, registers->gprs[Rn] - offset, temp);
  assert(success);
}

static inline void ArmSTRH_DBW(ArmGeneralPurposeRegisters *registers,
                               Memory *memory, ArmRegisterIndex Rd,
                               ArmRegisterIndex Rn, uint_fast8_t offset) {
  registers->gprs[Rn] -= offset;
  uint16_t temp = registers->gprs[Rd];
  bool success = ArmStore16LE(memory, registers->gprs[Rn], temp);
  assert(success);
}

static inline void ArmSTRH_IAW(ArmGeneralPurposeRegisters *registers,
                               Memory *memory, ArmRegisterIndex Rd,
                               ArmRegisterIndex Rn, uint_fast8_t offset) {
  uint16_t temp = registers->gprs[Rd];
  bool success = ArmStore16LE(memory, registers->gprs[Rn], temp);
  assert(success);
  registers->gprs[Rn] += offset;
}

static inline void ArmSTRH_IB(const ArmGeneralPurposeRegisters *registers,
                              Memory *memory, ArmRegisterIndex Rd,
                              ArmRegisterIndex Rn, uint32_t offset) {
  uint16_t temp = registers->gprs[Rd];
  bool success = ArmStore16LE(memory, registers->gprs[Rn] + offset, temp);
  assert(success);
}

static inline void ArmSTRH_IBW(ArmGeneralPurposeRegisters *registers,
                               Memory *memory, ArmRegisterIndex Rd,
                               ArmRegisterIndex Rn, uint_fast8_t offset) {
  registers->gprs[Rn] += offset;
  uint16_t temp = registers->gprs[Rd];
  bool success = ArmStore16LE(memory, registers->gprs[Rn], temp);
  assert(success);
}

static inline void ArmSTRSB_DAW(ArmGeneralPurposeRegisters *registers,
                                Memory *memory, ArmRegisterIndex Rd,
                                ArmRegisterIndex Rn, uint_fast8_t offset) {
  int8_t temp = registers->gprs_s[Rd];
  bool success = Store8S(memory, registers->gprs[Rn], temp);
  assert(success);
  registers->gprs[Rn] -= offset;
}

static inline void ArmSTRSB_DB(const ArmGeneralPurposeRegisters *registers,
                               Memory *memory, ArmRegisterIndex Rd,
                               ArmRegisterIndex Rn, uint32_t offset) {
  int8_t temp = registers->gprs_s[Rd];
  bool success = Store8S(memory, registers->gprs[Rn] - offset, temp);
  assert(success);
}

static inline void ArmSTRSB_DBW(ArmGeneralPurposeRegisters *registers,
                                Memory *memory, ArmRegisterIndex Rd,
                                ArmRegisterIndex Rn, uint_fast8_t offset) {
  registers->gprs[Rn] -= offset;
  int8_t temp = registers->gprs_s[Rd];
  bool success = Store8S(memory, registers->gprs[Rn], temp);
  assert(success);
}

static inline void ArmSTRSB_IAW(ArmGeneralPurposeRegisters *registers,
                                Memory *memory, ArmRegisterIndex Rd,
                                ArmRegisterIndex Rn, uint_fast8_t offset) {
  int8_t temp = registers->gprs_s[Rd];
  bool success = Store8S(memory, registers->gprs[Rn], temp);
  assert(success);
  registers->gprs[Rn] += offset;
}

static inline void ArmSTRSB_IB(const ArmGeneralPurposeRegisters *registers,
                               Memory *memory, ArmRegisterIndex Rd,
                               ArmRegisterIndex Rn, uint32_t offset) {
  int8_t temp = registers->gprs_s[Rd];
  bool success = Store8S(memory, registers->gprs[Rn] + offset, temp);
  assert(success);
}

static inline void ArmSTRSB_IBW(ArmGeneralPurposeRegisters *registers,
                                Memory *memory, ArmRegisterIndex Rd,
                                ArmRegisterIndex Rn, uint_fast8_t offset) {
  registers->gprs[Rn] += offset;
  int8_t temp = registers->gprs_s[Rd];
  bool success = Store8S(memory, registers->gprs[Rn], temp);
  assert(success);
}

static inline void ArmSTRSH_DAW(ArmGeneralPurposeRegisters *registers,
                                Memory *memory, ArmRegisterIndex Rd,
                                ArmRegisterIndex Rn, uint_fast8_t offset) {
  int16_t temp = registers->gprs_s[Rd];
  bool success = ArmStore16SLE(memory, registers->gprs[Rn], temp);
  assert(success);
  registers->gprs[Rn] -= offset;
}

static inline void ArmSTRSH_DB(const ArmGeneralPurposeRegisters *registers,
                               Memory *memory, ArmRegisterIndex Rd,
                               ArmRegisterIndex Rn, uint32_t offset) {
  int16_t temp = registers->gprs_s[Rd];
  bool success = ArmStore16SLE(memory, registers->gprs[Rn] - offset, temp);
  assert(success);
}

static inline void ArmSTRSH_DBW(ArmGeneralPurposeRegisters *registers,
                                Memory *memory, ArmRegisterIndex Rd,
                                ArmRegisterIndex Rn, uint_fast8_t offset) {
  registers->gprs[Rn] -= offset;
  int16_t temp = registers->gprs_s[Rd];
  bool success = ArmStore16SLE(memory, registers->gprs[Rn], temp);
  assert(success);
}

static inline void ArmSTRSH_IAW(ArmGeneralPurposeRegisters *registers,
                                Memory *memory, ArmRegisterIndex Rd,
                                ArmRegisterIndex Rn, uint_fast8_t offset) {
  int16_t temp = registers->gprs_s[Rd];
  bool success = ArmStore16SLE(memory, registers->gprs[Rn], temp);
  assert(success);
  registers->gprs[Rn] += offset;
}

static inline void ArmSTRSH_IB(const ArmGeneralPurposeRegisters *registers,
                               Memory *memory, ArmRegisterIndex Rd,
                               ArmRegisterIndex Rn, uint32_t offset) {
  int16_t temp = registers->gprs_s[Rd];
  bool success = ArmStore16SLE(memory, registers->gprs[Rn] + offset, temp);
  assert(success);
}

static inline void ArmSTRSH_IBW(ArmGeneralPurposeRegisters *registers,
                                Memory *memory, ArmRegisterIndex Rd,
                                ArmRegisterIndex Rn, uint_fast8_t offset) {
  registers->gprs[Rn] += offset;
  int16_t temp = registers->gprs_s[Rd];
  bool success = ArmStore16SLE(memory, registers->gprs[Rn], temp);
  assert(success);
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_SIGNED_DATA_TRANSFER_