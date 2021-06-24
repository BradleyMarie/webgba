#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_SIGNED_DATA_TRANSFER_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_SIGNED_DATA_TRANSFER_

#include "emulator/cpu/arm7tdmi/arm7tdmi.h"
#include "emulator/memory.h"

static inline void ArmLDRH(ArmGeneralPurposeRegisters *registers,
                           Memory *memory, ArmRegisterIndex Rd,
                           uint32_t address) {
  uint16_t temp;
  bool success = Load16LE(memory, address, &temp);
  assert(success);
  registers->gprs[Rd] = temp;
}

static inline void ArmLDRH_DecrementPreIndexed(
    ArmGeneralPurposeRegisters *registers, Memory *memory, ArmRegisterIndex Rd,
    ArmRegisterIndex Rn, uint_fast16_t offset) {
  registers->gprs[Rn] -= offset;
  uint16_t temp;
  bool success = Load16LE(memory, registers->gprs[Rn], &temp);
  assert(success);
  registers->gprs[Rd] = temp;
}

static inline void ArmLDRH_DecrementPostIndexed(
    ArmGeneralPurposeRegisters *registers, Memory *memory, ArmRegisterIndex Rd,
    ArmRegisterIndex Rn, uint_fast16_t offset) {
  uint16_t temp;
  bool success = Load16LE(memory, registers->gprs[Rn], &temp);
  assert(success);
  registers->gprs[Rd] = temp;
  registers->gprs[Rn] -= offset;
}

static inline void ArmLDRH_IncrementPreIndexed(
    ArmGeneralPurposeRegisters *registers, Memory *memory, ArmRegisterIndex Rd,
    ArmRegisterIndex Rn, uint_fast16_t offset) {
  registers->gprs[Rn] += offset;
  uint16_t temp;
  bool success = Load16LE(memory, registers->gprs[Rn], &temp);
  assert(success);
  registers->gprs[Rd] = temp;
}

static inline void ArmLDRH_IncrementPostIndexed(
    ArmGeneralPurposeRegisters *registers, Memory *memory, ArmRegisterIndex Rd,
    ArmRegisterIndex Rn, uint_fast16_t offset) {
  uint16_t temp;
  bool success = Load16LE(memory, registers->gprs[Rn], &temp);
  assert(success);
  registers->gprs[Rd] = temp;
  registers->gprs[Rn] += offset;
}

static inline void ArmLDRSB(ArmGeneralPurposeRegisters *registers,
                            Memory *memory, ArmRegisterIndex Rd,
                            uint32_t address) {
  int8_t temp;
  bool success = Load8S(memory, address, &temp);
  assert(success);
  registers->gprs_s[Rd] = temp;
}

static inline void ArmLDRSB_DecrementPreIndexed(
    ArmGeneralPurposeRegisters *registers, Memory *memory, ArmRegisterIndex Rd,
    ArmRegisterIndex Rn, uint_fast16_t offset) {
  registers->gprs[Rn] -= offset;
  int8_t temp;
  bool success = Load8S(memory, registers->gprs[Rn], &temp);
  assert(success);
  registers->gprs_s[Rd] = temp;
}

static inline void ArmLDRSB_DecrementPostIndexed(
    ArmGeneralPurposeRegisters *registers, Memory *memory, ArmRegisterIndex Rd,
    ArmRegisterIndex Rn, uint_fast16_t offset) {
  int8_t temp;
  bool success = Load8S(memory, registers->gprs[Rn], &temp);
  assert(success);
  registers->gprs_s[Rd] = temp;
  registers->gprs[Rn] -= offset;
}

static inline void ArmLDRSB_IncrementPreIndexed(
    ArmGeneralPurposeRegisters *registers, Memory *memory, ArmRegisterIndex Rd,
    ArmRegisterIndex Rn, uint_fast16_t offset) {
  registers->gprs[Rn] += offset;
  int8_t temp;
  bool success = Load8S(memory, registers->gprs[Rn], &temp);
  assert(success);
  registers->gprs_s[Rd] = temp;
}

static inline void ArmLDRSB_IncrementPostIndexed(
    ArmGeneralPurposeRegisters *registers, Memory *memory, ArmRegisterIndex Rd,
    ArmRegisterIndex Rn, uint_fast16_t offset) {
  int8_t temp;
  bool success = Load8S(memory, registers->gprs[Rn], &temp);
  assert(success);
  registers->gprs_s[Rd] = temp;
  registers->gprs[Rn] += offset;
}

static inline void ArmLDRSH(ArmGeneralPurposeRegisters *registers,
                            Memory *memory, ArmRegisterIndex Rd,
                            uint32_t address) {
  int16_t temp;
  bool success = Load16SLE(memory, address, &temp);
  assert(success);
  registers->gprs_s[Rd] = temp;
}

static inline void ArmLDRSH_DecrementPreIndexed(
    ArmGeneralPurposeRegisters *registers, Memory *memory, ArmRegisterIndex Rd,
    ArmRegisterIndex Rn, uint_fast16_t offset) {
  registers->gprs[Rn] -= offset;
  int16_t temp;
  bool success = Load16SLE(memory, registers->gprs[Rn], &temp);
  assert(success);
  registers->gprs_s[Rd] = temp;
}

