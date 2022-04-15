extern "C" {
#include "emulator/cpu/arm7tdmi/decoders/thumb/shift.h"
}

#include <cstring>

#include "googletest/include/gtest/gtest.h"

ArmAllRegisters CreateArmAllRegisters() {
  ArmAllRegisters registers;
  memset(&registers, 0, sizeof(ArmAllRegisters));
  registers.current.user.cpsr.thumb = true;
  return registers;
}

bool ArmAllRegistersAreZero(const ArmAllRegisters& regs) {
  auto zero = CreateArmAllRegisters();
  zero.current.user.cpsr.thumb = true;
  return !memcmp(&zero, &regs, sizeof(ArmAllRegisters));
}

TEST(ThumbASRS_I, NonZero) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.gprs.r0_s = -16;
  ThumbASRS_I(&registers, REGISTER_R1, REGISTER_R0, 2u);
  EXPECT_EQ(-4, registers.current.user.gprs.r1_s);
  EXPECT_EQ(-16, registers.current.user.gprs.r0_s);
  EXPECT_FALSE(registers.current.user.cpsr.carry);
  EXPECT_TRUE(registers.current.user.cpsr.negative);
  EXPECT_FALSE(registers.current.user.cpsr.zero);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  ArmAllRegistersAreZero(registers);
}

TEST(ThumbASRS_I, Negative) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.gprs.r0_s = -16;
  ThumbASRS_I(&registers, REGISTER_R1, REGISTER_R0, 0u);
  EXPECT_EQ(-1, registers.current.user.gprs.r1_s);
  EXPECT_EQ(-16, registers.current.user.gprs.r0_s);
  EXPECT_TRUE(registers.current.user.cpsr.carry);
  EXPECT_TRUE(registers.current.user.cpsr.negative);
  EXPECT_FALSE(registers.current.user.cpsr.zero);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  ArmAllRegistersAreZero(registers);
}

TEST(ThumbASRS_I, Positive) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.gprs.r0_s = 16;
  ThumbASRS_I(&registers, REGISTER_R1, REGISTER_R0, 0u);
  EXPECT_EQ(0, registers.current.user.gprs.r1_s);
  EXPECT_EQ(16, registers.current.user.gprs.r0_s);
  EXPECT_FALSE(registers.current.user.cpsr.carry);
  EXPECT_FALSE(registers.current.user.cpsr.negative);
  EXPECT_TRUE(registers.current.user.cpsr.zero);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  ArmAllRegistersAreZero(registers);
}

TEST(ThumbASRS_R, Zero) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1_s = -16;
  ThumbASRS_R(&registers, REGISTER_R1, REGISTER_R0);
  EXPECT_EQ(-16, registers.current.user.gprs.r1_s);
  EXPECT_EQ(0u, registers.current.user.gprs.r0);
  EXPECT_FALSE(registers.current.user.cpsr.carry);
  EXPECT_TRUE(registers.current.user.cpsr.negative);
  EXPECT_FALSE(registers.current.user.cpsr.zero);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  ArmAllRegistersAreZero(registers);
}

TEST(ThumbASRS_R, NonZero) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.gprs.r0 = 1u;
  registers.current.user.gprs.r1_s = -16;
  ThumbASRS_R(&registers, REGISTER_R1, REGISTER_R0);
  EXPECT_EQ(-8, registers.current.user.gprs.r1_s);
  EXPECT_EQ(1u, registers.current.user.gprs.r0);
  EXPECT_FALSE(registers.current.user.cpsr.carry);
  EXPECT_TRUE(registers.current.user.cpsr.negative);
  EXPECT_FALSE(registers.current.user.cpsr.zero);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  ArmAllRegistersAreZero(registers);
}

TEST(ThumbASRS_R, Negative) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.gprs.r0 = 32u;
  registers.current.user.gprs.r1_s = -16;
  ThumbASRS_R(&registers, REGISTER_R1, REGISTER_R0);
  EXPECT_EQ(-1, registers.current.user.gprs.r1_s);
  EXPECT_EQ(32u, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.carry);
  EXPECT_TRUE(registers.current.user.cpsr.negative);
  EXPECT_FALSE(registers.current.user.cpsr.zero);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  ArmAllRegistersAreZero(registers);
}

