#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_LOAD_STORE_REGISTER_BYTE_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_LOAD_STORE_REGISTER_BYTE_

#include "emulator/cpu/arm7tdmi/exceptions.h"
#include "emulator/cpu/arm7tdmi/instructions/address_mode.h"
#include "emulator/cpu/arm7tdmi/memory.h"
#include "emulator/cpu/arm7tdmi/registers.h"
#include "util/macros.h"

static inline void ArmLDR(ArmAllRegisters *registers, const Memory *memory,
                          ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                          uint_fast16_t offset, ArmAddressMode address_mode,
                          bool writeback) {
  uint32_t input_address = registers->current.user.gprs.gprs[Rn];

  uint32_t load_address, output_address;
  switch (address_mode) {
    case ADDRESS_MODE_DECREMENT_AFTER:
      load_address = input_address;
      output_address = input_address - offset;
      break;
    case ADDRESS_MODE_DECREMENT_BEFORE:
      load_address = input_address - offset;
      output_address = load_address;
      break;
    case ADDRESS_MODE_INCREMENT_AFTER:
      load_address = input_address;
      output_address = input_address + offset;
      break;
    default:
      codegen_assert(false);
    case ADDRESS_MODE_INCREMENT_BEFORE:
      load_address = input_address + offset;
      output_address = load_address;
      break;
  }

  uint32_t value;
  bool success = ArmLoad32LEWithRotation(memory, load_address, &value);

  if (writeback) {
    ArmLoadGPSR(registers, Rn, output_address);
  }

  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }

  if (!writeback || Rn != REGISTER_PC) {
    ArmAdvanceProgramCounter(registers);
  }

  ArmLoadGPSR(registers, Rd, value);
}

static inline void ArmLDR_DAW(ArmAllRegisters *registers, const Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint_fast16_t offset) {
  ArmLDR(registers, memory, Rd, Rn, offset, ADDRESS_MODE_DECREMENT_AFTER,
         /*writeback=*/true);
}

static inline void ArmLDR_DB(ArmAllRegisters *registers, const Memory *memory,
                             ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                             uint32_t offset) {
  ArmLDR(registers, memory, Rd, Rn, offset, ADDRESS_MODE_DECREMENT_BEFORE,
         /*writeback=*/false);
}

static inline void ArmLDR_DBW(ArmAllRegisters *registers, const Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint_fast16_t offset) {
  ArmLDR(registers, memory, Rd, Rn, offset, ADDRESS_MODE_DECREMENT_BEFORE,
         /*writeback=*/true);
}

static inline void ArmLDR_IAW(ArmAllRegisters *registers, const Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint_fast16_t offset) {
  ArmLDR(registers, memory, Rd, Rn, offset, ADDRESS_MODE_INCREMENT_AFTER,
         /*writeback=*/true);
}

static inline void ArmLDR_IB(ArmAllRegisters *registers, const Memory *memory,
                             ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                             uint32_t offset) {
  ArmLDR(registers, memory, Rd, Rn, offset, ADDRESS_MODE_INCREMENT_BEFORE,
         /*writeback=*/false);
}

static inline void ArmLDR_IBW(ArmAllRegisters *registers, const Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint_fast16_t offset) {
  ArmLDR(registers, memory, Rd, Rn, offset, ADDRESS_MODE_INCREMENT_BEFORE,
         /*writeback=*/true);
}

static inline void ArmLDRT(ArmAllRegisters *registers, const Memory *memory,
                           ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                           uint_fast16_t offset, ArmAddressMode address_mode,
                           bool writeback) {
  ArmProgramStatusRegister current_status = registers->current.user.cpsr;
  ArmProgramStatusRegister temporary_status = current_status;
  temporary_status.mode = MODE_USR;

  uint32_t input_address = registers->current.user.gprs.gprs[Rn];

  uint32_t load_address, output_address;
  switch (address_mode) {
    case ADDRESS_MODE_DECREMENT_AFTER:
      load_address = input_address;
      output_address = input_address - offset;
      break;
    case ADDRESS_MODE_DECREMENT_BEFORE:
      load_address = input_address - offset;
      output_address = load_address;
      break;
    case ADDRESS_MODE_INCREMENT_AFTER:
      load_address = input_address;
      output_address = input_address + offset;
      break;
    default:
      codegen_assert(false);
    case ADDRESS_MODE_INCREMENT_BEFORE:
      load_address = input_address + offset;
      output_address = load_address;
      break;
  }

  ArmLoadCPSR(registers, temporary_status);
  uint32_t value;
  bool success = ArmLoad32LEWithRotation(memory, load_address, &value);
  ArmLoadCPSR(registers, current_status);

  if (writeback) {
    ArmLoadGPSR(registers, Rn, output_address);
  }

  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }

  if (!writeback || Rn != REGISTER_PC) {
    ArmAdvanceProgramCounter(registers);
  }

  ArmLoadGPSR(registers, Rd, value);
}

