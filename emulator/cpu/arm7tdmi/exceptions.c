#include "emulator/cpu/arm7tdmi/exceptions.h"

void ArmExceptionDataABT(ArmAllRegisters* registers) {
  uint32_t aborted_instruction = ArmCurrentInstruction(registers);

  ArmProgramStatusRegister old_cpsr = registers->current.user.cpsr;
  ArmProgramStatusRegister next_status = old_cpsr;
  next_status.mode = MODE_ABT;
  next_status.thumb = false;
  next_status.irq_disable = true;
  ArmLoadCPSR(registers, next_status);

  registers->current.user.gprs.r14 = aborted_instruction + 8u;
  registers->current.spsr = old_cpsr;

  ArmLoadProgramCounter(registers, 0x10u);
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
  registers->current.spsr = old_cpsr;

  ArmLoadProgramCounter(registers, 0xCu);
}

void ArmExceptionFIQ(ArmAllRegisters* registers) {
  uint32_t next_instruction = ArmNextInstruction(registers);

  ArmProgramStatusRegister old_cpsr = registers->current.user.cpsr;
  ArmProgramStatusRegister next_status = old_cpsr;
  next_status.mode = MODE_FIQ;
  next_status.thumb = false;
  next_status.irq_disable = true;
  next_status.fiq_disable = true;
  ArmLoadCPSR(registers, next_status);

  registers->current.user.gprs.r14 = next_instruction + 4u;
  registers->current.spsr = old_cpsr;

  ArmLoadProgramCounter(registers, 0x1Cu);
}

void ArmExceptionIRQ(ArmAllRegisters* registers) {
  uint32_t next_instruction = ArmNextInstruction(registers);

  ArmProgramStatusRegister old_cpsr = registers->current.user.cpsr;
  ArmProgramStatusRegister next_status = old_cpsr;
  next_status.mode = MODE_IRQ;
  next_status.thumb = false;
  next_status.irq_disable = true;
  ArmLoadCPSR(registers, next_status);

  registers->current.user.gprs.r14 = next_instruction + 4u;
  registers->current.spsr = old_cpsr;

  ArmLoadProgramCounter(registers, 0x18u);
}

void ArmExceptionRST(ArmAllRegisters* registers) {
  ArmProgramStatusRegister old_cpsr = registers->current.user.cpsr;
  ArmProgramStatusRegister next_status = old_cpsr;
  next_status.mode = MODE_SVC;
  next_status.thumb = false;
  next_status.irq_disable = true;
  next_status.fiq_disable = true;
  ArmLoadCPSR(registers, next_status);

  ArmLoadProgramCounter(registers, 0x0u);
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
  registers->current.spsr = old_cpsr;

  ArmLoadProgramCounter(registers, 0x8u);
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
  registers->current.spsr = old_cpsr;

  ArmLoadProgramCounter(registers, 0x4u);
}