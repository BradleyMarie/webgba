extern "C" {
#include "emulator/cpu/arm7tdmi/instructions/branch.h"
}

#include <cstring>

#include "googletest/include/gtest/gtest.h"

ArmUserRegisters CreateArmUserRegisters() {
  ArmUserRegisters registers;
  memset(&registers, 0, sizeof(ArmUserRegisters));
  return registers;
}

bool ArmUserRegistersAreZero(const ArmUserRegisters& regs) {
  auto zero = CreateArmUserRegisters();
  return !memcmp(&zero, &regs, sizeof(ArmUserRegisters));
}

TEST(ArmB, Branch) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.pc = 208u;
  ArmB(&registers, 100);
  EXPECT_EQ(608u, registers.gprs.pc);

  registers.gprs.pc = 0u;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmB, BranchThumb) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.pc = 208u;
  registers.cpsr.thumb = true;
  ArmB(&registers, 100);
  EXPECT_EQ(408u, registers.gprs.pc);

  registers.gprs.pc = 0u;
  registers.cpsr.thumb = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmBL, BranchLink) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.pc = 208u;
  ArmBL(&registers, 100);
  EXPECT_EQ(204u, registers.gprs.lr);
  EXPECT_EQ(608u, registers.gprs.pc);

  registers.gprs.pc = 0u;
  registers.gprs.lr = 0u;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmBL, BranchLinkThumb) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.pc = 204u;
  registers.cpsr.thumb = true;
  ArmBL(&registers, 100);
  EXPECT_EQ(203u, registers.gprs.lr);
  EXPECT_EQ(404u, registers.gprs.pc);

  registers.gprs.pc = 0u;
  registers.gprs.lr = 0u;
  registers.cpsr.thumb = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}