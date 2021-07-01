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

ArmPrivilegedRegisters CreateArmPrivilegedRegisters() {
  ArmPrivilegedRegisters registers;
  memset(&registers, 0, sizeof(ArmPrivilegedRegisters));
  return registers;
}

bool ArmPrivilegedRegistersAreZero(const ArmPrivilegedRegisters& regs) {
  auto zero = CreateArmPrivilegedRegisters();
  return !memcmp(&zero, &regs, sizeof(ArmPrivilegedRegisters));
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
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 1u;
  ArmADCS(&registers, REGISTER_R1, REGISTER_R0, 2u);
  EXPECT_EQ(3u, registers.current.user.gprs.r1);
  EXPECT_EQ(1u, registers.current.user.gprs.r0);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmADCS, R15ToUSR) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;
  registers.current.spsr.mode = MODE_USR;

  registers.current.user.gprs.r15 = 1u;
  ArmADCS(&registers, REGISTER_R15, REGISTER_R15, 2u);
  EXPECT_EQ(3u, registers.current.user.gprs.r15);
  EXPECT_EQ(MODE_USR, registers.current.user.cpsr.mode);

  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.mode = 0;
  EXPECT_TRUE(ArmPrivilegedRegistersAreZero(registers.current));
}

TEST(ArmADCS, ComputeWithCarry) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 1u;
  registers.current.user.cpsr.carry = true;
  ArmADCS(&registers, REGISTER_R1, REGISTER_R0, 2u);
  EXPECT_EQ(4u, registers.current.user.gprs.r1);
  EXPECT_EQ(1u, registers.current.user.gprs.r0);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmADCS, SameSourceAndDest) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 2u;
  ArmADCS(&registers, REGISTER_R0, REGISTER_R0, 2u);
  EXPECT_EQ(4u, registers.current.user.gprs.r0);

  registers.current.user.gprs.r0 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmADCS, Zero) {
  auto registers = CreateArmAllRegisters();

  ArmADCS(&registers, REGISTER_R0, REGISTER_R0, 0);
  EXPECT_TRUE(registers.current.user.cpsr.zero);

  registers.current.user.cpsr.zero = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmADCS, Negative) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = UINT32_MAX;
  ArmADCS(&registers, REGISTER_R0, REGISTER_R0, 0);
  EXPECT_EQ(UINT32_MAX, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.negative);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.cpsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmADCS, NegativeWithCarry) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = UINT32_MAX - 1;
  registers.current.user.cpsr.carry = true;
  ArmADCS(&registers, REGISTER_R0, REGISTER_R0, 0);
  EXPECT_EQ(UINT32_MAX, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.negative);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.cpsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmADCS, Carry) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = UINT32_MAX;
  ArmADCS(&registers, REGISTER_R0, REGISTER_R0, 1);
  EXPECT_EQ(0u, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.carry);
  EXPECT_TRUE(registers.current.user.cpsr.zero);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.cpsr.carry = false;
  registers.current.user.cpsr.zero = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmADCS, CarryWithCarry) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = UINT32_MAX;
  registers.current.user.cpsr.carry = true;
  ArmADCS(&registers, REGISTER_R0, REGISTER_R0, 0);
  EXPECT_EQ(0u, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.carry);
  EXPECT_TRUE(registers.current.user.cpsr.zero);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.cpsr.carry = false;
  registers.current.user.cpsr.zero = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmADCS, Overflow) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0_s = INT32_MAX;
  ArmADCS(&registers, REGISTER_R0, REGISTER_R0, 1);
  EXPECT_EQ(INT32_MIN, registers.current.user.gprs.r0_s);
  EXPECT_TRUE(registers.current.user.cpsr.overflow);
  EXPECT_TRUE(registers.current.user.cpsr.negative);

  registers.current.user.gprs.r0_s = 0;
  registers.current.user.cpsr.overflow = false;
  registers.current.user.cpsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmADCS, OverflowWithCarry) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0_s = INT32_MAX;
  registers.current.user.cpsr.carry = true;
  ArmADCS(&registers, REGISTER_R0, REGISTER_R0, 0);
  EXPECT_EQ(INT32_MIN, registers.current.user.gprs.r0_s);
  EXPECT_TRUE(registers.current.user.cpsr.overflow);
  EXPECT_TRUE(registers.current.user.cpsr.negative);

  registers.current.user.gprs.r0_s = 0;
  registers.current.user.cpsr.overflow = false;
  registers.current.user.cpsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmADCS, Underflow) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0_s = -1;
  registers.current.user.gprs.r1_s = INT32_MIN;
  ArmADCS(&registers, REGISTER_R0, REGISTER_R0, registers.current.user.gprs.r1);
  EXPECT_EQ(INT32_MAX, registers.current.user.gprs.r0_s);
  EXPECT_TRUE(registers.current.user.cpsr.overflow);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0_s = 0;
  registers.current.user.gprs.r1_s = 0;
  registers.current.user.cpsr.overflow = false;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
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
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 1u;
  ArmADDS(&registers, REGISTER_R1, REGISTER_R0, 2u);
  EXPECT_EQ(3u, registers.current.user.gprs.r1);
  EXPECT_EQ(1u, registers.current.user.gprs.r0);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmADDS, SameSourceAndDest) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 2u;
  ArmADDS(&registers, REGISTER_R0, REGISTER_R0, 2u);
  EXPECT_EQ(4u, registers.current.user.gprs.r0);

  registers.current.user.gprs.r0 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmADDS, R15ToUSR) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;
  registers.current.spsr.mode = MODE_USR;

  registers.current.user.gprs.r15 = 1u;
  ArmADDS(&registers, REGISTER_R15, REGISTER_R15, 2u);
  EXPECT_EQ(3u, registers.current.user.gprs.r15);
  EXPECT_EQ(MODE_USR, registers.current.user.cpsr.mode);

  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.mode = 0;
  EXPECT_TRUE(ArmPrivilegedRegistersAreZero(registers.current));
}

