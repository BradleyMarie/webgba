#include "emulator/cpu/arm7tdmi/instructions/block_data_transfer.h"

#include <assert.h>

#include "emulator/cpu/arm7tdmi/exceptions.h"

static inline bool ArmModeIsUsrOrSys(ArmProgramStatusRegister cpsr) {
  return cpsr.mode == MODE_USR || cpsr.mode == MODE_SYS;
}

static inline uint_fast8_t PopCount(uint_fast16_t value) {
  return __builtin_popcount(value);
}

bool ArmLDMDA(ArmAllRegisters *registers, const Memory *memory,
              ArmRegisterIndex Rn, uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);
  uint32_t base = registers->current.user.gprs.gprs[Rn];
  uint32_t address =
      registers->current.user.gprs.gprs[Rn] - PopCount(register_list) * 4u + 4u;
  bool success = true;
  for (uint_fast8_t i = 0u; i < 16u; i++) {
    if (register_list & (1u << i)) {
      bool success =
          ArmLoad32LE(memory, address, &registers->current.user.gprs.gprs[i]);
      if (!success) {
        ArmExceptionDataABT(registers);
        registers->current.user.gprs.gprs[Rn] = base;
        break;
      }
      address += 4u;
    }
  }
  return success;
}

bool ArmLDMDB(ArmAllRegisters *registers, const Memory *memory,
              ArmRegisterIndex Rn, uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);
  uint32_t base = registers->current.user.gprs.gprs[Rn];
  uint32_t address =
      registers->current.user.gprs.gprs[Rn] - PopCount(register_list) * 4u;
  bool success = true;
  for (uint_fast8_t i = 0u; i < 16u; i++) {
    if (register_list & (1u << i)) {
      bool success =
          ArmLoad32LE(memory, address, &registers->current.user.gprs.gprs[i]);
      if (!success) {
        ArmExceptionDataABT(registers);
        registers->current.user.gprs.gprs[Rn] = base;
        break;
      }
      address += 4u;
    }
  }
  return success;
}

bool ArmLDMDAW(ArmAllRegisters *registers, const Memory *memory,
               ArmRegisterIndex Rn, uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);
  uint_fast8_t size = PopCount(register_list) * 4u;
  uint32_t address = registers->current.user.gprs.gprs[Rn] - size + 4u;
  bool success = true;
  for (uint_fast8_t i = 0u; i < 16u; i++) {
    if (register_list & (1u << i)) {
      bool success =
          ArmLoad32LE(memory, address, &registers->current.user.gprs.gprs[i]);
      if (!success) {
        ArmExceptionDataABT(registers);
        break;
      }
      address += 4u;
    }
  }
  registers->current.user.gprs.gprs[Rn] -= size;
  return success;
}

bool ArmLDMDBW(ArmAllRegisters *registers, const Memory *memory,
               ArmRegisterIndex Rn, uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);
  uint_fast8_t size = PopCount(register_list) * 4u;
  uint32_t address = registers->current.user.gprs.gprs[Rn] - size;
  bool success = true;
  for (uint_fast8_t i = 0u; i < 16u; i++) {
    if (register_list & (1u << i)) {
      bool success =
          ArmLoad32LE(memory, address, &registers->current.user.gprs.gprs[i]);
      if (!success) {
        ArmExceptionDataABT(registers);
        break;
      }
      address += 4u;
    }
  }
  registers->current.user.gprs.gprs[Rn] -= size;
  return success;
}

bool ArmLDMIA(ArmAllRegisters *registers, const Memory *memory,
              ArmRegisterIndex Rn, uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);
  uint32_t base = registers->current.user.gprs.gprs[Rn];
  uint32_t address = registers->current.user.gprs.gprs[Rn];
  bool success = true;
  for (uint_fast8_t i = 0u; i < 16u; i++) {
    if (register_list & (1u << i)) {
      bool success =
          ArmLoad32LE(memory, address, &registers->current.user.gprs.gprs[i]);
      if (!success) {
        ArmExceptionDataABT(registers);
        registers->current.user.gprs.gprs[Rn] = base;
        break;
      }
      address += 4u;
    }
  }
  return success;
}

