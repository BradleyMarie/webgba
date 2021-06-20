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

ArmAllRegisters CreateArmAllRegisters() {
  ArmAllRegisters registers;
  memset(&registers, 0, sizeof(ArmAllRegisters));
  return registers;
}

bool ArmAllRegistersAreZero(const ArmAllRegisters& regs) {
  auto zero = CreateArmAllRegisters();
  return !memcmp(&zero, &regs, sizeof(ArmAllRegisters));
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
  EXPECT_TRUE(registers.cpsr.carry);

  registers.gprs.r0 = 0u;
  registers.cpsr.zero = false;
  registers.cpsr.carry = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmCMP, Negative) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0_s = INT32_MIN;
  ArmCMP(&registers, REGISTER_R0, 0);
  EXPECT_EQ(INT32_MIN, registers.gprs.r0_s);
  EXPECT_TRUE(registers.cpsr.negative);
  EXPECT_TRUE(registers.cpsr.carry);

  registers.gprs.r0_s = 0;
  registers.cpsr.negative = false;
  registers.cpsr.carry = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmCMP, NotCarry) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 1u;
  ArmCMP(&registers, REGISTER_R0, UINT32_MAX);
  EXPECT_EQ(1u, registers.gprs.r0);

  registers.gprs.r0 = 0u;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmCMP, Overflow) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0_s = INT32_MAX;
  ArmCMP(&registers, REGISTER_R0, UINT32_MAX);
  EXPECT_EQ(INT32_MAX, registers.gprs.r0_s);
  EXPECT_TRUE(registers.cpsr.overflow);
  EXPECT_TRUE(registers.cpsr.negative);

  registers.gprs.r0_s = 0;
  registers.cpsr.overflow = false;
  registers.cpsr.negative = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmCMP, Underflow) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0_s = INT32_MIN;
  ArmCMP(&registers, REGISTER_R0, 1);
  EXPECT_EQ(INT32_MIN, registers.gprs.r0_s);
  EXPECT_TRUE(registers.cpsr.overflow);
  EXPECT_TRUE(registers.cpsr.carry);

  registers.gprs.r0_s = 0;
  registers.cpsr.overflow = false;
  registers.cpsr.carry = false;
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

TEST(ArmMOV, Compute) {
  auto registers = CreateArmGeneralPurposeRegistersRegisters();

  ArmMOV(&registers, REGISTER_R0, 3u);
  EXPECT_EQ(3u, registers.r0);

  registers.r0 = 0u;
  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
}

