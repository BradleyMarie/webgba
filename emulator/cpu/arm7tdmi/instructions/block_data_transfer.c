#include "emulator/cpu/arm7tdmi/instructions/block_data_transfer.h"

#include <assert.h>

#include "emulator/cpu/arm7tdmi/exceptions.h"

static inline bool ArmModeIsUsrOrSys(ArmProgramStatusRegister cpsr) {
  return cpsr.mode == MODE_USR || cpsr.mode == MODE_SYS;
}

static inline uint_fast8_t PopCount(uint_fast16_t value) {
  return __builtin_popcount(value);
}

static inline bool LoadUpdatesPCNoWriteback(uint_fast16_t register_list) {
  return register_list & (1u << REGISTER_PC);
}

static inline bool LoadUpdatesPC(uint_fast16_t register_list,
                                 ArmRegisterIndex Rn) {
  return register_list & (1u << REGISTER_PC) || Rn == REGISTER_PC;
}

static inline bool StoreUpdatesPC(ArmRegisterIndex Rn) {
  return Rn == REGISTER_PC;
}

static inline bool NextRegister(uint_fast16_t *register_list, int *index) {
  if (*register_list == 0u) {
    return false;
  }

  *index = __builtin_ctz(*register_list);
  *register_list ^= 1u << *index;

  return true;
}

void ArmLDMDA(ArmAllRegisters *registers, const Memory *memory,
              ArmRegisterIndex Rn, uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);

  bool updates_pc = LoadUpdatesPCNoWriteback(register_list);

  uint32_t address =
      registers->current.user.gprs.gprs[Rn] - PopCount(register_list) * 4u + 4u;

  int i;
  while (NextRegister(&register_list, &i)) {
    bool success =
        ArmLoad32LE(memory, address, &registers->current.user.gprs.gprs[i]);
    if (!success) {
      ArmExceptionDataABT(registers);
      return;
    }
    address += 4u;
  }

  if (updates_pc) {
    ArmLoadGPSR(registers, REGISTER_PC, registers->current.user.gprs.pc);
  }
}

void ArmLDMDB(ArmAllRegisters *registers, const Memory *memory,
              ArmRegisterIndex Rn, uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);

  bool updates_pc = LoadUpdatesPCNoWriteback(register_list);

  uint32_t address =
      registers->current.user.gprs.gprs[Rn] - PopCount(register_list) * 4u;

  int i;
  while (NextRegister(&register_list, &i)) {
    bool success =
        ArmLoad32LE(memory, address, &registers->current.user.gprs.gprs[i]);
    if (!success) {
      ArmExceptionDataABT(registers);
      return;
    }
    address += 4u;
  }

  if (updates_pc) {
    ArmLoadGPSR(registers, REGISTER_PC, registers->current.user.gprs.pc);
  }
}

void ArmLDMDAW(ArmAllRegisters *registers, const Memory *memory,
               ArmRegisterIndex Rn, uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);

  bool updates_pc = LoadUpdatesPC(register_list, Rn);

  // Including the writeback register in the register list for a load store
  // multiple causes undefined behavior in ARMv4. The ARM7TDMI handles this by
  // ignoring the write to the writeback register.
  bool skip_writeback = (1u << Rn) & register_list;

  uint_fast8_t size = PopCount(register_list) * 4u;
  uint32_t address = registers->current.user.gprs.gprs[Rn] - size + 4u;

  bool success = true;
  int i;
  while (NextRegister(&register_list, &i)) {
    success =
        ArmLoad32LE(memory, address, &registers->current.user.gprs.gprs[i]);
    if (!success) {
      break;
    }
    address += 4u;
  }

  if (!skip_writeback) {
    registers->current.user.gprs.gprs[Rn] -= size;
  }

  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }

  if (updates_pc) {
    ArmLoadGPSR(registers, REGISTER_PC, registers->current.user.gprs.pc);
  }
}