bool ArmLDMIB(ArmAllRegisters *registers, const Memory *memory,
              ArmRegisterIndex Rn, uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);
  uint32_t base = registers->current.user.gprs.gprs[Rn];
  uint32_t address = registers->current.user.gprs.gprs[Rn];
  bool success = true;
  for (uint_fast8_t i = 0u; i < 16u; i++) {
    if (register_list & (1u << i)) {
      address += 4u;
      bool success =
          ArmLoad32LE(memory, address, &registers->current.user.gprs.gprs[i]);
      if (!success) {
        ArmExceptionDataABT(registers);
        registers->current.user.gprs.gprs[Rn] = base;
        break;
      }
    }
  }
  return success;
}

bool ArmLDMIAW(ArmAllRegisters *registers, const Memory *memory,
               ArmRegisterIndex Rn, uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);
  uint32_t address = registers->current.user.gprs.gprs[Rn];
  bool success = true;
  for (uint_fast8_t i = 0u; i < 16u; i++) {
    if (register_list & (1u << i)) {
      bool success =
          ArmLoad32LE(memory, address, &registers->current.user.gprs.gprs[i]);
      if (!success) {
        ArmExceptionDataABT(registers);
        break;
      }
      address += 4u;
    }
  }
  registers->current.user.gprs.gprs[Rn] = address;
  return success;
}

bool ArmLDMIBW(ArmAllRegisters *registers, const Memory *memory,
               ArmRegisterIndex Rn, uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);
  uint32_t address = registers->current.user.gprs.gprs[Rn];
  bool success = true;
  for (uint_fast8_t i = 0u; i < 16u; i++) {
    if (register_list & (1u << i)) {
      address += 4u;
      bool success =
          ArmLoad32LE(memory, address, &registers->current.user.gprs.gprs[i]);
      if (!success) {
        ArmExceptionDataABT(registers);
        break;
      }
    }
  }
  registers->current.user.gprs.gprs[Rn] = address;
  return success;
}

bool ArmLDMSDA(ArmAllRegisters *registers, const Memory *memory,
               ArmRegisterIndex Rn, uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);
  bool loads_pc = register_list & (1u << REGISTER_R15);
  bool is_usr_or_sys = ArmModeIsUsrOrSys(registers->current.user.cpsr);
  bool modify_banked_registers = !loads_pc && !is_usr_or_sys;

  uint32_t base = registers->current.user.gprs.gprs[Rn];
  uint32_t address =
      registers->current.user.gprs.gprs[Rn] - PopCount(register_list) * 4u + 4u;

  ArmProgramStatusRegister old_status;
  if (modify_banked_registers) {
    old_status = registers->current.user.cpsr;
    ArmProgramStatusRegister temporary_status = old_status;
    temporary_status.mode = MODE_USR;
    ArmLoadCPSR(registers, temporary_status);
  }

  bool success = true;
  for (uint_fast8_t i = 0u; i < 16u; i++) {
    if (register_list & (1u << i)) {
      bool success =
          ArmLoad32LE(memory, address, &registers->current.user.gprs.gprs[i]);
      if (!success) {
        registers->current.user.gprs.gprs[Rn] = base;
        break;
      }
      address += 4u;
    }
  }

  if (modify_banked_registers) {
    ArmLoadCPSR(registers, old_status);
  } else if (loads_pc && !is_usr_or_sys) {
    ArmLoadCPSR(registers, registers->current.spsr);
  }

  if (!success) {
    ArmExceptionDataABT(registers);
  }

  return success;
}