TEST(ArmADDS, Zero) {
  auto registers = CreateArmAllRegisters();

  ArmADDS(&registers, REGISTER_R0, REGISTER_R0, 0);
  EXPECT_TRUE(registers.current.user.cpsr.zero);

  registers.current.user.cpsr.zero = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmADDS, Negative) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = UINT32_MAX;
  ArmADDS(&registers, REGISTER_R0, REGISTER_R0, 0);
  EXPECT_EQ(UINT32_MAX, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.negative);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.cpsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmADDS, Carry) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = UINT32_MAX;
  ArmADDS(&registers, REGISTER_R0, REGISTER_R0, 1);
  EXPECT_EQ(0u, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.carry);
  EXPECT_TRUE(registers.current.user.cpsr.zero);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.cpsr.carry = false;
  registers.current.user.cpsr.zero = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmADDS, Overflow) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0_s = INT32_MAX;
  ArmADDS(&registers, REGISTER_R0, REGISTER_R0, 1);
  EXPECT_EQ(INT32_MIN, registers.current.user.gprs.r0_s);
  EXPECT_TRUE(registers.current.user.cpsr.overflow);
  EXPECT_TRUE(registers.current.user.cpsr.negative);

  registers.current.user.gprs.r0_s = 0;
  registers.current.user.cpsr.overflow = false;
  registers.current.user.cpsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmADDS, Underflow) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0_s = -1;
  registers.current.user.gprs.r1_s = INT32_MIN;
  ArmADDS(&registers, REGISTER_R0, REGISTER_R0, registers.current.user.gprs.r1);
  EXPECT_EQ(INT32_MAX, registers.current.user.gprs.r0_s);
  EXPECT_TRUE(registers.current.user.cpsr.overflow);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0_s = 0;
  registers.current.user.gprs.r1_s = 0;
  registers.current.user.cpsr.overflow = false;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
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
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 15u;
  ArmANDS(&registers, REGISTER_R1, REGISTER_R0, 23u, false);
  EXPECT_EQ(7u, registers.current.user.gprs.r1);
  EXPECT_EQ(15u, registers.current.user.gprs.r0);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmANDS, SameSourceAndDest) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 15u;
  ArmANDS(&registers, REGISTER_R0, REGISTER_R0, 23u, false);
  EXPECT_EQ(7u, registers.current.user.gprs.r0);

  registers.current.user.gprs.r0 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmANDS, R15ToUSR) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;
  registers.current.spsr.mode = MODE_USR;

  registers.current.user.gprs.r15 = 1u;
  ArmANDS(&registers, REGISTER_R15, REGISTER_R15, 3u, false);
  EXPECT_EQ(1u, registers.current.user.gprs.r15);
  EXPECT_EQ(MODE_USR, registers.current.user.cpsr.mode);

  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.mode = 0;
  EXPECT_TRUE(ArmPrivilegedRegistersAreZero(registers.current));
}

