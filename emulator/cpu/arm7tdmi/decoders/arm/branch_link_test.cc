extern "C" {
#include "emulator/cpu/arm7tdmi/decoders/arm/branch_link.h"
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

TEST(ArmBL, BranchLink) {
  auto registers = CreateArmGeneralPurposeRegistersRegisters();

  registers.pc = 208u;
  ArmBL(&registers, 100u);
  EXPECT_EQ(204u, registers.lr);
  EXPECT_EQ(312u, registers.pc);

  registers.pc = 0u;
  registers.lr = 0u;
  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
}