TEST(ArmMOVS, Compute) {
  auto registers = CreateArmAllRegisters();

  ArmMOVS(&registers, REGISTER_R0, 3u, false);
  EXPECT_EQ(3u, registers.current.user.gprs.r0);

  registers.current.user.gprs.r0 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmMOVS, Zero) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 1u;
  ArmMOVS(&registers, REGISTER_R0, 0u, false);
  EXPECT_TRUE(registers.current.user.cpsr.zero);

  registers.current.user.cpsr.zero = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmMOVS, Negative) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 0u;
  ArmMOVS(&registers, REGISTER_R0, UINT32_MAX, false);
  EXPECT_EQ(UINT32_MAX, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.negative);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.cpsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmMOVS, Carry) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 0u;
  ArmMOVS(&registers, REGISTER_R0, 1u, true);
  EXPECT_EQ(1u, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmMOVS, R15ToUSR) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r13 = 26u;
  registers.current.user.gprs.r14 = 28u;
  registers.current.user.cpsr.mode = MODE_SVC;
  registers.current.spsr.mode = MODE_USR;
  registers.banked_gprs[USR_BANK_INDEX][BANKED_R9_INDEX] = 9u;
  registers.banked_gprs[USR_BANK_INDEX][BANKED_R10_INDEX] = 10u;
  registers.banked_gprs[USR_BANK_INDEX][BANKED_R11_INDEX] = 11u;
  registers.banked_gprs[USR_BANK_INDEX][BANKED_R12_INDEX] = 12u;
  registers.banked_gprs[USR_BANK_INDEX][BANKED_R13_INDEX] = 13u;
  registers.banked_gprs[USR_BANK_INDEX][BANKED_R14_INDEX] = 14u;
  ArmMOVS(&registers, REGISTER_R15, 15u, true);
  EXPECT_EQ(9u, registers.current.user.gprs.r9);
  EXPECT_EQ(10u, registers.current.user.gprs.r10);
  EXPECT_EQ(11u, registers.current.user.gprs.r11);
  EXPECT_EQ(12u, registers.current.user.gprs.r12);
  EXPECT_EQ(13u, registers.current.user.gprs.r13);
  EXPECT_EQ(14u, registers.current.user.gprs.r14);
  EXPECT_EQ(15u, registers.current.user.gprs.r15);
  EXPECT_EQ(MODE_USR, registers.current.user.cpsr.mode);
  EXPECT_EQ(26u, registers.banked_gprs[SVC_BANK_INDEX][BANKED_R13_INDEX]);
  EXPECT_EQ(28u, registers.banked_gprs[SVC_BANK_INDEX][BANKED_R14_INDEX]);

  registers.current.user.gprs.r9 = 0u;
  registers.current.user.gprs.r10 = 0u;
  registers.current.user.gprs.r11 = 0u;
  registers.current.user.gprs.r12 = 0u;
  registers.current.user.gprs.r13 = 0u;
  registers.current.user.gprs.r14 = 0u;
  registers.current.user.gprs.r15 = 0u;
  registers.banked_gprs[USR_BANK_INDEX][BANKED_R9_INDEX] = 0u;
  registers.banked_gprs[USR_BANK_INDEX][BANKED_R10_INDEX] = 0u;
  registers.banked_gprs[USR_BANK_INDEX][BANKED_R11_INDEX] = 0u;
  registers.banked_gprs[USR_BANK_INDEX][BANKED_R12_INDEX] = 0u;
  registers.banked_gprs[USR_BANK_INDEX][BANKED_R13_INDEX] = 0u;
  registers.banked_gprs[USR_BANK_INDEX][BANKED_R14_INDEX] = 0u;
  registers.banked_gprs[SVC_BANK_INDEX][BANKED_R13_INDEX] = 0u;
  registers.banked_gprs[SVC_BANK_INDEX][BANKED_R14_INDEX] = 0u;
  registers.banked_spsrs[SVC_BANK_INDEX].mode = 0;
  registers.current.user.cpsr.mode = 0;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmMOVS, R15ToSVC) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r13 = 26u;
  registers.current.user.gprs.r14 = 28u;
  registers.current.user.cpsr.mode = MODE_IRQ;
  registers.current.spsr.mode = MODE_SVC;
  registers.banked_gprs[SVC_BANK_INDEX][BANKED_R13_INDEX] = 13u;
  registers.banked_gprs[SVC_BANK_INDEX][BANKED_R14_INDEX] = 14u;
  registers.banked_spsrs[SVC_BANK_INDEX].thumb = true;
  ArmMOVS(&registers, REGISTER_R15, 15u, true);
  EXPECT_EQ(13u, registers.current.user.gprs.r13);
  EXPECT_EQ(14u, registers.current.user.gprs.r14);
  EXPECT_EQ(15u, registers.current.user.gprs.r15);
  EXPECT_EQ(MODE_SVC, registers.current.user.cpsr.mode);
  EXPECT_TRUE(registers.current.spsr.thumb);
  EXPECT_EQ(26u, registers.banked_gprs[IRQ_BANK_INDEX][BANKED_R13_INDEX]);
  EXPECT_EQ(28u, registers.banked_gprs[IRQ_BANK_INDEX][BANKED_R14_INDEX]);

  registers.current.user.gprs.r13 = 0u;
  registers.current.user.gprs.r14 = 0u;
  registers.current.user.gprs.r15 = 0u;
  registers.banked_gprs[SVC_BANK_INDEX][BANKED_R13_INDEX] = 0u;
  registers.banked_gprs[SVC_BANK_INDEX][BANKED_R14_INDEX] = 0u;
  registers.banked_gprs[IRQ_BANK_INDEX][BANKED_R13_INDEX] = 0u;
  registers.banked_gprs[IRQ_BANK_INDEX][BANKED_R14_INDEX] = 0u;
  registers.banked_spsrs[IRQ_BANK_INDEX].mode = 0;
  registers.banked_spsrs[SVC_BANK_INDEX].thumb = false;
  registers.current.user.cpsr.mode = 0;
  registers.current.spsr.thumb = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmMVN, Compute) {
  auto registers = CreateArmGeneralPurposeRegistersRegisters();

  registers.r0 = 5u;
  ArmMVN(&registers, REGISTER_R0, UINT32_MAX - 1u);
  EXPECT_EQ(1u, registers.r0);

  registers.r0 = 0u;
  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
}