TEST(ArmANDS, Zero) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 1u;
  ArmANDS(&registers, REGISTER_R0, REGISTER_R0, 2u, false);
  EXPECT_TRUE(registers.current.user.cpsr.zero);

  registers.current.user.cpsr.zero = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmANDS, Negative) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = UINT32_MAX;
  ArmANDS(&registers, REGISTER_R0, REGISTER_R0, UINT32_MAX, false);
  EXPECT_EQ(UINT32_MAX, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.negative);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.cpsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmANDS, Carry) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = UINT32_MAX;
  ArmANDS(&registers, REGISTER_R0, REGISTER_R0, 1u, true);
  EXPECT_EQ(1u, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
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
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 15u;
  ArmBICS(&registers, REGISTER_R1, REGISTER_R0, 1u, false);
  EXPECT_EQ(14u, registers.current.user.gprs.r1);
  EXPECT_EQ(15u, registers.current.user.gprs.r0);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmBICS, SameSourceAndDest) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 15u;
  ArmBICS(&registers, REGISTER_R0, REGISTER_R0, 1u, false);
  EXPECT_EQ(14u, registers.current.user.gprs.r0);

  registers.current.user.gprs.r0 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmBICS, R15ToUSR) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;
  registers.current.spsr.mode = MODE_USR;

  registers.current.user.gprs.r15 = 15u;
  ArmBICS(&registers, REGISTER_R15, REGISTER_R15, 1u, false);
  EXPECT_EQ(14u, registers.current.user.gprs.r15);
  EXPECT_EQ(MODE_USR, registers.current.user.cpsr.mode);

  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.mode = 0;
  EXPECT_TRUE(ArmPrivilegedRegistersAreZero(registers.current));
}

TEST(ArmBICS, Zero) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 1u;
  ArmBICS(&registers, REGISTER_R0, REGISTER_R0, 1u, false);
  EXPECT_TRUE(registers.current.user.cpsr.zero);

  registers.current.user.cpsr.zero = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmBICS, Negative) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = UINT32_MAX;
  ArmBICS(&registers, REGISTER_R0, REGISTER_R0, 1, false);
  EXPECT_EQ(UINT32_MAX - 1u, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.negative);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.cpsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmBICS, Carry) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = UINT32_MAX;
  ArmBICS(&registers, REGISTER_R0, REGISTER_R0, 1, true);
  EXPECT_EQ(UINT32_MAX - 1u, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.negative);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.cpsr.negative = false;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
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
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 5u;
  ArmEORS(&registers, REGISTER_R1, REGISTER_R0, 3u, false);
  EXPECT_EQ(6u, registers.current.user.gprs.r1);
  EXPECT_EQ(5u, registers.current.user.gprs.r0);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmEORS, SameSourceAndDest) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 5u;
  ArmEORS(&registers, REGISTER_R0, REGISTER_R0, 3u, false);
  EXPECT_EQ(6u, registers.current.user.gprs.r0);

  registers.current.user.gprs.r0 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmEORS, R15ToUSR) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;
  registers.current.spsr.mode = MODE_USR;

  registers.current.user.gprs.r15 = 5u;
  ArmEORS(&registers, REGISTER_R15, REGISTER_R15, 3u, false);
  EXPECT_EQ(6u, registers.current.user.gprs.r15);
  EXPECT_EQ(MODE_USR, registers.current.user.cpsr.mode);

  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.mode = 0;
  EXPECT_TRUE(ArmPrivilegedRegistersAreZero(registers.current));
}