void ArmLDMDBW(ArmAllRegisters *registers, const Memory *memory,
               ArmRegisterIndex Rn, uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);

  bool updates_pc = LoadUpdatesPC(register_list, Rn);

  // Including the writeback register in the register list for a load store
  // multiple causes undefined behavior in ARMv4. The ARM7TDMI handles this by
  // ignoring the write to the writeback register.
  bool skip_writeback = (1u << Rn) & register_list;

  uint_fast8_t size = PopCount(register_list) * 4u;
  uint32_t address = registers->current.user.gprs.gprs[Rn] - size;

  bool success = true;
  int i;
  while (NextRegister(&register_list, &i)) {
    success =
        ArmLoad32LE(memory, address, &registers->current.user.gprs.gprs[i]);
    if (!success) {
      break;
    }
    address += 4u;
  }

  if (!skip_writeback) {
    registers->current.user.gprs.gprs[Rn] -= size;
  }

  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }

  if (updates_pc) {
    ArmLoadGPSR(registers, REGISTER_PC, registers->current.user.gprs.pc);
  }
}

void ArmLDMIA(ArmAllRegisters *registers, const Memory *memory,
              ArmRegisterIndex Rn, uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);

  bool updates_pc = LoadUpdatesPCNoWriteback(register_list);

  uint32_t address = registers->current.user.gprs.gprs[Rn];

  int i;
  while (NextRegister(&register_list, &i)) {
    bool success =
        ArmLoad32LE(memory, address, &registers->current.user.gprs.gprs[i]);
    if (!success) {
      ArmExceptionDataABT(registers);
      return;
    }
    address += 4u;
  }

  if (updates_pc) {
    ArmLoadGPSR(registers, REGISTER_PC, registers->current.user.gprs.pc);
  }
}

void ArmLDMIB(ArmAllRegisters *registers, const Memory *memory,
              ArmRegisterIndex Rn, uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);

  bool updates_pc = LoadUpdatesPCNoWriteback(register_list);

  uint32_t address = registers->current.user.gprs.gprs[Rn];

  int i;
  while (NextRegister(&register_list, &i)) {
    address += 4u;
    bool success =
        ArmLoad32LE(memory, address, &registers->current.user.gprs.gprs[i]);
    if (!success) {
      ArmExceptionDataABT(registers);
      return;
    }
  }

  if (updates_pc) {
    ArmLoadGPSR(registers, REGISTER_PC, registers->current.user.gprs.pc);
  }
}

void ArmLDMIAW(ArmAllRegisters *registers, const Memory *memory,
               ArmRegisterIndex Rn, uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);

  bool updates_pc = LoadUpdatesPC(register_list, Rn);

  // Including the writeback register in the register list for a load store
  // multiple causes undefined behavior in ARMv4. The ARM7TDMI handles this by
  // ignoring the write to the writeback register.
  bool skip_writeback = (1u << Rn) & register_list;

  uint_fast8_t size = PopCount(register_list) * 4u;
  uint32_t address = registers->current.user.gprs.gprs[Rn];

  bool success = true;
  int i;
  while (NextRegister(&register_list, &i)) {
    success =
        ArmLoad32LE(memory, address, &registers->current.user.gprs.gprs[i]);
    if (!success) {
      break;
    }
    address += 4u;
  }

  if (!skip_writeback) {
    registers->current.user.gprs.gprs[Rn] += size;
  }

  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }

  if (updates_pc) {
    ArmLoadGPSR(registers, REGISTER_PC, registers->current.user.gprs.pc);
  }
}

void ArmLDMIBW(ArmAllRegisters *registers, const Memory *memory,
               ArmRegisterIndex Rn, uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);

  bool updates_pc = LoadUpdatesPC(register_list, Rn);

  // Including the writeback register in the register list for a load store
  // multiple causes undefined behavior in ARMv4. The ARM7TDMI handles this by
  // ignoring the write to the writeback register.
  bool skip_writeback = (1u << Rn) & register_list;

  uint_fast8_t size = PopCount(register_list) * 4u;
  uint32_t address = registers->current.user.gprs.gprs[Rn];

  bool success = true;
  int i;
  while (NextRegister(&register_list, &i)) {
    address += 4u;
    success =
        ArmLoad32LE(memory, address, &registers->current.user.gprs.gprs[i]);
    if (!success) {
      break;
    }
  }

  if (!skip_writeback) {
    registers->current.user.gprs.gprs[Rn] += size;
  }

  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }

  if (updates_pc) {
    ArmLoadGPSR(registers, REGISTER_PC, registers->current.user.gprs.pc);
  }
}

