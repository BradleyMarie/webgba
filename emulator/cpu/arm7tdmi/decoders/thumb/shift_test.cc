extern "C" {
#include "emulator/cpu/arm7tdmi/decoders/thumb/shift.h"
}

#include <cstring>

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

TEST(ThumbASRS_I, NonZero) {
    auto registers = CreateArmUserRegisters();
    registers.gprs.r0_s = -16;
    ThumbASRS_I(&registers, REGISTER_R1, REGISTER_R0, 2u);
    EXPECT_EQ(-4, registers.gprs.r1_s);
    EXPECT_EQ(-16, registers.gprs.r0_s);
    EXPECT_FALSE(registers.cpsr.carry);
    EXPECT_TRUE(registers.cpsr.negative);
    EXPECT_FALSE(registers.cpsr.zero);

    registers.gprs.r0 = 0u;
    registers.gprs.r1 = 0u;
    ArmUserRegistersAreZero(registers);
}

TEST(ThumbASRS_I, Negative) {
    auto registers = CreateArmUserRegisters();
    registers.gprs.r0_s = -16;
    ThumbASRS_I(&registers, REGISTER_R1, REGISTER_R0, 0u);
    EXPECT_EQ(-1, registers.gprs.r1_s);
    EXPECT_EQ(-16, registers.gprs.r0_s);
    EXPECT_TRUE(registers.cpsr.carry);
    EXPECT_TRUE(registers.cpsr.negative);
    EXPECT_FALSE(registers.cpsr.zero);

    registers.gprs.r0 = 0u;
    registers.gprs.r1 = 0u;
    ArmUserRegistersAreZero(registers);
}

TEST(ThumbASRS_I, Positive) {
    auto registers = CreateArmUserRegisters();
    registers.gprs.r0_s = 16;
    ThumbASRS_I(&registers, REGISTER_R1, REGISTER_R0, 0u);
    EXPECT_EQ(0, registers.gprs.r1_s);
    EXPECT_EQ(16, registers.gprs.r0_s);
    EXPECT_FALSE(registers.cpsr.carry);
    EXPECT_FALSE(registers.cpsr.negative);
    EXPECT_TRUE(registers.cpsr.zero);

    registers.gprs.r0 = 0u;
    registers.gprs.r1 = 0u;
    ArmUserRegistersAreZero(registers);
}

TEST(ThumbASRS_R, Zero) {
    auto registers = CreateArmUserRegisters();
    registers.gprs.r0 = 0u;
    registers.gprs.r1_s = -16;
    ThumbASRS_R(&registers, REGISTER_R1, REGISTER_R0);
    EXPECT_EQ(-16, registers.gprs.r1_s);
    EXPECT_EQ(0u, registers.gprs.r0);
    EXPECT_FALSE(registers.cpsr.carry);
    EXPECT_TRUE(registers.cpsr.negative);
    EXPECT_FALSE(registers.cpsr.zero);

    registers.gprs.r0 = 0u;
    registers.gprs.r1 = 0u;
    ArmUserRegistersAreZero(registers);
}

TEST(ThumbASRS_R, NonZero) {
    auto registers = CreateArmUserRegisters();
    registers.gprs.r0 = 1u;
    registers.gprs.r1_s = -16;
    ThumbASRS_R(&registers, REGISTER_R1, REGISTER_R0);
    EXPECT_EQ(-8, registers.gprs.r1_s);
    EXPECT_EQ(1u, registers.gprs.r0);
    EXPECT_FALSE(registers.cpsr.carry);
    EXPECT_TRUE(registers.cpsr.negative);
    EXPECT_FALSE(registers.cpsr.zero);

    registers.gprs.r0 = 0u;
    registers.gprs.r1 = 0u;
    ArmUserRegistersAreZero(registers);
}

