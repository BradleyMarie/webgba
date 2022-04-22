#include "emulator/cpu/arm7tdmi/instructions/block_data_transfer.h"

#include <assert.h>

#include "emulator/cpu/arm7tdmi/exceptions.h"
#include "util/macros.h"

typedef enum {
  ADDRESS_MODE_DECREMENT_AFTER,
  ADDRESS_MODE_DECREMENT_BEFORE,
  ADDRESS_MODE_INCREMENT_AFTER,
  ADDRESS_MODE_INCREMENT_BEFORE,
} ArmAddressMode;

static inline uint_fast8_t PopCount(uint_fast16_t value) {
  return __builtin_popcount(value);
}

static inline bool NextRegister(uint_fast16_t *register_list, int *index) {
  if (*register_list == 0u) {
    return false;
  }

  *index = __builtin_ctz(*register_list);
  *register_list ^= 1u << *index;

  return true;
}

static inline void ArmLDM(ArmAllRegisters *registers, const Memory *memory,
                          ArmRegisterIndex Rn, uint_fast16_t register_list,
                          ArmAddressMode address_mode, bool writeback) {
  assert(register_list <= UINT16_MAX);

  uint_fast8_t size = PopCount(register_list) * 4u;
  uint32_t input_address = registers->current.user.gprs.gprs[Rn];

  if (register_list != 0) {
    uint32_t output_address, load_address;
    switch (address_mode) {
      case ADDRESS_MODE_DECREMENT_AFTER:
        output_address = input_address - size;
        load_address = output_address + 4u;
        break;
      case ADDRESS_MODE_DECREMENT_BEFORE:
        output_address = input_address - size;
        load_address = output_address;
        break;
      case ADDRESS_MODE_INCREMENT_AFTER:
        output_address = input_address + size;
        load_address = input_address;
        break;
      default:
        codegen_assert(false);
      case ADDRESS_MODE_INCREMENT_BEFORE:
        output_address = input_address + size;
        load_address = input_address + 4u;
        break;
    }

    uint_fast16_t register_list_mutable = register_list;

    bool success;
    int i;
    while (NextRegister(&register_list_mutable, &i)) {
      success = ArmLoad32LE(memory, load_address,
                            &registers->current.user.gprs.gprs[i]);
      if (!success) {
        break;
      }
      load_address += 4u;
    }

    writeback = writeback && (register_list & (1u << Rn)) == 0u;
    if (writeback) {
      registers->current.user.gprs.gprs[Rn] = output_address;
    }

    if (!success) {
      ArmExceptionDataABT(registers);
      return;
    }

    if ((register_list & (1u << REGISTER_PC)) != 0u ||
        (writeback && Rn == REGISTER_PC)) {
      ArmLoadGPSR(registers, REGISTER_PC, registers->current.user.gprs.pc);
    } else {
      ArmAdvanceProgramCounter(registers);
    }
  } else {
    uint32_t output_address;
    switch (address_mode) {
      case ADDRESS_MODE_DECREMENT_AFTER:
      case ADDRESS_MODE_DECREMENT_BEFORE:
        output_address = input_address - 64u;
        break;
      default:
        codegen_assert(false);
      case ADDRESS_MODE_INCREMENT_AFTER:
      case ADDRESS_MODE_INCREMENT_BEFORE:
        output_address = input_address + 64u;
        break;
    }

    uint32_t value;
    bool success = ArmLoad32LE(memory, input_address, &value);
    if (!success) {
      ArmExceptionDataABT(registers);
      return;
    }

    ArmLoadGPSR(registers, REGISTER_PC, value);

    if (writeback) {
      ArmLoadGPSR(registers, Rn, output_address);
    }
  }
}