bool ArmLDMSDB(ArmAllRegisters *registers, const Memory *memory,
               ArmRegisterIndex Rn, uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);
  bool loads_pc = register_list & (1u << REGISTER_R15);
  bool is_usr_or_sys = ArmModeIsUsrOrSys(registers->current.user.cpsr);
  bool modify_banked_registers = !loads_pc && !is_usr_or_sys;

  uint32_t base = registers->current.user.gprs.gprs[Rn];
  uint32_t address =
      registers->current.user.gprs.gprs[Rn] - PopCount(register_list) * 4u;

  ArmProgramStatusRegister old_status;
  if (modify_banked_registers) {
    old_status = registers->current.user.cpsr;
    ArmProgramStatusRegister temporary_status = old_status;
    temporary_status.mode = MODE_USR;
    ArmLoadCPSR(registers, temporary_status);
  }

  bool success = true;
  for (uint_fast8_t i = 0u; i < 16u; i++) {
    if (register_list & (1u << i)) {
      bool success =
          ArmLoad32LE(memory, address, &registers->current.user.gprs.gprs[i]);
      if (!success) {
        registers->current.user.gprs.gprs[Rn] = base;
        break;
      }
      address += 4u;
    }
  }

  if (modify_banked_registers) {
    ArmLoadCPSR(registers, old_status);
  } else if (loads_pc && !is_usr_or_sys) {
    ArmLoadCPSR(registers, registers->current.spsr);
  }

  if (!success) {
    ArmExceptionDataABT(registers);
  }

  return success;
}

bool ArmLDMSDAW(ArmAllRegisters *registers, const Memory *memory,
                ArmRegisterIndex Rn, uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);
  bool loads_pc = register_list & (1u << REGISTER_R15);
  bool is_usr_or_sys = ArmModeIsUsrOrSys(registers->current.user.cpsr);
  bool modify_banked_registers = !loads_pc && !is_usr_or_sys;

  uint_fast8_t size = PopCount(register_list) * 4u;
  uint32_t address = registers->current.user.gprs.gprs[Rn] - size + 4u;

  ArmProgramStatusRegister old_status;
  if (modify_banked_registers) {
    old_status = registers->current.user.cpsr;
    ArmProgramStatusRegister temporary_status = old_status;
    temporary_status.mode = MODE_USR;
    ArmLoadCPSR(registers, temporary_status);
  }

  bool success = true;
  for (uint_fast8_t i = 0u; i < 16u; i++) {
    if (register_list & (1u << i)) {
      bool success =
          ArmLoad32LE(memory, address, &registers->current.user.gprs.gprs[i]);
      if (!success) {
        break;
      }
      address += 4u;
    }
  }
  registers->current.user.gprs.gprs[Rn] -= size;

  if (modify_banked_registers) {
    ArmLoadCPSR(registers, old_status);
  } else if (loads_pc && !is_usr_or_sys) {
    ArmLoadCPSR(registers, registers->current.spsr);
  }

  if (!success) {
    ArmExceptionDataABT(registers);
  }

  return success;
}

bool ArmLDMSDBW(ArmAllRegisters *registers, const Memory *memory,
                ArmRegisterIndex Rn, uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);
  bool loads_pc = register_list & (1u << REGISTER_R15);
  bool is_usr_or_sys = ArmModeIsUsrOrSys(registers->current.user.cpsr);
  bool modify_banked_registers = !loads_pc && !is_usr_or_sys;

  uint_fast8_t size = PopCount(register_list) * 4u;
  uint32_t address = registers->current.user.gprs.gprs[Rn] - size;

  ArmProgramStatusRegister old_status;
  if (modify_banked_registers) {
    old_status = registers->current.user.cpsr;
    ArmProgramStatusRegister temporary_status = old_status;
    temporary_status.mode = MODE_USR;
    ArmLoadCPSR(registers, temporary_status);
  }

  bool success = true;
  for (uint_fast8_t i = 0u; i < 16u; i++) {
    if (register_list & (1u << i)) {
      bool success =
          ArmLoad32LE(memory, address, &registers->current.user.gprs.gprs[i]);
      if (!success) {
        break;
      }
      address += 4u;
    }
  }
  registers->current.user.gprs.gprs[Rn] -= size;

  if (modify_banked_registers) {
    ArmLoadCPSR(registers, old_status);
  } else if (loads_pc && !is_usr_or_sys) {
    ArmLoadCPSR(registers, registers->current.spsr);
  }

  if (!success) {
    ArmExceptionDataABT(registers);
  }

  return success;
}