TEST(ArmMVNS, Compute) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 5u;
  ArmMVNS(&registers, REGISTER_R0, UINT32_MAX - 1u, false);
  EXPECT_EQ(1u, registers.current.user.gprs.r0);

  registers.current.user.gprs.r0 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmMVNS, Zero) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 1u;
  ArmMVNS(&registers, REGISTER_R0, UINT32_MAX, false);
  EXPECT_TRUE(registers.current.user.cpsr.zero);

  registers.current.user.cpsr.zero = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmMVNS, Negative) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 1u;
  ArmMVNS(&registers, REGISTER_R0, 0u, false);
  EXPECT_EQ(UINT32_MAX, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.negative);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.cpsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmMVNS, Carry) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 2u;
  ArmMVNS(&registers, REGISTER_R0, UINT32_MAX - 1, true);
  EXPECT_EQ(1u, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmMVNS, R15ToUSR) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r13 = 26u;
  registers.current.user.gprs.r14 = 28u;
  registers.current.user.cpsr.mode = MODE_SVC;
  registers.current.spsr.mode = MODE_USR;
  registers.banked_gprs[USR_BANK_INDEX][BANKED_R9_INDEX] = 9u;
  registers.banked_gprs[USR_BANK_INDEX][BANKED_R10_INDEX] = 10u;
  registers.banked_gprs[USR_BANK_INDEX][BANKED_R11_INDEX] = 11u;
  registers.banked_gprs[USR_BANK_INDEX][BANKED_R12_INDEX] = 12u;
  registers.banked_gprs[USR_BANK_INDEX][BANKED_R13_INDEX] = 13u;
  registers.banked_gprs[USR_BANK_INDEX][BANKED_R14_INDEX] = 14u;
  ArmMVNS(&registers, REGISTER_R15, ~15u, true);
  EXPECT_EQ(9u, registers.current.user.gprs.r9);
  EXPECT_EQ(10u, registers.current.user.gprs.r10);
  EXPECT_EQ(11u, registers.current.user.gprs.r11);
  EXPECT_EQ(12u, registers.current.user.gprs.r12);
  EXPECT_EQ(13u, registers.current.user.gprs.r13);
  EXPECT_EQ(14u, registers.current.user.gprs.r14);
  EXPECT_EQ(15u, registers.current.user.gprs.r15);
  EXPECT_EQ(MODE_USR, registers.current.user.cpsr.mode);
  EXPECT_EQ(26u, registers.banked_gprs[SVC_BANK_INDEX][BANKED_R13_INDEX]);
  EXPECT_EQ(28u, registers.banked_gprs[SVC_BANK_INDEX][BANKED_R14_INDEX]);

  registers.current.user.gprs.r9 = 0u;
  registers.current.user.gprs.r10 = 0u;
  registers.current.user.gprs.r11 = 0u;
  registers.current.user.gprs.r12 = 0u;
  registers.current.user.gprs.r13 = 0u;
  registers.current.user.gprs.r14 = 0u;
  registers.current.user.gprs.r15 = 0u;
  registers.banked_gprs[USR_BANK_INDEX][BANKED_R9_INDEX] = 0u;
  registers.banked_gprs[USR_BANK_INDEX][BANKED_R10_INDEX] = 0u;
  registers.banked_gprs[USR_BANK_INDEX][BANKED_R11_INDEX] = 0u;
  registers.banked_gprs[USR_BANK_INDEX][BANKED_R12_INDEX] = 0u;
  registers.banked_gprs[USR_BANK_INDEX][BANKED_R13_INDEX] = 0u;
  registers.banked_gprs[USR_BANK_INDEX][BANKED_R14_INDEX] = 0u;
  registers.banked_gprs[SVC_BANK_INDEX][BANKED_R13_INDEX] = 0u;
  registers.banked_gprs[SVC_BANK_INDEX][BANKED_R14_INDEX] = 0u;
  registers.banked_spsrs[SVC_BANK_INDEX].mode = 0;
  registers.current.user.cpsr.mode = 0;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmMVNS, R15ToSVC) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r13 = 26u;
  registers.current.user.gprs.r14 = 28u;
  registers.current.user.cpsr.mode = MODE_IRQ;
  registers.current.spsr.mode = MODE_SVC;
  registers.banked_gprs[SVC_BANK_INDEX][BANKED_R13_INDEX] = 13u;
  registers.banked_gprs[SVC_BANK_INDEX][BANKED_R14_INDEX] = 14u;
  registers.banked_spsrs[SVC_BANK_INDEX].thumb = true;
  ArmMVNS(&registers, REGISTER_R15, ~15u, true);
  EXPECT_EQ(13u, registers.current.user.gprs.r13);
  EXPECT_EQ(14u, registers.current.user.gprs.r14);
  EXPECT_EQ(15u, registers.current.user.gprs.r15);
  EXPECT_EQ(MODE_SVC, registers.current.user.cpsr.mode);
  EXPECT_TRUE(registers.current.spsr.thumb);
  EXPECT_EQ(26u, registers.banked_gprs[IRQ_BANK_INDEX][BANKED_R13_INDEX]);
  EXPECT_EQ(28u, registers.banked_gprs[IRQ_BANK_INDEX][BANKED_R14_INDEX]);

  registers.current.user.gprs.r13 = 0u;
  registers.current.user.gprs.r14 = 0u;
  registers.current.user.gprs.r15 = 0u;
  registers.banked_gprs[SVC_BANK_INDEX][BANKED_R13_INDEX] = 0u;
  registers.banked_gprs[SVC_BANK_INDEX][BANKED_R14_INDEX] = 0u;
  registers.banked_gprs[IRQ_BANK_INDEX][BANKED_R13_INDEX] = 0u;
  registers.banked_gprs[IRQ_BANK_INDEX][BANKED_R14_INDEX] = 0u;
  registers.banked_spsrs[IRQ_BANK_INDEX].mode = 0;
  registers.banked_spsrs[SVC_BANK_INDEX].thumb = false;
  registers.current.user.cpsr.mode = 0;
  registers.current.spsr.thumb = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmOOR, Compute) {
  auto registers = CreateArmGeneralPurposeRegistersRegisters();

  registers.r0 = 5u;
  ArmOOR(&registers, REGISTER_R1, REGISTER_R0, 3u);
  EXPECT_EQ(7u, registers.r1);
  EXPECT_EQ(5u, registers.r0);

  registers.r0 = 0u;
  registers.r1 = 0u;
  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
}

