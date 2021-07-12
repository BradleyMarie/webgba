#include "emulator/cpu/arm7tdmi/exceptions.h"

static inline void ArmException(ArmAllRegisters* registers, unsigned mode) {
  assert(mode == MODE_UND || mode == MODE_SVC || mode == MODE_ABT ||
         mode == MODE_FIQ || mode == MODE_IRQ);

  ArmProgramStatusRegister old_cpsr = registers->current.user.cpsr;
  ArmProgramStatusRegister next_status = old_cpsr;
  next_status.mode = mode;
  next_status.thumb = false;
  next_status.irq_disable = true;
  ArmLoadCPSR(registers, next_status);

  const static uint_fast8_t next_instruction_pc_offset[2] = {4, 2};
  registers->current.user.gprs.r14 = registers->current.user.gprs.pc -
                                     next_instruction_pc_offset[old_cpsr.thumb];
  registers->current.spsr = old_cpsr;
}

void ArmExceptionDataABT(ArmAllRegisters* registers) {
  ArmException(registers, MODE_ABT);
  registers->current.user.gprs.pc = 0x10u;
}

void ArmExceptionPrefetchABT(ArmAllRegisters* registers) {
  ArmException(registers, MODE_ABT);
  registers->current.user.gprs.pc = 0xCu;
}

void ArmExceptionFIQ(ArmAllRegisters* registers) {
  ArmException(registers, MODE_FIQ);
  registers->current.user.cpsr.fiq_disable = true;
  registers->current.user.gprs.pc = 0x1Cu;
}

void ArmExceptionIRQ(ArmAllRegisters* registers) {
  ArmException(registers, MODE_IRQ);
  registers->current.user.gprs.pc = 0x18u;
}

void ArmExceptionRST(ArmAllRegisters* registers) {
  ArmException(registers, MODE_SVC);
  registers->current.user.cpsr.fiq_disable = true;
  registers->current.user.gprs.pc = 0x0u;
}

void ArmExceptionSWI(ArmAllRegisters* registers) {
  ArmException(registers, MODE_SVC);
  registers->current.user.gprs.pc = 0x8u;
}

void ArmExceptionUND(ArmAllRegisters* registers) {
  ArmException(registers, MODE_UND);
  registers->current.user.gprs.pc = 0x4u;
}