void ArmLDMSDA(ArmAllRegisters *registers, const Memory *memory,
               ArmRegisterIndex Rn, uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);

  bool updates_pc = LoadUpdatesPCNoWriteback(register_list);

  bool is_usr_or_sys = ArmModeIsUsrOrSys(registers->current.user.cpsr);
  bool modify_banked_registers = !updates_pc && !is_usr_or_sys;

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
  int i;
  while (NextRegister(&register_list, &i)) {
    success =
        ArmLoad32LE(memory, address, &registers->current.user.gprs.gprs[i]);
    if (!success) {
      break;
    }
    address += 4u;
  }

  if (modify_banked_registers) {
    ArmLoadCPSR(registers, old_status);
  } else if (updates_pc && !is_usr_or_sys) {
    ArmLoadCPSR(registers, registers->current.spsr);
  }

  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }

  if (updates_pc) {
    ArmLoadGPSR(registers, REGISTER_PC, registers->current.user.gprs.pc);
  }
}

void ArmLDMSDB(ArmAllRegisters *registers, const Memory *memory,
               ArmRegisterIndex Rn, uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);

  bool updates_pc = LoadUpdatesPCNoWriteback(register_list);

  bool is_usr_or_sys = ArmModeIsUsrOrSys(registers->current.user.cpsr);
  bool modify_banked_registers = !updates_pc && !is_usr_or_sys;

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
  int i;
  while (NextRegister(&register_list, &i)) {
    success =
        ArmLoad32LE(memory, address, &registers->current.user.gprs.gprs[i]);
    if (!success) {
      break;
    }
    address += 4u;
  }

  if (modify_banked_registers) {
    ArmLoadCPSR(registers, old_status);
  } else if (updates_pc && !is_usr_or_sys) {
    ArmLoadCPSR(registers, registers->current.spsr);
  }

  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }

  if (updates_pc) {
    ArmLoadGPSR(registers, REGISTER_PC, registers->current.user.gprs.pc);
  }
}

void ArmLDMSDAW(ArmAllRegisters *registers, const Memory *memory,
                ArmRegisterIndex Rn, uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);

  bool loads_pc = LoadUpdatesPCNoWriteback(register_list);
  bool updates_pc = LoadUpdatesPC(register_list, Rn);

  // Including the writeback register in the register list for a load store
  // multiple causes undefined behavior in ARMv4. The ARM7TDMI handles this by
  // ignoring the write to the writeback register.
  bool skip_writeback = (1u << Rn) & register_list;

  bool is_usr_or_sys = ArmModeIsUsrOrSys(registers->current.user.cpsr);
  bool modify_banked_registers = !updates_pc && !is_usr_or_sys;

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
  int i;
  while (NextRegister(&register_list, &i)) {
    success =
        ArmLoad32LE(memory, address, &registers->current.user.gprs.gprs[i]);
    if (!success) {
      break;
    }
    address += 4u;
  }
  if (!skip_writeback) {
    registers->current.user.gprs.gprs[Rn] -= size;
  }

  if (modify_banked_registers) {
    ArmLoadCPSR(registers, old_status);
  } else if (loads_pc && !is_usr_or_sys) {
    ArmLoadCPSR(registers, registers->current.spsr);
  }

  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }

  if (updates_pc) {
    ArmLoadGPSR(registers, REGISTER_PC, registers->current.user.gprs.pc);
  }
}

void ArmLDMSDBW(ArmAllRegisters *registers, const Memory *memory,
                ArmRegisterIndex Rn, uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);

  bool loads_pc = LoadUpdatesPCNoWriteback(register_list);
  bool updates_pc = LoadUpdatesPC(register_list, Rn);

  // Including the writeback register in the register list for a load store
  // multiple causes undefined behavior in ARMv4. The ARM7TDMI handles this by
  // ignoring the write to the writeback register.
  bool skip_writeback = (1u << Rn) & register_list;

  bool is_usr_or_sys = ArmModeIsUsrOrSys(registers->current.user.cpsr);
  bool modify_banked_registers = !updates_pc && !is_usr_or_sys;

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
  int i;
  while (NextRegister(&register_list, &i)) {
    success =
        ArmLoad32LE(memory, address, &registers->current.user.gprs.gprs[i]);
    if (!success) {
      break;
    }
    address += 4u;
  }
  if (!skip_writeback) {
    registers->current.user.gprs.gprs[Rn] -= size;
  }

  if (modify_banked_registers) {
    ArmLoadCPSR(registers, old_status);
  } else if (loads_pc && !is_usr_or_sys) {
    ArmLoadCPSR(registers, registers->current.spsr);
  }

  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }

  if (updates_pc) {
    ArmLoadGPSR(registers, REGISTER_PC, registers->current.user.gprs.pc);
  }
}