static inline void ArmLDRSH_DecrementPostIndexed(
    ArmGeneralPurposeRegisters *registers, Memory *memory, ArmRegisterIndex Rd,
    ArmRegisterIndex Rn, uint_fast16_t offset) {
  int16_t temp;
  bool success = Load16SLE(memory, registers->gprs[Rn], &temp);
  assert(success);
  registers->gprs_s[Rd] = temp;
  registers->gprs[Rn] -= offset;
}

static inline void ArmLDRSH_IncrementPreIndexed(
    ArmGeneralPurposeRegisters *registers, Memory *memory, ArmRegisterIndex Rd,
    ArmRegisterIndex Rn, uint_fast16_t offset) {
  registers->gprs[Rn] += offset;
  int16_t temp;
  bool success = Load16SLE(memory, registers->gprs[Rn], &temp);
  assert(success);
  registers->gprs_s[Rd] = temp;
}

static inline void ArmLDRSH_IncrementPostIndexed(
    ArmGeneralPurposeRegisters *registers, Memory *memory, ArmRegisterIndex Rd,
    ArmRegisterIndex Rn, uint_fast16_t offset) {
  int16_t temp;
  bool success = Load16SLE(memory, registers->gprs[Rn], &temp);
  assert(success);
  registers->gprs_s[Rd] = temp;
  registers->gprs[Rn] += offset;
}

static inline void ArmSTRH(ArmGeneralPurposeRegisters *registers,
                           Memory *memory, ArmRegisterIndex Rd,
                           uint32_t address) {
  uint16_t temp = registers->gprs[Rd];
  bool success = Store16LE(memory, address, temp);
  assert(success);
}

static inline void ArmSTRH_DecrementPreIndexed(
    ArmGeneralPurposeRegisters *registers, Memory *memory, ArmRegisterIndex Rd,
    ArmRegisterIndex Rn, uint_fast16_t offset) {
  registers->gprs[Rn] -= offset;
  uint16_t temp = registers->gprs[Rd];
  bool success = Store16LE(memory, registers->gprs[Rn], temp);
  assert(success);
}

static inline void ArmSTRH_DecrementPostIndexed(
    ArmGeneralPurposeRegisters *registers, Memory *memory, ArmRegisterIndex Rd,
    ArmRegisterIndex Rn, uint_fast16_t offset) {
  uint16_t temp = registers->gprs[Rd];
  bool success = Store16LE(memory, registers->gprs[Rn], temp);
  assert(success);
  registers->gprs[Rn] -= offset;
}

static inline void ArmSTRH_IncrementPreIndexed(
    ArmGeneralPurposeRegisters *registers, Memory *memory, ArmRegisterIndex Rd,
    ArmRegisterIndex Rn, uint_fast16_t offset) {
  registers->gprs[Rn] += offset;
  uint16_t temp = registers->gprs[Rd];
  bool success = Store16LE(memory, registers->gprs[Rn], temp);
  assert(success);
}

static inline void ArmSTRH_IncrementPostIndexed(
    ArmGeneralPurposeRegisters *registers, Memory *memory, ArmRegisterIndex Rd,
    ArmRegisterIndex Rn, uint_fast16_t offset) {
  uint16_t temp = registers->gprs[Rd];
  bool success = Store16LE(memory, registers->gprs[Rn], temp);
  assert(success);
  registers->gprs[Rn] += offset;
}

static inline void ArmSTRSB(ArmGeneralPurposeRegisters *registers,
                            Memory *memory, ArmRegisterIndex Rd,
                            uint32_t address) {
  int8_t temp = registers->gprs_s[Rd];
  bool success = Store8S(memory, address, temp);
  assert(success);
}

static inline void ArmSTRSB_DecrementPreIndexed(
    ArmGeneralPurposeRegisters *registers, Memory *memory, ArmRegisterIndex Rd,
    ArmRegisterIndex Rn, uint_fast16_t offset) {
  registers->gprs[Rn] -= offset;
  int8_t temp = registers->gprs_s[Rd];
  bool success = Store8S(memory, registers->gprs[Rn], temp);
  assert(success);
}

static inline void ArmSTRSB_DecrementPostIndexed(
    ArmGeneralPurposeRegisters *registers, Memory *memory, ArmRegisterIndex Rd,
    ArmRegisterIndex Rn, uint_fast16_t offset) {
  int8_t temp = registers->gprs_s[Rd];
  bool success = Store8S(memory, registers->gprs[Rn], temp);
  assert(success);
  registers->gprs[Rn] -= offset;
}

static inline void ArmSTRSB_IncrementPreIndexed(
    ArmGeneralPurposeRegisters *registers, Memory *memory, ArmRegisterIndex Rd,
    ArmRegisterIndex Rn, uint_fast16_t offset) {
  registers->gprs[Rn] += offset;
  int8_t temp = registers->gprs_s[Rd];
  bool success = Store8S(memory, registers->gprs[Rn], temp);
  assert(success);
}

static inline void ArmSTRSB_IncrementPostIndexed(
    ArmGeneralPurposeRegisters *registers, Memory *memory, ArmRegisterIndex Rd,
    ArmRegisterIndex Rn, uint_fast16_t offset) {
  int8_t temp = registers->gprs_s[Rd];
  bool success = Store8S(memory, registers->gprs[Rn], temp);
  assert(success);
  registers->gprs[Rn] += offset;
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_SIGNED_DATA_TRANSFER_