static inline void ArmLDMS(ArmAllRegisters *registers, const Memory *memory,
                           ArmRegisterIndex Rn, uint_fast16_t register_list,
                           ArmAddressMode address_mode, bool writeback) {
  assert(register_list <= UINT16_MAX);

  bool mode_has_spsr = registers->current.user.cpsr.mode != MODE_USR ||
                       registers->current.user.cpsr.mode != MODE_SYS;
  bool load_pc = (register_list & 1u << REGISTER_PC) != 0u;
  bool load_spsr = load_pc && mode_has_spsr;
  bool load_usr_mode = !load_spsr && mode_has_spsr;

  ArmProgramStatusRegister original_cpsr = registers->current.user.cpsr;
  if (load_usr_mode) {
    ArmProgramStatusRegister temporary_cpsr = registers->current.user.cpsr;
    temporary_cpsr.mode = MODE_USR;
    ArmLoadCPSR(registers, temporary_cpsr);
  }

  uint_fast8_t size = PopCount(register_list) * 4u;
  uint32_t input_address = registers->current.user.gprs.gprs[Rn];

  if (register_list != 0) {
    uint32_t output_address, load_address;
    switch (address_mode) {
      case ADDRESS_MODE_DECREMENT_AFTER:
        output_address = input_address - size;
        load_address = output_address + 4u;
        break;
      case ADDRESS_MODE_DECREMENT_BEFORE:
        output_address = input_address - size;
        load_address = output_address;
        break;
      case ADDRESS_MODE_INCREMENT_AFTER:
        output_address = input_address + size;
        load_address = input_address;
        break;
      default:
        codegen_assert(false);
      case ADDRESS_MODE_INCREMENT_BEFORE:
        output_address = input_address + size;
        load_address = input_address + 4u;
        break;
    }

    uint_fast16_t register_list_mutable = register_list;

    bool success;
    int i;
    while (NextRegister(&register_list_mutable, &i)) {
      success = ArmLoad32LE(memory, load_address,
                            &registers->current.user.gprs.gprs[i]);
      if (!success) {
        break;
      }
      load_address += 4u;
    }

    writeback = writeback && (register_list & (1u << Rn)) == 0u;
    if (writeback) {
      registers->current.user.gprs.gprs[Rn] = output_address;
    }

    if (!success) {
      if (load_usr_mode) {
        ArmLoadCPSR(registers, original_cpsr);
      }

      ArmExceptionDataABT(registers);
      return;
    }

    if (load_spsr) {
      ArmLoadCPSR(registers, registers->current.spsr);
    }

    if ((register_list & (1u << REGISTER_PC)) != 0u ||
        (writeback && Rn == REGISTER_PC)) {
      ArmLoadGPSR(registers, REGISTER_PC, registers->current.user.gprs.pc);
    } else {
      ArmAdvanceProgramCounter(registers);
    }
  } else {
    uint32_t output_address;
    switch (address_mode) {
      case ADDRESS_MODE_DECREMENT_AFTER:
      case ADDRESS_MODE_DECREMENT_BEFORE:
        output_address = input_address - 64u;
        break;
      default:
        codegen_assert(false);
      case ADDRESS_MODE_INCREMENT_AFTER:
      case ADDRESS_MODE_INCREMENT_BEFORE:
        output_address = input_address + 64u;
        break;
    }

    uint32_t value;
    bool success = ArmLoad32LE(memory, input_address, &value);
    if (!success) {
      if (load_usr_mode) {
        ArmLoadCPSR(registers, original_cpsr);
      }

      ArmExceptionDataABT(registers);
      return;
    }

    ArmLoadGPSR(registers, REGISTER_PC, value);

    if (writeback) {
      ArmLoadGPSR(registers, Rn, output_address);
    }
  }

  if (load_usr_mode) {
    ArmLoadCPSR(registers, original_cpsr);
  }
}

void ArmLDMDA(ArmAllRegisters *registers, const Memory *memory,
              ArmRegisterIndex Rn, uint_fast16_t register_list) {
  ArmLDM(registers, memory, Rn, register_list, ADDRESS_MODE_DECREMENT_AFTER,
         /*writeback=*/false);
}

void ArmLDMDAW(ArmAllRegisters *registers, const Memory *memory,
               ArmRegisterIndex Rn, uint_fast16_t register_list) {
  ArmLDM(registers, memory, Rn, register_list, ADDRESS_MODE_DECREMENT_AFTER,
         /*writeback=*/true);
}

