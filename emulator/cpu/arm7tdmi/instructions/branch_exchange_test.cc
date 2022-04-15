extern "C" {
#include "emulator/cpu/arm7tdmi/instructions/branch_exchange.h"
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

TEST(ArmBX, BranchExchange) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 65536u;
  ArmBX(&registers, REGISTER_R0);
  EXPECT_EQ(65536u, ArmNextInstruction(&registers));
  EXPECT_FALSE(registers.current.user.cpsr.thumb);
  EXPECT_FALSE(registers.execution_control.thumb);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmBX, BranchExchangeThumb) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 65537u;
  ArmBX(&registers, REGISTER_R0);
  EXPECT_EQ(65536u, ArmNextInstruction(&registers));
  EXPECT_TRUE(registers.current.user.cpsr.thumb);
  EXPECT_TRUE(registers.execution_control.thumb);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.pc = 0u;
  registers.current.user.cpsr.thumb = false;
  registers.execution_control.thumb = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}