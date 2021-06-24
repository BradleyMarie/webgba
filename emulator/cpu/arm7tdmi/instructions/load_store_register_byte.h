#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_LOAD_STORE_REGISTER_BYTE_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_LOAD_STORE_REGISTER_BYTE_

#include "emulator/cpu/arm7tdmi/arm7tdmi.h"
#include "emulator/memory.h"

static inline void ArmLDR(ArmGeneralPurposeRegisters *registers, Memory *memory,
                          ArmRegisterIndex Rd, uint32_t address) {
  bool success = Load32LE(memory, address, &registers->gprs[Rd]);
  assert(success);
}

static inline void ArmLDR_DecrementPreIndexed(
    ArmGeneralPurposeRegisters *registers, Memory *memory, ArmRegisterIndex Rd,
    ArmRegisterIndex Rn, uint_fast16_t offset) {
  registers->gprs[Rn] -= offset;
  bool success = Load32LE(memory, registers->gprs[Rn], &registers->gprs[Rd]);
  assert(success);
}

static inline void ArmLDR_DecrementPostIndexed(
    ArmGeneralPurposeRegisters *registers, Memory *memory, ArmRegisterIndex Rd,
    ArmRegisterIndex Rn, uint_fast16_t offset) {
  bool success = Load32LE(memory, registers->gprs[Rn], &registers->gprs[Rd]);
  assert(success);
  registers->gprs[Rn] -= offset;
}

static inline void ArmLDR_IncrementPreIndexed(
    ArmGeneralPurposeRegisters *registers, Memory *memory, ArmRegisterIndex Rd,
    ArmRegisterIndex Rn, uint_fast16_t offset) {
  registers->gprs[Rn] += offset;
  bool success = Load32LE(memory, registers->gprs[Rn], &registers->gprs[Rd]);
  assert(success);
}

static inline void ArmLDR_IncrementPostIndexed(
    ArmGeneralPurposeRegisters *registers, Memory *memory, ArmRegisterIndex Rd,
    ArmRegisterIndex Rn, uint_fast16_t offset) {
  bool success = Load32LE(memory, registers->gprs[Rn], &registers->gprs[Rd]);
  assert(success);
  registers->gprs[Rn] += offset;
}

static inline void ArmLDRB(ArmGeneralPurposeRegisters *registers,
                           Memory *memory, ArmRegisterIndex Rd,
                           uint32_t address) {
  uint8_t temp;
  bool success = Load8(memory, address, &temp);
  assert(success);
  registers->gprs[Rd] = temp;
}

static inline void ArmLDRB_DecrementPreIndexed(
    ArmGeneralPurposeRegisters *registers, Memory *memory, ArmRegisterIndex Rd,
    ArmRegisterIndex Rn, uint_fast16_t offset) {
  registers->gprs[Rn] -= offset;
  uint8_t temp;
  bool success = Load8(memory, registers->gprs[Rn], &temp);
  assert(success);
  registers->gprs[Rd] = temp;
}

static inline void ArmLDRB_DecrementPostIndexed(
    ArmGeneralPurposeRegisters *registers, Memory *memory, ArmRegisterIndex Rd,
    ArmRegisterIndex Rn, uint_fast16_t offset) {
  uint8_t temp;
  bool success = Load8(memory, registers->gprs[Rn], &temp);
  assert(success);
  registers->gprs[Rd] = temp;
  registers->gprs[Rn] -= offset;
}

static inline void ArmLDRB_IncrementPreIndexed(
    ArmGeneralPurposeRegisters *registers, Memory *memory, ArmRegisterIndex Rd,
    ArmRegisterIndex Rn, uint_fast16_t offset) {
  registers->gprs[Rn] += offset;
  uint8_t temp;
  bool success = Load8(memory, registers->gprs[Rn], &temp);
  assert(success);
  registers->gprs[Rd] = temp;
}

static inline void ArmLDRB_IncrementPostIndexed(
    ArmGeneralPurposeRegisters *registers, Memory *memory, ArmRegisterIndex Rd,
    ArmRegisterIndex Rn, uint_fast16_t offset) {
  uint8_t temp;
  bool success = Load8(memory, registers->gprs[Rn], &temp);
  assert(success);
  registers->gprs[Rd] = temp;
  registers->gprs[Rn] += offset;
}

