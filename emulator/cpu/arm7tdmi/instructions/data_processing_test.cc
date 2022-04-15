extern "C" {
#include "emulator/cpu/arm7tdmi/instructions/data_processing.h"
}

#include <strings.h>

#include "googletest/include/gtest/gtest.h"

bool ArmPrivilegedRegistersAreZero(const ArmPrivilegedRegisters& regs) {
  ArmPrivilegedRegisters zero;
  memset(&zero, 0, sizeof(ArmPrivilegedRegisters));
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
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 1u;
  ArmADC(&registers, REGISTER_R1, REGISTER_R0, 2u);
  EXPECT_EQ(3u, registers.current.user.gprs.r1);
  EXPECT_EQ(1u, registers.current.user.gprs.r0);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmADC, ComputeWithCarry) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 1u;
  registers.current.user.cpsr.carry = true;
  ArmADC(&registers, REGISTER_R1, REGISTER_R0, 2u);
  EXPECT_EQ(4u, registers.current.user.gprs.r1);
  EXPECT_EQ(1u, registers.current.user.gprs.r0);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmADC, SameSourceAndDest) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 2u;
  ArmADC(&registers, REGISTER_R0, REGISTER_R0, 2u);
  EXPECT_EQ(4u, registers.current.user.gprs.r0);

  registers.current.user.gprs.r0 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
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

  registers.current.user.gprs.r15 = 4u;
  ArmADCS(&registers, REGISTER_R15, REGISTER_R15, 8u);
  EXPECT_EQ(12u, ArmNextInstruction(&registers));
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
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 1u;
  ArmADD(&registers, REGISTER_R1, REGISTER_R0, 2u);
  EXPECT_EQ(3u, registers.current.user.gprs.r1);
  EXPECT_EQ(1u, registers.current.user.gprs.r0);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmADD, SameSourceAndDest) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 2u;
  ArmADD(&registers, REGISTER_R0, REGISTER_R0, 2u);
  EXPECT_EQ(4u, registers.current.user.gprs.r0);

  registers.current.user.gprs.r0 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
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

  registers.current.user.gprs.r15 = 4u;
  ArmADDS(&registers, REGISTER_R15, REGISTER_R15, 8u);
  EXPECT_EQ(12u, ArmNextInstruction(&registers));
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
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 15u;
  ArmAND(&registers, REGISTER_R1, REGISTER_R0, 23u);
  EXPECT_EQ(7u, registers.current.user.gprs.r1);
  EXPECT_EQ(15u, registers.current.user.gprs.r0);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmAND, SameSourceAndDest) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 15u;
  ArmAND(&registers, REGISTER_R0, REGISTER_R0, 23u);
  EXPECT_EQ(7u, registers.current.user.gprs.r0);

  registers.current.user.gprs.r0 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
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

  registers.current.user.gprs.r15 = 0xEFu;
  ArmANDS(&registers, REGISTER_R15, REGISTER_R15, 0xF0u, false);
  EXPECT_EQ(0xE0u, ArmNextInstruction(&registers));
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
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 15u;
  ArmBIC(&registers, REGISTER_R1, REGISTER_R0, 1u);
  EXPECT_EQ(14u, registers.current.user.gprs.r1);
  EXPECT_EQ(15u, registers.current.user.gprs.r0);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmBIC, SameSourceAndDest) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 15u;
  ArmBIC(&registers, REGISTER_R0, REGISTER_R0, 1u);
  EXPECT_EQ(14u, registers.current.user.gprs.r0);

  registers.current.user.gprs.r0 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
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
  ArmBICS(&registers, REGISTER_R15, REGISTER_R15, 3u, false);
  EXPECT_EQ(12u, ArmNextInstruction(&registers));
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
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 0u;
  ArmCMN(&registers, REGISTER_R0, 0u);
  EXPECT_TRUE(registers.current.user.cpsr.zero);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.cpsr.zero = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmCMN, Negative) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0_s = INT32_MIN;
  ArmCMN(&registers, REGISTER_R0, 0);
  EXPECT_EQ(INT32_MIN, registers.current.user.gprs.r0_s);
  EXPECT_TRUE(registers.current.user.cpsr.negative);

  registers.current.user.gprs.r0_s = 0;
  registers.current.user.cpsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmCMN, Carry) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = UINT32_MAX;
  ArmCMN(&registers, REGISTER_R0, 1);
  EXPECT_EQ(UINT32_MAX, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.carry);
  EXPECT_TRUE(registers.current.user.cpsr.zero);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.cpsr.carry = false;
  registers.current.user.cpsr.zero = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmCMN, Overflow) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0_s = INT32_MAX;
  ArmCMN(&registers, REGISTER_R0, 1);
  EXPECT_EQ(INT32_MAX, registers.current.user.gprs.r0_s);
  EXPECT_TRUE(registers.current.user.cpsr.overflow);
  EXPECT_TRUE(registers.current.user.cpsr.negative);

  registers.current.user.gprs.r0_s = 0;
  registers.current.user.cpsr.overflow = false;
  registers.current.user.cpsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmCMN, Underflow) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0_s = -1;
  ArmCMN(&registers, REGISTER_R0, INT32_MIN);
  EXPECT_EQ(-1, registers.current.user.gprs.r0_s);
  EXPECT_TRUE(registers.current.user.cpsr.overflow);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0_s = 0;
  registers.current.user.cpsr.overflow = false;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmCMP, Zero) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 1u;
  ArmCMP(&registers, REGISTER_R0, 1u);
  EXPECT_TRUE(registers.current.user.cpsr.zero);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.cpsr.zero = false;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmCMP, Negative) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0_s = INT32_MIN;
  ArmCMP(&registers, REGISTER_R0, 0);
  EXPECT_EQ(INT32_MIN, registers.current.user.gprs.r0_s);
  EXPECT_TRUE(registers.current.user.cpsr.negative);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0_s = 0;
  registers.current.user.cpsr.negative = false;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmCMP, NotCarry) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 1u;
  ArmCMP(&registers, REGISTER_R0, UINT32_MAX);
  EXPECT_EQ(1u, registers.current.user.gprs.r0);

  registers.current.user.gprs.r0 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmCMP, Overflow) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0_s = INT32_MAX;
  ArmCMP(&registers, REGISTER_R0, UINT32_MAX);
  EXPECT_EQ(INT32_MAX, registers.current.user.gprs.r0_s);
  EXPECT_TRUE(registers.current.user.cpsr.overflow);
  EXPECT_TRUE(registers.current.user.cpsr.negative);

  registers.current.user.gprs.r0_s = 0;
  registers.current.user.cpsr.overflow = false;
  registers.current.user.cpsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmCMP, Underflow) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0_s = INT32_MIN;
  ArmCMP(&registers, REGISTER_R0, 1);
  EXPECT_EQ(INT32_MIN, registers.current.user.gprs.r0_s);
  EXPECT_TRUE(registers.current.user.cpsr.overflow);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0_s = 0;
  registers.current.user.cpsr.overflow = false;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmEOR, Compute) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 5u;
  ArmEOR(&registers, REGISTER_R1, REGISTER_R0, 3u);
  EXPECT_EQ(6u, registers.current.user.gprs.r1);
  EXPECT_EQ(5u, registers.current.user.gprs.r0);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmEOR, SameSourceAndDest) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 5u;
  ArmEOR(&registers, REGISTER_R0, REGISTER_R0, 3u);
  EXPECT_EQ(6u, registers.current.user.gprs.r0);

  registers.current.user.gprs.r0 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
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

  registers.current.user.gprs.r15 = 7u;
  ArmEORS(&registers, REGISTER_R15, REGISTER_R15, 3u, false);
  EXPECT_EQ(4u, ArmNextInstruction(&registers));
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
  auto registers = CreateArmAllRegisters();

  ArmMOV(&registers, REGISTER_R0, 3u);
  EXPECT_EQ(3u, registers.current.user.gprs.r0);

  registers.current.user.gprs.r0 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
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
  ArmMOVS(&registers, REGISTER_R15, 16u, true);
  EXPECT_EQ(16u, ArmNextInstruction(&registers));
  EXPECT_EQ(MODE_USR, registers.current.user.cpsr.mode);

  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.mode = 0;
  EXPECT_TRUE(ArmPrivilegedRegistersAreZero(registers.current));
}

