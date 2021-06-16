extern "C" {
#include "emulator/cpu/arm7tdmi/instructions/branch_exchange.h"
}

#include <strings.h>

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

TEST(ArmBX, BranchExchange) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 65536u;
  ArmBX(&registers, REGISTER_R0);
  EXPECT_EQ(65536u, registers.gprs.pc);
  EXPECT_FALSE(registers.cpsr.thumb);

  registers.gprs.r0 = 0u;
  registers.gprs.pc = 0u;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmBX, BranchExchangeThumb) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 65537u;
  ArmBX(&registers, REGISTER_R0);
  EXPECT_EQ(65536u, registers.gprs.pc);
  EXPECT_TRUE(registers.cpsr.thumb);

  registers.gprs.r0 = 0u;
  registers.gprs.pc = 0u;
  registers.cpsr.thumb = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}