static inline void ArmLDRT_DAW(ArmAllRegisters *registers, const Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast16_t offset) {
  ArmLDRT(registers, memory, Rd, Rn, offset, ADDRESS_MODE_DECREMENT_AFTER,
          /*writeback=*/true);
}

static inline void ArmLDRT_DB(ArmAllRegisters *registers, const Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint32_t offset) {
  ArmLDRT(registers, memory, Rd, Rn, offset, ADDRESS_MODE_DECREMENT_BEFORE,
          /*writeback=*/false);
}

static inline void ArmLDRT_IAW(ArmAllRegisters *registers, const Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast16_t offset) {
  ArmLDRT(registers, memory, Rd, Rn, offset, ADDRESS_MODE_INCREMENT_AFTER,
          /*writeback=*/true);
}

static inline void ArmLDRT_IB(ArmAllRegisters *registers, const Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint32_t offset) {
  ArmLDRT(registers, memory, Rd, Rn, offset, ADDRESS_MODE_INCREMENT_BEFORE,
          /*writeback=*/false);
}

static inline void ArmLDRB(ArmAllRegisters *registers, const Memory *memory,
                           ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                           uint_fast16_t offset, ArmAddressMode address_mode,
                           bool writeback) {
  uint32_t input_address = registers->current.user.gprs.gprs[Rn];

  uint32_t load_address, output_address;
  switch (address_mode) {
    case ADDRESS_MODE_DECREMENT_AFTER:
      load_address = input_address;
      output_address = input_address - offset;
      break;
    case ADDRESS_MODE_DECREMENT_BEFORE:
      load_address = input_address - offset;
      output_address = load_address;
      break;
    case ADDRESS_MODE_INCREMENT_AFTER:
      load_address = input_address;
      output_address = input_address + offset;
      break;
    default:
      codegen_assert(false);
    case ADDRESS_MODE_INCREMENT_BEFORE:
      load_address = input_address + offset;
      output_address = load_address;
      break;
  }

  uint8_t value;
  bool success = Load8(memory, load_address, &value);

  if (writeback) {
    ArmLoadGPSR(registers, Rn, output_address);
  }

  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }

  if (!writeback || Rn != REGISTER_PC) {
    ArmAdvanceProgramCounter(registers);
  }

  ArmLoadGPSR(registers, Rd, value);
}

static inline void ArmLDRB_DAW(ArmAllRegisters *registers, const Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast16_t offset) {
  ArmLDRB(registers, memory, Rd, Rn, offset, ADDRESS_MODE_DECREMENT_AFTER,
          /*writeback=*/true);
}

static inline void ArmLDRB_DB(ArmAllRegisters *registers, const Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint32_t offset) {
  ArmLDRB(registers, memory, Rd, Rn, offset, ADDRESS_MODE_DECREMENT_BEFORE,
          /*writeback=*/false);
}

static inline void ArmLDRB_DBW(ArmAllRegisters *registers, const Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast16_t offset) {
  ArmLDRB(registers, memory, Rd, Rn, offset, ADDRESS_MODE_DECREMENT_BEFORE,
          /*writeback=*/true);
}

static inline void ArmLDRB_IAW(ArmAllRegisters *registers, const Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast16_t offset) {
  ArmLDRB(registers, memory, Rd, Rn, offset, ADDRESS_MODE_INCREMENT_AFTER,
          /*writeback=*/true);
}

static inline void ArmLDRB_IB(ArmAllRegisters *registers, const Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint32_t offset) {
  ArmLDRB(registers, memory, Rd, Rn, offset, ADDRESS_MODE_INCREMENT_BEFORE,
          /*writeback=*/false);
}

static inline void ArmLDRB_IBW(ArmAllRegisters *registers, const Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast16_t offset) {
  ArmLDRB(registers, memory, Rd, Rn, offset, ADDRESS_MODE_INCREMENT_BEFORE,
          /*writeback=*/true);
}