void ArmLDMDB(ArmAllRegisters *registers, const Memory *memory,
              ArmRegisterIndex Rn, uint_fast16_t register_list) {
  ArmLDM(registers, memory, Rn, register_list, ADDRESS_MODE_DECREMENT_BEFORE,
         /*writeback=*/false);
}

void ArmLDMDBW(ArmAllRegisters *registers, const Memory *memory,
               ArmRegisterIndex Rn, uint_fast16_t register_list) {
  ArmLDM(registers, memory, Rn, register_list, ADDRESS_MODE_DECREMENT_BEFORE,
         /*writeback=*/true);
}

void ArmLDMIA(ArmAllRegisters *registers, const Memory *memory,
              ArmRegisterIndex Rn, uint_fast16_t register_list) {
  ArmLDM(registers, memory, Rn, register_list, ADDRESS_MODE_INCREMENT_AFTER,
         /*writeback=*/false);
}

void ArmLDMIB(ArmAllRegisters *registers, const Memory *memory,
              ArmRegisterIndex Rn, uint_fast16_t register_list) {
  ArmLDM(registers, memory, Rn, register_list, ADDRESS_MODE_INCREMENT_BEFORE,
         /*writeback=*/false);
}

void ArmLDMIAW(ArmAllRegisters *registers, const Memory *memory,
               ArmRegisterIndex Rn, uint_fast16_t register_list) {
  ArmLDM(registers, memory, Rn, register_list, ADDRESS_MODE_INCREMENT_AFTER,
         /*writeback=*/true);
}

void ArmLDMIBW(ArmAllRegisters *registers, const Memory *memory,
               ArmRegisterIndex Rn, uint_fast16_t register_list) {
  ArmLDM(registers, memory, Rn, register_list, ADDRESS_MODE_INCREMENT_BEFORE,
         /*writeback=*/true);
}

void ArmLDMSDA(ArmAllRegisters *registers, const Memory *memory,
               ArmRegisterIndex Rn, uint_fast16_t register_list) {
  ArmLDMS(registers, memory, Rn, register_list, ADDRESS_MODE_DECREMENT_AFTER,
          /*writeback=*/false);
}

void ArmLDMSDB(ArmAllRegisters *registers, const Memory *memory,
               ArmRegisterIndex Rn, uint_fast16_t register_list) {
  ArmLDMS(registers, memory, Rn, register_list, ADDRESS_MODE_DECREMENT_BEFORE,
          /*writeback=*/false);
}

void ArmLDMSDAW(ArmAllRegisters *registers, const Memory *memory,
                ArmRegisterIndex Rn, uint_fast16_t register_list) {
  ArmLDMS(registers, memory, Rn, register_list, ADDRESS_MODE_DECREMENT_AFTER,
          /*writeback=*/true);
}

void ArmLDMSDBW(ArmAllRegisters *registers, const Memory *memory,
                ArmRegisterIndex Rn, uint_fast16_t register_list) {
  ArmLDMS(registers, memory, Rn, register_list, ADDRESS_MODE_DECREMENT_BEFORE,
          /*writeback=*/true);
}

void ArmLDMSIA(ArmAllRegisters *registers, const Memory *memory,
               ArmRegisterIndex Rn, uint_fast16_t register_list) {
  ArmLDMS(registers, memory, Rn, register_list, ADDRESS_MODE_INCREMENT_AFTER,
          /*writeback=*/false);
}

void ArmLDMSIB(ArmAllRegisters *registers, const Memory *memory,
               ArmRegisterIndex Rn, uint_fast16_t register_list) {
  ArmLDMS(registers, memory, Rn, register_list, ADDRESS_MODE_INCREMENT_BEFORE,
          /*writeback=*/false);
}

void ArmLDMSIAW(ArmAllRegisters *registers, const Memory *memory,
                ArmRegisterIndex Rn, uint_fast16_t register_list) {
  ArmLDMS(registers, memory, Rn, register_list, ADDRESS_MODE_INCREMENT_AFTER,
          /*writeback=*/true);
}

void ArmLDMSIBW(ArmAllRegisters *registers, const Memory *memory,
                ArmRegisterIndex Rn, uint_fast16_t register_list) {
  ArmLDMS(registers, memory, Rn, register_list, ADDRESS_MODE_INCREMENT_BEFORE,
          /*writeback=*/true);
}

