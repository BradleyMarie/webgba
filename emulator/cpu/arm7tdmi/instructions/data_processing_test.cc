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

TEST(ArmADC, Compute) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 1u;
  ArmADC(&registers, REGISTER_R1, REGISTER_R0, 2u);
  EXPECT_EQ(3u, registers.gprs.r1);
  EXPECT_EQ(1u, registers.gprs.r0);

  registers.gprs.r0 = 0u;
  registers.gprs.r1 = 0u;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmADC, ComputeWithCarry) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 1u;
  registers.cpsr.carry = true;
  ArmADC(&registers, REGISTER_R1, REGISTER_R0, 2u);
  EXPECT_EQ(4u, registers.gprs.r1);
  EXPECT_EQ(1u, registers.gprs.r0);

  registers.gprs.r0 = 0u;
  registers.gprs.r1 = 0u;
  registers.cpsr.carry = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmADC, SameSourceAndDest) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 2u;
  ArmADC(&registers, REGISTER_R0, REGISTER_R0, 2u);
  EXPECT_EQ(4u, registers.gprs.r0);

  registers.gprs.r0 = 0u;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmADCS, Compute) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 1u;
  ArmADCS(&registers, REGISTER_R1, REGISTER_R0, 2u);
  EXPECT_EQ(3u, registers.gprs.r1);
  EXPECT_EQ(1u, registers.gprs.r0);

  registers.gprs.r0 = 0u;
  registers.gprs.r1 = 0u;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmADCS, ComputeWithCarry) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 1u;
  registers.cpsr.carry = true;
  ArmADCS(&registers, REGISTER_R1, REGISTER_R0, 2u);
  EXPECT_EQ(4u, registers.gprs.r1);
  EXPECT_EQ(1u, registers.gprs.r0);

  registers.gprs.r0 = 0u;
  registers.gprs.r1 = 0u;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmADCS, SameSourceAndDest) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 2u;
  ArmADCS(&registers, REGISTER_R0, REGISTER_R0, 2u);
  EXPECT_EQ(4u, registers.gprs.r0);

  registers.gprs.r0 = 0u;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmADCS, Zero) {
  auto registers = CreateArmUserRegisters();

  ArmADCS(&registers, REGISTER_R0, REGISTER_R0, 0);
  EXPECT_TRUE(registers.cpsr.zero);

  registers.cpsr.zero = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmADCS, Negative) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = UINT32_MAX;
  ArmADCS(&registers, REGISTER_R0, REGISTER_R0, 0);
  EXPECT_EQ(UINT32_MAX, registers.gprs.r0);
  EXPECT_TRUE(registers.cpsr.negative);

  registers.gprs.r0 = 0u;
  registers.cpsr.negative = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmADCS, NegativeWithCarry) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = UINT32_MAX - 1;
  registers.cpsr.carry = true;
  ArmADCS(&registers, REGISTER_R0, REGISTER_R0, 0);
  EXPECT_EQ(UINT32_MAX, registers.gprs.r0);
  EXPECT_TRUE(registers.cpsr.negative);

  registers.gprs.r0 = 0u;
  registers.cpsr.negative = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmADCS, Carry) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = UINT32_MAX;
  ArmADCS(&registers, REGISTER_R0, REGISTER_R0, 1);
  EXPECT_EQ(0u, registers.gprs.r0);
  EXPECT_TRUE(registers.cpsr.carry);
  EXPECT_TRUE(registers.cpsr.zero);

  registers.gprs.r0 = 0u;
  registers.cpsr.carry = false;
  registers.cpsr.zero = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmADCS, CarryWithCarry) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = UINT32_MAX;
  registers.cpsr.carry = true;
  ArmADCS(&registers, REGISTER_R0, REGISTER_R0, 0);
  EXPECT_EQ(0u, registers.gprs.r0);
  EXPECT_TRUE(registers.cpsr.carry);
  EXPECT_TRUE(registers.cpsr.zero);

  registers.gprs.r0 = 0u;
  registers.cpsr.carry = false;
  registers.cpsr.zero = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmADCS, Overflow) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0_s = INT32_MAX;
  ArmADCS(&registers, REGISTER_R0, REGISTER_R0, 1);
  EXPECT_EQ(INT32_MIN, registers.gprs.r0_s);
  EXPECT_TRUE(registers.cpsr.overflow);
  EXPECT_TRUE(registers.cpsr.negative);

  registers.gprs.r0_s = 0;
  registers.cpsr.overflow = false;
  registers.cpsr.negative = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmADCS, OverflowWithCarry) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0_s = INT32_MAX;
  registers.cpsr.carry = true;
  ArmADCS(&registers, REGISTER_R0, REGISTER_R0, 0);
  EXPECT_EQ(INT32_MIN, registers.gprs.r0_s);
  EXPECT_TRUE(registers.cpsr.overflow);
  EXPECT_TRUE(registers.cpsr.negative);

  registers.gprs.r0_s = 0;
  registers.cpsr.overflow = false;
  registers.cpsr.negative = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmADCS, Underflow) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0_s = -1;
  registers.gprs.r1_s = INT32_MIN;
  ArmADCS(&registers, REGISTER_R0, REGISTER_R0, registers.gprs.r1);
  EXPECT_EQ(INT32_MAX, registers.gprs.r0_s);
  EXPECT_TRUE(registers.cpsr.overflow);
  EXPECT_TRUE(registers.cpsr.carry);

  registers.gprs.r0_s = 0;
  registers.gprs.r1_s = 0;
  registers.cpsr.overflow = false;
  registers.cpsr.carry = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
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

  ArmADDS(&registers, REGISTER_R0, REGISTER_R0, 0);
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
  EXPECT_TRUE(registers.cpsr.zero);

  registers.gprs.r0 = 0u;
  registers.cpsr.carry = false;
  registers.cpsr.zero = false;
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