bool ArmLDMSIA(ArmAllRegisters *registers, const Memory *memory,
               ArmRegisterIndex Rn, uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);
  bool loads_pc = register_list & (1u << REGISTER_R15);
  bool is_usr_or_sys = ArmModeIsUsrOrSys(registers->current.user.cpsr);
  bool modify_banked_registers = !loads_pc && !is_usr_or_sys;

  uint32_t base = registers->current.user.gprs.gprs[Rn];
  uint32_t address = registers->current.user.gprs.gprs[Rn];

  ArmProgramStatusRegister old_status;
  if (modify_banked_registers) {
    old_status = registers->current.user.cpsr;
    ArmProgramStatusRegister temporary_status = old_status;
    temporary_status.mode = MODE_USR;
    ArmLoadCPSR(registers, temporary_status);
  }

  bool success = true;
  for (uint_fast8_t i = 0u; i < 16u; i++) {
    if (register_list & (1u << i)) {
      bool success =
          ArmLoad32LE(memory, address, &registers->current.user.gprs.gprs[i]);
      if (!success) {
        registers->current.user.gprs.gprs[Rn] = base;
        break;
      }
      address += 4u;
    }
  }

  if (modify_banked_registers) {
    ArmLoadCPSR(registers, old_status);
  } else if (loads_pc && !is_usr_or_sys) {
    ArmLoadCPSR(registers, registers->current.spsr);
  }

  if (!success) {
    ArmExceptionDataABT(registers);
  }

  return success;
}

bool ArmLDMSIB(ArmAllRegisters *registers, const Memory *memory,
               ArmRegisterIndex Rn, uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);
  bool loads_pc = register_list & (1u << REGISTER_R15);
  bool is_usr_or_sys = ArmModeIsUsrOrSys(registers->current.user.cpsr);
  bool modify_banked_registers = !loads_pc && !is_usr_or_sys;

  uint32_t base = registers->current.user.gprs.gprs[Rn];
  uint32_t address = registers->current.user.gprs.gprs[Rn];

  ArmProgramStatusRegister old_status;
  if (modify_banked_registers) {
    old_status = registers->current.user.cpsr;
    ArmProgramStatusRegister temporary_status = old_status;
    temporary_status.mode = MODE_USR;
    ArmLoadCPSR(registers, temporary_status);
  }

  bool success = true;
  for (uint_fast8_t i = 0u; i < 16u; i++) {
    if (register_list & (1u << i)) {
      address += 4u;
      bool success =
          ArmLoad32LE(memory, address, &registers->current.user.gprs.gprs[i]);
      if (!success) {
        registers->current.user.gprs.gprs[Rn] = base;
        break;
      }
    }
  }

  if (modify_banked_registers) {
    ArmLoadCPSR(registers, old_status);
  } else if (loads_pc && !is_usr_or_sys) {
    ArmLoadCPSR(registers, registers->current.spsr);
  }

  if (!success) {
    ArmExceptionDataABT(registers);
  }

  return success;
}

bool ArmLDMSIAW(ArmAllRegisters *registers, const Memory *memory,
                ArmRegisterIndex Rn, uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);
  bool loads_pc = register_list & (1u << REGISTER_R15);
  bool is_usr_or_sys = ArmModeIsUsrOrSys(registers->current.user.cpsr);
  bool modify_banked_registers = !loads_pc && !is_usr_or_sys;

  uint32_t address = registers->current.user.gprs.gprs[Rn];

  ArmProgramStatusRegister old_status;
  if (modify_banked_registers) {
    old_status = registers->current.user.cpsr;
    ArmProgramStatusRegister temporary_status = old_status;
    temporary_status.mode = MODE_USR;
    ArmLoadCPSR(registers, temporary_status);
  }

  bool success = true;
  for (uint_fast8_t i = 0u; i < 16u; i++) {
    if (register_list & (1u << i)) {
      bool success =
          ArmLoad32LE(memory, address, &registers->current.user.gprs.gprs[i]);
      if (!success) {
        break;
      }
      address += 4u;
    }
  }
  registers->current.user.gprs.gprs[Rn] = address;

  if (modify_banked_registers) {
    ArmLoadCPSR(registers, old_status);
  } else if (loads_pc && !is_usr_or_sys) {
    ArmLoadCPSR(registers, registers->current.spsr);
  }

  if (!success) {
    ArmExceptionDataABT(registers);
  }

  return success;
}

