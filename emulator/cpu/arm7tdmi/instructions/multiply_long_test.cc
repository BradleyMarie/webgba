extern "C" {
#include "emulator/cpu/arm7tdmi/instructions/multiply_long.h"
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

TEST(ArmUMULL, Compute) {
  auto registers = CreateArmGeneralPurposeRegistersRegisters();

  registers.r0 = 65537u;
  registers.r1 = 65539u;
  ArmUMULL(&registers, REGISTER_R3, REGISTER_R2, REGISTER_R0, REGISTER_R1);
  EXPECT_EQ(65537u, registers.r0);
  EXPECT_EQ(65539u, registers.r1);
  EXPECT_EQ(1u, registers.r2);
  EXPECT_EQ(262147u, registers.r3);

  registers.r0 = 0u;
  registers.r1 = 0u;
  registers.r2 = 0u;
  registers.r3 = 0u;
  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
}

TEST(ArmUMULL, ReuseSourceAndDest) {
  auto registers = CreateArmGeneralPurposeRegistersRegisters();

  registers.r0 = 65537u;
  registers.r1 = 65539u;
  ArmUMULL(&registers, REGISTER_R0, REGISTER_R1, REGISTER_R0, REGISTER_R1);
  EXPECT_EQ(262147u, registers.r0);
  EXPECT_EQ(1u, registers.r1);

  registers.r0 = 0u;
  registers.r1 = 0u;
  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
}

TEST(ArmUMULLS, Compute) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 65537u;
  registers.gprs.r1 = 65539u;
  ArmUMULLS(&registers, REGISTER_R3, REGISTER_R2, REGISTER_R0, REGISTER_R1);
  EXPECT_EQ(65537u, registers.gprs.r0);
  EXPECT_EQ(65539u, registers.gprs.r1);
  EXPECT_EQ(1u, registers.gprs.r2);
  EXPECT_EQ(262147u, registers.gprs.r3);

  registers.gprs.r0 = 0u;
  registers.gprs.r1 = 0u;
  registers.gprs.r2 = 0u;
  registers.gprs.r3 = 0u;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmUMULLS, ReuseSourceAndDest) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 65537u;
  registers.gprs.r1 = 65539u;
  ArmUMULLS(&registers, REGISTER_R0, REGISTER_R1, REGISTER_R0, REGISTER_R1);
  EXPECT_EQ(262147u, registers.gprs.r0);
  EXPECT_EQ(1u, registers.gprs.r1);

  registers.gprs.r0 = 0u;
  registers.gprs.r1 = 0u;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmUMULLS, Zero) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 65537u;
  ArmUMULLS(&registers, REGISTER_R3, REGISTER_R2, REGISTER_R0, REGISTER_R1);
  EXPECT_EQ(65537u, registers.gprs.r0);
  EXPECT_TRUE(registers.cpsr.zero);

  registers.gprs.r0 = 0u;
  registers.cpsr.zero = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmUMULLS, Negative) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 0xFFFFFFFF;
  ArmUMULLS(&registers, REGISTER_R1, REGISTER_R0, REGISTER_R0, REGISTER_R0);
  EXPECT_EQ(0xFFFFFFFE, registers.gprs.r0);
  EXPECT_EQ(1u, registers.gprs.r1);
  EXPECT_TRUE(registers.cpsr.negative);

  registers.gprs.r0 = 0u;
  registers.gprs.r1 = 0u;
  registers.cpsr.negative = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}