void ArmLDMSIA(ArmAllRegisters *registers, const Memory *memory,
               ArmRegisterIndex Rn, uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);

  bool updates_pc = LoadUpdatesPCNoWriteback(register_list);

  bool is_usr_or_sys = ArmModeIsUsrOrSys(registers->current.user.cpsr);
  bool modify_banked_registers = !updates_pc && !is_usr_or_sys;

  uint32_t address = registers->current.user.gprs.gprs[Rn];

  ArmProgramStatusRegister old_status;
  if (modify_banked_registers) {
    old_status = registers->current.user.cpsr;
    ArmProgramStatusRegister temporary_status = old_status;
    temporary_status.mode = MODE_USR;
    ArmLoadCPSR(registers, temporary_status);
  }

  bool success = true;
  int i;
  while (NextRegister(&register_list, &i)) {
    success =
        ArmLoad32LE(memory, address, &registers->current.user.gprs.gprs[i]);
    if (!success) {
      break;
    }
    address += 4u;
  }

  if (modify_banked_registers) {
    ArmLoadCPSR(registers, old_status);
  } else if (updates_pc && !is_usr_or_sys) {
    ArmLoadCPSR(registers, registers->current.spsr);
  }

  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }

  if (updates_pc) {
    ArmLoadGPSR(registers, REGISTER_PC, registers->current.user.gprs.pc);
  }
}

void ArmLDMSIB(ArmAllRegisters *registers, const Memory *memory,
               ArmRegisterIndex Rn, uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);

  bool updates_pc = LoadUpdatesPCNoWriteback(register_list);

  bool is_usr_or_sys = ArmModeIsUsrOrSys(registers->current.user.cpsr);
  bool modify_banked_registers = !updates_pc && !is_usr_or_sys;

  uint32_t address = registers->current.user.gprs.gprs[Rn];

  ArmProgramStatusRegister old_status;
  if (modify_banked_registers) {
    old_status = registers->current.user.cpsr;
    ArmProgramStatusRegister temporary_status = old_status;
    temporary_status.mode = MODE_USR;
    ArmLoadCPSR(registers, temporary_status);
  }

  bool success = true;
  int i;
  while (NextRegister(&register_list, &i)) {
    address += 4u;
    success =
        ArmLoad32LE(memory, address, &registers->current.user.gprs.gprs[i]);
    if (!success) {
      break;
    }
  }

  if (modify_banked_registers) {
    ArmLoadCPSR(registers, old_status);
  } else if (updates_pc && !is_usr_or_sys) {
    ArmLoadCPSR(registers, registers->current.spsr);
  }

  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }

  if (updates_pc) {
    ArmLoadGPSR(registers, REGISTER_PC, registers->current.user.gprs.pc);
  }
}

void ArmLDMSIAW(ArmAllRegisters *registers, const Memory *memory,
                ArmRegisterIndex Rn, uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);

  bool loads_pc = LoadUpdatesPCNoWriteback(register_list);
  bool updates_pc = LoadUpdatesPC(register_list, Rn);

  // Including the writeback register in the register list for a load store
  // multiple causes undefined behavior in ARMv4. The ARM7TDMI handles this by
  // ignoring the write to the writeback register.
  bool skip_writeback = (1u << Rn) & register_list;

  bool is_usr_or_sys = ArmModeIsUsrOrSys(registers->current.user.cpsr);
  bool modify_banked_registers = !updates_pc && !is_usr_or_sys;

  uint_fast8_t size = PopCount(register_list) * 4u;
  uint32_t address = registers->current.user.gprs.gprs[Rn];

  ArmProgramStatusRegister old_status;
  if (modify_banked_registers) {
    old_status = registers->current.user.cpsr;
    ArmProgramStatusRegister temporary_status = old_status;
    temporary_status.mode = MODE_USR;
    ArmLoadCPSR(registers, temporary_status);
  }

  bool success = true;
  int i;
  while (NextRegister(&register_list, &i)) {
    success =
        ArmLoad32LE(memory, address, &registers->current.user.gprs.gprs[i]);
    if (!success) {
      break;
    }
    address += 4u;
  }
  if (!skip_writeback) {
    registers->current.user.gprs.gprs[Rn] += size;
  }

  if (modify_banked_registers) {
    ArmLoadCPSR(registers, old_status);
  } else if (loads_pc && !is_usr_or_sys) {
    ArmLoadCPSR(registers, registers->current.spsr);
  }

  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }

  if (updates_pc) {
    ArmLoadGPSR(registers, REGISTER_PC, registers->current.user.gprs.pc);
  }
}

