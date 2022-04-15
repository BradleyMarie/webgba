extern "C" {
#include "emulator/cpu/arm7tdmi/decoders/arm/branch_link.h"
}

#include <cstring>

#include "googletest/include/gtest/gtest.h"

ArmAllRegisters CreateArmAllRegistersRegisters() {
  ArmAllRegisters registers;
  memset(&registers, 0, sizeof(ArmAllRegisters));
  return registers;
}

bool ArmAllRegistersAreZero(const ArmAllRegisters& regs) {
  auto zero = CreateArmAllRegistersRegisters();
  return !memcmp(&zero, &regs, sizeof(ArmAllRegisters));
}

TEST(ArmBL, BranchLink) {
  auto registers = CreateArmAllRegistersRegisters();

  registers.current.user.gprs.pc = 208u;
  ArmBL(&registers, 100u);
  EXPECT_EQ(204u, registers.current.user.gprs.lr);
  EXPECT_EQ(312u, registers.current.user.gprs.pc);

  registers.current.user.gprs.pc = 0u;
  registers.current.user.gprs.lr = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}