TEST(ThumbASRS_R, Negative) {
    auto registers = CreateArmUserRegisters();
    registers.gprs.r0 = 32u;
    registers.gprs.r1_s = -16;
    ThumbASRS_R(&registers, REGISTER_R1, REGISTER_R0);
    EXPECT_EQ(-1, registers.gprs.r1_s);
    EXPECT_EQ(32u, registers.gprs.r0);
    EXPECT_TRUE(registers.cpsr.carry);
    EXPECT_TRUE(registers.cpsr.negative);
    EXPECT_FALSE(registers.cpsr.zero);

    registers.gprs.r0 = 0u;
    registers.gprs.r1 = 0u;
    ArmUserRegistersAreZero(registers);
}

TEST(ThumbASRS_R, Positive) {
    auto registers = CreateArmUserRegisters();
    registers.gprs.r0 = 32u;
    registers.gprs.r1_s = 16;
    ThumbASRS_R(&registers, REGISTER_R1, REGISTER_R0);
    EXPECT_EQ(0, registers.gprs.r1_s);
    EXPECT_EQ(32u, registers.gprs.r0);
    EXPECT_FALSE(registers.cpsr.carry);
    EXPECT_FALSE(registers.cpsr.negative);
    EXPECT_TRUE(registers.cpsr.zero);

    registers.gprs.r0 = 0u;
    registers.gprs.r1 = 0u;
    ArmUserRegistersAreZero(registers);
}

TEST(ThumbLSLS_I, NonZero) {
    auto registers = CreateArmUserRegisters();
    registers.gprs.r0 = 16u;
    ThumbLSLS_I(&registers, REGISTER_R1, REGISTER_R0, 1u);
    EXPECT_EQ(32u, registers.gprs.r1);
    EXPECT_EQ(16u, registers.gprs.r0);
    EXPECT_FALSE(registers.cpsr.carry);
    EXPECT_FALSE(registers.cpsr.negative);
    EXPECT_FALSE(registers.cpsr.zero);

    registers.gprs.r0 = 0u;
    registers.gprs.r1 = 0u;
    ArmUserRegistersAreZero(registers);
}

TEST(ThumbLSLS_I, Zero) {
    auto registers = CreateArmUserRegisters();
    registers.gprs.r0 = 16u;
    ThumbLSLS_I(&registers, REGISTER_R1, REGISTER_R0, 0u);
    EXPECT_EQ(16u, registers.gprs.r1);
    EXPECT_EQ(16u, registers.gprs.r0);
    EXPECT_FALSE(registers.cpsr.carry);
    EXPECT_FALSE(registers.cpsr.negative);
    EXPECT_FALSE(registers.cpsr.zero);

    registers.gprs.r0 = 0u;
    registers.gprs.r1 = 0u;
    ArmUserRegistersAreZero(registers);
}

TEST(ThumbLSLS_I, CarryAndNegative) {
    auto registers = CreateArmUserRegisters();
    registers.gprs.r0 = 0xFFFFFFFFu;
    ThumbLSLS_I(&registers, REGISTER_R1, REGISTER_R0, 1u);
    EXPECT_EQ(0xFFFFFFFEu, registers.gprs.r1);
    EXPECT_EQ(0xFFFFFFFFu, registers.gprs.r0);
    EXPECT_TRUE(registers.cpsr.carry);
    EXPECT_TRUE(registers.cpsr.negative);
    EXPECT_FALSE(registers.cpsr.zero);

    registers.gprs.r0 = 0u;
    registers.gprs.r1 = 0u;
    ArmUserRegistersAreZero(registers);
}

TEST(ThumbLSLS_I, ZeroFlag) {
    auto registers = CreateArmUserRegisters();
    registers.gprs.r0 = 0xFFFFFFF0u;
    ThumbLSLS_I(&registers, REGISTER_R1, REGISTER_R0, 31u);
    EXPECT_EQ(0u, registers.gprs.r1);
    EXPECT_EQ(0xFFFFFFF0u, registers.gprs.r0);
    EXPECT_FALSE(registers.cpsr.carry);
    EXPECT_FALSE(registers.cpsr.negative);
    EXPECT_TRUE(registers.cpsr.zero);

    registers.gprs.r0 = 0u;
    registers.gprs.r1 = 0u;
    ArmUserRegistersAreZero(registers);
}

