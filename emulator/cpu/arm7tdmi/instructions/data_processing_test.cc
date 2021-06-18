extern "C" {
#include "emulator/cpu/arm7tdmi/instructions/data_processing.h"
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

TEST(ArmADD, Compute) {
  auto registers = CreateArmGeneralPurposeRegistersRegisters();

  registers.r0 = 1u;
  ArmADD(&registers, REGISTER_R1, REGISTER_R0, 2u);
  EXPECT_EQ(3u, registers.r1);
  EXPECT_EQ(1u, registers.r0);

  registers.r0 = 0u;
  registers.r1 = 0u;
  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
}

TEST(ArmADD, SameSourceAndDest) {
  auto registers = CreateArmGeneralPurposeRegistersRegisters();

  registers.r0 = 2u;
  ArmADD(&registers, REGISTER_R0, REGISTER_R0, 2u);
  EXPECT_EQ(4u, registers.r0);

  registers.r0 = 0u;
  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
}

TEST(ArmADDS, Compute) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 1u;
  ArmADDS(&registers, REGISTER_R1, REGISTER_R0, 2u);
  EXPECT_EQ(3u, registers.gprs.r1);
  EXPECT_EQ(1u, registers.gprs.r0);

  registers.gprs.r0 = 0u;
  registers.gprs.r1 = 0u;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmADDS, SameSourceAndDest) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 2u;
  ArmADDS(&registers, REGISTER_R0, REGISTER_R0, 2u);
  EXPECT_EQ(4u, registers.gprs.r0);

  registers.gprs.r0 = 0u;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmADDS, Zero) {
  auto registers = CreateArmUserRegisters();

  ArmADDS(&registers, REGISTER_R0, REGISTER_R0, REGISTER_R0);
  EXPECT_TRUE(registers.cpsr.zero);

  registers.cpsr.zero = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmADDS, Negative) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = UINT32_MAX;
  ArmADDS(&registers, REGISTER_R0, REGISTER_R0, 0);
  EXPECT_EQ(UINT32_MAX, registers.gprs.r0);
  EXPECT_TRUE(registers.cpsr.negative);

  registers.gprs.r0 = 0u;
  registers.cpsr.negative = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmADDS, Carry) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = UINT32_MAX;
  ArmADDS(&registers, REGISTER_R0, REGISTER_R0, 1);
  EXPECT_EQ(0u, registers.gprs.r0);
  EXPECT_TRUE(registers.cpsr.carry);

  registers.gprs.r0 = 0u;
  registers.cpsr.carry = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmADDS, Overflow) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0_s = INT32_MAX;
  ArmADDS(&registers, REGISTER_R0, REGISTER_R0, 1);
  EXPECT_EQ(INT32_MIN, registers.gprs.r0_s);
  EXPECT_TRUE(registers.cpsr.overflow);
  EXPECT_TRUE(registers.cpsr.negative);

  registers.gprs.r0_s = 0;
  registers.cpsr.overflow = false;
  registers.cpsr.negative = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmADDS, Underflow) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0_s = -1;
  registers.gprs.r1_s = INT32_MIN;
  ArmADDS(&registers, REGISTER_R0, REGISTER_R0, registers.gprs.r1);
  EXPECT_EQ(INT32_MAX, registers.gprs.r0_s);
  EXPECT_TRUE(registers.cpsr.overflow);
  EXPECT_TRUE(registers.cpsr.carry);

  registers.gprs.r0_s = 0;
  registers.gprs.r1_s = 0;
  registers.cpsr.overflow = false;
  registers.cpsr.carry = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}