TEST(ArmAND, Compute) {
  auto registers = CreateArmGeneralPurposeRegistersRegisters();

  registers.r0 = 15u;
  ArmAND(&registers, REGISTER_R1, REGISTER_R0, 23u);
  EXPECT_EQ(7u, registers.r1);
  EXPECT_EQ(15u, registers.r0);

  registers.r0 = 0u;
  registers.r1 = 0u;
  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
}

TEST(ArmAND, SameSourceAndDest) {
  auto registers = CreateArmGeneralPurposeRegistersRegisters();

  registers.r0 = 15u;
  ArmAND(&registers, REGISTER_R0, REGISTER_R0, 23u);
  EXPECT_EQ(7u, registers.r0);

  registers.r0 = 0u;
  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
}

TEST(ArmANDS, Compute) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 15u;
  ArmANDS(&registers, REGISTER_R1, REGISTER_R0, 23u, false);
  EXPECT_EQ(7u, registers.gprs.r1);
  EXPECT_EQ(15u, registers.gprs.r0);

  registers.gprs.r0 = 0u;
  registers.gprs.r1 = 0u;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmANDS, SameSourceAndDest) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 15u;
  ArmANDS(&registers, REGISTER_R0, REGISTER_R0, 23u, false);
  EXPECT_EQ(7u, registers.gprs.r0);

  registers.gprs.r0 = 0u;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmANDS, Zero) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 1u;
  ArmANDS(&registers, REGISTER_R0, REGISTER_R0, 2u, false);
  EXPECT_TRUE(registers.cpsr.zero);

  registers.cpsr.zero = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmANDS, Negative) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = UINT32_MAX;
  ArmANDS(&registers, REGISTER_R0, REGISTER_R0, UINT32_MAX, false);
  EXPECT_EQ(UINT32_MAX, registers.gprs.r0);
  EXPECT_TRUE(registers.cpsr.negative);

  registers.gprs.r0 = 0u;
  registers.cpsr.negative = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmANDS, Carry) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = UINT32_MAX;
  ArmANDS(&registers, REGISTER_R0, REGISTER_R0, 1u, true);
  EXPECT_EQ(1u, registers.gprs.r0);
  EXPECT_TRUE(registers.cpsr.carry);

  registers.gprs.r0 = 0u;
  registers.cpsr.carry = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmBIC, Compute) {
  auto registers = CreateArmGeneralPurposeRegistersRegisters();

  registers.r0 = 15u;
  ArmBIC(&registers, REGISTER_R1, REGISTER_R0, 1u);
  EXPECT_EQ(14u, registers.r1);
  EXPECT_EQ(15u, registers.r0);

  registers.r0 = 0u;
  registers.r1 = 0u;
  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
}