bool ArmLDMSIBW(ArmAllRegisters *registers, const Memory *memory,
                ArmRegisterIndex Rn, uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);
  bool loads_pc = register_list & (1u << REGISTER_R15);
  bool is_usr_or_sys = ArmModeIsUsrOrSys(registers->current.user.cpsr);
  bool modify_banked_registers = !loads_pc && !is_usr_or_sys;

  uint32_t address = registers->current.user.gprs.gprs[Rn];

  ArmProgramStatusRegister old_status;
  if (modify_banked_registers) {
    old_status = registers->current.user.cpsr;
    ArmProgramStatusRegister temporary_status = old_status;
    temporary_status.mode = MODE_USR;
    ArmLoadCPSR(registers, temporary_status);
  }

  bool success = true;
  for (uint_fast8_t i = 0u; i < 16u; i++) {
    if (register_list & (1u << i)) {
      address += 4u;
      bool success =
          ArmLoad32LE(memory, address, &registers->current.user.gprs.gprs[i]);
      if (!success) {
        break;
      }
    }
  }
  registers->current.user.gprs.gprs[Rn] = address;

  if (modify_banked_registers) {
    ArmLoadCPSR(registers, old_status);
  } else if (loads_pc && !is_usr_or_sys) {
    ArmLoadCPSR(registers, registers->current.spsr);
  }

  if (!success) {
    ArmExceptionDataABT(registers);
  }

  return success;
}

bool ArmSTMDA(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
              uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);
  uint32_t base = registers->current.user.gprs.gprs[Rn];
  uint32_t address =
      registers->current.user.gprs.gprs[Rn] - PopCount(register_list) * 4u + 4u;
  bool success = true;
  for (uint_fast8_t i = 0u; i < 16u; i++) {
    if (register_list & (1u << i)) {
      bool success =
          ArmStore32LE(memory, address, registers->current.user.gprs.gprs[i]);
      if (!success) {
        ArmExceptionDataABT(registers);
        registers->current.user.gprs.gprs[Rn] = base;
        break;
      }
      address += 4u;
    }
  }
  return success;
}

bool ArmSTMDB(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
              uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);
  uint32_t base = registers->current.user.gprs.gprs[Rn];
  uint32_t address =
      registers->current.user.gprs.gprs[Rn] - PopCount(register_list) * 4u;
  bool success = true;
  for (uint_fast8_t i = 0u; i < 16u; i++) {
    if (register_list & (1u << i)) {
      bool success =
          ArmStore32LE(memory, address, registers->current.user.gprs.gprs[i]);
      if (!success) {
        ArmExceptionDataABT(registers);
        registers->current.user.gprs.gprs[Rn] = base;
        break;
      }
      address += 4u;
    }
  }
  return success;
}

bool ArmSTMDAW(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
               uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);
  uint_fast8_t size = PopCount(register_list) * 4u;
  uint32_t address = registers->current.user.gprs.gprs[Rn] - size + 4u;
  bool success = true;
  for (uint_fast8_t i = 0u; i < 16u; i++) {
    if (register_list & (1u << i)) {
      bool success =
          ArmStore32LE(memory, address, registers->current.user.gprs.gprs[i]);
      if (!success) {
        ArmExceptionDataABT(registers);
        break;
      }
      address += 4u;
    }
  }
  registers->current.user.gprs.gprs[Rn] -= size;
  return success;
}

bool ArmSTMDBW(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
               uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);
  uint_fast8_t size = PopCount(register_list) * 4u;
  uint32_t address = registers->current.user.gprs.gprs[Rn] - size;
  bool success = true;
  for (uint_fast8_t i = 0u; i < 16u; i++) {
    if (register_list & (1u << i)) {
      bool success =
          ArmStore32LE(memory, address, registers->current.user.gprs.gprs[i]);
      if (!success) {
        ArmExceptionDataABT(registers);
        break;
      }
      address += 4u;
    }
  }
  registers->current.user.gprs.gprs[Rn] -= size;
  return success;
}

