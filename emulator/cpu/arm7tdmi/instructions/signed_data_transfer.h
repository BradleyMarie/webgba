#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_SIGNED_DATA_TRANSFER_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_SIGNED_DATA_TRANSFER_

#include "emulator/cpu/arm7tdmi/exceptions.h"
#include "emulator/cpu/arm7tdmi/instructions/address_mode.h"
#include "emulator/cpu/arm7tdmi/memory.h"
#include "emulator/cpu/arm7tdmi/registers.h"
#include "util/macros.h"

static inline void ArmLDRH(ArmAllRegisters *registers, const Memory *memory,
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
  bool success = ArmLoad16LEWithRotation(memory, load_address, &value);

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

static inline void ArmLDRH_DAW(ArmAllRegisters *registers, const Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast16_t offset) {
  ArmLDRH(registers, memory, Rd, Rn, offset, ADDRESS_MODE_DECREMENT_AFTER,
          /*writeback=*/true);
}

static inline void ArmLDRH_DB(ArmAllRegisters *registers, const Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint32_t offset) {
  ArmLDRH(registers, memory, Rd, Rn, offset, ADDRESS_MODE_DECREMENT_BEFORE,
          /*writeback=*/false);
}

static inline void ArmLDRH_DBW(ArmAllRegisters *registers, const Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast16_t offset) {
  ArmLDRH(registers, memory, Rd, Rn, offset, ADDRESS_MODE_DECREMENT_BEFORE,
          /*writeback=*/true);
}

static inline void ArmLDRH_IAW(ArmAllRegisters *registers, const Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast16_t offset) {
  ArmLDRH(registers, memory, Rd, Rn, offset, ADDRESS_MODE_INCREMENT_AFTER,
          /*writeback=*/true);
}

static inline void ArmLDRH_IB(ArmAllRegisters *registers, const Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint32_t offset) {
  ArmLDRH(registers, memory, Rd, Rn, offset, ADDRESS_MODE_INCREMENT_BEFORE,
          /*writeback=*/false);
}

static inline void ArmLDRH_IBW(ArmAllRegisters *registers, const Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast16_t offset) {
  ArmLDRH(registers, memory, Rd, Rn, offset, ADDRESS_MODE_INCREMENT_BEFORE,
          /*writeback=*/true);
}

static inline void ArmLDRSB(ArmAllRegisters *registers, const Memory *memory,
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

  int8_t value;
  bool success = Load8S(memory, load_address, &value);

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

  ArmLoadGPSR(registers, Rd, (int32_t)value);
}

static inline void ArmLDRSB_DAW(ArmAllRegisters *registers,
                                const Memory *memory, ArmRegisterIndex Rd,
                                ArmRegisterIndex Rn, uint_fast16_t offset) {
  ArmLDRSB(registers, memory, Rd, Rn, offset, ADDRESS_MODE_DECREMENT_AFTER,
           /*writeback=*/true);
}

static inline void ArmLDRSB_DB(ArmAllRegisters *registers, const Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint32_t offset) {
  ArmLDRSB(registers, memory, Rd, Rn, offset, ADDRESS_MODE_DECREMENT_BEFORE,
           /*writeback=*/false);
}

static inline void ArmLDRSB_DBW(ArmAllRegisters *registers,
                                const Memory *memory, ArmRegisterIndex Rd,
                                ArmRegisterIndex Rn, uint_fast16_t offset) {
  ArmLDRSB(registers, memory, Rd, Rn, offset, ADDRESS_MODE_DECREMENT_BEFORE,
           /*writeback=*/true);
}

static inline void ArmLDRSB_IAW(ArmAllRegisters *registers,
                                const Memory *memory, ArmRegisterIndex Rd,
                                ArmRegisterIndex Rn, uint_fast16_t offset) {
  ArmLDRSB(registers, memory, Rd, Rn, offset, ADDRESS_MODE_INCREMENT_AFTER,
           /*writeback=*/true);
}

static inline void ArmLDRSB_IB(ArmAllRegisters *registers, const Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint32_t offset) {
  ArmLDRSB(registers, memory, Rd, Rn, offset, ADDRESS_MODE_INCREMENT_BEFORE,
           /*writeback=*/false);
}

static inline void ArmLDRSB_IBW(ArmAllRegisters *registers,
                                const Memory *memory, ArmRegisterIndex Rd,
                                ArmRegisterIndex Rn, uint_fast16_t offset) {
  ArmLDRSB(registers, memory, Rd, Rn, offset, ADDRESS_MODE_INCREMENT_BEFORE,
           /*writeback=*/true);
}

static inline void ArmLDRSH(ArmAllRegisters *registers, const Memory *memory,
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

  int32_t value;
  bool success = ArmLoad16SLEWithRotation(memory, load_address, &value);

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

  ArmLoadGPSR(registers, Rd, (int32_t)value);
}

static inline void ArmLDRSH_DAW(ArmAllRegisters *registers,
                                const Memory *memory, ArmRegisterIndex Rd,
                                ArmRegisterIndex Rn, uint_fast16_t offset) {
  ArmLDRSH(registers, memory, Rd, Rn, offset, ADDRESS_MODE_DECREMENT_AFTER,
           /*writeback=*/true);
}

static inline void ArmLDRSH_DB(ArmAllRegisters *registers, const Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint32_t offset) {
  ArmLDRSH(registers, memory, Rd, Rn, offset, ADDRESS_MODE_DECREMENT_BEFORE,
           /*writeback=*/false);
}

static inline void ArmLDRSH_DBW(ArmAllRegisters *registers,
                                const Memory *memory, ArmRegisterIndex Rd,
                                ArmRegisterIndex Rn, uint_fast16_t offset) {
  ArmLDRSH(registers, memory, Rd, Rn, offset, ADDRESS_MODE_DECREMENT_BEFORE,
           /*writeback=*/true);
}

static inline void ArmLDRSH_IAW(ArmAllRegisters *registers,
                                const Memory *memory, ArmRegisterIndex Rd,
                                ArmRegisterIndex Rn, uint_fast16_t offset) {
  ArmLDRSH(registers, memory, Rd, Rn, offset, ADDRESS_MODE_INCREMENT_AFTER,
           /*writeback=*/true);
}

static inline void ArmLDRSH_IB(ArmAllRegisters *registers, const Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint32_t offset) {
  ArmLDRSH(registers, memory, Rd, Rn, offset, ADDRESS_MODE_INCREMENT_BEFORE,
           /*writeback=*/false);
}

static inline void ArmLDRSH_IBW(ArmAllRegisters *registers,
                                const Memory *memory, ArmRegisterIndex Rd,
                                ArmRegisterIndex Rn, uint_fast16_t offset) {
  ArmLDRSH(registers, memory, Rd, Rn, offset, ADDRESS_MODE_INCREMENT_BEFORE,
           /*writeback=*/true);
}

static inline void ArmSTRH(ArmAllRegisters *registers, Memory *memory,
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

  bool success = ArmStore16LE(memory, write_address, (int16_t)value);

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

static inline void ArmSTRH_DAW(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast16_t offset) {
  ArmSTRH(registers, memory, Rd, Rn, offset, ADDRESS_MODE_DECREMENT_AFTER,
          /*writeback=*/true);
}

static inline void ArmSTRH_DB(ArmAllRegisters *registers, Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint32_t offset) {
  ArmSTRH(registers, memory, Rd, Rn, offset, ADDRESS_MODE_DECREMENT_BEFORE,
          /*writeback=*/false);
}

static inline void ArmSTRH_DBW(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast16_t offset) {
  ArmSTRH(registers, memory, Rd, Rn, offset, ADDRESS_MODE_DECREMENT_BEFORE,
          /*writeback=*/true);
}

static inline void ArmSTRH_IAW(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast16_t offset) {
  ArmSTRH(registers, memory, Rd, Rn, offset, ADDRESS_MODE_INCREMENT_AFTER,
          /*writeback=*/true);
}

static inline void ArmSTRH_IB(ArmAllRegisters *registers, Memory *memory,
                              ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                              uint32_t offset) {
  ArmSTRH(registers, memory, Rd, Rn, offset, ADDRESS_MODE_INCREMENT_BEFORE,
          /*writeback=*/false);
}

static inline void ArmSTRH_IBW(ArmAllRegisters *registers, Memory *memory,
                               ArmRegisterIndex Rd, ArmRegisterIndex Rn,
                               uint_fast16_t offset) {
  ArmSTRH(registers, memory, Rd, Rn, offset, ADDRESS_MODE_INCREMENT_BEFORE,
          /*writeback=*/true);
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTRUCTIONS_SIGNED_DATA_TRANSFER_