TEST(ArmBIC, SameSourceAndDest) {
  auto registers = CreateArmGeneralPurposeRegistersRegisters();

  registers.r0 = 15u;
  ArmBIC(&registers, REGISTER_R0, REGISTER_R0, 1u);
  EXPECT_EQ(14u, registers.r0);

  registers.r0 = 0u;
  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
}

TEST(ArmBICS, Compute) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 15u;
  ArmBICS(&registers, REGISTER_R1, REGISTER_R0, 1u);
  EXPECT_EQ(14u, registers.gprs.r1);
  EXPECT_EQ(15u, registers.gprs.r0);

  registers.gprs.r0 = 0u;
  registers.gprs.r1 = 0u;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmBICS, SameSourceAndDest) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 15u;
  ArmBICS(&registers, REGISTER_R0, REGISTER_R0, 1u);
  EXPECT_EQ(14u, registers.gprs.r0);

  registers.gprs.r0 = 0u;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmBICS, Zero) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 1u;
  ArmBICS(&registers, REGISTER_R0, REGISTER_R0, 1u);
  EXPECT_TRUE(registers.cpsr.zero);

  registers.cpsr.zero = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmBICS, Negative) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = UINT32_MAX;
  ArmBICS(&registers, REGISTER_R0, REGISTER_R0, 1u);
  EXPECT_EQ(UINT32_MAX - 1u, registers.gprs.r0);
  EXPECT_TRUE(registers.cpsr.negative);

  registers.gprs.r0 = 0u;
  registers.cpsr.negative = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmCMN, Zero) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 0u;
  ArmCMN(&registers, REGISTER_R0, 0u);
  EXPECT_TRUE(registers.cpsr.zero);

  registers.gprs.r0 = 0u;
  registers.cpsr.zero = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmCMN, Negative) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0_s = INT32_MIN;
  ArmCMN(&registers, REGISTER_R0, 0);
  EXPECT_EQ(INT32_MIN, registers.gprs.r0_s);
  EXPECT_TRUE(registers.cpsr.negative);

  registers.gprs.r0_s = 0;
  registers.cpsr.negative = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmCMN, Carry) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = UINT32_MAX;
  ArmCMN(&registers, REGISTER_R0, 1);
  EXPECT_EQ(UINT32_MAX, registers.gprs.r0);
  EXPECT_TRUE(registers.cpsr.carry);
  EXPECT_TRUE(registers.cpsr.zero);

  registers.gprs.r0 = 0u;
  registers.cpsr.carry = false;
  registers.cpsr.zero = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmCMN, Overflow) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0_s = INT32_MAX;
  ArmCMN(&registers, REGISTER_R0, 1);
  EXPECT_EQ(INT32_MAX, registers.gprs.r0_s);
  EXPECT_TRUE(registers.cpsr.overflow);
  EXPECT_TRUE(registers.cpsr.negative);

  registers.gprs.r0_s = 0;
  registers.cpsr.overflow = false;
  registers.cpsr.negative = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmCMN, Underflow) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0_s = -1;
  ArmCMN(&registers, REGISTER_R0, INT32_MIN);
  EXPECT_EQ(-1, registers.gprs.r0_s);
  EXPECT_TRUE(registers.cpsr.overflow);
  EXPECT_TRUE(registers.cpsr.carry);

  registers.gprs.r0_s = 0;
  registers.cpsr.overflow = false;
  registers.cpsr.carry = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmCMP, Zero) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 1u;
  ArmCMP(&registers, REGISTER_R0, 1u);
  EXPECT_TRUE(registers.cpsr.zero);

  registers.gprs.r0 = 0u;
  registers.cpsr.zero = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmCMP, Negative) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0_s = INT32_MIN;
  ArmCMP(&registers, REGISTER_R0, 0);
  EXPECT_EQ(INT32_MIN, registers.gprs.r0_s);
  EXPECT_TRUE(registers.cpsr.negative);

  registers.gprs.r0_s = 0;
  registers.cpsr.negative = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmCMP, Carry) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 1u;
  ArmCMP(&registers, REGISTER_R0, UINT32_MAX);
  EXPECT_EQ(1u, registers.gprs.r0);
  EXPECT_TRUE(registers.cpsr.carry);

  registers.gprs.r0 = 0u;
  registers.cpsr.carry = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmCMP, Overflow) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0_s = INT32_MAX;
  ArmCMP(&registers, REGISTER_R0, UINT32_MAX);
  EXPECT_EQ(INT32_MAX, registers.gprs.r0_s);
  EXPECT_TRUE(registers.cpsr.overflow);
  EXPECT_TRUE(registers.cpsr.negative);
  EXPECT_TRUE(registers.cpsr.carry);

  registers.gprs.r0_s = 0;
  registers.cpsr.overflow = false;
  registers.cpsr.negative = false;
  registers.cpsr.carry = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmCMP, Underflow) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0_s = INT32_MIN;
  ArmCMP(&registers, REGISTER_R0, 1);
  EXPECT_EQ(INT32_MIN, registers.gprs.r0_s);
  EXPECT_TRUE(registers.cpsr.overflow);

  registers.gprs.r0_s = 0;
  registers.cpsr.overflow = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmEOR, Compute) {
  auto registers = CreateArmGeneralPurposeRegistersRegisters();

  registers.r0 = 5u;
  ArmEOR(&registers, REGISTER_R1, REGISTER_R0, 3u);
  EXPECT_EQ(6u, registers.r1);
  EXPECT_EQ(5u, registers.r0);

  registers.r0 = 0u;
  registers.r1 = 0u;
  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
}