static inline void ArmSTM(ArmAllRegisters *registers, Memory *memory,
                          ArmRegisterIndex Rn, uint_fast16_t register_list,
                          ArmAddressMode address_mode, bool writeback) {
  assert(register_list <= UINT16_MAX);

  uint_fast8_t size = PopCount(register_list) * 4u;
  uint32_t input_address = registers->current.user.gprs.gprs[Rn];

  uint32_t output_address, load_address;
  switch (address_mode) {
    case ADDRESS_MODE_DECREMENT_AFTER:
      output_address = input_address - size;
      load_address = output_address + 4u;
      break;
    case ADDRESS_MODE_DECREMENT_BEFORE:
      output_address = input_address - size;
      load_address = output_address;
      break;
    case ADDRESS_MODE_INCREMENT_AFTER:
      output_address = input_address + size;
      load_address = input_address;
      break;
    default:
      codegen_assert(false);
    case ADDRESS_MODE_INCREMENT_BEFORE:
      output_address = input_address + size;
      load_address = input_address + 4u;
      break;
  }

  uint_fast16_t register_list_mutable = register_list;

  bool success = true;
  int i;
  while (NextRegister(&register_list_mutable, &i)) {
    uint32_t value = registers->current.user.gprs.gprs[i];
    // Including R15 in the register list triggers unpredictable behavior
    if (i == REGISTER_R15) {
      codegen_assert(!registers->current.user.cpsr.thumb);
      value += 4u;
    }

    success = ArmStore32LE(memory, load_address, value);
    if (!success) {
      break;
    }
    load_address += 4u;
  }

  if (writeback) {
    registers->current.user.gprs.gprs[Rn] = output_address;
  }

  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }

  if (writeback && Rn == REGISTER_PC) {
    ArmLoadGPSR(registers, REGISTER_PC, registers->current.user.gprs.pc);
  } else {
    ArmAdvanceProgramCounter(registers);
  }
}

static inline void ArmSTMS(ArmAllRegisters *registers, Memory *memory,
                           ArmRegisterIndex Rn, uint_fast16_t register_list,
                           ArmAddressMode address_mode, bool writeback) {
  assert(register_list <= UINT16_MAX);

  bool change_to_usr = registers->current.user.cpsr.mode != MODE_USR ||
                       registers->current.user.cpsr.mode != MODE_SYS;

  ArmProgramStatusRegister original_cpsr = registers->current.user.cpsr;
  if (change_to_usr) {
    ArmProgramStatusRegister temporary_cpsr = registers->current.user.cpsr;
    temporary_cpsr.mode = MODE_USR;
    ArmLoadCPSR(registers, temporary_cpsr);
  }

  uint_fast8_t size = PopCount(register_list) * 4u;
  uint32_t input_address = registers->current.user.gprs.gprs[Rn];

  uint32_t output_address, load_address;
  switch (address_mode) {
    case ADDRESS_MODE_DECREMENT_AFTER:
      output_address = input_address - size;
      load_address = output_address + 4u;
      break;
    case ADDRESS_MODE_DECREMENT_BEFORE:
      output_address = input_address - size;
      load_address = output_address;
      break;
    case ADDRESS_MODE_INCREMENT_AFTER:
      output_address = input_address + size;
      load_address = input_address;
      break;
    default:
      codegen_assert(false);
    case ADDRESS_MODE_INCREMENT_BEFORE:
      output_address = input_address + size;
      load_address = input_address + 4u;
      break;
  }

  uint_fast16_t register_list_mutable = register_list;

  bool success = true;
  int i;
  while (NextRegister(&register_list_mutable, &i)) {
    uint32_t value = registers->current.user.gprs.gprs[i];
    // Including R15 in the register list triggers unpredictable behavior
    if (i == REGISTER_R15) {
      codegen_assert(!registers->current.user.cpsr.thumb);
      value += 4u;
    }

    success = ArmStore32LE(memory, load_address, value);
    if (!success) {
      break;
    }
    load_address += 4u;
  }

  if (writeback) {
    registers->current.user.gprs.gprs[Rn] = output_address;
  }

  if (!success) {
    if (change_to_usr) {
      ArmLoadCPSR(registers, original_cpsr);
    }

    ArmExceptionDataABT(registers);
    return;
  }

  if (writeback && Rn == REGISTER_PC) {
    ArmLoadGPSR(registers, REGISTER_PC, registers->current.user.gprs.pc);
  } else {
    ArmAdvanceProgramCounter(registers);
  }

  if (change_to_usr) {
    ArmLoadCPSR(registers, original_cpsr);
  }
}

