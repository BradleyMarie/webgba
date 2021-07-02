#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_LOAD_STORE_REGISTER_BYTE_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_LOAD_STORE_REGISTER_BYTE_

#include "emulator/cpu/arm7tdmi/arm7tdmi.h"
#include "emulator/memory.h"

static inline void ArmLDR(ArmGeneralPurposeRegisters *registers,
                          const Memory *memory, ArmRegisterIndex Rd,
                          uint32_t address) {
  bool success = Load32LE(memory, address, &registers->gprs[Rd]);
  assert(success);
}

static inline void ArmLDR_DBW(ArmGeneralPurposeRegisters *registers,
                              const Memory *memory, ArmRegisterIndex Rd,
                              ArmRegisterIndex Rn, uint_fast16_t offset) {
  registers->gprs[Rn] -= offset;
  bool success = Load32LE(memory, registers->gprs[Rn], &registers->gprs[Rd]);
  assert(success);
}

static inline void ArmLDR_DAW(ArmGeneralPurposeRegisters *registers,
                              const Memory *memory, ArmRegisterIndex Rd,
                              ArmRegisterIndex Rn, uint_fast16_t offset) {
  bool success = Load32LE(memory, registers->gprs[Rn], &registers->gprs[Rd]);
  assert(success);
  registers->gprs[Rn] -= offset;
}

static inline void ArmLDR_IBW(ArmGeneralPurposeRegisters *registers,
                              const Memory *memory, ArmRegisterIndex Rd,
                              ArmRegisterIndex Rn, uint_fast16_t offset) {
  registers->gprs[Rn] += offset;
  bool success = Load32LE(memory, registers->gprs[Rn], &registers->gprs[Rd]);
  assert(success);
}

static inline void ArmLDR_IAW(ArmGeneralPurposeRegisters *registers,
                              const Memory *memory, ArmRegisterIndex Rd,
                              ArmRegisterIndex Rn, uint_fast16_t offset) {
  bool success = Load32LE(memory, registers->gprs[Rn], &registers->gprs[Rd]);
  assert(success);
  registers->gprs[Rn] += offset;
}

static inline void ArmLDRT(ArmAllRegisters *registers, const Memory *memory,
                           ArmRegisterIndex Rd, uint32_t address) {
  ArmProgramStatusRegister current_status = registers->current.user.cpsr;
  ArmProgramStatusRegister temporary_status = current_status;
  temporary_status.mode = MODE_USR;

  ArmLoadCPSR(registers, temporary_status);
  uint32_t value;
  bool success = Load32LE(memory, address, &value);
  assert(success);
  ArmLoadCPSR(registers, current_status);

  registers->current.user.gprs.gprs[Rd] = value;
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
  bool success = Load32LE(memory, address, &value);
  assert(success);
  ArmLoadCPSR(registers, current_status);

  registers->current.user.gprs.gprs[Rd] = value;
  registers->current.user.gprs.gprs[Rn] -= offset;
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
  bool success = Load32LE(memory, address, &value);
  assert(success);
  ArmLoadCPSR(registers, current_status);

  registers->current.user.gprs.gprs[Rd] = value;
  registers->current.user.gprs.gprs[Rn] += offset;
}

static inline void ArmLDRB(ArmGeneralPurposeRegisters *registers,
                           const Memory *memory, ArmRegisterIndex Rd,
                           uint32_t address) {
  uint8_t temp;
  bool success = Load8(memory, address, &temp);
  assert(success);
  registers->gprs[Rd] = temp;
}

static inline void ArmLDRB_DBW(ArmGeneralPurposeRegisters *registers,
                               const Memory *memory, ArmRegisterIndex Rd,
                               ArmRegisterIndex Rn, uint_fast16_t offset) {
  registers->gprs[Rn] -= offset;
  uint8_t temp;
  bool success = Load8(memory, registers->gprs[Rn], &temp);
  assert(success);
  registers->gprs[Rd] = temp;
}

static inline void ArmLDRB_DAW(ArmGeneralPurposeRegisters *registers,
                               const Memory *memory, ArmRegisterIndex Rd,
                               ArmRegisterIndex Rn, uint_fast16_t offset) {
  uint8_t temp;
  bool success = Load8(memory, registers->gprs[Rn], &temp);
  assert(success);
  registers->gprs[Rd] = temp;
  registers->gprs[Rn] -= offset;
}

static inline void ArmLDRB_IBW(ArmGeneralPurposeRegisters *registers,
                               const Memory *memory, ArmRegisterIndex Rd,
                               ArmRegisterIndex Rn, uint_fast16_t offset) {
  registers->gprs[Rn] += offset;
  uint8_t temp;
  bool success = Load8(memory, registers->gprs[Rn], &temp);
  assert(success);
  registers->gprs[Rd] = temp;
}