TEST(ArmEOR, SameSourceAndDest) {
  auto registers = CreateArmGeneralPurposeRegistersRegisters();

  registers.r0 = 5u;
  ArmEOR(&registers, REGISTER_R0, REGISTER_R0, 3u);
  EXPECT_EQ(6u, registers.r0);

  registers.r0 = 0u;
  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
}

TEST(ArmEORS, Compute) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 5u;
  ArmEORS(&registers, REGISTER_R1, REGISTER_R0, 3u, false);
  EXPECT_EQ(6u, registers.gprs.r1);
  EXPECT_EQ(5u, registers.gprs.r0);

  registers.gprs.r0 = 0u;
  registers.gprs.r1 = 0u;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmEORS, SameSourceAndDest) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 5u;
  ArmEORS(&registers, REGISTER_R0, REGISTER_R0, 3u, false);
  EXPECT_EQ(6u, registers.gprs.r0);

  registers.gprs.r0 = 0u;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmEORS, Zero) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 1u;
  ArmEORS(&registers, REGISTER_R0, REGISTER_R0, 1u, false);
  EXPECT_TRUE(registers.cpsr.zero);

  registers.cpsr.zero = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmEORS, Negative) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = UINT32_MAX;
  ArmEORS(&registers, REGISTER_R0, REGISTER_R0, 0u, false);
  EXPECT_EQ(UINT32_MAX, registers.gprs.r0);
  EXPECT_TRUE(registers.cpsr.negative);

  registers.gprs.r0 = 0u;
  registers.cpsr.negative = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmEORS, Carry) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = UINT32_MAX;
  ArmEORS(&registers, REGISTER_R0, REGISTER_R0, UINT32_MAX - 1, true);
  EXPECT_EQ(1u, registers.gprs.r0);
  EXPECT_TRUE(registers.cpsr.carry);

  registers.gprs.r0 = 0u;
  registers.cpsr.carry = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}