extern "C" {
#include "emulator/cpu/arm7tdmi/instructions/multiply_long.h"
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

TEST(ArmUMULL, Compute) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 65537u;
  registers.current.user.gprs.r1 = 65539u;
  ArmUMULL(&registers, REGISTER_R3, REGISTER_R2, REGISTER_R0, REGISTER_R1);
  EXPECT_EQ(65537u, registers.current.user.gprs.r0);
  EXPECT_EQ(65539u, registers.current.user.gprs.r1);
  EXPECT_EQ(1u, registers.current.user.gprs.r2);
  EXPECT_EQ(262147u, registers.current.user.gprs.r3);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r2 = 0u;
  registers.current.user.gprs.r3 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmUMULL, ReuseSourceAndDest) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 65537u;
  registers.current.user.gprs.r1 = 65539u;
  ArmUMULL(&registers, REGISTER_R0, REGISTER_R1, REGISTER_R0, REGISTER_R1);
  EXPECT_EQ(262147u, registers.current.user.gprs.r0);
  EXPECT_EQ(1u, registers.current.user.gprs.r1);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmUMULLS, Compute) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 65537u;
  registers.current.user.gprs.r1 = 65539u;
  ArmUMULLS(&registers, REGISTER_R3, REGISTER_R2, REGISTER_R0, REGISTER_R1);
  EXPECT_EQ(65537u, registers.current.user.gprs.r0);
  EXPECT_EQ(65539u, registers.current.user.gprs.r1);
  EXPECT_EQ(1u, registers.current.user.gprs.r2);
  EXPECT_EQ(262147u, registers.current.user.gprs.r3);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r2 = 0u;
  registers.current.user.gprs.r3 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmUMULLS, ReuseSourceAndDest) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 65537u;
  registers.current.user.gprs.r1 = 65539u;
  ArmUMULLS(&registers, REGISTER_R0, REGISTER_R1, REGISTER_R0, REGISTER_R1);
  EXPECT_EQ(262147u, registers.current.user.gprs.r0);
  EXPECT_EQ(1u, registers.current.user.gprs.r1);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmUMULLS, Zero) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 65537u;
  ArmUMULLS(&registers, REGISTER_R3, REGISTER_R2, REGISTER_R0, REGISTER_R1);
  EXPECT_EQ(65537u, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.zero);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.cpsr.zero = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmUMULLS, Negative) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 0xFFFFFFFF;
  ArmUMULLS(&registers, REGISTER_R1, REGISTER_R0, REGISTER_R0, REGISTER_R0);
  EXPECT_EQ(0xFFFFFFFE, registers.current.user.gprs.r0);
  EXPECT_EQ(1u, registers.current.user.gprs.r1);
  EXPECT_TRUE(registers.current.user.cpsr.negative);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.cpsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmUMLAL, Compute) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 65537u;
  registers.current.user.gprs.r1 = 65539u;
  registers.current.user.gprs.r3 = 1;
  registers.current.user.gprs.r2 = 1;
  ArmUMLAL(&registers, REGISTER_R3, REGISTER_R2, REGISTER_R0, REGISTER_R1);
  EXPECT_EQ(65537u, registers.current.user.gprs.r0);
  EXPECT_EQ(65539u, registers.current.user.gprs.r1);
  EXPECT_EQ(2u, registers.current.user.gprs.r2);
  EXPECT_EQ(262148u, registers.current.user.gprs.r3);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r2 = 0u;
  registers.current.user.gprs.r3 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmUMLAL, ReuseSourceAndDest) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 65537u;
  registers.current.user.gprs.r1 = 1u;
  ArmUMLAL(&registers, REGISTER_R0, REGISTER_R1, REGISTER_R0, REGISTER_R1);
  EXPECT_EQ(131074u, registers.current.user.gprs.r0);
  EXPECT_EQ(1u, registers.current.user.gprs.r1);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmUMLALS, Compute) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 65537u;
  registers.current.user.gprs.r1 = 65539u;
  registers.current.user.gprs.r3 = 1u;
  registers.current.user.gprs.r2 = 1u;
  ArmUMLALS(&registers, REGISTER_R3, REGISTER_R2, REGISTER_R0, REGISTER_R1);
  EXPECT_EQ(65537u, registers.current.user.gprs.r0);
  EXPECT_EQ(65539u, registers.current.user.gprs.r1);
  EXPECT_EQ(2u, registers.current.user.gprs.r2);
  EXPECT_EQ(262148u, registers.current.user.gprs.r3);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r2 = 0u;
  registers.current.user.gprs.r3 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmUMLALS, ReuseSourceAndDest) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 65537u;
  registers.current.user.gprs.r1 = 1u;
  ArmUMLALS(&registers, REGISTER_R0, REGISTER_R1, REGISTER_R0, REGISTER_R1);
  EXPECT_EQ(131074u, registers.current.user.gprs.r0);
  EXPECT_EQ(1u, registers.current.user.gprs.r1);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmUMLALS, Zero) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 65537u;
  ArmUMLALS(&registers, REGISTER_R3, REGISTER_R2, REGISTER_R0, REGISTER_R1);
  EXPECT_EQ(65537u, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.zero);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.cpsr.zero = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmUMLALS, Negative) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 0xFFFFFFFF;
  ArmUMLALS(&registers, REGISTER_R1, REGISTER_R0, REGISTER_R2, REGISTER_R3);
  EXPECT_EQ(0xFFFFFFFF, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.negative);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.cpsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmSMULL, Compute) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0_s = 65537;
  registers.current.user.gprs.r1_s = -1;
  ArmSMULL(&registers, REGISTER_R3, REGISTER_R2, REGISTER_R0, REGISTER_R1);
  EXPECT_EQ(65537, registers.current.user.gprs.r0_s);
  EXPECT_EQ(-1, registers.current.user.gprs.r1_s);
  EXPECT_EQ(-1, registers.current.user.gprs.r2_s);
  EXPECT_EQ(-65537, registers.current.user.gprs.r3_s);

  registers.current.user.gprs.r0_s = 0;
  registers.current.user.gprs.r1_s = 0;
  registers.current.user.gprs.r2_s = 0;
  registers.current.user.gprs.r3_s = 0;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmSMULL, ReuseSourceAndDest) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0_s = 65537;
  registers.current.user.gprs.r1_s = -1;
  ArmSMULL(&registers, REGISTER_R1, REGISTER_R0, REGISTER_R0, REGISTER_R1);
  EXPECT_EQ(-65537, registers.current.user.gprs.r1_s);
  EXPECT_EQ(-1, registers.current.user.gprs.r0_s);

  registers.current.user.gprs.r0_s = 0;
  registers.current.user.gprs.r1_s = 0;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmSMULLS, Compute) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0_s = 65537;
  registers.current.user.gprs.r1_s = 65539;
  ArmSMULLS(&registers, REGISTER_R3, REGISTER_R2, REGISTER_R0, REGISTER_R1);
  EXPECT_EQ(65537, registers.current.user.gprs.r0_s);
  EXPECT_EQ(65539, registers.current.user.gprs.r1_s);
  EXPECT_EQ(1, registers.current.user.gprs.r2_s);
  EXPECT_EQ(262147, registers.current.user.gprs.r3_s);

  registers.current.user.gprs.r0_s = 0;
  registers.current.user.gprs.r1_s = 0;
  registers.current.user.gprs.r2_s = 0;
  registers.current.user.gprs.r3_s = 0;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmSMULLS, ReuseSourceAndDest) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0_s = 65537u;
  registers.current.user.gprs.r1_s = 65539u;
  ArmSMULLS(&registers, REGISTER_R0, REGISTER_R1, REGISTER_R0, REGISTER_R1);
  EXPECT_EQ(262147, registers.current.user.gprs.r0_s);
  EXPECT_EQ(1, registers.current.user.gprs.r1_s);

  registers.current.user.gprs.r0_s = 0;
  registers.current.user.gprs.r1_s = 0;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmSMULLS, Zero) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0_s = 65537u;
  ArmSMULLS(&registers, REGISTER_R3, REGISTER_R2, REGISTER_R0, REGISTER_R1);
  EXPECT_EQ(65537u, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.zero);

  registers.current.user.gprs.r0_s = 0;
  registers.current.user.cpsr.zero = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmSMULLS, Negative) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0_s = -1;
  registers.current.user.gprs.r1_s = 1;
  ArmSMULLS(&registers, REGISTER_R1, REGISTER_R0, REGISTER_R1, REGISTER_R0);
  EXPECT_EQ(-1, registers.current.user.gprs.r0_s);
  EXPECT_EQ(-1, registers.current.user.gprs.r1_s);
  EXPECT_TRUE(registers.current.user.cpsr.negative);

  registers.current.user.gprs.r0_s = 0;
  registers.current.user.gprs.r1_s = 0;
  registers.current.user.cpsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmSMLAL, Compute) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0_s = 65537;
  registers.current.user.gprs.r1_s = -1;
  registers.current.user.gprs.r2_s = -2;
  registers.current.user.gprs.r3_s = -1;
  ArmSMLAL(&registers, REGISTER_R3, REGISTER_R2, REGISTER_R0, REGISTER_R1);
  EXPECT_EQ(65537, registers.current.user.gprs.r0_s);
  EXPECT_EQ(-1, registers.current.user.gprs.r1_s);
  EXPECT_EQ(-2, registers.current.user.gprs.r2_s);
  EXPECT_EQ(-65538, registers.current.user.gprs.r3_s);

  registers.current.user.gprs.r0_s = 0;
  registers.current.user.gprs.r1_s = 0;
  registers.current.user.gprs.r2_s = 0;
  registers.current.user.gprs.r3_s = 0;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmSMLAL, ReuseSourceAndDest) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0_s = 65537;
  registers.current.user.gprs.r1_s = -1;
  ArmSMLAL(&registers, REGISTER_R1, REGISTER_R0, REGISTER_R0, REGISTER_R1);
  EXPECT_EQ(-65538, registers.current.user.gprs.r1_s);
  EXPECT_EQ(65537, registers.current.user.gprs.r0_s);

  registers.current.user.gprs.r0_s = 0;
  registers.current.user.gprs.r1_s = 0;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmSMLALS, Compute) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0_s = 65537;
  registers.current.user.gprs.r1_s = -1;
  registers.current.user.gprs.r2_s = 65537;
  registers.current.user.gprs.r3_s = 65537;
  ArmSMLALS(&registers, REGISTER_R3, REGISTER_R2, REGISTER_R0, REGISTER_R1);
  EXPECT_EQ(65537, registers.current.user.gprs.r0_s);
  EXPECT_EQ(-1, registers.current.user.gprs.r1_s);
  EXPECT_EQ(65537, registers.current.user.gprs.r2_s);
  EXPECT_EQ(0, registers.current.user.gprs.r3_s);

  registers.current.user.gprs.r0_s = 0;
  registers.current.user.gprs.r1_s = 0;
  registers.current.user.gprs.r2_s = 0;
  registers.current.user.gprs.r3_s = 0;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmSMLALS, ReuseSourceAndDest) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0_s = 65537;
  registers.current.user.gprs.r1_s = 1;
  ArmSMLALS(&registers, REGISTER_R0, REGISTER_R1, REGISTER_R0, REGISTER_R1);
  EXPECT_EQ(131074, registers.current.user.gprs.r0_s);
  EXPECT_EQ(1, registers.current.user.gprs.r1_s);

  registers.current.user.gprs.r0_s = 0;
  registers.current.user.gprs.r1_s = 0;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmSMLALS, Zero) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0_s = 65537;
  ArmSMLALS(&registers, REGISTER_R3, REGISTER_R2, REGISTER_R0, REGISTER_R1);
  EXPECT_EQ(65537u, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.zero);

  registers.current.user.gprs.r0_s = 0;
  registers.current.user.cpsr.zero = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmSMLALS, Negative) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0_s = -1;
  registers.current.user.gprs.r1_s = -1;
  registers.current.user.gprs.r2_s = 1;
  ArmSMLALS(&registers, REGISTER_R1, REGISTER_R0, REGISTER_R2, REGISTER_R1);
  EXPECT_EQ(-1, registers.current.user.gprs.r0_s);
  EXPECT_EQ(-2, registers.current.user.gprs.r1_s);
  EXPECT_EQ(1, registers.current.user.gprs.r2_s);
  EXPECT_TRUE(registers.current.user.cpsr.negative);

  registers.current.user.gprs.r0_s = 0;
  registers.current.user.gprs.r1_s = 0;
  registers.current.user.gprs.r2_s = 0;
  registers.current.user.cpsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}