bool ArmSTMIA(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
              uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);
  uint32_t base = registers->current.user.gprs.gprs[Rn];
  uint32_t address = registers->current.user.gprs.gprs[Rn];
  bool success = true;
  for (uint_fast8_t i = 0u; i < 16u; i++) {
    if (register_list & (1u << i)) {
      bool success =
          ArmStore32LE(memory, address, registers->current.user.gprs.gprs[i]);
      if (!success) {
        ArmExceptionDataABT(registers);
        registers->current.user.gprs.gprs[Rn] = base;
        break;
      }
      address += 4u;
    }
  }
  return success;
}

bool ArmSTMIB(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
              uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);
  uint32_t base = registers->current.user.gprs.gprs[Rn];
  uint32_t address = registers->current.user.gprs.gprs[Rn];
  bool success = true;
  for (uint_fast8_t i = 0u; i < 16u; i++) {
    if (register_list & (1u << i)) {
      address += 4u;
      bool success =
          ArmStore32LE(memory, address, registers->current.user.gprs.gprs[i]);
      if (!success) {
        ArmExceptionDataABT(registers);
        registers->current.user.gprs.gprs[Rn] = base;
        break;
      }
    }
  }
  return success;
}

bool ArmSTMIAW(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
               uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);
  uint32_t address = registers->current.user.gprs.gprs[Rn];
  bool success = true;
  for (uint_fast8_t i = 0u; i < 16u; i++) {
    if (register_list & (1u << i)) {
      bool success =
          ArmStore32LE(memory, address, registers->current.user.gprs.gprs[i]);
      if (!success) {
        ArmExceptionDataABT(registers);
        break;
      }
      address += 4u;
    }
  }
  registers->current.user.gprs.gprs[Rn] = address;
  return success;
}

bool ArmSTMIBW(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
               uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);
  uint32_t address = registers->current.user.gprs.gprs[Rn];
  bool success = true;
  for (uint_fast8_t i = 0u; i < 16u; i++) {
    if (register_list & (1u << i)) {
      address += 4u;
      bool success =
          ArmStore32LE(memory, address, registers->current.user.gprs.gprs[i]);
      if (!success) {
        ArmExceptionDataABT(registers);
        break;
      }
    }
  }
  registers->current.user.gprs.gprs[Rn] = address;
  return success;
}

bool ArmSTMSDA(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
               uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);
  bool is_usr_or_sys = ArmModeIsUsrOrSys(registers->current.user.cpsr);
  bool modify_banked_registers = !is_usr_or_sys;

  uint32_t base = registers->current.user.gprs.gprs[Rn];
  uint32_t address =
      registers->current.user.gprs.gprs[Rn] - PopCount(register_list) * 4u + 4u;

  ArmProgramStatusRegister old_status;
  if (modify_banked_registers) {
    old_status = registers->current.user.cpsr;
    ArmProgramStatusRegister temporary_status = old_status;
    temporary_status.mode = MODE_USR;
    ArmLoadCPSR(registers, temporary_status);
  }

  bool success = true;
  for (uint_fast8_t i = 0u; i < 16u; i++) {
    if (register_list & (1u << i)) {
      bool success =
          ArmStore32LE(memory, address, registers->current.user.gprs.gprs[i]);
      if (!success) {
        registers->current.user.gprs.gprs[Rn] = base;
        break;
      }
      address += 4u;
    }
  }

  if (modify_banked_registers) {
    ArmLoadCPSR(registers, old_status);
  }

  if (!success) {
    ArmExceptionDataABT(registers);
  }

  return success;
}

