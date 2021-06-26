#include "emulator/cpu/arm7tdmi/instructions/block_data_transfer.h"

static inline bool ArmModeIsUsrOrSys(ArmProgramStatusRegister cpsr) {
  return cpsr.mode == MODE_USR || cpsr.mode == MODE_SYS;
}

void ArmLDMDA(ArmGeneralPurposeRegisters *registers, const Memory *memory,
              ArmRegisterIndex Rn, uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);
  uint32_t address = registers->gprs[Rn];
  for (uint32_t i = 0u; i < 16u; i++) {
    uint32_t index = 15u - i;
    if (register_list & (1u << index)) {
      bool success = Load32LE(memory, address, &registers->gprs[index]);
      assert(success);
      address -= 4u;
    }
  }
}

void ArmLDMDB(ArmGeneralPurposeRegisters *registers, const Memory *memory,
              ArmRegisterIndex Rn, uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);
  uint32_t address = registers->gprs[Rn];
  for (uint32_t i = 0u; i < 16u; i++) {
    uint32_t index = 15u - i;
    if (register_list & (1u << index)) {
      address -= 4u;
      bool success = Load32LE(memory, address, &registers->gprs[index]);
      assert(success);
    }
  }
}

void ArmLDMDAW(ArmGeneralPurposeRegisters *registers, const Memory *memory,
               ArmRegisterIndex Rn, uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);
  uint32_t address = registers->gprs[Rn];
  for (uint32_t i = 0u; i < 16u; i++) {
    uint32_t index = 15u - i;
    if (register_list & (1u << index)) {
      bool success = Load32LE(memory, address, &registers->gprs[index]);
      assert(success);
      address -= 4u;
    }
  }
  registers->gprs[Rn] = address;
}

void ArmLDMDBW(ArmGeneralPurposeRegisters *registers, const Memory *memory,
               ArmRegisterIndex Rn, uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);
  uint32_t address = registers->gprs[Rn];
  for (uint32_t i = 0u; i < 16u; i++) {
    uint32_t index = 15u - i;
    if (register_list & (1u << index)) {
      address -= 4u;
      bool success = Load32LE(memory, address, &registers->gprs[index]);
      assert(success);
    }
  }
  registers->gprs[Rn] = address;
}

void ArmLDMIA(ArmGeneralPurposeRegisters *registers, const Memory *memory,
              ArmRegisterIndex Rn, uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);
  uint32_t address = registers->gprs[Rn];
  for (uint32_t i = 0u; i < 16u; i++) {
    if (register_list & (1u << i)) {
      bool success = Load32LE(memory, address, &registers->gprs[i]);
      assert(success);
      address += 4u;
    }
  }
}

void ArmLDMIB(ArmGeneralPurposeRegisters *registers, const Memory *memory,
              ArmRegisterIndex Rn, uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);
  uint32_t address = registers->gprs[Rn];
  for (uint32_t i = 0u; i < 16u; i++) {
    if (register_list & (1u << i)) {
      address += 4u;
      bool success = Load32LE(memory, address, &registers->gprs[i]);
      assert(success);
    }
  }
}

void ArmLDMIAW(ArmGeneralPurposeRegisters *registers, const Memory *memory,
               ArmRegisterIndex Rn, uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);
  uint32_t address = registers->gprs[Rn];
  for (uint32_t i = 0u; i < 16u; i++) {
    if (register_list & (1u << i)) {
      bool success = Load32LE(memory, address, &registers->gprs[i]);
      assert(success);
      address += 4u;
    }
  }
  registers->gprs[Rn] = address;
}

void ArmLDMIBW(ArmGeneralPurposeRegisters *registers, const Memory *memory,
               ArmRegisterIndex Rn, uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);
  uint32_t address = registers->gprs[Rn];
  for (uint32_t i = 0u; i < 16u; i++) {
    if (register_list & (1u << i)) {
      address += 4u;
      bool success = Load32LE(memory, address, &registers->gprs[i]);
      assert(success);
    }
  }
  registers->gprs[Rn] = address;
}

void ArmLDMSDA(ArmAllRegisters *registers, const Memory *memory,
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

  for (uint32_t i = 0u; i < 16u; i++) {
    uint32_t index = 15u - i;
    if (register_list & (1u << index)) {
      bool success =
          Load32LE(memory, address, &registers->current.user.gprs.gprs[index]);
      assert(success);
      address -= 4u;
    }
  }

  if (modify_banked_registers) {
    ArmLoadCPSR(registers, old_status);
  } else if (loads_pc && !is_usr_or_sys) {
    ArmLoadCPSR(registers, registers->current.spsr);
  }
}

