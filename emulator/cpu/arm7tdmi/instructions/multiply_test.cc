extern "C" {
#include "emulator/cpu/arm7tdmi/instructions/multiply.h"
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

TEST(ArmMUL, Compute) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 2u;
  registers.current.user.gprs.r1 = 50u;
  ArmMUL(&registers, REGISTER_R2, REGISTER_R0, REGISTER_R1);
  EXPECT_EQ(2u, registers.current.user.gprs.r0);
  EXPECT_EQ(50u, registers.current.user.gprs.r1);
  EXPECT_EQ(100u, registers.current.user.gprs.r2);
  EXPECT_EQ(0x4u, registers.current.user.gprs.pc);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r2 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmMUL, SameSourceAndDest) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 2u;
  ArmMUL(&registers, REGISTER_R0, REGISTER_R0, REGISTER_R0);
  EXPECT_EQ(4u, registers.current.user.gprs.r0);
  EXPECT_EQ(0x4u, registers.current.user.gprs.pc);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmMULS, Compute) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 2u;
  registers.current.user.gprs.r1 = 50u;
  ArmMULS(&registers, REGISTER_R2, REGISTER_R0, REGISTER_R1);
  EXPECT_EQ(2u, registers.current.user.gprs.r0);
  EXPECT_EQ(50u, registers.current.user.gprs.r1);
  EXPECT_EQ(100u, registers.current.user.gprs.r2);
  EXPECT_EQ(0x4u, registers.current.user.gprs.pc);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r2 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmMULS, SameSourceAndDest) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.gprs.r0 = 2u;
  ArmMULS(&registers, REGISTER_R0, REGISTER_R0, REGISTER_R0);
  EXPECT_EQ(4u, registers.current.user.gprs.r0);
  EXPECT_EQ(0x4u, registers.current.user.gprs.pc);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmMULS, Zero) {
  auto registers = CreateArmAllRegisters();

  ArmMULS(&registers, REGISTER_R0, REGISTER_R0, REGISTER_R0);
  EXPECT_EQ(0x4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.zero);

  registers.current.user.gprs.pc = 0u;
  registers.current.user.cpsr.zero = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmMULS, Negative) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 64000u;
  registers.current.user.gprs.r1 = 66000u;
  ArmMULS(&registers, REGISTER_R2, REGISTER_R0, REGISTER_R1);
  EXPECT_EQ(4224000000u, registers.current.user.gprs.r2);
  EXPECT_EQ(0x4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.negative);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r2 = 0u;
  registers.current.user.gprs.pc = 0u;
  registers.current.user.cpsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmMLA, Compute) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 2u;
  registers.current.user.gprs.r1 = 50u;
  registers.current.user.gprs.r3 = 10u;
  ArmMLA(&registers, REGISTER_R2, REGISTER_R0, REGISTER_R1, REGISTER_R3);
  EXPECT_EQ(2u, registers.current.user.gprs.r0);
  EXPECT_EQ(50u, registers.current.user.gprs.r1);
  EXPECT_EQ(110u, registers.current.user.gprs.r2);
  EXPECT_EQ(10u, registers.current.user.gprs.r3);
  EXPECT_EQ(0x4u, registers.current.user.gprs.pc);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r2 = 0u;
  registers.current.user.gprs.r3 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmMLA, SameSourceAndDest) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 2u;
  ArmMLA(&registers, REGISTER_R0, REGISTER_R0, REGISTER_R0, REGISTER_R0);
  EXPECT_EQ(6u, registers.current.user.gprs.r0);
  EXPECT_EQ(0x4u, registers.current.user.gprs.pc);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmMLAS, Compute) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 2u;
  registers.current.user.gprs.r1 = 50u;
  registers.current.user.gprs.r3 = 10u;
  ArmMLAS(&registers, REGISTER_R2, REGISTER_R0, REGISTER_R1, REGISTER_R3);
  EXPECT_EQ(2u, registers.current.user.gprs.r0);
  EXPECT_EQ(50u, registers.current.user.gprs.r1);
  EXPECT_EQ(110u, registers.current.user.gprs.r2);
  EXPECT_EQ(10u, registers.current.user.gprs.r3);
  EXPECT_EQ(0x4u, registers.current.user.gprs.pc);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r2 = 0u;
  registers.current.user.gprs.r3 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmMLAS, SameSourceAndDest) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.gprs.r0 = 2u;
  ArmMLAS(&registers, REGISTER_R0, REGISTER_R0, REGISTER_R0, REGISTER_R0);
  EXPECT_EQ(6u, registers.current.user.gprs.r0);
  EXPECT_EQ(0x4u, registers.current.user.gprs.pc);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmMLAS, Zero) {
  auto registers = CreateArmAllRegisters();

  ArmMLAS(&registers, REGISTER_R0, REGISTER_R0, REGISTER_R0, REGISTER_R0);
  EXPECT_EQ(0x4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.zero);

  registers.current.user.gprs.pc = 0u;
  registers.current.user.cpsr.zero = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmMLAS, Negative) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 64000u;
  registers.current.user.gprs.r1 = 66000u;
  registers.current.user.gprs.r3 = 1u;
  ArmMLAS(&registers, REGISTER_R2, REGISTER_R0, REGISTER_R1, REGISTER_R3);
  EXPECT_EQ(4224000001u, registers.current.user.gprs.r2);
  EXPECT_EQ(0x4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.negative);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r2 = 0u;
  registers.current.user.gprs.r3 = 0u;
  registers.current.user.gprs.pc = 0u;
  registers.current.user.cpsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}