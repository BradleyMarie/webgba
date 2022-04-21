#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_SINGLE_DATA_SWAP_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_SINGLE_DATA_SWAP_

#include "emulator/cpu/arm7tdmi/exceptions.h"
#include "emulator/cpu/arm7tdmi/memory.h"
#include "emulator/cpu/arm7tdmi/registers.h"

static inline void ArmSWP(ArmAllRegisters *registers, Memory *memory,
                          ArmRegisterIndex Rd, ArmRegisterIndex Rm,
                          ArmRegisterIndex Rn) {
  uint32_t temp;
  bool success = ArmLoad32LEWithRotation(
      memory, registers->current.user.gprs.gprs[Rn], &temp);
  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }

  success = ArmStore32LE(memory, registers->current.user.gprs.gprs[Rn],
                         registers->current.user.gprs.gprs[Rm]);
  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rd, temp);
}

static inline void ArmSWPB(ArmAllRegisters *registers, Memory *memory,
                           ArmRegisterIndex Rd, ArmRegisterIndex Rm,
                           ArmRegisterIndex Rn) {
  uint8_t temp;
  bool success = Load8(memory, registers->current.user.gprs.gprs[Rn], &temp);
  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }

  success = Store8(memory, registers->current.user.gprs.gprs[Rn],
                   (uint8_t)registers->current.user.gprs.gprs[Rm]);
  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }

  ArmAdvanceProgramCounter(registers);
  ArmLoadGPSR(registers, Rd, temp);
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_SINGLE_DATA_SWAP_