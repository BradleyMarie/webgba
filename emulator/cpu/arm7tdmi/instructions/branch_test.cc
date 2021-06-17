extern "C" {
#include "emulator/cpu/arm7tdmi/instructions/branch.h"
}

#include <strings.h>

#include "googletest/include/gtest/gtest.h"

ArmGeneralPurposeRegisters CreateArmGeneralPurposeRegistersRegisters() {
  ArmGeneralPurposeRegisters registers;
  memset(&registers, 0, sizeof(ArmGeneralPurposeRegisters));
  return registers;
}

bool ArmGeneralPurposeRegistersAreZero(const ArmGeneralPurposeRegisters& regs) {
  auto zero = CreateArmGeneralPurposeRegistersRegisters();
  return !memcmp(&zero, &regs, sizeof(ArmGeneralPurposeRegisters));
}

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
  auto registers = CreateArmGeneralPurposeRegistersRegisters();

  registers.pc = 208u;
  ArmB(&registers, 100);
  EXPECT_EQ(608u, registers.pc);

  registers.pc = 0u;
  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
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
  EXPECT_EQ(604u, registers.gprs.pc);

  registers.gprs.pc = 0u;
  registers.gprs.lr = 0u;
  registers.cpsr.thumb = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}