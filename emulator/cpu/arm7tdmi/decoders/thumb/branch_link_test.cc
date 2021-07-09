extern "C" {
#include "emulator/cpu/arm7tdmi/decoders/thumb/branch_link.h"
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

TEST(ThumbBL1, BranchLink1) {
  auto registers = CreateArmGeneralPurposeRegistersRegisters();

  registers.pc = 208u;
  ThumbBL1(&registers, 100);
  EXPECT_EQ(308u, registers.lr);
  EXPECT_EQ(208u, registers.pc);

  registers.pc = 0u;
  registers.lr = 0u;
  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
}

TEST(ThumbBL2, BranchLink2) {
  auto registers = CreateArmGeneralPurposeRegistersRegisters();

  registers.pc = 208u;
  registers.lr = 308u;
  ThumbBL2(&registers, 100);
  EXPECT_EQ(207u, registers.lr);
  EXPECT_EQ(408u, registers.pc);

  registers.pc = 0u;
  registers.lr = 0u;
  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
}