void ArmLDMSDB(ArmAllRegisters *registers, const Memory *memory,
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

  for (uint32_t i = 0u; i < 16u; i++) {
    uint32_t index = 15u - i;
    if (register_list & (1u << index)) {
      address -= 4u;
      bool success =
          Load32LE(memory, address, &registers->current.user.gprs.gprs[index]);
      assert(success);
    }
  }

  if (modify_banked_registers) {
    ArmLoadCPSR(registers, old_status);
  } else if (loads_pc && !is_usr_or_sys) {
    ArmLoadCPSR(registers, registers->current.spsr);
  }
}

void ArmLDMSDAW(ArmAllRegisters *registers, const Memory *memory,
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

  for (uint32_t i = 0u; i < 16u; i++) {
    uint32_t index = 15u - i;
    if (register_list & (1u << index)) {
      bool success =
          Load32LE(memory, address, &registers->current.user.gprs.gprs[index]);
      assert(success);
      address -= 4u;
    }
  }
  registers->current.user.gprs.gprs[Rn] = address;

  if (modify_banked_registers) {
    ArmLoadCPSR(registers, old_status);
  } else if (loads_pc && !is_usr_or_sys) {
    ArmLoadCPSR(registers, registers->current.spsr);
  }
}

void ArmLDMSDBW(ArmAllRegisters *registers, const Memory *memory,
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

  for (uint32_t i = 0u; i < 16u; i++) {
    uint32_t index = 15u - i;
    if (register_list & (1u << index)) {
      address -= 4u;
      bool success =
          Load32LE(memory, address, &registers->current.user.gprs.gprs[index]);
      assert(success);
    }
  }
  registers->current.user.gprs.gprs[Rn] = address;

  if (modify_banked_registers) {
    ArmLoadCPSR(registers, old_status);
  } else if (loads_pc && !is_usr_or_sys) {
    ArmLoadCPSR(registers, registers->current.spsr);
  }
}

void ArmLDMSIA(ArmAllRegisters *registers, const Memory *memory,
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

  for (uint32_t i = 0u; i < 16u; i++) {
    if (register_list & (1u << i)) {
      bool success =
          Load32LE(memory, address, &registers->current.user.gprs.gprs[i]);
      assert(success);
      address += 4u;
    }
  }

  if (modify_banked_registers) {
    ArmLoadCPSR(registers, old_status);
  } else if (loads_pc && !is_usr_or_sys) {
    ArmLoadCPSR(registers, registers->current.spsr);
  }
}

void ArmLDMSIB(ArmAllRegisters *registers, const Memory *memory,
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

  for (uint32_t i = 0u; i < 16u; i++) {
    if (register_list & (1u << i)) {
      address += 4u;
      bool success =
          Load32LE(memory, address, &registers->current.user.gprs.gprs[i]);
      assert(success);
    }
  }

  if (modify_banked_registers) {
    ArmLoadCPSR(registers, old_status);
  } else if (loads_pc && !is_usr_or_sys) {
    ArmLoadCPSR(registers, registers->current.spsr);
  }
}

void ArmLDMSIAW(ArmAllRegisters *registers, const Memory *memory,
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

  for (uint32_t i = 0u; i < 16u; i++) {
    if (register_list & (1u << i)) {
      bool success =
          Load32LE(memory, address, &registers->current.user.gprs.gprs[i]);
      assert(success);
      address += 4u;
    }
  }
  registers->current.user.gprs.gprs[Rn] = address;

  if (modify_banked_registers) {
    ArmLoadCPSR(registers, old_status);
  } else if (loads_pc && !is_usr_or_sys) {
    ArmLoadCPSR(registers, registers->current.spsr);
  }
}

void ArmLDMSIBW(ArmAllRegisters *registers, const Memory *memory,
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

  for (uint32_t i = 0u; i < 16u; i++) {
    if (register_list & (1u << i)) {
      address += 4u;
      bool success =
          Load32LE(memory, address, &registers->current.user.gprs.gprs[i]);
      assert(success);
    }
  }
  registers->current.user.gprs.gprs[Rn] = address;

  if (modify_banked_registers) {
    ArmLoadCPSR(registers, old_status);
  } else if (loads_pc && !is_usr_or_sys) {
    ArmLoadCPSR(registers, registers->current.spsr);
  }
}

void ArmSTMDA(const ArmGeneralPurposeRegisters *registers, Memory *memory,
              ArmRegisterIndex Rn, uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);
  uint32_t address = registers->gprs[Rn];
  for (uint32_t i = 0u; i < 16u; i++) {
    uint32_t index = 15u - i;
    if (register_list & (1u << index)) {
      bool success = Store32LE(memory, address, registers->gprs[index]);
      assert(success);
      address -= 4u;
    }
  }
}