TEST(ArmMVN, Compute) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 5u;
  ArmMVN(&registers, REGISTER_R0, UINT32_MAX - 1u);
  EXPECT_EQ(1u, registers.current.user.gprs.r0);

  registers.current.user.gprs.r0 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
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
  ArmMVNS(&registers, REGISTER_R15, ~16u, true);
  EXPECT_EQ(16u, ArmNextInstruction(&registers));
  EXPECT_EQ(MODE_USR, registers.current.user.cpsr.mode);

  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.mode = 0;
  EXPECT_TRUE(ArmPrivilegedRegistersAreZero(registers.current));
}

TEST(ArmORR, Compute) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 5u;
  ArmORR(&registers, REGISTER_R1, REGISTER_R0, 3u);
  EXPECT_EQ(7u, registers.current.user.gprs.r1);
  EXPECT_EQ(5u, registers.current.user.gprs.r0);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmORR, SameSourceAndDest) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 5u;
  ArmORR(&registers, REGISTER_R0, REGISTER_R0, 3u);
  EXPECT_EQ(7u, registers.current.user.gprs.r0);

  registers.current.user.gprs.r0 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
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

  registers.current.user.gprs.r15 = 4u;
  ArmORRS(&registers, REGISTER_R15, REGISTER_R15, 8u, false);
  EXPECT_EQ(12u, ArmNextInstruction(&registers));
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
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 1u;
  ArmRSB(&registers, REGISTER_R1, REGISTER_R0, 3u);
  EXPECT_EQ(2u, registers.current.user.gprs.r1);
  EXPECT_EQ(1u, registers.current.user.gprs.r0);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmRSB, SameSourceAndDest) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 2u;
  ArmRSB(&registers, REGISTER_R0, REGISTER_R0, 4u);
  EXPECT_EQ(2u, registers.current.user.gprs.r0);

  registers.current.user.gprs.r0 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
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

  registers.current.user.gprs.r15 = 4u;
  ArmRSBS(&registers, REGISTER_R15, REGISTER_R15, 12u);
  EXPECT_EQ(8u, ArmNextInstruction(&registers));
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
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 1u;
  ArmRSC(&registers, REGISTER_R1, REGISTER_R0, 4u);
  EXPECT_EQ(2u, registers.current.user.gprs.r1);
  EXPECT_EQ(1u, registers.current.user.gprs.r0);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmRSC, ComputeWithCarry) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 1u;
  registers.current.user.cpsr.carry = true;
  ArmRSC(&registers, REGISTER_R1, REGISTER_R0, 6u);
  EXPECT_EQ(5u, registers.current.user.gprs.r1);
  EXPECT_EQ(1u, registers.current.user.gprs.r0);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmRSC, SameSourceAndDest) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 2u;
  ArmRSC(&registers, REGISTER_R0, REGISTER_R0, 6u);
  EXPECT_EQ(3u, registers.current.user.gprs.r0);

  registers.current.user.gprs.r0 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
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