void ArmLDMSIBW(ArmAllRegisters *registers, const Memory *memory,
                ArmRegisterIndex Rn, uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);

  bool loads_pc = LoadUpdatesPCNoWriteback(register_list);
  bool updates_pc = LoadUpdatesPC(register_list, Rn);

  // Including the writeback register in the register list for a load store
  // multiple causes undefined behavior in ARMv4. The ARM7TDMI handles this by
  // ignoring the write to the writeback register.
  bool skip_writeback = (1u << Rn) & register_list;

  bool is_usr_or_sys = ArmModeIsUsrOrSys(registers->current.user.cpsr);
  bool modify_banked_registers = !updates_pc && !is_usr_or_sys;

  uint_fast8_t size = PopCount(register_list) * 4u;
  uint32_t address = registers->current.user.gprs.gprs[Rn];

  ArmProgramStatusRegister old_status;
  if (modify_banked_registers) {
    old_status = registers->current.user.cpsr;
    ArmProgramStatusRegister temporary_status = old_status;
    temporary_status.mode = MODE_USR;
    ArmLoadCPSR(registers, temporary_status);
  }

  bool success = true;
  int i;
  while (NextRegister(&register_list, &i)) {
    address += 4u;
    success =
        ArmLoad32LE(memory, address, &registers->current.user.gprs.gprs[i]);
    if (!success) {
      break;
    }
  }
  if (!skip_writeback) {
    registers->current.user.gprs.gprs[Rn] += size;
  }

  if (modify_banked_registers) {
    ArmLoadCPSR(registers, old_status);
  } else if (loads_pc && !is_usr_or_sys) {
    ArmLoadCPSR(registers, registers->current.spsr);
  }

  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }

  if (updates_pc) {
    ArmLoadGPSR(registers, REGISTER_PC, registers->current.user.gprs.pc);
  }
}

void ArmSTMDA(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
              uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);

  uint32_t address =
      registers->current.user.gprs.gprs[Rn] - PopCount(register_list) * 4u + 4u;

  int i;
  while (NextRegister(&register_list, &i)) {
    bool success =
        ArmStore32LE(memory, address, registers->current.user.gprs.gprs[i]);
    if (!success) {
      ArmExceptionDataABT(registers);
      return;
    }
    address += 4u;
  }
}

void ArmSTMDB(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
              uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);

  uint32_t address =
      registers->current.user.gprs.gprs[Rn] - PopCount(register_list) * 4u;

  int i;
  while (NextRegister(&register_list, &i)) {
    bool success =
        ArmStore32LE(memory, address, registers->current.user.gprs.gprs[i]);
    if (!success) {
      ArmExceptionDataABT(registers);
      return;
    }
    address += 4u;
  }
}

void ArmSTMDAW(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
               uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);

  bool updates_pc = StoreUpdatesPC(Rn);

  uint_fast8_t size = PopCount(register_list) * 4u;
  uint32_t address = registers->current.user.gprs.gprs[Rn] - size + 4u;

  bool success = true;
  int i;
  while (NextRegister(&register_list, &i)) {
    success =
        ArmStore32LE(memory, address, registers->current.user.gprs.gprs[i]);
    if (!success) {
      break;
    }
    address += 4u;
  }

  registers->current.user.gprs.gprs[Rn] -= size;

  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }

  if (updates_pc) {
    ArmLoadGPSR(registers, REGISTER_PC, registers->current.user.gprs.pc);
  }
}