bool ArmSTMSDB(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
               uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);
  bool is_usr_or_sys = ArmModeIsUsrOrSys(registers->current.user.cpsr);
  bool modify_banked_registers = !is_usr_or_sys;

  uint32_t base = registers->current.user.gprs.gprs[Rn];
  uint32_t address =
      registers->current.user.gprs.gprs[Rn] - PopCount(register_list) * 4u;

  ArmProgramStatusRegister old_status;
  if (modify_banked_registers) {
    old_status = registers->current.user.cpsr;
    ArmProgramStatusRegister temporary_status = old_status;
    temporary_status.mode = MODE_USR;
    ArmLoadCPSR(registers, temporary_status);
  }

  bool success = true;
  for (uint_fast8_t i = 0u; i < 16u; i++) {
    if (register_list & (1u << i)) {
      bool success =
          ArmStore32LE(memory, address, registers->current.user.gprs.gprs[i]);
      if (!success) {
        ArmExceptionDataABT(registers);
        registers->current.user.gprs.gprs[Rn] = base;
        break;
      }
      address += 4u;
    }
  }

  if (modify_banked_registers) {
    ArmLoadCPSR(registers, old_status);
  }

  if (!success) {
    ArmExceptionDataABT(registers);
  }

  return success;
}

bool ArmSTMSDAW(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
                uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);
  bool is_usr_or_sys = ArmModeIsUsrOrSys(registers->current.user.cpsr);
  bool modify_banked_registers = !is_usr_or_sys;

  uint_fast8_t size = PopCount(register_list) * 4u;
  uint32_t address = registers->current.user.gprs.gprs[Rn] - size + 4u;

  ArmProgramStatusRegister old_status;
  if (modify_banked_registers) {
    old_status = registers->current.user.cpsr;
    ArmProgramStatusRegister temporary_status = old_status;
    temporary_status.mode = MODE_USR;
    ArmLoadCPSR(registers, temporary_status);
  }

  bool success = true;
  for (uint_fast8_t i = 0u; i < 16u; i++) {
    if (register_list & (1u << i)) {
      bool success =
          ArmStore32LE(memory, address, registers->current.user.gprs.gprs[i]);
      if (!success) {
        ArmExceptionDataABT(registers);
        break;
      }
      address += 4u;
    }
  }
  registers->current.user.gprs.gprs[Rn] -= size;

  if (modify_banked_registers) {
    ArmLoadCPSR(registers, old_status);
  }

  if (!success) {
    ArmExceptionDataABT(registers);
  }

  return success;
}

bool ArmSTMSDBW(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
                uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);
  bool is_usr_or_sys = ArmModeIsUsrOrSys(registers->current.user.cpsr);
  bool modify_banked_registers = !is_usr_or_sys;

  uint_fast8_t size = PopCount(register_list) * 4u;
  uint32_t address = registers->current.user.gprs.gprs[Rn] - size;

  ArmProgramStatusRegister old_status;
  if (modify_banked_registers) {
    old_status = registers->current.user.cpsr;
    ArmProgramStatusRegister temporary_status = old_status;
    temporary_status.mode = MODE_USR;
    ArmLoadCPSR(registers, temporary_status);
  }

  bool success = true;
  for (uint_fast8_t i = 0u; i < 16u; i++) {
    if (register_list & (1u << i)) {
      bool success =
          ArmStore32LE(memory, address, registers->current.user.gprs.gprs[i]);
      if (!success) {
        ArmExceptionDataABT(registers);
        break;
      }
      address += 4u;
    }
  }
  registers->current.user.gprs.gprs[Rn] -= size;

  if (modify_banked_registers) {
    ArmLoadCPSR(registers, old_status);
  }

  if (!success) {
    ArmExceptionDataABT(registers);
  }

  return success;
}

bool ArmSTMSIA(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
               uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);
  bool is_usr_or_sys = ArmModeIsUsrOrSys(registers->current.user.cpsr);
  bool modify_banked_registers = !is_usr_or_sys;

  uint32_t base = registers->current.user.gprs.gprs[Rn];
  uint32_t address = registers->current.user.gprs.gprs[Rn];

  ArmProgramStatusRegister old_status;
  if (modify_banked_registers) {
    old_status = registers->current.user.cpsr;
    ArmProgramStatusRegister temporary_status = old_status;
    temporary_status.mode = MODE_USR;
    ArmLoadCPSR(registers, temporary_status);
  }

  bool success = true;
  for (uint_fast8_t i = 0u; i < 16u; i++) {
    if (register_list & (1u << i)) {
      bool success =
          ArmStore32LE(memory, address, registers->current.user.gprs.gprs[i]);
      if (!success) {
        ArmExceptionDataABT(registers);
        registers->current.user.gprs.gprs[Rn] = base;
        break;
      }
      address += 4u;
    }
  }

  if (modify_banked_registers) {
    ArmLoadCPSR(registers, old_status);
  }

  if (!success) {
    ArmExceptionDataABT(registers);
  }

  return success;
}