static inline void ArmLDRBT(ArmAllRegisters *registers, const Memory *memory,
                            ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                            uint_fast16_t offset, ArmAddressMode address_mode,
                            bool writeback) {
  ArmProgramStatusRegister current_status = registers->current.user.cpsr;
  ArmProgramStatusRegister temporary_status = current_status;
  temporary_status.mode = MODE_USR;

  uint32_t input_address = registers->current.user.gprs.gprs[Rn];

  uint32_t load_address, output_address;
  switch (address_mode) {
    case ADDRESS_MODE_DECREMENT_AFTER:
      load_address = input_address;
      output_address = input_address - offset;
      break;
    case ADDRESS_MODE_DECREMENT_BEFORE:
      load_address = input_address - offset;
      output_address = load_address;
      break;
    case ADDRESS_MODE_INCREMENT_AFTER:
      load_address = input_address;
      output_address = input_address + offset;
      break;
    default:
      codegen_assert(false);
    case ADDRESS_MODE_INCREMENT_BEFORE:
      load_address = input_address + offset;
      output_address = load_address;
      break;
  }

  ArmLoadCPSR(registers, temporary_status);
  uint8_t value;
  bool success = Load8(memory, load_address, &value);
  ArmLoadCPSR(registers, current_status);

  if (writeback) {
    ArmLoadGPSR(registers, Rn, output_address);
  }

  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }

  if (!writeback || Rn != REGISTER_PC) {
    ArmAdvanceProgramCounter(registers);
  }

  ArmLoadGPSR(registers, Rd, value);
}

static inline void ArmLDRBT_DAW(ArmAllRegisters *registers,
                                const Memory *memory, ArmRegisterIndex Rd,
                                ArmRegisterIndex Rn, uint_fast16_t offset) {
  ArmLDRBT(registers, memory, Rd, Rn, offset, ADDRESS_MODE_DECREMENT_AFTER,
           /*writeback=*/true);
}

static inline void ArmLDRBT_DB(ArmAllRegisters *registers, const Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint32_t offset) {
  ArmLDRBT(registers, memory, Rd, Rn, offset, ADDRESS_MODE_DECREMENT_BEFORE,
           /*writeback=*/false);
}

static inline void ArmLDRBT_IAW(ArmAllRegisters *registers,
                                const Memory *memory, ArmRegisterIndex Rd,
                                ArmRegisterIndex Rn, uint_fast16_t offset) {
  ArmLDRBT(registers, memory, Rd, Rn, offset, ADDRESS_MODE_INCREMENT_AFTER,
           /*writeback=*/true);
}

static inline void ArmLDRBT_IB(ArmAllRegisters *registers, const Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint32_t offset) {
  ArmLDRBT(registers, memory, Rd, Rn, offset, ADDRESS_MODE_INCREMENT_BEFORE,
           /*writeback=*/false);
}

static inline void ArmSTR(ArmAllRegisters *registers, Memory *memory,
                          ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                          uint_fast16_t offset, ArmAddressMode address_mode,
                          bool writeback) {
  uint32_t input_address = registers->current.user.gprs.gprs[Rn];

  uint32_t write_address, output_address;
  switch (address_mode) {
    case ADDRESS_MODE_DECREMENT_AFTER:
      write_address = input_address;
      output_address = input_address - offset;
      break;
    case ADDRESS_MODE_DECREMENT_BEFORE:
      write_address = input_address - offset;
      output_address = write_address;
      break;
    case ADDRESS_MODE_INCREMENT_AFTER:
      write_address = input_address;
      output_address = input_address + offset;
      break;
    default:
      codegen_assert(false);
    case ADDRESS_MODE_INCREMENT_BEFORE:
      write_address = input_address + offset;
      output_address = write_address;
      break;
  }

  uint32_t value = registers->current.user.gprs.gprs[Rd];
  if (Rd == REGISTER_R15) {
    codegen_assert(!registers->current.user.cpsr.thumb);
    value += 4u;
  }

  bool success = ArmStore32LE(memory, write_address, value);

  if (writeback) {
    ArmLoadGPSR(registers, Rn, output_address);
  }

  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }

  if (!writeback || Rn != REGISTER_PC) {
    ArmAdvanceProgramCounter(registers);
  }
}

static inline void ArmSTR_DAW(ArmAllRegisters *registers, Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint_fast16_t offset) {
  ArmSTR(registers, memory, Rd, Rn, offset, ADDRESS_MODE_DECREMENT_AFTER,
         /*writeback=*/true);
}

static inline void ArmSTR_DB(ArmAllRegisters *registers, Memory *memory,
                             ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                             uint32_t offset) {
  ArmSTR(registers, memory, Rd, Rn, offset, ADDRESS_MODE_DECREMENT_BEFORE,
         /*writeback=*/false);
}

static inline void ArmSTR_DBW(ArmAllRegisters *registers, Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint_fast16_t offset) {
  ArmSTR(registers, memory, Rd, Rn, offset, ADDRESS_MODE_DECREMENT_BEFORE,
         /*writeback=*/true);
}