TEST(ArmEORS, Zero) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 1u;
  ArmEORS(&registers, REGISTER_R0, REGISTER_R0, 1u, false);
  EXPECT_TRUE(registers.current.user.cpsr.zero);

  registers.current.user.cpsr.zero = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmEORS, Negative) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = UINT32_MAX;
  ArmEORS(&registers, REGISTER_R0, REGISTER_R0, 0u, false);
  EXPECT_EQ(UINT32_MAX, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.negative);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.cpsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmEORS, Carry) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = UINT32_MAX;
  ArmEORS(&registers, REGISTER_R0, REGISTER_R0, UINT32_MAX - 1, true);
  EXPECT_EQ(1u, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
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
  ArmMOVS(&registers, REGISTER_R15, 15u, true);
  EXPECT_EQ(15u, registers.current.user.gprs.r15);
  EXPECT_EQ(MODE_USR, registers.current.user.cpsr.mode);

  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.mode = 0;
  EXPECT_TRUE(ArmPrivilegedRegistersAreZero(registers.current));
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
  ArmMVNS(&registers, REGISTER_R15, ~15u, true);
  EXPECT_EQ(15u, registers.current.user.gprs.r15);
  EXPECT_EQ(MODE_USR, registers.current.user.cpsr.mode);

  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.mode = 0;
  EXPECT_TRUE(ArmPrivilegedRegistersAreZero(registers.current));
}

TEST(ArmORR, Compute) {
  auto registers = CreateArmGeneralPurposeRegistersRegisters();

  registers.r0 = 5u;
  ArmORR(&registers, REGISTER_R1, REGISTER_R0, 3u);
  EXPECT_EQ(7u, registers.r1);
  EXPECT_EQ(5u, registers.r0);

  registers.r0 = 0u;
  registers.r1 = 0u;
  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
}

TEST(ArmORR, SameSourceAndDest) {
  auto registers = CreateArmGeneralPurposeRegistersRegisters();

  registers.r0 = 5u;
  ArmORR(&registers, REGISTER_R0, REGISTER_R0, 3u);
  EXPECT_EQ(7u, registers.r0);

  registers.r0 = 0u;
  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
}

TEST(ArmORRS, Compute) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 5u;
  ArmORRS(&registers, REGISTER_R1, REGISTER_R0, 3u, false);
  EXPECT_EQ(7u, registers.current.user.gprs.r1);
  EXPECT_EQ(5u, registers.current.user.gprs.r0);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmORRS, SameSourceAndDest) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 5u;
  ArmORRS(&registers, REGISTER_R0, REGISTER_R0, 3u, false);
  EXPECT_EQ(7u, registers.current.user.gprs.r0);

  registers.current.user.gprs.r0 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmORRS, R15ToUSR) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;
  registers.current.spsr.mode = MODE_USR;

  registers.current.user.gprs.r15 = 5u;
  ArmORRS(&registers, REGISTER_R15, REGISTER_R15, 3u, false);
  EXPECT_EQ(7u, registers.current.user.gprs.r15);
  EXPECT_EQ(MODE_USR, registers.current.user.cpsr.mode);

  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.mode = 0;
  EXPECT_TRUE(ArmPrivilegedRegistersAreZero(registers.current));
}

TEST(ArmORRS, Zero) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 0u;
  ArmORRS(&registers, REGISTER_R0, REGISTER_R0, 0u, false);
  EXPECT_TRUE(registers.current.user.cpsr.zero);

  registers.current.user.cpsr.zero = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmORRS, Negative) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = UINT32_MAX;
  ArmORRS(&registers, REGISTER_R0, REGISTER_R0, 0u, false);
  EXPECT_EQ(UINT32_MAX, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.negative);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.cpsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmORRS, Carry) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 5u;
  ArmORRS(&registers, REGISTER_R0, REGISTER_R0, 3u, true);
  EXPECT_EQ(7u, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
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
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 1u;
  ArmRSBS(&registers, REGISTER_R1, REGISTER_R0, 3u);
  EXPECT_EQ(2u, registers.current.user.gprs.r1);
  EXPECT_EQ(1u, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmRSBS, SameSourceAndDest) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 2u;
  ArmRSBS(&registers, REGISTER_R0, REGISTER_R0, 4u);
  EXPECT_EQ(2u, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmRSBS, R15ToUSR) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;
  registers.current.spsr.mode = MODE_USR;

  registers.current.user.gprs.r15 = 2u;
  ArmRSBS(&registers, REGISTER_R15, REGISTER_R15, 4u);
  EXPECT_EQ(2u, registers.current.user.gprs.r15);
  EXPECT_EQ(MODE_USR, registers.current.user.cpsr.mode);

  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.mode = 0;
  EXPECT_TRUE(ArmPrivilegedRegistersAreZero(registers.current));
}