bool ArmSTMSIB(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
               uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);
  bool is_usr_or_sys = ArmModeIsUsrOrSys(registers->current.user.cpsr);
  bool modify_banked_registers = !is_usr_or_sys;

  uint32_t base = registers->current.user.gprs.gprs[Rn];
  uint32_t address = registers->current.user.gprs.gprs[Rn];

  ArmProgramStatusRegister old_status;
  if (modify_banked_registers) {
    old_status = registers->current.user.cpsr;
    ArmProgramStatusRegister temporary_status = old_status;
    temporary_status.mode = MODE_USR;
    ArmLoadCPSR(registers, temporary_status);
  }

  bool success = true;
  for (uint_fast8_t i = 0u; i < 16u; i++) {
    if (register_list & (1u << i)) {
      address += 4u;
      bool success =
          ArmStore32LE(memory, address, registers->current.user.gprs.gprs[i]);
      if (!success) {
        ArmExceptionDataABT(registers);
        registers->current.user.gprs.gprs[Rn] = base;
        break;
      }
    }
  }

  if (modify_banked_registers) {
    ArmLoadCPSR(registers, old_status);
  }

  if (!success) {
    ArmExceptionDataABT(registers);
  }

  return success;
}

bool ArmSTMSIAW(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
                uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);
  bool is_usr_or_sys = ArmModeIsUsrOrSys(registers->current.user.cpsr);
  bool modify_banked_registers = !is_usr_or_sys;

  uint32_t address = registers->current.user.gprs.gprs[Rn];

  ArmProgramStatusRegister old_status;
  if (modify_banked_registers) {
    old_status = registers->current.user.cpsr;
    ArmProgramStatusRegister temporary_status = old_status;
    temporary_status.mode = MODE_USR;
    ArmLoadCPSR(registers, temporary_status);
  }

  bool success = true;
  for (uint_fast8_t i = 0u; i < 16u; i++) {
    if (register_list & (1u << i)) {
      bool success =
          ArmStore32LE(memory, address, registers->current.user.gprs.gprs[i]);
      if (!success) {
        ArmExceptionDataABT(registers);
        break;
      }
      address += 4u;
    }
  }
  registers->current.user.gprs.gprs[Rn] = address;

  if (modify_banked_registers) {
    ArmLoadCPSR(registers, old_status);
  }

  if (!success) {
    ArmExceptionDataABT(registers);
  }

  return success;
}

bool ArmSTMSIBW(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
                uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);
  bool is_usr_or_sys = ArmModeIsUsrOrSys(registers->current.user.cpsr);
  bool modify_banked_registers = !is_usr_or_sys;

  uint32_t address = registers->current.user.gprs.gprs[Rn];

  ArmProgramStatusRegister old_status;
  if (modify_banked_registers) {
    old_status = registers->current.user.cpsr;
    ArmProgramStatusRegister temporary_status = old_status;
    temporary_status.mode = MODE_USR;
    ArmLoadCPSR(registers, temporary_status);
  }

  bool success = true;
  for (uint_fast8_t i = 0u; i < 16u; i++) {
    if (register_list & (1u << i)) {
      address += 4u;
      bool success =
          ArmStore32LE(memory, address, registers->current.user.gprs.gprs[i]);
      if (!success) {
        ArmExceptionDataABT(registers);
        break;
      }
    }
  }
  registers->current.user.gprs.gprs[Rn] = address;

  if (modify_banked_registers) {
    ArmLoadCPSR(registers, old_status);
  }

  if (!success) {
    ArmExceptionDataABT(registers);
  }

  return success;
}