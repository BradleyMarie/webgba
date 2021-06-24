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

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_SIGNED_DATA_TRANSFER_