TEST(ArmOOR, SameSourceAndDest) {
  auto registers = CreateArmGeneralPurposeRegistersRegisters();

  registers.r0 = 5u;
  ArmOOR(&registers, REGISTER_R0, REGISTER_R0, 3u);
  EXPECT_EQ(7u, registers.r0);

  registers.r0 = 0u;
  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
}

TEST(ArmOORS, Compute) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 5u;
  ArmOORS(&registers, REGISTER_R1, REGISTER_R0, 3u, false);
  EXPECT_EQ(7u, registers.gprs.r1);
  EXPECT_EQ(5u, registers.gprs.r0);

  registers.gprs.r0 = 0u;
  registers.gprs.r1 = 0u;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmOORS, SameSourceAndDest) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 5u;
  ArmOORS(&registers, REGISTER_R0, REGISTER_R0, 3u, false);
  EXPECT_EQ(7u, registers.gprs.r0);

  registers.gprs.r0 = 0u;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmOORS, Zero) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 0u;
  ArmOORS(&registers, REGISTER_R0, REGISTER_R0, 0u, false);
  EXPECT_TRUE(registers.cpsr.zero);

  registers.cpsr.zero = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmOORS, Negative) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = UINT32_MAX;
  ArmOORS(&registers, REGISTER_R0, REGISTER_R0, 0u, false);
  EXPECT_EQ(UINT32_MAX, registers.gprs.r0);
  EXPECT_TRUE(registers.cpsr.negative);

  registers.gprs.r0 = 0u;
  registers.cpsr.negative = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmOORS, Carry) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 5u;
  ArmOORS(&registers, REGISTER_R0, REGISTER_R0, 3u, true);
  EXPECT_EQ(7u, registers.gprs.r0);
  EXPECT_TRUE(registers.cpsr.carry);

  registers.gprs.r0 = 0u;
  registers.cpsr.carry = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmRSB, Compute) {
  auto registers = CreateArmGeneralPurposeRegistersRegisters();

  registers.r0 = 1u;
  ArmRSB(&registers, REGISTER_R1, REGISTER_R0, 3u);
  EXPECT_EQ(2u, registers.r1);
  EXPECT_EQ(1u, registers.r0);

  registers.r0 = 0u;
  registers.r1 = 0u;
  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
}