void ArmSTMDA(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
              uint_fast16_t register_list) {
  ArmSTM(registers, memory, Rn, register_list, ADDRESS_MODE_DECREMENT_AFTER,
         /*writeback=*/false);
}

void ArmSTMDB(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
              uint_fast16_t register_list) {
  ArmSTM(registers, memory, Rn, register_list, ADDRESS_MODE_DECREMENT_BEFORE,
         /*writeback=*/false);
}

void ArmSTMDAW(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
               uint_fast16_t register_list) {
  ArmSTM(registers, memory, Rn, register_list, ADDRESS_MODE_DECREMENT_AFTER,
         /*writeback=*/true);
}

void ArmSTMDBW(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
               uint_fast16_t register_list) {
  ArmSTM(registers, memory, Rn, register_list, ADDRESS_MODE_DECREMENT_BEFORE,
         /*writeback=*/true);
}

void ArmSTMIA(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
              uint_fast16_t register_list) {
  ArmSTM(registers, memory, Rn, register_list, ADDRESS_MODE_INCREMENT_AFTER,
         /*writeback=*/false);
}

void ArmSTMIB(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
              uint_fast16_t register_list) {
  ArmSTM(registers, memory, Rn, register_list, ADDRESS_MODE_INCREMENT_BEFORE,
         /*writeback=*/false);
}

void ArmSTMIAW(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
               uint_fast16_t register_list) {
  ArmSTM(registers, memory, Rn, register_list, ADDRESS_MODE_INCREMENT_AFTER,
         /*writeback=*/true);
}

void ArmSTMIBW(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
               uint_fast16_t register_list) {
  ArmSTM(registers, memory, Rn, register_list, ADDRESS_MODE_INCREMENT_BEFORE,
         /*writeback=*/true);
}

void ArmSTMSDA(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
               uint_fast16_t register_list) {
  ArmSTMS(registers, memory, Rn, register_list, ADDRESS_MODE_DECREMENT_AFTER,
          /*writeback=*/false);
}

void ArmSTMSDB(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
               uint_fast16_t register_list) {
  ArmSTMS(registers, memory, Rn, register_list, ADDRESS_MODE_DECREMENT_BEFORE,
          /*writeback=*/false);
}

void ArmSTMSDAW(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
                uint_fast16_t register_list) {
  ArmSTMS(registers, memory, Rn, register_list, ADDRESS_MODE_DECREMENT_AFTER,
          /*writeback=*/true);
}

void ArmSTMSDBW(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
                uint_fast16_t register_list) {
  ArmSTMS(registers, memory, Rn, register_list, ADDRESS_MODE_DECREMENT_BEFORE,
          /*writeback=*/true);
}

void ArmSTMSIA(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
               uint_fast16_t register_list) {
  ArmSTMS(registers, memory, Rn, register_list, ADDRESS_MODE_INCREMENT_AFTER,
          /*writeback=*/false);
}

void ArmSTMSIB(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
               uint_fast16_t register_list) {
  ArmSTMS(registers, memory, Rn, register_list, ADDRESS_MODE_INCREMENT_BEFORE,
          /*writeback=*/false);
}

void ArmSTMSIAW(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
                uint_fast16_t register_list) {
  ArmSTMS(registers, memory, Rn, register_list, ADDRESS_MODE_INCREMENT_AFTER,
          /*writeback=*/true);
}

void ArmSTMSIBW(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
                uint_fast16_t register_list) {
  ArmSTMS(registers, memory, Rn, register_list, ADDRESS_MODE_INCREMENT_BEFORE,
          /*writeback=*/true);
}