TEST(ArmRSBS, Zero) {
  auto registers = CreateArmAllRegisters();

  ArmRSBS(&registers, REGISTER_R0, REGISTER_R0, 0);
  EXPECT_TRUE(registers.current.user.cpsr.zero);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.cpsr.zero = false;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmRSBS, Negative) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 0;
  ArmRSBS(&registers, REGISTER_R0, REGISTER_R0, UINT32_MAX);
  EXPECT_EQ(UINT32_MAX, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.negative);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.cpsr.negative = false;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmRSBS, NotCarry) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 1;
  ArmRSBS(&registers, REGISTER_R0, REGISTER_R0, 0);
  EXPECT_EQ(UINT32_MAX, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.negative);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.cpsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmRSBS, Overflow) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0_s = -1;
  ArmRSBS(&registers, REGISTER_R0, REGISTER_R0, INT32_MAX);
  EXPECT_EQ(INT32_MIN, registers.current.user.gprs.r0_s);
  EXPECT_TRUE(registers.current.user.cpsr.overflow);
  EXPECT_TRUE(registers.current.user.cpsr.negative);

  registers.current.user.gprs.r0_s = 0;
  registers.current.user.cpsr.overflow = false;
  registers.current.user.cpsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmRSBS, Underflow) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 1;
  registers.current.user.gprs.r1_s = INT32_MIN;
  ArmRSBS(&registers, REGISTER_R0, REGISTER_R0, registers.current.user.gprs.r1);
  EXPECT_EQ(INT32_MAX, registers.current.user.gprs.r0_s);
  EXPECT_TRUE(registers.current.user.cpsr.overflow);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0_s = 0;
  registers.current.user.gprs.r1_s = 0;
  registers.current.user.cpsr.overflow = false;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
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
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 1u;
  ArmRSCS(&registers, REGISTER_R1, REGISTER_R0, 4u);
  EXPECT_EQ(2u, registers.current.user.gprs.r1);
  EXPECT_EQ(1u, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmRSCS, ComputeWithCarry) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 1u;
  registers.current.user.cpsr.carry = true;
  ArmRSCS(&registers, REGISTER_R1, REGISTER_R0, 5u);
  EXPECT_EQ(4u, registers.current.user.gprs.r1);
  EXPECT_EQ(1u, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmRSCS, SameSourceAndDest) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 2u;
  ArmRSCS(&registers, REGISTER_R0, REGISTER_R0, 6u);
  EXPECT_EQ(3u, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmASCS, R15ToUSR) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;
  registers.current.spsr.mode = MODE_USR;

  registers.current.user.gprs.r15 = 2u;
  ArmRSCS(&registers, REGISTER_R15, REGISTER_R15, 6u);
  EXPECT_EQ(3u, registers.current.user.gprs.r15);
  EXPECT_EQ(MODE_USR, registers.current.user.cpsr.mode);

  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.mode = 0;
  EXPECT_TRUE(ArmPrivilegedRegistersAreZero(registers.current));
}