TEST(ArmRSB, SameSourceAndDest) {
  auto registers = CreateArmGeneralPurposeRegistersRegisters();

  registers.r0 = 2u;
  ArmRSB(&registers, REGISTER_R0, REGISTER_R0, 4u);
  EXPECT_EQ(2u, registers.r0);

  registers.r0 = 0u;
  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
}

TEST(ArmRSBS, Compute) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 1u;
  ArmRSBS(&registers, REGISTER_R1, REGISTER_R0, 3u);
  EXPECT_EQ(2u, registers.gprs.r1);
  EXPECT_EQ(1u, registers.gprs.r0);
  EXPECT_TRUE(registers.cpsr.carry);

  registers.gprs.r0 = 0u;
  registers.gprs.r1 = 0u;
  registers.cpsr.carry = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmRSBS, SameSourceAndDest) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 2u;
  ArmRSBS(&registers, REGISTER_R0, REGISTER_R0, 4u);
  EXPECT_EQ(2u, registers.gprs.r0);
  EXPECT_TRUE(registers.cpsr.carry);

  registers.gprs.r0 = 0u;
  registers.cpsr.carry = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmRSBS, Zero) {
  auto registers = CreateArmUserRegisters();

  ArmRSBS(&registers, REGISTER_R0, REGISTER_R0, 0);
  EXPECT_TRUE(registers.cpsr.zero);
  EXPECT_TRUE(registers.cpsr.carry);

  registers.cpsr.zero = false;
  registers.cpsr.carry = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmRSBS, Negative) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 0;
  ArmRSBS(&registers, REGISTER_R0, REGISTER_R0, UINT32_MAX);
  EXPECT_EQ(UINT32_MAX, registers.gprs.r0);
  EXPECT_TRUE(registers.cpsr.negative);
  EXPECT_TRUE(registers.cpsr.carry);

  registers.gprs.r0 = 0u;
  registers.cpsr.negative = false;
  registers.cpsr.carry = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmRSBS, NotCarry) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 1;
  ArmRSBS(&registers, REGISTER_R0, REGISTER_R0, 0);
  EXPECT_EQ(UINT32_MAX, registers.gprs.r0);
  EXPECT_TRUE(registers.cpsr.negative);

  registers.gprs.r0 = 0u;
  registers.cpsr.negative = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmRSBS, Overflow) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0_s = -1;
  ArmRSBS(&registers, REGISTER_R0, REGISTER_R0, INT32_MAX);
  EXPECT_EQ(INT32_MIN, registers.gprs.r0_s);
  EXPECT_TRUE(registers.cpsr.overflow);
  EXPECT_TRUE(registers.cpsr.negative);

  registers.gprs.r0_s = 0;
  registers.cpsr.overflow = false;
  registers.cpsr.negative = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmRSBS, Underflow) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 1;
  registers.gprs.r1_s = INT32_MIN;
  ArmRSBS(&registers, REGISTER_R0, REGISTER_R0, registers.gprs.r1);
  EXPECT_EQ(INT32_MAX, registers.gprs.r0_s);
  EXPECT_TRUE(registers.cpsr.overflow);
  EXPECT_TRUE(registers.cpsr.carry);

  registers.gprs.r0_s = 0;
  registers.gprs.r1_s = 0;
  registers.cpsr.overflow = false;
  registers.cpsr.carry = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmRSC, Compute) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 1u;
  ArmRSC(&registers, REGISTER_R1, REGISTER_R0, 4u);
  EXPECT_EQ(2u, registers.gprs.r1);
  EXPECT_EQ(1u, registers.gprs.r0);

  registers.gprs.r0 = 0u;
  registers.gprs.r1 = 0u;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmRSC, ComputeWithCarry) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 1u;
  registers.cpsr.carry = true;
  ArmRSC(&registers, REGISTER_R1, REGISTER_R0, 6u);
  EXPECT_EQ(5u, registers.gprs.r1);
  EXPECT_EQ(1u, registers.gprs.r0);

  registers.gprs.r0 = 0u;
  registers.gprs.r1 = 0u;
  registers.cpsr.carry = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmRSC, SameSourceAndDest) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 2u;
  ArmRSC(&registers, REGISTER_R0, REGISTER_R0, 6u);
  EXPECT_EQ(3u, registers.gprs.r0);

  registers.gprs.r0 = 0u;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmRSCS, Compute) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 1u;
  ArmRSCS(&registers, REGISTER_R1, REGISTER_R0, 4u);
  EXPECT_EQ(2u, registers.gprs.r1);
  EXPECT_EQ(1u, registers.gprs.r0);
  EXPECT_TRUE(registers.cpsr.carry);

  registers.gprs.r0 = 0u;
  registers.gprs.r1 = 0u;
  registers.cpsr.carry = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmRSCS, ComputeWithCarry) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 1u;
  registers.cpsr.carry = true;
  ArmRSCS(&registers, REGISTER_R1, REGISTER_R0, 5u);
  EXPECT_EQ(4u, registers.gprs.r1);
  EXPECT_EQ(1u, registers.gprs.r0);
  EXPECT_TRUE(registers.cpsr.carry);

  registers.gprs.r0 = 0u;
  registers.gprs.r1 = 0u;
  registers.cpsr.carry = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmRSCS, SameSourceAndDest) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 2u;
  ArmRSCS(&registers, REGISTER_R0, REGISTER_R0, 6u);
  EXPECT_EQ(3u, registers.gprs.r0);
  EXPECT_TRUE(registers.cpsr.carry);

  registers.gprs.r0 = 0u;
  registers.cpsr.carry = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmRSCS, Zero) {
  auto registers = CreateArmUserRegisters();

  ArmRSCS(&registers, REGISTER_R0, REGISTER_R0, 1u);
  EXPECT_TRUE(registers.cpsr.zero);
  EXPECT_TRUE(registers.cpsr.carry);

  registers.cpsr.zero = false;
  registers.cpsr.carry = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmRSCS, Negative) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 0u;
  ArmRSCS(&registers, REGISTER_R0, REGISTER_R0, UINT32_MAX);
  EXPECT_EQ(UINT32_MAX - 1u, registers.gprs.r0);
  EXPECT_TRUE(registers.cpsr.negative);
  EXPECT_TRUE(registers.cpsr.carry);

  registers.gprs.r0 = 0u;
  registers.cpsr.negative = false;
  registers.cpsr.carry = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmRSCS, NegativeWithCarry) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 0u;
  registers.cpsr.carry = true;
  ArmRSCS(&registers, REGISTER_R0, REGISTER_R0, UINT32_MAX);
  EXPECT_EQ(UINT32_MAX, registers.gprs.r0);
  EXPECT_TRUE(registers.cpsr.negative);
  EXPECT_TRUE(registers.cpsr.carry);

  registers.gprs.r0 = 0u;
  registers.cpsr.negative = false;
  registers.cpsr.carry = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmRSCS, Carry) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 0u;
  ArmRSCS(&registers, REGISTER_R0, REGISTER_R0, UINT32_MAX);
  EXPECT_EQ(UINT32_MAX - 1u, registers.gprs.r0);
  EXPECT_TRUE(registers.cpsr.negative);
  EXPECT_TRUE(registers.cpsr.carry);

  registers.gprs.r0 = 0u;
  registers.cpsr.negative = false;
  registers.cpsr.carry = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmRSCS, CarryWithCarry) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 0u;
  registers.cpsr.carry = true;
  ArmRSCS(&registers, REGISTER_R0, REGISTER_R0, UINT32_MAX);
  EXPECT_EQ(UINT32_MAX, registers.gprs.r0);
  EXPECT_TRUE(registers.cpsr.negative);
  EXPECT_TRUE(registers.cpsr.carry);

  registers.gprs.r0 = 0u;
  registers.cpsr.negative = false;
  registers.cpsr.carry = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmRSCS, Overflow) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0_s = -2;
  ArmRSCS(&registers, REGISTER_R0, REGISTER_R0, (uint32_t)INT32_MAX);
  EXPECT_EQ(INT32_MIN, registers.gprs.r0_s);
  EXPECT_TRUE(registers.cpsr.overflow);
  EXPECT_TRUE(registers.cpsr.negative);

  registers.gprs.r0_s = 0;
  registers.cpsr.overflow = false;
  registers.cpsr.negative = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmRSCS, Underflow) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0_s = 0;
  registers.gprs.r1_s = INT32_MIN;
  ArmRSCS(&registers, REGISTER_R0, REGISTER_R0, registers.gprs.r1);
  EXPECT_EQ(INT32_MAX, registers.gprs.r0_s);
  EXPECT_TRUE(registers.cpsr.overflow);
  EXPECT_TRUE(registers.cpsr.carry);

  registers.gprs.r0_s = 0;
  registers.gprs.r1_s = 0;
  registers.cpsr.overflow = false;
  registers.cpsr.carry = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmRSCS, UnderflowWithCarry) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0_s = 1;
  registers.gprs.r1_s = INT32_MIN;
  registers.cpsr.carry = true;
  ArmRSCS(&registers, REGISTER_R0, REGISTER_R0, registers.gprs.r1);
  EXPECT_EQ(INT32_MAX, registers.gprs.r0_s);
  EXPECT_TRUE(registers.cpsr.overflow);
  EXPECT_TRUE(registers.cpsr.carry);

  registers.gprs.r0_s = 0;
  registers.gprs.r1_s = 0;
  registers.cpsr.overflow = false;
  registers.cpsr.carry = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}