static inline void ArmSTR_IAW(ArmAllRegisters *registers, Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint_fast16_t offset) {
  ArmSTR(registers, memory, Rd, Rn, offset, ADDRESS_MODE_INCREMENT_AFTER,
         /*writeback=*/true);
}

static inline void ArmSTR_IB(ArmAllRegisters *registers, Memory *memory,
                             ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                             uint32_t offset) {
  ArmSTR(registers, memory, Rd, Rn, offset, ADDRESS_MODE_INCREMENT_BEFORE,
         /*writeback=*/false);
}

static inline void ArmSTR_IBW(ArmAllRegisters *registers, Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint_fast16_t offset) {
  ArmSTR(registers, memory, Rd, Rn, offset, ADDRESS_MODE_INCREMENT_BEFORE,
         /*writeback=*/true);
}

static inline void ArmSTRT(ArmAllRegisters *registers, Memory *memory,
                           ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                           uint_fast16_t offset, ArmAddressMode address_mode,
                           bool writeback) {
  ArmProgramStatusRegister current_status = registers->current.user.cpsr;
  ArmProgramStatusRegister temporary_status = current_status;
  temporary_status.mode = MODE_USR;

  uint32_t input_address = registers->current.user.gprs.gprs[Rn];

  uint32_t write_address, output_address;
  switch (address_mode) {
    case ADDRESS_MODE_DECREMENT_AFTER:
      write_address = input_address;
      output_address = input_address - offset;
      break;
    case ADDRESS_MODE_DECREMENT_BEFORE:
      write_address = input_address - offset;
      output_address = write_address;
      break;
    case ADDRESS_MODE_INCREMENT_AFTER:
      write_address = input_address;
      output_address = input_address + offset;
      break;
    default:
      codegen_assert(false);
    case ADDRESS_MODE_INCREMENT_BEFORE:
      write_address = input_address + offset;
      output_address = write_address;
      break;
  }

  uint32_t value = registers->current.user.gprs.gprs[Rd];
  if (Rd == REGISTER_R15) {
    codegen_assert(!registers->current.user.cpsr.thumb);
    value += 4u;
  }

  ArmLoadCPSR(registers, temporary_status);
  bool success = ArmStore32LE(memory, write_address, value);
  ArmLoadCPSR(registers, current_status);

  if (writeback) {
    ArmLoadGPSR(registers, Rn, output_address);
  }

  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }

  if (!writeback || Rn != REGISTER_PC) {
    ArmAdvanceProgramCounter(registers);
  }
}

static inline void ArmSTRT_DAW(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast16_t offset) {
  ArmSTRT(registers, memory, Rd, Rn, offset, ADDRESS_MODE_DECREMENT_AFTER,
          /*writeback=*/true);
}

static inline void ArmSTRT_DB(ArmAllRegisters *registers, Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint32_t offset) {
  ArmSTRT(registers, memory, Rd, Rn, offset, ADDRESS_MODE_DECREMENT_BEFORE,
          /*writeback=*/false);
}

static inline void ArmSTRT_IB(ArmAllRegisters *registers, Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint32_t offset) {
  ArmSTRT(registers, memory, Rd, Rn, offset, ADDRESS_MODE_INCREMENT_BEFORE,
          /*writeback=*/false);
}

static inline void ArmSTRT_IAW(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast16_t offset) {
  ArmSTRT(registers, memory, Rd, Rn, offset, ADDRESS_MODE_INCREMENT_AFTER,
          /*writeback=*/true);
}

static inline void ArmSTRB(ArmAllRegisters *registers, Memory *memory,
                           ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                           uint_fast16_t offset, ArmAddressMode address_mode,
                           bool writeback) {
  uint32_t input_address = registers->current.user.gprs.gprs[Rn];

  uint32_t write_address, output_address;
  switch (address_mode) {
    case ADDRESS_MODE_DECREMENT_AFTER:
      write_address = input_address;
      output_address = input_address - offset;
      break;
    case ADDRESS_MODE_DECREMENT_BEFORE:
      write_address = input_address - offset;
      output_address = write_address;
      break;
    case ADDRESS_MODE_INCREMENT_AFTER:
      write_address = input_address;
      output_address = input_address + offset;
      break;
    default:
      codegen_assert(false);
    case ADDRESS_MODE_INCREMENT_BEFORE:
      write_address = input_address + offset;
      output_address = write_address;
      break;
  }

  uint32_t value = registers->current.user.gprs.gprs[Rd];
  if (Rd == REGISTER_R15) {
    codegen_assert(!registers->current.user.cpsr.thumb);
    value += 4u;
  }

  bool success = Store8(memory, write_address, (uint8_t)value);

  if (writeback) {
    ArmLoadGPSR(registers, Rn, output_address);
  }

  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }

  if (!writeback || Rn != REGISTER_PC) {
    ArmAdvanceProgramCounter(registers);
  }
}

