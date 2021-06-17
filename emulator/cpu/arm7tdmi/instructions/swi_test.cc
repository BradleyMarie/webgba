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
  registers.current.user.cpsr.mode = MODE_USR;
  auto old_cpsr = registers.current.user.cpsr;
  ArmSWI(&registers);
  EXPECT_EQ(104u, registers.banked_gprs[SVC_BANK_INDEX][BANKED_R14_INDEX]);
  EXPECT_EQ(old_cpsr.value, registers.banked_spsrs[SVC_BANK_INDEX].value);
  EXPECT_EQ(8u, registers.current.user.gprs.pc);
  EXPECT_EQ(MODE_SVC, registers.current.user.cpsr.mode);
  EXPECT_FALSE(registers.current.user.cpsr.thumb);
  EXPECT_TRUE(registers.current.user.cpsr.irq_disable);

  registers.current.user.gprs.pc = 0u;
  registers.current.user.cpsr.mode = 0u;
  registers.current.user.cpsr.irq_disable = 0u;
  registers.banked_gprs[SVC_BANK_INDEX][BANKED_R14_INDEX] = 0u;
  registers.banked_spsrs[SVC_BANK_INDEX].mode = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmSWI, SoftwareInterruptThumb) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.pc = 104u;
  registers.current.user.cpsr.mode = MODE_USR;
  registers.current.user.cpsr.thumb = true;
  auto old_cpsr = registers.current.user.cpsr;
  ArmSWI(&registers);
  EXPECT_EQ(102u, registers.banked_gprs[SVC_BANK_INDEX][BANKED_R14_INDEX]);
  EXPECT_EQ(old_cpsr.value, registers.banked_spsrs[SVC_BANK_INDEX].value);
  EXPECT_EQ(8u, registers.current.user.gprs.pc);
  EXPECT_EQ(MODE_SVC, registers.current.user.cpsr.mode);
  EXPECT_FALSE(registers.current.user.cpsr.thumb);
  EXPECT_TRUE(registers.current.user.cpsr.irq_disable);

  registers.current.user.gprs.pc = 0u;
  registers.current.user.cpsr.mode = 0u;
  registers.current.user.cpsr.irq_disable = 0u;
  registers.banked_gprs[SVC_BANK_INDEX][BANKED_R14_INDEX] = 0u;
  registers.banked_spsrs[SVC_BANK_INDEX].thumb = false;
  registers.banked_spsrs[SVC_BANK_INDEX].mode = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}