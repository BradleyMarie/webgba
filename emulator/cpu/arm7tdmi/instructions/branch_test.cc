extern "C" {
#include "emulator/cpu/arm7tdmi/instructions/branch.h"
}

#include <cstring>

#include "googletest/include/gtest/gtest.h"

ArmAllRegisters CreateArmAllRegisters() {
  ArmAllRegisters registers;
  memset(&registers, 0, sizeof(ArmAllRegisters));
  return registers;
}

bool ArmArmAllRegistersAreZero(const ArmAllRegisters& regs) {
  auto zero = CreateArmAllRegisters();
  return !memcmp(&zero, &regs, sizeof(ArmAllRegisters));
}

TEST(ArmB, Branch) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.pc = 208u;
  ArmB(&registers, 100u);
  EXPECT_EQ(308u, ArmNextInstruction(&registers));

  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmArmAllRegistersAreZero(registers));
}