void ArmSTMDB(const ArmGeneralPurposeRegisters *registers, Memory *memory,
              ArmRegisterIndex Rn, uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);
  uint32_t address = registers->gprs[Rn];
  for (uint32_t i = 0u; i < 16u; i++) {
    uint32_t index = 15u - i;
    if (register_list & (1u << index)) {
      address -= 4u;
      bool success = Store32LE(memory, address, registers->gprs[index]);
      assert(success);
    }
  }
}

void ArmSTMDAW(ArmGeneralPurposeRegisters *registers, Memory *memory,
               ArmRegisterIndex Rn, uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);
  uint32_t address = registers->gprs[Rn];
  for (uint32_t i = 0u; i < 16u; i++) {
    uint32_t index = 15u - i;
    if (register_list & (1u << index)) {
      bool success = Store32LE(memory, address, registers->gprs[index]);
      assert(success);
      address -= 4u;
    }
  }
  registers->gprs[Rn] = address;
}

void ArmSTMDBW(ArmGeneralPurposeRegisters *registers, Memory *memory,
               ArmRegisterIndex Rn, uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);
  uint32_t address = registers->gprs[Rn];
  for (uint32_t i = 0u; i < 16u; i++) {
    uint32_t index = 15u - i;
    if (register_list & (1u << index)) {
      address -= 4u;
      bool success = Store32LE(memory, address, registers->gprs[index]);
      assert(success);
    }
  }
  registers->gprs[Rn] = address;
}

void ArmSTMIA(const ArmGeneralPurposeRegisters *registers, Memory *memory,
              ArmRegisterIndex Rn, uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);
  uint32_t address = registers->gprs[Rn];
  for (uint32_t i = 0u; i < 16u; i++) {
    if (register_list & (1u << i)) {
      bool success = Store32LE(memory, address, registers->gprs[i]);
      assert(success);
      address += 4u;
    }
  }
}

void ArmSTMIB(const ArmGeneralPurposeRegisters *registers, Memory *memory,
              ArmRegisterIndex Rn, uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);
  uint32_t address = registers->gprs[Rn];
  for (uint32_t i = 0u; i < 16u; i++) {
    if (register_list & (1u << i)) {
      address += 4u;
      bool success = Store32LE(memory, address, registers->gprs[i]);
      assert(success);
    }
  }
}

void ArmSTMIAW(ArmGeneralPurposeRegisters *registers, Memory *memory,
               ArmRegisterIndex Rn, uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);
  uint32_t address = registers->gprs[Rn];
  for (uint32_t i = 0u; i < 16u; i++) {
    if (register_list & (1u << i)) {
      bool success = Store32LE(memory, address, registers->gprs[i]);
      assert(success);
      address += 4u;
    }
  }
  registers->gprs[Rn] = address;
}

void ArmSTMIBW(ArmGeneralPurposeRegisters *registers, Memory *memory,
               ArmRegisterIndex Rn, uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);
  uint32_t address = registers->gprs[Rn];
  for (uint32_t i = 0u; i < 16u; i++) {
    if (register_list & (1u << i)) {
      address += 4u;
      bool success = Store32LE(memory, address, registers->gprs[i]);
      assert(success);
    }
  }
  registers->gprs[Rn] = address;
}

void ArmSTMSDA(const ArmAllRegisters *registers, Memory *memory,
               ArmRegisterIndex Rn, uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);
  bool is_usr_or_sys = ArmModeIsUsrOrSys(registers->current.user.cpsr);
  bool modify_banked_registers = !is_usr_or_sys;

  uint32_t address = registers->current.user.gprs.gprs[Rn];

  const ArmAllRegisters *registers_to_read;
  ArmAllRegisters local_registers;
  ArmProgramStatusRegister old_status;
  if (modify_banked_registers) {
    local_registers = *registers;
    old_status = local_registers.current.user.cpsr;
    ArmProgramStatusRegister temporary_status = old_status;
    temporary_status.mode = MODE_USR;
    ArmLoadCPSR(&local_registers, temporary_status);
    registers_to_read = &local_registers;
  } else {
    registers_to_read = registers;
  }

  for (uint32_t i = 0u; i < 16u; i++) {
    uint32_t index = 15u - i;
    if (register_list & (1u << index)) {
      bool success = Store32LE(
          memory, address, registers_to_read->current.user.gprs.gprs[index]);
      assert(success);
      address -= 4u;
    }
  }
}

void ArmSTMSDB(const ArmAllRegisters *registers, Memory *memory,
               ArmRegisterIndex Rn, uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);
  bool is_usr_or_sys = ArmModeIsUsrOrSys(registers->current.user.cpsr);
  bool modify_banked_registers = !is_usr_or_sys;

  uint32_t address = registers->current.user.gprs.gprs[Rn];

  const ArmAllRegisters *registers_to_read;
  ArmAllRegisters local_registers;
  ArmProgramStatusRegister old_status;
  if (modify_banked_registers) {
    local_registers = *registers;
    old_status = local_registers.current.user.cpsr;
    ArmProgramStatusRegister temporary_status = old_status;
    temporary_status.mode = MODE_USR;
    ArmLoadCPSR(&local_registers, temporary_status);
    registers_to_read = &local_registers;
  } else {
    registers_to_read = registers;
  }

  for (uint32_t i = 0u; i < 16u; i++) {
    uint32_t index = 15u - i;
    if (register_list & (1u << index)) {
      address -= 4u;
      bool success = Store32LE(
          memory, address, registers_to_read->current.user.gprs.gprs[index]);
      assert(success);
    }
  }
}

