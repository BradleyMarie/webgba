extern "C" {
#include "emulator/cpu/arm7tdmi/instructions/branch.h"
}

#include <cstring>

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

TEST(ArmB, Branch) {
  auto registers = CreateArmGeneralPurposeRegistersRegisters();

  registers.pc = 208u;
  ArmB(&registers, 100u);
  EXPECT_EQ(308u, registers.pc);

  registers.pc = 0u;
  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
}