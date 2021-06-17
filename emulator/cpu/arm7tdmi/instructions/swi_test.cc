extern "C" {
#include "emulator/cpu/arm7tdmi/instructions/swi.h"
}

#include <strings.h>

#include "googletest/include/gtest/gtest.h"

ArmAllRegisters CreateArmAllRegisters() {
  ArmAllRegisters registers;
  memset(&registers, 0, sizeof(ArmAllRegisters));
  return registers;
}

bool ArmAllRegistersAreZero(const ArmAllRegisters& regs) {
  auto zero = CreateArmAllRegisters();
  return !memcmp(&zero, &regs, sizeof(ArmAllRegisters));
}

TEST(ArmSWI, SoftwareInterrupt) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.pc = 108u;
  registers.current.user.cpsr.mode = MODE_IRQ;
  registers.current.user.gprs.r13 = 13u;
  registers.current.user.gprs.r14 = 14u;
  registers.current.spsr.zero = true;
  registers.banked_gprs[SVC_BANK_INDEX][BANKED_R13_INDEX] = 28u;
  auto old_cpsr = registers.current.user.cpsr;

  ArmSWI(&registers);
  EXPECT_EQ(28u, registers.current.user.gprs.r13);
  EXPECT_EQ(104u, registers.current.user.gprs.r14);
  EXPECT_EQ(old_cpsr.value, registers.current.spsr.value);
  EXPECT_EQ(8u, registers.current.user.gprs.pc);
  EXPECT_EQ(MODE_SVC, registers.current.user.cpsr.mode);
  EXPECT_FALSE(registers.current.user.cpsr.thumb);
  EXPECT_TRUE(registers.current.user.cpsr.irq_disable);
  EXPECT_EQ(13u, registers.banked_gprs[IRQ_BANK_INDEX][BANKED_R13_INDEX]);
  EXPECT_EQ(14u, registers.banked_gprs[IRQ_BANK_INDEX][BANKED_R14_INDEX]);
  EXPECT_TRUE(registers.banked_spsrs[IRQ_BANK_INDEX].zero);

  registers.current.user.gprs.r13 = 0u;
  registers.current.user.gprs.r14 = 0u;
  registers.current.user.gprs.pc = 0u;
  registers.current.user.cpsr.mode = 0u;
  registers.current.user.cpsr.irq_disable = 0u;
  registers.current.spsr.mode = 0u;
  registers.banked_gprs[SVC_BANK_INDEX][BANKED_R13_INDEX] = 0u;
  registers.banked_gprs[IRQ_BANK_INDEX][BANKED_R13_INDEX] = 0u;
  registers.banked_gprs[IRQ_BANK_INDEX][BANKED_R14_INDEX] = 0u;
  registers.banked_spsrs[IRQ_BANK_INDEX].zero = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmSWI, SoftwareInterruptThumb) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.pc = 104u;
  registers.current.user.cpsr.mode = MODE_IRQ;
  registers.current.user.cpsr.thumb = true;
  registers.current.user.gprs.r13 = 13u;
  registers.current.user.gprs.r14 = 14u;
  registers.current.spsr.zero = true;
  registers.banked_gprs[SVC_BANK_INDEX][BANKED_R13_INDEX] = 28u;
  auto old_cpsr = registers.current.user.cpsr;

  ArmSWI(&registers);
  EXPECT_EQ(28u, registers.current.user.gprs.r13);
  EXPECT_EQ(102u, registers.current.user.gprs.r14);
  EXPECT_EQ(old_cpsr.value, registers.current.spsr.value);
  EXPECT_EQ(8u, registers.current.user.gprs.pc);
  EXPECT_EQ(MODE_SVC, registers.current.user.cpsr.mode);
  EXPECT_FALSE(registers.current.user.cpsr.thumb);
  EXPECT_TRUE(registers.current.user.cpsr.irq_disable);
  EXPECT_EQ(13u, registers.banked_gprs[IRQ_BANK_INDEX][BANKED_R13_INDEX]);
  EXPECT_EQ(14u, registers.banked_gprs[IRQ_BANK_INDEX][BANKED_R14_INDEX]);
  EXPECT_TRUE(registers.banked_spsrs[IRQ_BANK_INDEX].zero);

  registers.current.user.gprs.r13 = 0u;
  registers.current.user.gprs.r14 = 0u;
  registers.current.user.gprs.pc = 0u;
  registers.current.user.cpsr.mode = 0u;
  registers.current.user.cpsr.irq_disable = 0u;
  registers.current.spsr.mode = 0u;
  registers.current.spsr.thumb = false;
  registers.banked_gprs[SVC_BANK_INDEX][BANKED_R13_INDEX] = 0u;
  registers.banked_gprs[IRQ_BANK_INDEX][BANKED_R13_INDEX] = 0u;
  registers.banked_gprs[IRQ_BANK_INDEX][BANKED_R14_INDEX] = 0u;
  registers.banked_spsrs[IRQ_BANK_INDEX].zero = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}