static inline void ArmSTRB_DAW(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast16_t offset) {
  ArmSTRB(registers, memory, Rd, Rn, offset, ADDRESS_MODE_DECREMENT_AFTER,
          /*writeback=*/true);
}

static inline void ArmSTRB_DB(ArmAllRegisters *registers, Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint32_t offset) {
  ArmSTRB(registers, memory, Rd, Rn, offset, ADDRESS_MODE_DECREMENT_BEFORE,
          /*writeback=*/false);
}

static inline void ArmSTRB_DBW(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast16_t offset) {
  ArmSTRB(registers, memory, Rd, Rn, offset, ADDRESS_MODE_DECREMENT_BEFORE,
          /*writeback=*/true);
}

static inline void ArmSTRB_IAW(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast16_t offset) {
  ArmSTRB(registers, memory, Rd, Rn, offset, ADDRESS_MODE_INCREMENT_AFTER,
          /*writeback=*/true);
}

static inline void ArmSTRB_IB(ArmAllRegisters *registers, Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint32_t offset) {
  ArmSTRB(registers, memory, Rd, Rn, offset, ADDRESS_MODE_INCREMENT_BEFORE,
          /*writeback=*/false);
}

static inline void ArmSTRB_IBW(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast16_t offset) {
  ArmSTRB(registers, memory, Rd, Rn, offset, ADDRESS_MODE_INCREMENT_BEFORE,
          /*writeback=*/true);
}

static inline void ArmSTRBT(ArmAllRegisters *registers, Memory *memory,
                            ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                            uint_fast16_t offset, ArmAddressMode address_mode,
                            bool writeback) {
  ArmProgramStatusRegister current_status = registers->current.user.cpsr;
  ArmProgramStatusRegister temporary_status = current_status;
  temporary_status.mode = MODE_USR;

  uint32_t input_address = registers->current.user.gprs.gprs[Rn];

  uint32_t write_address, output_address;
  switch (address_mode) {
    case ADDRESS_MODE_DECREMENT_AFTER:
      write_address = input_address;
      output_address = input_address - offset;
      break;
    case ADDRESS_MODE_DECREMENT_BEFORE:
      write_address = input_address - offset;
      output_address = write_address;
      break;
    case ADDRESS_MODE_INCREMENT_AFTER:
      write_address = input_address;
      output_address = input_address + offset;
      break;
    default:
      codegen_assert(false);
    case ADDRESS_MODE_INCREMENT_BEFORE:
      write_address = input_address + offset;
      output_address = write_address;
      break;
  }

  uint32_t value = registers->current.user.gprs.gprs[Rd];
  if (Rd == REGISTER_R15) {
    codegen_assert(!registers->current.user.cpsr.thumb);
    value += 4u;
  }

  ArmLoadCPSR(registers, temporary_status);
  bool success = Store8(memory, write_address, (uint8_t)value);
  ArmLoadCPSR(registers, current_status);

  if (writeback) {
    ArmLoadGPSR(registers, Rn, output_address);
  }

  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }

  if (!writeback || Rn != REGISTER_PC) {
    ArmAdvanceProgramCounter(registers);
  }
}

static inline void ArmSTRBT_DAW(ArmAllRegisters *registers, Memory *memory,
                                ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                                uint_fast16_t offset) {
  ArmSTRBT(registers, memory, Rd, Rn, offset, ADDRESS_MODE_DECREMENT_AFTER,
           /*writeback=*/true);
}

static inline void ArmSTRBT_DB(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint32_t offset) {
  ArmSTRBT(registers, memory, Rd, Rn, offset, ADDRESS_MODE_DECREMENT_BEFORE,
           /*writeback=*/false);
}

static inline void ArmSTRBT_IB(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint32_t offset) {
  ArmSTRBT(registers, memory, Rd, Rn, offset, ADDRESS_MODE_INCREMENT_BEFORE,
           /*writeback=*/false);
}

static inline void ArmSTRBT_IAW(ArmAllRegisters *registers, Memory *memory,
                                ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                                uint_fast16_t offset) {
  ArmSTRBT(registers, memory, Rd, Rn, offset, ADDRESS_MODE_INCREMENT_AFTER,
           /*writeback=*/true);
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_LOAD_STORE_REGISTER_BYTE_