TEST(ArmRSCS, Zero) {
  auto registers = CreateArmAllRegisters();

  ArmRSCS(&registers, REGISTER_R0, REGISTER_R0, 1u);
  EXPECT_TRUE(registers.current.user.cpsr.zero);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.cpsr.zero = false;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmRSCS, Negative) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 0u;
  ArmRSCS(&registers, REGISTER_R0, REGISTER_R0, UINT32_MAX);
  EXPECT_EQ(UINT32_MAX - 1u, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.negative);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.cpsr.negative = false;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmRSCS, NegativeWithCarry) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.cpsr.carry = true;
  ArmRSCS(&registers, REGISTER_R0, REGISTER_R0, UINT32_MAX);
  EXPECT_EQ(UINT32_MAX, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.negative);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.cpsr.negative = false;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmRSCS, Carry) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 0u;
  ArmRSCS(&registers, REGISTER_R0, REGISTER_R0, UINT32_MAX);
  EXPECT_EQ(UINT32_MAX - 1u, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.negative);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.cpsr.negative = false;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmRSCS, CarryWithCarry) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.cpsr.carry = true;
  ArmRSCS(&registers, REGISTER_R0, REGISTER_R0, UINT32_MAX);
  EXPECT_EQ(UINT32_MAX, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.negative);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.cpsr.negative = false;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmRSCS, Overflow) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0_s = -2;
  ArmRSCS(&registers, REGISTER_R0, REGISTER_R0, (uint32_t)INT32_MAX);
  EXPECT_EQ(INT32_MIN, registers.current.user.gprs.r0_s);
  EXPECT_TRUE(registers.current.user.cpsr.overflow);
  EXPECT_TRUE(registers.current.user.cpsr.negative);

  registers.current.user.gprs.r0_s = 0;
  registers.current.user.cpsr.overflow = false;
  registers.current.user.cpsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmRSCS, Underflow) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0_s = 0;
  registers.current.user.gprs.r1_s = INT32_MIN;
  ArmRSCS(&registers, REGISTER_R0, REGISTER_R0, registers.current.user.gprs.r1);
  EXPECT_EQ(INT32_MAX, registers.current.user.gprs.r0_s);
  EXPECT_TRUE(registers.current.user.cpsr.overflow);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0_s = 0;
  registers.current.user.gprs.r1_s = 0;
  registers.current.user.cpsr.overflow = false;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmRSCS, UnderflowWithCarry) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0_s = 1;
  registers.current.user.gprs.r1_s = INT32_MIN;
  registers.current.user.cpsr.carry = true;
  ArmRSCS(&registers, REGISTER_R0, REGISTER_R0, registers.current.user.gprs.r1);
  EXPECT_EQ(INT32_MAX, registers.current.user.gprs.r0_s);
  EXPECT_TRUE(registers.current.user.cpsr.overflow);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0_s = 0;
  registers.current.user.gprs.r1_s = 0;
  registers.current.user.cpsr.overflow = false;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmSBC, Compute) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 4u;
  ArmSBC(&registers, REGISTER_R1, REGISTER_R0, 1u);
  EXPECT_EQ(2u, registers.gprs.r1);
  EXPECT_EQ(4u, registers.gprs.r0);

  registers.gprs.r0 = 0u;
  registers.gprs.r1 = 0u;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmSBC, ComputeWithCarry) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 6u;
  registers.cpsr.carry = true;
  ArmSBC(&registers, REGISTER_R1, REGISTER_R0, 1u);
  EXPECT_EQ(5u, registers.gprs.r1);
  EXPECT_EQ(6u, registers.gprs.r0);

  registers.gprs.r0 = 0u;
  registers.gprs.r1 = 0u;
  registers.cpsr.carry = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmSBC, SameSourceAndDest) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 6u;
  ArmSBC(&registers, REGISTER_R0, REGISTER_R0, 2u);
  EXPECT_EQ(3u, registers.gprs.r0);

  registers.gprs.r0 = 0u;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmSBCS, Compute) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 4u;
  ArmSBCS(&registers, REGISTER_R1, REGISTER_R0, 1u);
  EXPECT_EQ(2u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmSBCS, ComputeWithCarry) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 5u;
  registers.current.user.cpsr.carry = true;
  ArmSBCS(&registers, REGISTER_R1, REGISTER_R0, 1u);
  EXPECT_EQ(4u, registers.current.user.gprs.r1);
  EXPECT_EQ(5u, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmSBCS, SameSourceAndDest) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 6u;
  ArmSBCS(&registers, REGISTER_R0, REGISTER_R0, 2u);
  EXPECT_EQ(3u, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmSBCS, R15ToUSR) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;
  registers.current.spsr.mode = MODE_USR;

  registers.current.user.gprs.r15 = 6u;
  ArmSBCS(&registers, REGISTER_R15, REGISTER_R15, 2u);
  EXPECT_EQ(3u, registers.current.user.gprs.r15);
  EXPECT_EQ(MODE_USR, registers.current.user.cpsr.mode);

  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.mode = 0;
  EXPECT_TRUE(ArmPrivilegedRegistersAreZero(registers.current));
}