void ArmSTMDBW(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
               uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);

  bool updates_pc = StoreUpdatesPC(Rn);

  uint_fast8_t size = PopCount(register_list) * 4u;
  uint32_t address = registers->current.user.gprs.gprs[Rn] - size;

  bool success = true;
  int i;
  while (NextRegister(&register_list, &i)) {
    success =
        ArmStore32LE(memory, address, registers->current.user.gprs.gprs[i]);
    if (!success) {
      break;
    }
    address += 4u;
  }

  registers->current.user.gprs.gprs[Rn] -= size;

  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }

  if (updates_pc) {
    ArmLoadGPSR(registers, REGISTER_PC, registers->current.user.gprs.pc);
  }
}

void ArmSTMIA(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
              uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);

  uint32_t address = registers->current.user.gprs.gprs[Rn];

  int i;
  while (NextRegister(&register_list, &i)) {
    bool success =
        ArmStore32LE(memory, address, registers->current.user.gprs.gprs[i]);
    if (!success) {
      ArmExceptionDataABT(registers);
      return;
    }
    address += 4u;
  }
}

void ArmSTMIB(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
              uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);

  uint32_t address = registers->current.user.gprs.gprs[Rn];

  int i;
  while (NextRegister(&register_list, &i)) {
    address += 4u;
    bool success =
        ArmStore32LE(memory, address, registers->current.user.gprs.gprs[i]);
    if (!success) {
      ArmExceptionDataABT(registers);
      return;
    }
  }
}

void ArmSTMIAW(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
               uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);

  bool updates_pc = StoreUpdatesPC(Rn);

  uint_fast8_t size = PopCount(register_list) * 4u;
  uint32_t address = registers->current.user.gprs.gprs[Rn];

  bool success = true;
  int i;
  while (NextRegister(&register_list, &i)) {
    success =
        ArmStore32LE(memory, address, registers->current.user.gprs.gprs[i]);
    if (!success) {
      break;
    }
    address += 4u;
  }
  registers->current.user.gprs.gprs[Rn] += size;

  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }

  if (updates_pc) {
    ArmLoadGPSR(registers, REGISTER_PC, registers->current.user.gprs.pc);
  }
}

void ArmSTMIBW(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
               uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);

  bool updates_pc = StoreUpdatesPC(Rn);

  uint_fast8_t size = PopCount(register_list) * 4u;
  uint32_t address = registers->current.user.gprs.gprs[Rn];

  bool success = true;
  int i;
  while (NextRegister(&register_list, &i)) {
    address += 4u;
    success =
        ArmStore32LE(memory, address, registers->current.user.gprs.gprs[i]);
    if (!success) {
      break;
    }
  }

  registers->current.user.gprs.gprs[Rn] += size;

  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }

  if (updates_pc) {
    ArmLoadGPSR(registers, REGISTER_PC, registers->current.user.gprs.pc);
  }
}

void ArmSTMSDA(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
               uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);
  bool is_usr_or_sys = ArmModeIsUsrOrSys(registers->current.user.cpsr);
  bool modify_banked_registers = !is_usr_or_sys;

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
  int i;
  while (NextRegister(&register_list, &i)) {
    success =
        ArmStore32LE(memory, address, registers->current.user.gprs.gprs[i]);
    if (!success) {
      break;
    }
    address += 4u;
  }

  if (modify_banked_registers) {
    ArmLoadCPSR(registers, old_status);
  }

  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }
}

void ArmSTMSDB(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
               uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);
  bool is_usr_or_sys = ArmModeIsUsrOrSys(registers->current.user.cpsr);
  bool modify_banked_registers = !is_usr_or_sys;

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
  int i;
  while (NextRegister(&register_list, &i)) {
    success =
        ArmStore32LE(memory, address, registers->current.user.gprs.gprs[i]);
    if (!success) {
      break;
    }
    address += 4u;
  }

  if (modify_banked_registers) {
    ArmLoadCPSR(registers, old_status);
  }

  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }
}

void ArmSTMSDAW(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
                uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);

  bool updates_pc = StoreUpdatesPC(Rn);

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
  int i;
  while (NextRegister(&register_list, &i)) {
    success =
        ArmStore32LE(memory, address, registers->current.user.gprs.gprs[i]);
    if (!success) {
      break;
    }
    address += 4u;
  }
  registers->current.user.gprs.gprs[Rn] -= size;

  if (modify_banked_registers) {
    ArmLoadCPSR(registers, old_status);
  }

  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }

  if (updates_pc) {
    ArmLoadGPSR(registers, REGISTER_PC, registers->current.user.gprs.pc);
  }
}