TEST(ThumbASRS_R, Positive) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.gprs.r0 = 32u;
  registers.current.user.gprs.r1_s = 16;
  ThumbASRS_R(&registers, REGISTER_R1, REGISTER_R0);
  EXPECT_EQ(0, registers.current.user.gprs.r1_s);
  EXPECT_EQ(32u, registers.current.user.gprs.r0);
  EXPECT_FALSE(registers.current.user.cpsr.carry);
  EXPECT_FALSE(registers.current.user.cpsr.negative);
  EXPECT_TRUE(registers.current.user.cpsr.zero);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  ArmAllRegistersAreZero(registers);
}

TEST(ThumbLSLS_I, NonZero) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.gprs.r0 = 16u;
  ThumbLSLS_I(&registers, REGISTER_R1, REGISTER_R0, 1u);
  EXPECT_EQ(32u, registers.current.user.gprs.r1);
  EXPECT_EQ(16u, registers.current.user.gprs.r0);
  EXPECT_FALSE(registers.current.user.cpsr.carry);
  EXPECT_FALSE(registers.current.user.cpsr.negative);
  EXPECT_FALSE(registers.current.user.cpsr.zero);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  ArmAllRegistersAreZero(registers);
}

TEST(ThumbLSLS_I, Zero) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.gprs.r0 = 16u;
  ThumbLSLS_I(&registers, REGISTER_R1, REGISTER_R0, 0u);
  EXPECT_EQ(16u, registers.current.user.gprs.r1);
  EXPECT_EQ(16u, registers.current.user.gprs.r0);
  EXPECT_FALSE(registers.current.user.cpsr.carry);
  EXPECT_FALSE(registers.current.user.cpsr.negative);
  EXPECT_FALSE(registers.current.user.cpsr.zero);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  ArmAllRegistersAreZero(registers);
}

TEST(ThumbLSLS_I, CarryAndNegative) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.gprs.r0 = 0xFFFFFFFFu;
  ThumbLSLS_I(&registers, REGISTER_R1, REGISTER_R0, 1u);
  EXPECT_EQ(0xFFFFFFFEu, registers.current.user.gprs.r1);
  EXPECT_EQ(0xFFFFFFFFu, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.carry);
  EXPECT_TRUE(registers.current.user.cpsr.negative);
  EXPECT_FALSE(registers.current.user.cpsr.zero);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  ArmAllRegistersAreZero(registers);
}

TEST(ThumbLSLS_I, ZeroFlag) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.gprs.r0 = 0xFFFFFFF0u;
  ThumbLSLS_I(&registers, REGISTER_R1, REGISTER_R0, 31u);
  EXPECT_EQ(0u, registers.current.user.gprs.r1);
  EXPECT_EQ(0xFFFFFFF0u, registers.current.user.gprs.r0);
  EXPECT_FALSE(registers.current.user.cpsr.carry);
  EXPECT_FALSE(registers.current.user.cpsr.negative);
  EXPECT_TRUE(registers.current.user.cpsr.zero);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  ArmAllRegistersAreZero(registers);
}

TEST(ThumbLSLS_R, NonZero) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.gprs.r0 = 16u;
  registers.current.user.gprs.r1 = 1u;
  ThumbLSLS_R(&registers, REGISTER_R0, REGISTER_R1);
  EXPECT_EQ(1u, registers.current.user.gprs.r1);
  EXPECT_EQ(32u, registers.current.user.gprs.r0);
  EXPECT_FALSE(registers.current.user.cpsr.carry);
  EXPECT_FALSE(registers.current.user.cpsr.negative);
  EXPECT_FALSE(registers.current.user.cpsr.zero);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  ArmAllRegistersAreZero(registers);
}

TEST(ThumbLSLS_R, Zero) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.gprs.r0 = 16u;
  registers.current.user.gprs.r1 = 0u;
  ThumbLSLS_R(&registers, REGISTER_R0, REGISTER_R1);
  EXPECT_EQ(0u, registers.current.user.gprs.r1);
  EXPECT_EQ(16u, registers.current.user.gprs.r0);
  EXPECT_FALSE(registers.current.user.cpsr.carry);
  EXPECT_FALSE(registers.current.user.cpsr.negative);
  EXPECT_FALSE(registers.current.user.cpsr.zero);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  ArmAllRegistersAreZero(registers);
}