TEST(ThumbLSLS_R, NonZero) {
    auto registers = CreateArmUserRegisters();
    registers.gprs.r0 = 16u;
    registers.gprs.r1 = 1u;
    ThumbLSLS_R(&registers, REGISTER_R0, REGISTER_R1);
    EXPECT_EQ(1u, registers.gprs.r1);
    EXPECT_EQ(32u, registers.gprs.r0);
    EXPECT_FALSE(registers.cpsr.carry);
    EXPECT_FALSE(registers.cpsr.negative);
    EXPECT_FALSE(registers.cpsr.zero);

    registers.gprs.r0 = 0u;
    registers.gprs.r1 = 0u;
    ArmUserRegistersAreZero(registers);
}

TEST(ThumbLSLS_R, Zero) {
    auto registers = CreateArmUserRegisters();
    registers.gprs.r0 = 16u;
    registers.gprs.r1 = 0u;
    ThumbLSLS_R(&registers, REGISTER_R0, REGISTER_R1);
    EXPECT_EQ(0u, registers.gprs.r1);
    EXPECT_EQ(16u, registers.gprs.r0);
    EXPECT_FALSE(registers.cpsr.carry);
    EXPECT_FALSE(registers.cpsr.negative);
    EXPECT_FALSE(registers.cpsr.zero);

    registers.gprs.r0 = 0u;
    registers.gprs.r1 = 0u;
    ArmUserRegistersAreZero(registers);
}

TEST(ThumbLSLS_R, CarryAndNegative) {
    auto registers = CreateArmUserRegisters();
    registers.gprs.r0 = 0xFFFFFFFFu;
    registers.gprs.r1 = 1u;
    ThumbLSLS_R(&registers, REGISTER_R0, REGISTER_R1);
    EXPECT_EQ(1u, registers.gprs.r1);
    EXPECT_EQ(0xFFFFFFFEu, registers.gprs.r0);
    EXPECT_TRUE(registers.cpsr.carry);
    EXPECT_TRUE(registers.cpsr.negative);
    EXPECT_FALSE(registers.cpsr.zero);

    registers.gprs.r0 = 0u;
    registers.gprs.r1 = 0u;
    ArmUserRegistersAreZero(registers);
}

TEST(ThumbLSLS_R, ZeroFlag) {
    auto registers = CreateArmUserRegisters();
    registers.gprs.r0 = 0xFFFFFFFFu;
    registers.gprs.r1 = 32u;
    ThumbLSLS_R(&registers, REGISTER_R0, REGISTER_R1);
    EXPECT_EQ(32u, registers.gprs.r1);
    EXPECT_EQ(0u, registers.gprs.r0);
    EXPECT_TRUE(registers.cpsr.carry);
    EXPECT_FALSE(registers.cpsr.negative);
    EXPECT_TRUE(registers.cpsr.zero);

    registers.gprs.r0 = 0u;
    registers.gprs.r1 = 0u;
    ArmUserRegistersAreZero(registers);
}

TEST(ThumbLSLS_R, Large) {
    auto registers = CreateArmUserRegisters();
    registers.gprs.r0 = 0xFFFFFFFFu;
    registers.gprs.r1 = 33u;
    ThumbLSLS_R(&registers, REGISTER_R0, REGISTER_R1);
    EXPECT_EQ(33u, registers.gprs.r1);
    EXPECT_EQ(0u, registers.gprs.r0);
    EXPECT_FALSE(registers.cpsr.carry);
    EXPECT_FALSE(registers.cpsr.negative);
    EXPECT_TRUE(registers.cpsr.zero);

    registers.gprs.r0 = 0u;
    registers.gprs.r1 = 0u;
    ArmUserRegistersAreZero(registers);
}