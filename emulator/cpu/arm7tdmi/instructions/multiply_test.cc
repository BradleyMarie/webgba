extern "C" {
#include "emulator/cpu/arm7tdmi/instructions/multiply.h"
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

TEST(ArmMUL, Compute) {
  auto registers = CreateArmGeneralPurposeRegistersRegisters();

  registers.r0 = 2u;
  registers.r1 = 50u;
  ArmMUL(&registers, REGISTER_R2, REGISTER_R0, REGISTER_R1);
  EXPECT_EQ(2u, registers.r0);
  EXPECT_EQ(50u, registers.r1);
  EXPECT_EQ(100u, registers.r2);

  registers.r0 = 0u;
  registers.r1 = 0u;
  registers.r2 = 0u;
  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
}

TEST(ArmMUL, SameSourceAndDest) {
  auto registers = CreateArmGeneralPurposeRegistersRegisters();

  registers.r0 = 2u;
  ArmMUL(&registers, REGISTER_R0, REGISTER_R0, REGISTER_R0);
  EXPECT_EQ(4u, registers.r0);

  registers.r0 = 0u;
  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
}

TEST(ArmMULS, Compute) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 2u;
  registers.gprs.r1 = 50u;
  ArmMULS(&registers, REGISTER_R2, REGISTER_R0, REGISTER_R1);
  EXPECT_EQ(2u, registers.gprs.r0);
  EXPECT_EQ(50u, registers.gprs.r1);
  EXPECT_EQ(100u, registers.gprs.r2);

  registers.gprs.r0 = 0u;
  registers.gprs.r1 = 0u;
  registers.gprs.r2 = 0u;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmMULS, SameSourceAndDest) {
  auto registers = CreateArmUserRegisters();
  registers.gprs.r0 = 2u;
  ArmMULS(&registers, REGISTER_R0, REGISTER_R0, REGISTER_R0);
  EXPECT_EQ(4u, registers.gprs.r0);

  registers.gprs.r0 = 0u;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmMULS, Zero) {
  auto registers = CreateArmUserRegisters();

  ArmMULS(&registers, REGISTER_R0, REGISTER_R0, REGISTER_R0);
  EXPECT_TRUE(registers.cpsr.zero);

  registers.cpsr.zero = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmMULS, Negative) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 64000u;
  registers.gprs.r1 = 66000u;
  ArmMULS(&registers, REGISTER_R2, REGISTER_R0, REGISTER_R1);
  EXPECT_EQ(4224000000u, registers.gprs.r2);
  EXPECT_TRUE(registers.cpsr.negative);

  registers.gprs.r0 = 0u;
  registers.gprs.r1 = 0u;
  registers.gprs.r2 = 0u;
  registers.cpsr.negative = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}