TEST(ThumbLSLS_R, CarryAndNegative) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.gprs.r0 = 0xFFFFFFFFu;
  registers.current.user.gprs.r1 = 1u;
  ThumbLSLS_R(&registers, REGISTER_R0, REGISTER_R1);
  EXPECT_EQ(1u, registers.current.user.gprs.r1);
  EXPECT_EQ(0xFFFFFFFEu, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.carry);
  EXPECT_TRUE(registers.current.user.cpsr.negative);
  EXPECT_FALSE(registers.current.user.cpsr.zero);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  ArmAllRegistersAreZero(registers);
}

TEST(ThumbLSLS_R, ZeroFlag) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.gprs.r0 = 0xFFFFFFFFu;
  registers.current.user.gprs.r1 = 32u;
  ThumbLSLS_R(&registers, REGISTER_R0, REGISTER_R1);
  EXPECT_EQ(32u, registers.current.user.gprs.r1);
  EXPECT_EQ(0u, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.carry);
  EXPECT_FALSE(registers.current.user.cpsr.negative);
  EXPECT_TRUE(registers.current.user.cpsr.zero);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  ArmAllRegistersAreZero(registers);
}

TEST(ThumbLSLS_R, Large) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.gprs.r0 = 0xFFFFFFFFu;
  registers.current.user.gprs.r1 = 33u;
  ThumbLSLS_R(&registers, REGISTER_R0, REGISTER_R1);
  EXPECT_EQ(33u, registers.current.user.gprs.r1);
  EXPECT_EQ(0u, registers.current.user.gprs.r0);
  EXPECT_FALSE(registers.current.user.cpsr.carry);
  EXPECT_FALSE(registers.current.user.cpsr.negative);
  EXPECT_TRUE(registers.current.user.cpsr.zero);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  ArmAllRegistersAreZero(registers);
}

TEST(ThumbLSRS_I, NonZero) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.gprs.r0 = 16u;
  ThumbLSRS_I(&registers, REGISTER_R1, REGISTER_R0, 1u);
  EXPECT_EQ(8u, registers.current.user.gprs.r1);
  EXPECT_EQ(16u, registers.current.user.gprs.r0);
  EXPECT_FALSE(registers.current.user.cpsr.carry);
  EXPECT_FALSE(registers.current.user.cpsr.negative);
  EXPECT_FALSE(registers.current.user.cpsr.zero);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  ArmAllRegistersAreZero(registers);
}

TEST(ThumbLSRS_I, Zero) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.gprs.r0 = 16u;
  ThumbLSRS_I(&registers, REGISTER_R1, REGISTER_R0, 0u);
  EXPECT_EQ(0u, registers.current.user.gprs.r1);
  EXPECT_EQ(16u, registers.current.user.gprs.r0);
  EXPECT_FALSE(registers.current.user.cpsr.carry);
  EXPECT_FALSE(registers.current.user.cpsr.negative);
  EXPECT_TRUE(registers.current.user.cpsr.zero);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  ArmAllRegistersAreZero(registers);
}

TEST(ThumbLSRS_I, Carry) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.gprs.r0 = 0xFFFFFFFFu;
  ThumbLSRS_I(&registers, REGISTER_R1, REGISTER_R0, 1u);
  EXPECT_EQ(0x7FFFFFFFu, registers.current.user.gprs.r1);
  EXPECT_EQ(0xFFFFFFFFu, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.carry);
  EXPECT_FALSE(registers.current.user.cpsr.negative);
  EXPECT_FALSE(registers.current.user.cpsr.zero);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  ArmAllRegistersAreZero(registers);
}

TEST(ThumbLSRS_R, NonZero) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.gprs.r0 = 16u;
  registers.current.user.gprs.r1 = 1u;
  ThumbLSRS_R(&registers, REGISTER_R0, REGISTER_R1);
  EXPECT_EQ(1u, registers.current.user.gprs.r1);
  EXPECT_EQ(8u, registers.current.user.gprs.r0);
  EXPECT_FALSE(registers.current.user.cpsr.carry);
  EXPECT_FALSE(registers.current.user.cpsr.negative);
  EXPECT_FALSE(registers.current.user.cpsr.zero);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  ArmAllRegistersAreZero(registers);
}