TEST(ArmSBCS, Zero) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.cpsr.carry = true;
  ArmSBCS(&registers, REGISTER_R0, REGISTER_R0, 0u);
  EXPECT_TRUE(registers.current.user.cpsr.zero);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.cpsr.zero = false;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmSBCS, Negative) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = UINT32_MAX;
  ArmSBCS(&registers, REGISTER_R0, REGISTER_R0, 0u);
  EXPECT_EQ(UINT32_MAX - 1u, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.negative);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.cpsr.negative = false;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmSBCS, NegativeWithCarry) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = UINT32_MAX;
  registers.current.user.cpsr.carry = true;
  ArmSBCS(&registers, REGISTER_R0, REGISTER_R0, 0u);
  EXPECT_EQ(UINT32_MAX, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.negative);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.cpsr.negative = false;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmSBCS, Carry) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = UINT32_MAX;
  ArmSBCS(&registers, REGISTER_R0, REGISTER_R0, 0u);
  EXPECT_EQ(UINT32_MAX - 1u, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.negative);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.cpsr.negative = false;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmSBCS, CarryWithCarry) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = UINT32_MAX;
  registers.current.user.cpsr.carry = true;
  ArmSBCS(&registers, REGISTER_R0, REGISTER_R0, 0u);
  EXPECT_EQ(UINT32_MAX, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.negative);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.cpsr.negative = false;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmSBCS, Overflow) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0_s = INT32_MAX;
  ArmSBCS(&registers, REGISTER_R0, REGISTER_R0, (uint32_t)-2);
  EXPECT_EQ(INT32_MIN, registers.current.user.gprs.r0_s);
  EXPECT_TRUE(registers.current.user.cpsr.overflow);
  EXPECT_TRUE(registers.current.user.cpsr.negative);

  registers.current.user.gprs.r0_s = 0;
  registers.current.user.cpsr.overflow = false;
  registers.current.user.cpsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmSBCS, Underflow) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0_s = INT32_MIN;
  registers.current.user.gprs.r1_s = 0;
  ArmSBCS(&registers, REGISTER_R0, REGISTER_R0, registers.current.user.gprs.r1);
  EXPECT_EQ(INT32_MAX, registers.current.user.gprs.r0_s);
  EXPECT_TRUE(registers.current.user.cpsr.overflow);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0_s = 0;
  registers.current.user.gprs.r1_s = 0;
  registers.current.user.cpsr.overflow = false;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmSBCS, UnderflowWithCarry) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0_s = INT32_MIN;
  registers.current.user.gprs.r1_s = 1;
  registers.current.user.cpsr.carry = true;
  ArmSBCS(&registers, REGISTER_R0, REGISTER_R0, registers.current.user.gprs.r1);
  EXPECT_EQ(INT32_MAX, registers.current.user.gprs.r0_s);
  EXPECT_TRUE(registers.current.user.cpsr.overflow);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0_s = 0;
  registers.current.user.gprs.r1_s = 0;
  registers.current.user.cpsr.overflow = false;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmSUB, Compute) {
  auto registers = CreateArmGeneralPurposeRegistersRegisters();

  registers.r0 = 3u;
  ArmSUB(&registers, REGISTER_R1, REGISTER_R0, 1u);
  EXPECT_EQ(2u, registers.r1);
  EXPECT_EQ(3u, registers.r0);

  registers.r0 = 0u;
  registers.r1 = 0u;
  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
}

TEST(ArmSUB, SameSourceAndDest) {
  auto registers = CreateArmGeneralPurposeRegistersRegisters();

  registers.r0 = 4u;
  ArmSUB(&registers, REGISTER_R0, REGISTER_R0, 2u);
  EXPECT_EQ(2u, registers.r0);

  registers.r0 = 0u;
  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
}

TEST(ArmSUBS, Compute) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 3u;
  ArmSUBS(&registers, REGISTER_R1, REGISTER_R0, 1u);
  EXPECT_EQ(2u, registers.current.user.gprs.r1);
  EXPECT_EQ(3u, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmSUBS, SameSourceAndDest) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 4u;
  ArmSUBS(&registers, REGISTER_R0, REGISTER_R0, 2u);
  EXPECT_EQ(2u, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmSUBS, R15ToUSR) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;
  registers.current.spsr.mode = MODE_USR;

  registers.current.user.gprs.r15 = 4u;
  ArmSUBS(&registers, REGISTER_R15, REGISTER_R15, 2u);
  EXPECT_EQ(2u, registers.current.user.gprs.r15);
  EXPECT_EQ(MODE_USR, registers.current.user.cpsr.mode);

  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.mode = 0;
  EXPECT_TRUE(ArmPrivilegedRegistersAreZero(registers.current));
}

