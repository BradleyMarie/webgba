#include "emulator/cpu/arm7tdmi/exceptions.h"

static inline uint32_t ArmNextInstruction(ArmAllRegisters* registers) {
  static const uint_fast8_t next_instruction_pc_offset[2] = {4, 2};
  return registers->current.user.gprs.pc -
         next_instruction_pc_offset[registers->current.user.cpsr.thumb];
}

static inline uint32_t ArmCurrentInstruction(ArmAllRegisters* registers) {
  static const uint_fast8_t current_instruction_pc_offset[2] = {8, 4};
  return registers->current.user.gprs.pc -
         current_instruction_pc_offset[registers->current.user.cpsr.thumb];
}

void ArmExceptionDataABT(ArmAllRegisters* registers) {
  uint32_t aborted_instruction = ArmCurrentInstruction(registers);

  ArmProgramStatusRegister old_cpsr = registers->current.user.cpsr;
  ArmProgramStatusRegister next_status = old_cpsr;
  next_status.mode = MODE_ABT;
  next_status.thumb = false;
  next_status.irq_disable = true;
  ArmLoadCPSR(registers, next_status);

  registers->current.user.gprs.r14 = aborted_instruction + 8u;
  registers->current.user.gprs.pc = 0x10u;
  registers->current.spsr = old_cpsr;
}

void ArmExceptionPrefetchABT(ArmAllRegisters* registers) {
  uint32_t aborted_instruction = ArmCurrentInstruction(registers);

  ArmProgramStatusRegister old_cpsr = registers->current.user.cpsr;
  ArmProgramStatusRegister next_status = old_cpsr;
  next_status.mode = MODE_ABT;
  next_status.thumb = false;
  next_status.irq_disable = true;
  ArmLoadCPSR(registers, next_status);

  registers->current.user.gprs.r14 = aborted_instruction + 4u;
  registers->current.user.gprs.pc = 0xCu;
  registers->current.spsr = old_cpsr;
}

void ArmExceptionFIQ(ArmAllRegisters* registers) {
  // Since interrupts fire between instructions, the current instruction pointed
  // to by the program counter is actually the next instruction to be executed.
  uint32_t next_instruction = ArmCurrentInstruction(registers);

  ArmProgramStatusRegister old_cpsr = registers->current.user.cpsr;
  ArmProgramStatusRegister next_status = old_cpsr;
  next_status.mode = MODE_FIQ;
  next_status.thumb = false;
  next_status.irq_disable = true;
  next_status.fiq_disable = true;
  ArmLoadCPSR(registers, next_status);

  registers->current.user.gprs.r14 = next_instruction + 4u;
  registers->current.user.gprs.pc = 0x1Cu;
  registers->current.spsr = old_cpsr;
}

void ArmExceptionIRQ(ArmAllRegisters* registers) {
  // Since interrupts fire between instructions, the current instruction pointed
  // to by the program counter is actually the next instruction to be executed.
  uint32_t next_instruction = ArmCurrentInstruction(registers);

  ArmProgramStatusRegister old_cpsr = registers->current.user.cpsr;
  ArmProgramStatusRegister next_status = old_cpsr;
  next_status.mode = MODE_IRQ;
  next_status.thumb = false;
  next_status.irq_disable = true;
  ArmLoadCPSR(registers, next_status);

  registers->current.user.gprs.r14 = next_instruction + 4u;
  registers->current.user.gprs.pc = 0x18u;
  registers->current.spsr = old_cpsr;
}

void ArmExceptionRST(ArmAllRegisters* registers) {
  ArmProgramStatusRegister old_cpsr = registers->current.user.cpsr;
  ArmProgramStatusRegister next_status = old_cpsr;
  next_status.mode = MODE_SVC;
  next_status.thumb = false;
  next_status.irq_disable = true;
  next_status.fiq_disable = true;
  ArmLoadCPSR(registers, next_status);

  registers->current.user.gprs.pc = 0x0u;
}

void ArmExceptionSWI(ArmAllRegisters* registers) {
  uint32_t next_instruction = ArmNextInstruction(registers);

  ArmProgramStatusRegister old_cpsr = registers->current.user.cpsr;
  ArmProgramStatusRegister next_status = old_cpsr;
  next_status.mode = MODE_SVC;
  next_status.thumb = false;
  next_status.irq_disable = true;
  ArmLoadCPSR(registers, next_status);

  registers->current.user.gprs.r14 = next_instruction;
  registers->current.user.gprs.pc = 0x8u;
  registers->current.spsr = old_cpsr;
}

void ArmExceptionUND(ArmAllRegisters* registers) {
  uint32_t next_instruction = ArmNextInstruction(registers);

  ArmProgramStatusRegister old_cpsr = registers->current.user.cpsr;
  ArmProgramStatusRegister next_status = old_cpsr;
  next_status.mode = MODE_UND;
  next_status.thumb = false;
  next_status.irq_disable = true;
  ArmLoadCPSR(registers, next_status);

  registers->current.user.gprs.r14 = next_instruction;
  registers->current.user.gprs.pc = 0x4u;
  registers->current.spsr = old_cpsr;
}