TEST(ThumbLSRS_R, Zero) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.gprs.r0 = 16u;
  registers.current.user.gprs.r1 = 0u;
  ThumbLSRS_R(&registers, REGISTER_R0, REGISTER_R1);
  EXPECT_EQ(0u, registers.current.user.gprs.r1);
  EXPECT_EQ(16u, registers.current.user.gprs.r0);
  EXPECT_FALSE(registers.current.user.cpsr.carry);
  EXPECT_FALSE(registers.current.user.cpsr.negative);
  EXPECT_FALSE(registers.current.user.cpsr.zero);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  ArmAllRegistersAreZero(registers);
}

TEST(ThumbLSRS_R, ThirtyTwo) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.gprs.r0 = 0xFFFFFFFFu;
  registers.current.user.gprs.r1 = 32u;
  ThumbLSRS_R(&registers, REGISTER_R0, REGISTER_R1);
  EXPECT_EQ(32u, registers.current.user.gprs.r1);
  EXPECT_EQ(0u, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.carry);
  EXPECT_FALSE(registers.current.user.cpsr.negative);
  EXPECT_TRUE(registers.current.user.cpsr.zero);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  ArmAllRegistersAreZero(registers);
}

TEST(ThumbLSRS_R, Large) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.gprs.r0 = 0xFFFFFFFFu;
  registers.current.user.gprs.r1 = 33u;
  ThumbLSRS_R(&registers, REGISTER_R0, REGISTER_R1);
  EXPECT_EQ(33u, registers.current.user.gprs.r1);
  EXPECT_EQ(0u, registers.current.user.gprs.r0);
  EXPECT_FALSE(registers.current.user.cpsr.carry);
  EXPECT_FALSE(registers.current.user.cpsr.negative);
  EXPECT_TRUE(registers.current.user.cpsr.zero);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  ArmAllRegistersAreZero(registers);
}

TEST(ThumbRORS, NonZero) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.gprs.r0 = 0xF000000F;
  registers.current.user.gprs.r1 = 1u;
  ThumbRORS(&registers, REGISTER_R0, REGISTER_R1);
  EXPECT_EQ(1u, registers.current.user.gprs.r1);
  EXPECT_EQ(0xF8000007, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.carry);
  EXPECT_TRUE(registers.current.user.cpsr.negative);
  EXPECT_FALSE(registers.current.user.cpsr.zero);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  ArmAllRegistersAreZero(registers);
}

TEST(ThumbRORS, Zero) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.gprs.r0 = 16u;
  registers.current.user.gprs.r1 = 0u;
  ThumbRORS(&registers, REGISTER_R0, REGISTER_R1);
  EXPECT_EQ(0u, registers.current.user.gprs.r1);
  EXPECT_EQ(16u, registers.current.user.gprs.r0);
  EXPECT_FALSE(registers.current.user.cpsr.carry);
  EXPECT_FALSE(registers.current.user.cpsr.negative);
  EXPECT_FALSE(registers.current.user.cpsr.zero);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  ArmAllRegistersAreZero(registers);
}

TEST(ThumbRORS, ThirtyOne) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.gprs.r0 = 0xF000000Fu;
  registers.current.user.gprs.r1 = 31u;
  ThumbRORS(&registers, REGISTER_R0, REGISTER_R1);
  EXPECT_EQ(31u, registers.current.user.gprs.r1);
  EXPECT_EQ(0xE000001Fu, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.carry);
  EXPECT_TRUE(registers.current.user.cpsr.negative);
  EXPECT_FALSE(registers.current.user.cpsr.zero);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  ArmAllRegistersAreZero(registers);
}

TEST(ThumbRORS, ThirtyTwo) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.gprs.r0 = 0xF000000Fu;
  registers.current.user.gprs.r1 = 32u;
  ThumbRORS(&registers, REGISTER_R0, REGISTER_R1);
  EXPECT_EQ(32u, registers.current.user.gprs.r1);
  EXPECT_EQ(0xF000000Fu, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.carry);
  EXPECT_TRUE(registers.current.user.cpsr.negative);
  EXPECT_FALSE(registers.current.user.cpsr.zero);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  ArmAllRegistersAreZero(registers);
}