static inline void ArmLDRB_IAW(ArmGeneralPurposeRegisters *registers,
                               const Memory *memory, ArmRegisterIndex Rd,
                               ArmRegisterIndex Rn, uint_fast16_t offset) {
  uint8_t temp;
  bool success = Load8(memory, registers->gprs[Rn], &temp);
  assert(success);
  registers->gprs[Rd] = temp;
  registers->gprs[Rn] += offset;
}

static inline void ArmLDRBT(ArmAllRegisters *registers, const Memory *memory,
                            ArmRegisterIndex Rd, uint32_t address) {
  ArmProgramStatusRegister current_status = registers->current.user.cpsr;
  ArmProgramStatusRegister temporary_status = current_status;
  temporary_status.mode = MODE_USR;

  ArmLoadCPSR(registers, temporary_status);
  uint8_t value;
  bool success = Load8(memory, address, &value);
  assert(success);
  ArmLoadCPSR(registers, current_status);

  registers->current.user.gprs.gprs[Rd] = value;
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
  assert(success);
  ArmLoadCPSR(registers, current_status);

  registers->current.user.gprs.gprs[Rd] = value;
  registers->current.user.gprs.gprs[Rn] -= offset;
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
  assert(success);
  ArmLoadCPSR(registers, current_status);

  registers->current.user.gprs.gprs[Rd] = value;
  registers->current.user.gprs.gprs[Rn] += offset;
}

static inline void ArmSTR(const ArmGeneralPurposeRegisters *registers,
                          Memory *memory, ArmRegisterIndex Rd,
                          uint32_t address) {
  bool success = Store32LE(memory, address, registers->gprs[Rd]);
  assert(success);
}

static inline void ArmSTR_DBW(ArmGeneralPurposeRegisters *registers,
                              Memory *memory, ArmRegisterIndex Rd,
                              ArmRegisterIndex Rn, uint_fast16_t offset) {
  registers->gprs[Rn] -= offset;
  bool success = Store32LE(memory, registers->gprs[Rn], registers->gprs[Rd]);
  assert(success);
}

static inline void ArmSTR_DAW(ArmGeneralPurposeRegisters *registers,
                              Memory *memory, ArmRegisterIndex Rd,
                              ArmRegisterIndex Rn, uint_fast16_t offset) {
  bool success = Store32LE(memory, registers->gprs[Rn], registers->gprs[Rd]);
  assert(success);
  registers->gprs[Rn] -= offset;
}

static inline void ArmSTR_IBW(ArmGeneralPurposeRegisters *registers,
                              Memory *memory, ArmRegisterIndex Rd,
                              ArmRegisterIndex Rn, uint_fast16_t offset) {
  registers->gprs[Rn] += offset;
  bool success = Store32LE(memory, registers->gprs[Rn], registers->gprs[Rd]);
  assert(success);
}

static inline void ArmSTR_IAW(ArmGeneralPurposeRegisters *registers,
                              Memory *memory, ArmRegisterIndex Rd,
                              ArmRegisterIndex Rn, uint_fast16_t offset) {
  bool success = Store32LE(memory, registers->gprs[Rn], registers->gprs[Rd]);
  assert(success);
  registers->gprs[Rn] += offset;
}

static inline void ArmSTRT(ArmAllRegisters *registers, Memory *memory,
                           ArmRegisterIndex Rd, uint32_t address) {
  ArmProgramStatusRegister current_status = registers->current.user.cpsr;
  ArmProgramStatusRegister temporary_status = current_status;
  temporary_status.mode = MODE_USR;
  uint32_t value = registers->current.user.gprs.gprs[Rd];

  ArmLoadCPSR(registers, temporary_status);
  bool success = Store32LE(memory, address, value);
  assert(success);
  ArmLoadCPSR(registers, current_status);
}

static inline void ArmSTRT_DAW(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast16_t offset) {
  ArmProgramStatusRegister current_status = registers->current.user.cpsr;
  ArmProgramStatusRegister temporary_status = current_status;
  temporary_status.mode = MODE_USR;
  uint32_t address = registers->current.user.gprs.gprs[Rn];
  uint32_t value = registers->current.user.gprs.gprs[Rd];

  ArmLoadCPSR(registers, temporary_status);
  bool success = Store32LE(memory, address, value);
  assert(success);
  ArmLoadCPSR(registers, current_status);

  registers->current.user.gprs.gprs[Rn] -= offset;
}

