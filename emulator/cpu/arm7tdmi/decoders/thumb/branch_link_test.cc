extern "C" {
#include "emulator/cpu/arm7tdmi/decoders/thumb/branch_link.h"
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

TEST(ThumbBL1, BranchLink1) {
  auto registers = CreateArmAllRegistersRegisters();

  registers.current.user.cpsr.thumb = true;
  registers.current.user.gprs.pc = 208u;
  ThumbBL1(&registers, 100);
  EXPECT_EQ(308u, registers.current.user.gprs.lr);
  EXPECT_EQ(208u, registers.current.user.gprs.pc);

  registers.current.user.cpsr.thumb = false;
  registers.current.user.gprs.pc = 0u;
  registers.current.user.gprs.lr = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ThumbBL2, BranchLink2) {
  auto registers = CreateArmAllRegistersRegisters();

  registers.current.user.cpsr.thumb = true;
  registers.current.user.gprs.pc = 208u;
  registers.current.user.gprs.lr = 308u;
  ThumbBL2(&registers, 100);
  EXPECT_EQ(207u, registers.current.user.gprs.lr);
  EXPECT_EQ(410u, registers.current.user.gprs.pc);

  registers.current.user.cpsr.thumb = false;
  registers.current.user.gprs.pc = 0u;
  registers.current.user.gprs.lr = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}