TEST(ArmRSCS, R15ToUSR) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;
  registers.current.spsr.mode = MODE_USR;

  registers.current.user.gprs.r15 = 4u;
  ArmRSCS(&registers, REGISTER_R15, REGISTER_R15, 13u);
  EXPECT_EQ(8u, ArmNextInstruction(&registers));
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
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 4u;
  ArmSBC(&registers, REGISTER_R1, REGISTER_R0, 1u);
  EXPECT_EQ(2u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.r0);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmSBC, ComputeWithCarry) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 6u;
  registers.current.user.cpsr.carry = true;
  ArmSBC(&registers, REGISTER_R1, REGISTER_R0, 1u);
  EXPECT_EQ(5u, registers.current.user.gprs.r1);
  EXPECT_EQ(6u, registers.current.user.gprs.r0);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmSBC, SameSourceAndDest) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 6u;
  ArmSBC(&registers, REGISTER_R0, REGISTER_R0, 2u);
  EXPECT_EQ(3u, registers.current.user.gprs.r0);

  registers.current.user.gprs.r0 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
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

  registers.current.user.gprs.r15 = 13u;
  ArmSBCS(&registers, REGISTER_R15, REGISTER_R15, 4u);
  EXPECT_EQ(8u, ArmNextInstruction(&registers));
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
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 3u;
  ArmSUB(&registers, REGISTER_R1, REGISTER_R0, 1u);
  EXPECT_EQ(2u, registers.current.user.gprs.r1);
  EXPECT_EQ(3u, registers.current.user.gprs.r0);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmSUB, SameSourceAndDest) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 4u;
  ArmSUB(&registers, REGISTER_R0, REGISTER_R0, 2u);
  EXPECT_EQ(2u, registers.current.user.gprs.r0);

  registers.current.user.gprs.r0 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
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

  registers.current.user.gprs.r15 = 12u;
  ArmSUBS(&registers, REGISTER_R15, REGISTER_R15, 4u);
  EXPECT_EQ(8u, ArmNextInstruction(&registers));
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
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 5u;
  ArmTEQ(&registers, REGISTER_R0, 3u, false);
  EXPECT_EQ(5u, registers.current.user.gprs.r0);

  registers.current.user.gprs.r0 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmTEQ, SameSourceAndDest) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 5u;
  ArmTEQ(&registers, REGISTER_R0, 3u, false);
  EXPECT_EQ(5u, registers.current.user.gprs.r0);

  registers.current.user.gprs.r0 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmTEQ, Zero) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 1u;
  ArmTEQ(&registers, REGISTER_R0, 1u, false);
  EXPECT_EQ(1u, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.zero);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.cpsr.zero = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmTEQ, Negative) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = UINT32_MAX;
  ArmTEQ(&registers, REGISTER_R0, 0u, false);
  EXPECT_EQ(UINT32_MAX, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.negative);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.cpsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmTEQ, Carry) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = UINT32_MAX;
  ArmTEQ(&registers, REGISTER_R0, UINT32_MAX - 1, true);
  EXPECT_EQ(UINT32_MAX, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmTST, Compute) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 15u;
  ArmTST(&registers, REGISTER_R0, 23u, false);
  EXPECT_EQ(15u, registers.current.user.gprs.r0);

  registers.current.user.gprs.r0 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmTST, SameSourceAndDest) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 15u;
  ArmTST(&registers, REGISTER_R0, 23u, false);
  EXPECT_EQ(15u, registers.current.user.gprs.r0);

  registers.current.user.gprs.r0 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmTST, Zero) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 1u;
  ArmTST(&registers, REGISTER_R0, 2u, false);
  EXPECT_EQ(1u, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.zero);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.cpsr.zero = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmTST, Negative) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = UINT32_MAX;
  ArmTST(&registers, REGISTER_R0, UINT32_MAX, false);
  EXPECT_EQ(UINT32_MAX, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.negative);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.cpsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmTST, Carry) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = UINT32_MAX;
  ArmTST(&registers, REGISTER_R0, 1u, true);
  EXPECT_EQ(UINT32_MAX, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}