static inline void ArmSTR(ArmGeneralPurposeRegisters *registers, Memory *memory,
                          ArmRegisterIndex Rd, uint32_t address) {
  bool success = Store32LE(memory, address, registers->gprs[Rd]);
  assert(success);
}

static inline void ArmSTR_DecrementPreIndexed(
    ArmGeneralPurposeRegisters *registers, Memory *memory, ArmRegisterIndex Rd,
    ArmRegisterIndex Rn, uint_fast16_t offset) {
  registers->gprs[Rn] -= offset;
  bool success = Store32LE(memory, registers->gprs[Rn], registers->gprs[Rd]);
  assert(success);
}

static inline void ArmSTR_DecrementPostIndexed(
    ArmGeneralPurposeRegisters *registers, Memory *memory, ArmRegisterIndex Rd,
    ArmRegisterIndex Rn, uint_fast16_t offset) {
  bool success = Store32LE(memory, registers->gprs[Rn], registers->gprs[Rd]);
  assert(success);
  registers->gprs[Rn] -= offset;
}

static inline void ArmSTR_IncrementPreIndexed(
    ArmGeneralPurposeRegisters *registers, Memory *memory, ArmRegisterIndex Rd,
    ArmRegisterIndex Rn, uint_fast16_t offset) {
  registers->gprs[Rn] += offset;
  bool success = Store32LE(memory, registers->gprs[Rn], registers->gprs[Rd]);
  assert(success);
}

static inline void ArmSTR_IncrementPostIndexed(
    ArmGeneralPurposeRegisters *registers, Memory *memory, ArmRegisterIndex Rd,
    ArmRegisterIndex Rn, uint_fast16_t offset) {
  bool success = Store32LE(memory, registers->gprs[Rn], registers->gprs[Rd]);
  assert(success);
  registers->gprs[Rn] += offset;
}

static inline void ArmSTRB(ArmGeneralPurposeRegisters *registers,
                           Memory *memory, ArmRegisterIndex Rd,
                           uint32_t address) {
  uint8_t temp = registers->gprs[Rd];
  bool success = Store8(memory, address, temp);
  assert(success);
}

static inline void ArmSTRB_DecrementPreIndexed(
    ArmGeneralPurposeRegisters *registers, Memory *memory, ArmRegisterIndex Rd,
    ArmRegisterIndex Rn, uint_fast16_t offset) {
  registers->gprs[Rn] -= offset;
  uint8_t temp = registers->gprs[Rd];
  bool success = Store8(memory, registers->gprs[Rn], temp);
  assert(success);
}

static inline void ArmSTRB_DecrementPostIndexed(
    ArmGeneralPurposeRegisters *registers, Memory *memory, ArmRegisterIndex Rd,
    ArmRegisterIndex Rn, uint_fast16_t offset) {
  uint8_t temp = registers->gprs[Rd];
  bool success = Store8(memory, registers->gprs[Rn], temp);
  assert(success);
  registers->gprs[Rn] -= offset;
}

static inline void ArmSTRB_IncrementPreIndexed(
    ArmGeneralPurposeRegisters *registers, Memory *memory, ArmRegisterIndex Rd,
    ArmRegisterIndex Rn, uint_fast16_t offset) {
  registers->gprs[Rn] += offset;
  uint8_t temp = registers->gprs[Rd];
  bool success = Store8(memory, registers->gprs[Rn], temp);
  assert(success);
}

static inline void ArmSTRB_IncrementPostIndexed(
    ArmGeneralPurposeRegisters *registers, Memory *memory, ArmRegisterIndex Rd,
    ArmRegisterIndex Rn, uint_fast16_t offset) {
  uint8_t temp = registers->gprs[Rd];
  bool success = Store8(memory, registers->gprs[Rn], temp);
  assert(success);
  registers->gprs[Rn] += offset;
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_LOAD_STORE_REGISTER_BYTE_