TEST(ArmSUBS, Zero) {
  auto registers = CreateArmAllRegisters();

  ArmSUBS(&registers, REGISTER_R0, REGISTER_R0, 0);
  EXPECT_TRUE(registers.current.user.cpsr.zero);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.cpsr.zero = false;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmSUBS, Negative) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = UINT32_MAX;
  ArmSUBS(&registers, REGISTER_R0, REGISTER_R0, 0);
  EXPECT_EQ(UINT32_MAX, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.negative);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.cpsr.negative = false;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmSUBS, NotCarry) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 0u;
  ArmSUBS(&registers, REGISTER_R0, REGISTER_R0, 1u);
  EXPECT_EQ(UINT32_MAX, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.negative);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.cpsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmSUBS, Overflow) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0_s = INT32_MAX;
  ArmSUBS(&registers, REGISTER_R0, REGISTER_R0, (uint32_t)-1);
  EXPECT_EQ(INT32_MIN, registers.current.user.gprs.r0_s);
  EXPECT_TRUE(registers.current.user.cpsr.overflow);
  EXPECT_TRUE(registers.current.user.cpsr.negative);

  registers.current.user.gprs.r0_s = 0;
  registers.current.user.cpsr.overflow = false;
  registers.current.user.cpsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmSUBS, Underflow) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0_s = INT32_MIN;
  registers.current.user.gprs.r1_s = 1;
  ArmSUBS(&registers, REGISTER_R0, REGISTER_R0, registers.current.user.gprs.r1);
  EXPECT_EQ(INT32_MAX, registers.current.user.gprs.r0_s);
  EXPECT_TRUE(registers.current.user.cpsr.overflow);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0_s = 0;
  registers.current.user.gprs.r1_s = 0;
  registers.current.user.cpsr.overflow = false;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmTEQ, Compute) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 5u;
  ArmTEQ(&registers, REGISTER_R0, 3u, false);
  EXPECT_EQ(5u, registers.gprs.r0);

  registers.gprs.r0 = 0u;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmTEQ, SameSourceAndDest) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 5u;
  ArmTEQ(&registers, REGISTER_R0, 3u, false);
  EXPECT_EQ(5u, registers.gprs.r0);

  registers.gprs.r0 = 0u;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmTEQ, Zero) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 1u;
  ArmTEQ(&registers, REGISTER_R0, 1u, false);
  EXPECT_EQ(1u, registers.gprs.r0);
  EXPECT_TRUE(registers.cpsr.zero);

  registers.gprs.r0 = 0u;
  registers.cpsr.zero = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmTEQ, Negative) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = UINT32_MAX;
  ArmTEQ(&registers, REGISTER_R0, 0u, false);
  EXPECT_EQ(UINT32_MAX, registers.gprs.r0);
  EXPECT_TRUE(registers.cpsr.negative);

  registers.gprs.r0 = 0u;
  registers.cpsr.negative = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmTEQ, Carry) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = UINT32_MAX;
  ArmTEQ(&registers, REGISTER_R0, UINT32_MAX - 1, true);
  EXPECT_EQ(UINT32_MAX, registers.gprs.r0);
  EXPECT_TRUE(registers.cpsr.carry);

  registers.gprs.r0 = 0u;
  registers.cpsr.carry = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmTST, Compute) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 15u;
  ArmTST(&registers, REGISTER_R0, 23u, false);
  EXPECT_EQ(15u, registers.gprs.r0);

  registers.gprs.r0 = 0u;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmTST, SameSourceAndDest) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 15u;
  ArmTST(&registers, REGISTER_R0, 23u, false);
  EXPECT_EQ(15u, registers.gprs.r0);

  registers.gprs.r0 = 0u;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmTST, Zero) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = 1u;
  ArmTST(&registers, REGISTER_R0, 2u, false);
  EXPECT_EQ(1u, registers.gprs.r0);
  EXPECT_TRUE(registers.cpsr.zero);

  registers.gprs.r0 = 0u;
  registers.cpsr.zero = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmTST, Negative) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = UINT32_MAX;
  ArmTST(&registers, REGISTER_R0, UINT32_MAX, false);
  EXPECT_EQ(UINT32_MAX, registers.gprs.r0);
  EXPECT_TRUE(registers.cpsr.negative);

  registers.gprs.r0 = 0u;
  registers.cpsr.negative = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmTST, Carry) {
  auto registers = CreateArmUserRegisters();

  registers.gprs.r0 = UINT32_MAX;
  ArmTST(&registers, REGISTER_R0, 1u, true);
  EXPECT_EQ(UINT32_MAX, registers.gprs.r0);
  EXPECT_TRUE(registers.cpsr.carry);

  registers.gprs.r0 = 0u;
  registers.cpsr.carry = false;
  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}