void ArmSTMSDBW(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
                uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);

  bool updates_pc = StoreUpdatesPC(Rn);

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
  int i;
  while (NextRegister(&register_list, &i)) {
    success =
        ArmStore32LE(memory, address, registers->current.user.gprs.gprs[i]);
    if (!success) {
      break;
    }
    address += 4u;
  }
  registers->current.user.gprs.gprs[Rn] -= size;

  if (modify_banked_registers) {
    ArmLoadCPSR(registers, old_status);
  }

  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }

  if (updates_pc) {
    ArmLoadGPSR(registers, REGISTER_PC, registers->current.user.gprs.pc);
  }
}

void ArmSTMSIA(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
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
  int i;
  while (NextRegister(&register_list, &i)) {
    success =
        ArmStore32LE(memory, address, registers->current.user.gprs.gprs[i]);
    if (!success) {
      break;
    }
    address += 4u;
  }

  if (modify_banked_registers) {
    ArmLoadCPSR(registers, old_status);
  }

  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }
}

void ArmSTMSIB(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
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
  int i;
  while (NextRegister(&register_list, &i)) {
    address += 4u;
    success =
        ArmStore32LE(memory, address, registers->current.user.gprs.gprs[i]);
    if (!success) {
      break;
    }
  }

  if (modify_banked_registers) {
    ArmLoadCPSR(registers, old_status);
  }

  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }
}

void ArmSTMSIAW(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
                uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);

  bool updates_pc = StoreUpdatesPC(Rn);

  bool is_usr_or_sys = ArmModeIsUsrOrSys(registers->current.user.cpsr);
  bool modify_banked_registers = !is_usr_or_sys;

  uint_fast8_t size = PopCount(register_list) * 4u;
  uint32_t address = registers->current.user.gprs.gprs[Rn];

  ArmProgramStatusRegister old_status;
  if (modify_banked_registers) {
    old_status = registers->current.user.cpsr;
    ArmProgramStatusRegister temporary_status = old_status;
    temporary_status.mode = MODE_USR;
    ArmLoadCPSR(registers, temporary_status);
  }

  bool success = true;
  int i;
  while (NextRegister(&register_list, &i)) {
    success =
        ArmStore32LE(memory, address, registers->current.user.gprs.gprs[i]);
    if (!success) {
      break;
    }
    address += 4u;
  }
  registers->current.user.gprs.gprs[Rn] += size;

  if (modify_banked_registers) {
    ArmLoadCPSR(registers, old_status);
  }

  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }

  if (updates_pc) {
    ArmLoadGPSR(registers, REGISTER_PC, registers->current.user.gprs.pc);
  }
}

void ArmSTMSIBW(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
                uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);

  bool updates_pc = StoreUpdatesPC(Rn);

  bool is_usr_or_sys = ArmModeIsUsrOrSys(registers->current.user.cpsr);
  bool modify_banked_registers = !is_usr_or_sys;

  uint_fast8_t size = PopCount(register_list) * 4u;
  uint32_t address = registers->current.user.gprs.gprs[Rn];

  ArmProgramStatusRegister old_status;
  if (modify_banked_registers) {
    old_status = registers->current.user.cpsr;
    ArmProgramStatusRegister temporary_status = old_status;
    temporary_status.mode = MODE_USR;
    ArmLoadCPSR(registers, temporary_status);
  }

  bool success = true;
  int i;
  while (NextRegister(&register_list, &i)) {
    address += 4u;
    success =
        ArmStore32LE(memory, address, registers->current.user.gprs.gprs[i]);
    if (!success) {
      break;
    }
  }
  registers->current.user.gprs.gprs[Rn] += size;

  if (modify_banked_registers) {
    ArmLoadCPSR(registers, old_status);
  }

  if (!success) {
    ArmExceptionDataABT(registers);
    return;
  }

  if (updates_pc) {
    ArmLoadGPSR(registers, REGISTER_PC, registers->current.user.gprs.pc);
  }
}