void ArmSTMSDAW(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
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

  for (uint32_t i = 0u; i < 16u; i++) {
    uint32_t index = 15u - i;
    if (register_list & (1u << index)) {
      bool success =
          Store32LE(memory, address, registers->current.user.gprs.gprs[index]);
      assert(success);
      address -= 4u;
    }
  }
  registers->current.user.gprs.gprs[Rn] = address;

  if (modify_banked_registers) {
    ArmLoadCPSR(registers, old_status);
  }
}

void ArmSTMSDBW(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
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

  for (uint32_t i = 0u; i < 16u; i++) {
    uint32_t index = 15u - i;
    if (register_list & (1u << index)) {
      address -= 4u;
      bool success =
          Store32LE(memory, address, registers->current.user.gprs.gprs[index]);
      assert(success);
    }
  }
  registers->current.user.gprs.gprs[Rn] = address;

  if (modify_banked_registers) {
    ArmLoadCPSR(registers, old_status);
  }
}

void ArmSTMSIA(const ArmAllRegisters *registers, Memory *memory,
               ArmRegisterIndex Rn, uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);
  bool is_usr_or_sys = ArmModeIsUsrOrSys(registers->current.user.cpsr);
  bool modify_banked_registers = !is_usr_or_sys;

  uint32_t address = registers->current.user.gprs.gprs[Rn];

  const ArmAllRegisters *registers_to_read;
  ArmAllRegisters local_registers;
  ArmProgramStatusRegister old_status;
  if (modify_banked_registers) {
    local_registers = *registers;
    old_status = local_registers.current.user.cpsr;
    ArmProgramStatusRegister temporary_status = old_status;
    temporary_status.mode = MODE_USR;
    ArmLoadCPSR(&local_registers, temporary_status);
    registers_to_read = &local_registers;
  } else {
    registers_to_read = registers;
  }

  for (uint32_t i = 0u; i < 16u; i++) {
    if (register_list & (1u << i)) {
      bool success = Store32LE(memory, address,
                               registers_to_read->current.user.gprs.gprs[i]);
      assert(success);
      address += 4u;
    }
  }
}

void ArmSTMSIB(const ArmAllRegisters *registers, Memory *memory,
               ArmRegisterIndex Rn, uint_fast16_t register_list) {
  assert(register_list <= UINT16_MAX);
  bool is_usr_or_sys = ArmModeIsUsrOrSys(registers->current.user.cpsr);
  bool modify_banked_registers = !is_usr_or_sys;

  uint32_t address = registers->current.user.gprs.gprs[Rn];

  const ArmAllRegisters *registers_to_read;
  ArmAllRegisters local_registers;
  ArmProgramStatusRegister old_status;
  if (modify_banked_registers) {
    local_registers = *registers;
    old_status = local_registers.current.user.cpsr;
    ArmProgramStatusRegister temporary_status = old_status;
    temporary_status.mode = MODE_USR;
    ArmLoadCPSR(&local_registers, temporary_status);
    registers_to_read = &local_registers;
  } else {
    registers_to_read = registers;
  }

  for (uint32_t i = 0u; i < 16u; i++) {
    if (register_list & (1u << i)) {
      address += 4u;
      bool success = Store32LE(memory, address,
                               registers_to_read->current.user.gprs.gprs[i]);
      assert(success);
    }
  }
}

void ArmSTMSIAW(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
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

  for (uint32_t i = 0u; i < 16u; i++) {
    if (register_list & (1u << i)) {
      bool success =
          Store32LE(memory, address, registers->current.user.gprs.gprs[i]);
      assert(success);
      address += 4u;
    }
  }
  registers->current.user.gprs.gprs[Rn] = address;

  if (modify_banked_registers) {
    ArmLoadCPSR(registers, old_status);
  }
}

void ArmSTMSIBW(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
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

  for (uint32_t i = 0u; i < 16u; i++) {
    if (register_list & (1u << i)) {
      address += 4u;
      bool success =
          Store32LE(memory, address, registers->current.user.gprs.gprs[i]);
      assert(success);
    }
  }
  registers->current.user.gprs.gprs[Rn] = address;

  if (modify_banked_registers) {
    ArmLoadCPSR(registers, old_status);
  }
}