static inline void ArmSTRT_IAW(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast16_t offset) {
  ArmProgramStatusRegister current_status = registers->current.user.cpsr;
  ArmProgramStatusRegister temporary_status = current_status;
  uint32_t address = registers->current.user.gprs.gprs[Rn];
  uint32_t value = registers->current.user.gprs.gprs[Rd];

  ArmLoadCPSR(registers, temporary_status);
  bool success = Store32LE(memory, address, value);
  assert(success);
  ArmLoadCPSR(registers, current_status);

  registers->current.user.gprs.gprs[Rn] += offset;
}

static inline void ArmSTRB(const ArmGeneralPurposeRegisters *registers,
                           Memory *memory, ArmRegisterIndex Rd,
                           uint32_t address) {
  uint8_t temp = registers->gprs[Rd];
  bool success = Store8(memory, address, temp);
  assert(success);
}

static inline void ArmSTRB_DBW(ArmGeneralPurposeRegisters *registers,
                               Memory *memory, ArmRegisterIndex Rd,
                               ArmRegisterIndex Rn, uint_fast16_t offset) {
  registers->gprs[Rn] -= offset;
  uint8_t temp = registers->gprs[Rd];
  bool success = Store8(memory, registers->gprs[Rn], temp);
  assert(success);
}

static inline void ArmSTRB_DAW(ArmGeneralPurposeRegisters *registers,
                               Memory *memory, ArmRegisterIndex Rd,
                               ArmRegisterIndex Rn, uint_fast16_t offset) {
  uint8_t temp = registers->gprs[Rd];
  bool success = Store8(memory, registers->gprs[Rn], temp);
  assert(success);
  registers->gprs[Rn] -= offset;
}

static inline void ArmSTRB_IBW(ArmGeneralPurposeRegisters *registers,
                               Memory *memory, ArmRegisterIndex Rd,
                               ArmRegisterIndex Rn, uint_fast16_t offset) {
  registers->gprs[Rn] += offset;
  uint8_t temp = registers->gprs[Rd];
  bool success = Store8(memory, registers->gprs[Rn], temp);
  assert(success);
}

static inline void ArmSTRB_IAW(ArmGeneralPurposeRegisters *registers,
                               Memory *memory, ArmRegisterIndex Rd,
                               ArmRegisterIndex Rn, uint_fast16_t offset) {
  uint8_t temp = registers->gprs[Rd];
  bool success = Store8(memory, registers->gprs[Rn], temp);
  assert(success);
  registers->gprs[Rn] += offset;
}

static inline void ArmSTRBT(ArmAllRegisters *registers, Memory *memory,
                            ArmRegisterIndex Rd, uint32_t address) {
  ArmProgramStatusRegister current_status = registers->current.user.cpsr;
  ArmProgramStatusRegister temporary_status = current_status;
  temporary_status.mode = MODE_USR;
  uint8_t temp = registers->current.user.gprs.gprs[Rd];

  ArmLoadCPSR(registers, temporary_status);
  bool success = Store8(memory, address, temp);
  assert(success);
  ArmLoadCPSR(registers, current_status);
}

static inline void ArmSTRBT_DAW(ArmAllRegisters *registers, Memory *memory,
                                ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                                uint_fast16_t offset) {
  ArmProgramStatusRegister current_status = registers->current.user.cpsr;
  ArmProgramStatusRegister temporary_status = current_status;
  temporary_status.mode = MODE_USR;
  uint32_t address = registers->current.user.gprs.gprs[Rn];
  uint8_t value = registers->current.user.gprs.gprs[Rd];

  ArmLoadCPSR(registers, temporary_status);
  bool success = Store8(memory, address, value);
  assert(success);
  ArmLoadCPSR(registers, current_status);

  registers->current.user.gprs.gprs[Rn] -= offset;
}

static inline void ArmSTRBT_IAW(ArmAllRegisters *registers, Memory *memory,
                                ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                                uint_fast16_t offset) {
  ArmProgramStatusRegister current_status = registers->current.user.cpsr;
  ArmProgramStatusRegister temporary_status = current_status;
  temporary_status.mode = MODE_USR;
  uint32_t address = registers->current.user.gprs.gprs[Rn];
  uint8_t value = registers->current.user.gprs.gprs[Rd];

  ArmLoadCPSR(registers, temporary_status);
  bool success = Store8(memory, address, value);
  assert(success);
  ArmLoadCPSR(registers, current_status);

  registers->current.user.gprs.gprs[Rn] += offset;
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_LOAD_STORE_REGISTER_BYTE_