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

  ArmADC(&registers, REGISTER_R1, 1u, 2u);
  EXPECT_EQ(3u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r15 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmADC, ComputeWithCarry) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.cpsr.carry = true;
  ArmADC(&registers, REGISTER_R1, 1u, 2u);
  EXPECT_EQ(4u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmADCS, Compute) {
  auto registers = CreateArmAllRegisters();

  ArmADCS(&registers, REGISTER_R1, 1u, 2u);
  EXPECT_EQ(3u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r15 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmADCS, R15ToUSR) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;
  registers.current.spsr.mode = MODE_USR;

  registers.current.user.gprs.r15 = 4u;
  ArmADCS(&registers, REGISTER_R15, 4u, 8u);
  EXPECT_EQ(12u, ArmCurrentInstruction(&registers));
  EXPECT_EQ(MODE_USR, registers.current.user.cpsr.mode);

  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.mode = 0;
  EXPECT_TRUE(ArmPrivilegedRegistersAreZero(registers.current));
}

TEST(ArmADCS, ComputeWithCarry) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.cpsr.carry = true;
  ArmADCS(&registers, REGISTER_R1, 1u, 2u);
  EXPECT_EQ(4u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r15 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmADCS, Zero) {
  auto registers = CreateArmAllRegisters();

  ArmADCS(&registers, REGISTER_R0, 0u, 0u);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.zero);

  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.zero = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmADCS, Negative) {
  auto registers = CreateArmAllRegisters();

  ArmADCS(&registers, REGISTER_R0, UINT32_MAX, 0u);
  EXPECT_EQ(UINT32_MAX, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.negative);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmADCS, NegativeWithCarry) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.cpsr.carry = true;
  ArmADCS(&registers, REGISTER_R0, UINT32_MAX - 1u, 0u);
  EXPECT_EQ(UINT32_MAX, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.negative);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmADCS, Carry) {
  auto registers = CreateArmAllRegisters();

  ArmADCS(&registers, REGISTER_R0, UINT32_MAX, 1);
  EXPECT_EQ(0u, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.carry);
  EXPECT_TRUE(registers.current.user.cpsr.zero);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.carry = false;
  registers.current.user.cpsr.zero = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmADCS, CarryWithCarry) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.cpsr.carry = true;
  ArmADCS(&registers, REGISTER_R0, UINT32_MAX, 0u);
  EXPECT_EQ(0u, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.carry);
  EXPECT_TRUE(registers.current.user.cpsr.zero);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.carry = false;
  registers.current.user.cpsr.zero = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmADCS, Overflow) {
  auto registers = CreateArmAllRegisters();

  ArmADCS(&registers, REGISTER_R0, INT32_MAX, 1);
  EXPECT_EQ(INT32_MIN, registers.current.user.gprs.r0_s);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.overflow);
  EXPECT_TRUE(registers.current.user.cpsr.negative);

  registers.current.user.gprs.r0_s = 0;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.overflow = false;
  registers.current.user.cpsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmADCS, OverflowWithCarry) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.cpsr.carry = true;
  ArmADCS(&registers, REGISTER_R0, INT32_MAX, 0u);
  EXPECT_EQ(INT32_MIN, registers.current.user.gprs.r0_s);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.overflow);
  EXPECT_TRUE(registers.current.user.cpsr.negative);

  registers.current.user.gprs.r0_s = 0;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.overflow = false;
  registers.current.user.cpsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmADCS, Underflow) {
  auto registers = CreateArmAllRegisters();

  ArmADCS(&registers, REGISTER_R0, -1, INT32_MIN);
  EXPECT_EQ(INT32_MAX, registers.current.user.gprs.r0_s);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.overflow);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0_s = 0;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.overflow = false;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmADD, Compute) {
  auto registers = CreateArmAllRegisters();

  ArmADD(&registers, REGISTER_R1, 1u, 2u);
  EXPECT_EQ(3u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r15 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmADDS, Compute) {
  auto registers = CreateArmAllRegisters();

  ArmADDS(&registers, REGISTER_R1, 1u, 2u);
  EXPECT_EQ(3u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r15 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmADDS, R15ToUSR) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;
  registers.current.spsr.mode = MODE_USR;

  ArmADDS(&registers, REGISTER_R15, 4u, 8u);
  EXPECT_EQ(12u, ArmCurrentInstruction(&registers));
  EXPECT_EQ(MODE_USR, registers.current.user.cpsr.mode);

  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.mode = 0;
  EXPECT_TRUE(ArmPrivilegedRegistersAreZero(registers.current));
}

TEST(ArmADDS, Zero) {
  auto registers = CreateArmAllRegisters();

  ArmADDS(&registers, REGISTER_R0, 0u, 0u);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.zero);

  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.zero = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmADDS, Negative) {
  auto registers = CreateArmAllRegisters();

  ArmADDS(&registers, REGISTER_R0, UINT32_MAX, 0u);
  EXPECT_EQ(UINT32_MAX, registers.current.user.gprs.r0);
  EXPECT_TRUE(registers.current.user.cpsr.negative);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmADDS, Carry) {
  auto registers = CreateArmAllRegisters();

  ArmADDS(&registers, REGISTER_R0, UINT32_MAX, 1);
  EXPECT_EQ(0u, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.carry);
  EXPECT_TRUE(registers.current.user.cpsr.zero);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.carry = false;
  registers.current.user.cpsr.zero = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmADDS, Overflow) {
  auto registers = CreateArmAllRegisters();

  ArmADDS(&registers, REGISTER_R0, INT32_MAX, 1);
  EXPECT_EQ(INT32_MIN, registers.current.user.gprs.r0_s);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.overflow);
  EXPECT_TRUE(registers.current.user.cpsr.negative);

  registers.current.user.gprs.r0_s = 0;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.overflow = false;
  registers.current.user.cpsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmADDS, Underflow) {
  auto registers = CreateArmAllRegisters();

  ArmADDS(&registers, REGISTER_R0, -1, INT32_MIN);
  EXPECT_EQ(INT32_MAX, registers.current.user.gprs.r0_s);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.overflow);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0_s = 0;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.overflow = false;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmAND, Compute) {
  auto registers = CreateArmAllRegisters();

  ArmAND(&registers, REGISTER_R1, 15u, 23u);
  EXPECT_EQ(7u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r15 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmANDS, Compute) {
  auto registers = CreateArmAllRegisters();

  ArmANDS(&registers, REGISTER_R1, 15u, 23u, false);
  EXPECT_EQ(7u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r15 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmANDS, R15ToUSR) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;
  registers.current.spsr.mode = MODE_USR;

  registers.current.user.gprs.r15 = 0xEFu;
  ArmANDS(&registers, REGISTER_R15, 0xEFu, 0xF0u, false);
  EXPECT_EQ(0xE0u, ArmCurrentInstruction(&registers));
  EXPECT_EQ(MODE_USR, registers.current.user.cpsr.mode);

  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.mode = 0;
  EXPECT_TRUE(ArmPrivilegedRegistersAreZero(registers.current));
}

TEST(ArmANDS, Zero) {
  auto registers = CreateArmAllRegisters();

  ArmANDS(&registers, REGISTER_R0, 1u, 2u, false);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.zero);

  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.zero = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmANDS, Negative) {
  auto registers = CreateArmAllRegisters();

  ArmANDS(&registers, REGISTER_R0, UINT32_MAX, UINT32_MAX, false);
  EXPECT_EQ(UINT32_MAX, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.negative);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmANDS, Carry) {
  auto registers = CreateArmAllRegisters();

  ArmANDS(&registers, REGISTER_R0, UINT32_MAX, 1u, true);
  EXPECT_EQ(1u, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmBIC, Compute) {
  auto registers = CreateArmAllRegisters();

  ArmBIC(&registers, REGISTER_R1, 15u, 1u);
  EXPECT_EQ(14u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r15 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmBICS, Compute) {
  auto registers = CreateArmAllRegisters();

  ArmBICS(&registers, REGISTER_R1, 15u, 1u, false);
  EXPECT_EQ(14u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r15 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmBICS, R15ToUSR) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;
  registers.current.spsr.mode = MODE_USR;

  registers.current.user.gprs.r15 = 15u;
  ArmBICS(&registers, REGISTER_R15, 15u, 3u, false);
  EXPECT_EQ(12u, ArmCurrentInstruction(&registers));
  EXPECT_EQ(MODE_USR, registers.current.user.cpsr.mode);

  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.mode = 0;
  EXPECT_TRUE(ArmPrivilegedRegistersAreZero(registers.current));
}

TEST(ArmBICS, Zero) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 1u;
  ArmBICS(&registers, REGISTER_R0, 1u, 1u, false);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.zero);

  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.zero = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmBICS, Negative) {
  auto registers = CreateArmAllRegisters();

  ArmBICS(&registers, REGISTER_R0, UINT32_MAX, 1, false);
  EXPECT_EQ(UINT32_MAX - 1u, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.negative);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmBICS, Carry) {
  auto registers = CreateArmAllRegisters();

  ArmBICS(&registers, REGISTER_R0, UINT32_MAX, 1, true);
  EXPECT_EQ(UINT32_MAX - 1u, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.negative);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.negative = false;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmCMN, Zero) {
  auto registers = CreateArmAllRegisters();

  ArmCMN(&registers, REGISTER_R0, 0u, 0u);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.zero);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.zero = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmCMN, Negative) {
  auto registers = CreateArmAllRegisters();

  ArmCMN(&registers, REGISTER_R0, INT32_MIN, 0u);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.negative);

  registers.current.user.gprs.r0_s = 0;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmCMN, Carry) {
  auto registers = CreateArmAllRegisters();

  ArmCMN(&registers, REGISTER_R0, UINT32_MAX, 1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.carry);
  EXPECT_TRUE(registers.current.user.cpsr.zero);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.carry = false;
  registers.current.user.cpsr.zero = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmCMN, Overflow) {
  auto registers = CreateArmAllRegisters();

  ArmCMN(&registers, REGISTER_R0, INT32_MAX, 1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.overflow);
  EXPECT_TRUE(registers.current.user.cpsr.negative);

  registers.current.user.gprs.r0_s = 0;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.overflow = false;
  registers.current.user.cpsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmCMN, Underflow) {
  auto registers = CreateArmAllRegisters();

  ArmCMN(&registers, REGISTER_R0, -1, INT32_MIN);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.overflow);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0_s = 0;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.overflow = false;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmCMN, R15ToUSR) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;
  registers.current.spsr.mode = MODE_USR;

  ArmCMN(&registers, REGISTER_R15, 0u, 0u);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_EQ(MODE_USR, registers.current.user.cpsr.mode);

  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.mode = 0;
  EXPECT_TRUE(ArmPrivilegedRegistersAreZero(registers.current));
}

TEST(ArmCMP, Zero) {
  auto registers = CreateArmAllRegisters();

  ArmCMP(&registers, REGISTER_R0, 1u, 1u);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.zero);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.zero = false;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmCMP, Negative) {
  auto registers = CreateArmAllRegisters();

  ArmCMP(&registers, REGISTER_R0, INT32_MIN, 0u);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.negative);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0_s = 0;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.negative = false;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmCMP, NotCarry) {
  auto registers = CreateArmAllRegisters();

  ArmCMP(&registers, REGISTER_R0, 1u, UINT32_MAX);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r15 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmCMP, Overflow) {
  auto registers = CreateArmAllRegisters();

  ArmCMP(&registers, REGISTER_R0, INT32_MAX, UINT32_MAX);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.overflow);
  EXPECT_TRUE(registers.current.user.cpsr.negative);

  registers.current.user.gprs.r0_s = 0;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.overflow = false;
  registers.current.user.cpsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmCMP, Underflow) {
  auto registers = CreateArmAllRegisters();

  ArmCMP(&registers, REGISTER_R0, INT32_MIN, 1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.overflow);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0_s = 0;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.overflow = false;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmCMP, R15ToUSR) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;
  registers.current.spsr.mode = MODE_USR;

  ArmCMP(&registers, REGISTER_R15, 0u, 0u);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_EQ(MODE_USR, registers.current.user.cpsr.mode);

  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.mode = 0;
  EXPECT_TRUE(ArmPrivilegedRegistersAreZero(registers.current));
}

TEST(ArmEOR, Compute) {
  auto registers = CreateArmAllRegisters();

  ArmEOR(&registers, REGISTER_R1, 5u, 3u);
  EXPECT_EQ(6u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r15 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmEORS, Compute) {
  auto registers = CreateArmAllRegisters();

  ArmEORS(&registers, REGISTER_R1, 5u, 3u, false);
  EXPECT_EQ(6u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r15 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmEORS, R15ToUSR) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;
  registers.current.spsr.mode = MODE_USR;

  ArmEORS(&registers, REGISTER_R15, 7u, 3u, false);
  EXPECT_EQ(4u, ArmCurrentInstruction(&registers));
  EXPECT_EQ(MODE_USR, registers.current.user.cpsr.mode);

  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.mode = 0;
  EXPECT_TRUE(ArmPrivilegedRegistersAreZero(registers.current));
}

TEST(ArmEORS, Zero) {
  auto registers = CreateArmAllRegisters();

  ArmEORS(&registers, REGISTER_R0, 1u, 1u, false);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.zero);

  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.zero = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmEORS, Negative) {
  auto registers = CreateArmAllRegisters();

  ArmEORS(&registers, REGISTER_R0, UINT32_MAX, 0u, false);
  EXPECT_EQ(UINT32_MAX, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.negative);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmEORS, Carry) {
  auto registers = CreateArmAllRegisters();

  ArmEORS(&registers, REGISTER_R0, UINT32_MAX, UINT32_MAX - 1, true);
  EXPECT_EQ(1u, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmMOV, Compute) {
  auto registers = CreateArmAllRegisters();

  ArmMOV(&registers, REGISTER_R0, 3u);
  EXPECT_EQ(3u, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r15 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmMOVS, Compute) {
  auto registers = CreateArmAllRegisters();

  ArmMOVS(&registers, REGISTER_R0, 3u, false);
  EXPECT_EQ(3u, registers.current.user.gprs.r0);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r15 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmMOVS, Zero) {
  auto registers = CreateArmAllRegisters();

  ArmMOVS(&registers, REGISTER_R0, 0u, false);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.zero);

  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.zero = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmMOVS, Negative) {
  auto registers = CreateArmAllRegisters();

  ArmMOVS(&registers, REGISTER_R0, UINT32_MAX, false);
  EXPECT_EQ(UINT32_MAX, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.negative);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmMOVS, Carry) {
  auto registers = CreateArmAllRegisters();

  ArmMOVS(&registers, REGISTER_R0, 1u, true);
  EXPECT_EQ(1u, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r15 = 0u;
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
  EXPECT_EQ(16u, ArmCurrentInstruction(&registers));
  EXPECT_EQ(MODE_USR, registers.current.user.cpsr.mode);

  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.mode = 0;
  EXPECT_TRUE(ArmPrivilegedRegistersAreZero(registers.current));
}

TEST(ArmMVN, Compute) {
  auto registers = CreateArmAllRegisters();

  ArmMVN(&registers, REGISTER_R0, UINT32_MAX - 1u);
  EXPECT_EQ(1u, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r15 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmMVNS, Compute) {
  auto registers = CreateArmAllRegisters();

  ArmMVNS(&registers, REGISTER_R0, UINT32_MAX - 1u, false);
  EXPECT_EQ(1u, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r15 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmMVNS, Zero) {
  auto registers = CreateArmAllRegisters();

  ArmMVNS(&registers, REGISTER_R0, UINT32_MAX, false);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.zero);

  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.zero = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmMVNS, Negative) {
  auto registers = CreateArmAllRegisters();

  ArmMVNS(&registers, REGISTER_R0, 0u, false);
  EXPECT_EQ(UINT32_MAX, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.negative);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmMVNS, Carry) {
  auto registers = CreateArmAllRegisters();

  ArmMVNS(&registers, REGISTER_R0, UINT32_MAX - 1, true);
  EXPECT_EQ(1u, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r15 = 0u;
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
  EXPECT_EQ(16u, ArmCurrentInstruction(&registers));
  EXPECT_EQ(MODE_USR, registers.current.user.cpsr.mode);

  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.mode = 0;
  EXPECT_TRUE(ArmPrivilegedRegistersAreZero(registers.current));
}

TEST(ArmORR, Compute) {
  auto registers = CreateArmAllRegisters();

  ArmORR(&registers, REGISTER_R1, 5u, 3u);
  EXPECT_EQ(7u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r15 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmORRS, Compute) {
  auto registers = CreateArmAllRegisters();

  ArmORRS(&registers, REGISTER_R1, 5u, 3u, false);
  EXPECT_EQ(7u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r15 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmORRS, R15ToUSR) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;
  registers.current.spsr.mode = MODE_USR;

  ArmORRS(&registers, REGISTER_R15, 4u, 8u, false);
  EXPECT_EQ(12u, ArmCurrentInstruction(&registers));
  EXPECT_EQ(MODE_USR, registers.current.user.cpsr.mode);

  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.mode = 0;
  EXPECT_TRUE(ArmPrivilegedRegistersAreZero(registers.current));
}

TEST(ArmORRS, Zero) {
  auto registers = CreateArmAllRegisters();

  ArmORRS(&registers, REGISTER_R0, 0u, 0u, false);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.zero);

  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.zero = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmORRS, Negative) {
  auto registers = CreateArmAllRegisters();

  ArmORRS(&registers, REGISTER_R0, UINT32_MAX, 0u, false);
  EXPECT_EQ(UINT32_MAX, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.negative);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmORRS, Carry) {
  auto registers = CreateArmAllRegisters();

  ArmORRS(&registers, REGISTER_R0, 5u, 3u, true);
  EXPECT_EQ(7u, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmRSB, Compute) {
  auto registers = CreateArmAllRegisters();

  ArmRSB(&registers, REGISTER_R1, 1u, 3u);
  EXPECT_EQ(2u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r15 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmRSBS, Compute) {
  auto registers = CreateArmAllRegisters();

  ArmRSBS(&registers, REGISTER_R1, 1u, 3u);
  EXPECT_EQ(2u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmRSBS, R15ToUSR) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;
  registers.current.spsr.mode = MODE_USR;

  ArmRSBS(&registers, REGISTER_R15, 4u, 12u);
  EXPECT_EQ(8u, ArmCurrentInstruction(&registers));
  EXPECT_EQ(MODE_USR, registers.current.user.cpsr.mode);

  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.mode = 0;
  EXPECT_TRUE(ArmPrivilegedRegistersAreZero(registers.current));
}

TEST(ArmRSBS, Zero) {
  auto registers = CreateArmAllRegisters();

  ArmRSBS(&registers, REGISTER_R0, 0u, 0u);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.zero);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.zero = false;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmRSBS, Negative) {
  auto registers = CreateArmAllRegisters();

  ArmRSBS(&registers, REGISTER_R0, 0u, UINT32_MAX);
  EXPECT_EQ(UINT32_MAX, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.negative);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.negative = false;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmRSBS, NotCarry) {
  auto registers = CreateArmAllRegisters();

  ArmRSBS(&registers, REGISTER_R0, 1u, 0u);
  EXPECT_EQ(UINT32_MAX, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.negative);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmRSBS, Overflow) {
  auto registers = CreateArmAllRegisters();

  ArmRSBS(&registers, REGISTER_R0, -1, INT32_MAX);
  EXPECT_EQ(INT32_MIN, registers.current.user.gprs.r0_s);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.overflow);
  EXPECT_TRUE(registers.current.user.cpsr.negative);

  registers.current.user.gprs.r0_s = 0;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.overflow = false;
  registers.current.user.cpsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmRSBS, Underflow) {
  auto registers = CreateArmAllRegisters();

  ArmRSBS(&registers, REGISTER_R0, 1u, INT32_MIN);
  EXPECT_EQ(INT32_MAX, registers.current.user.gprs.r0_s);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.overflow);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0_s = 0;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.overflow = false;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmRSC, Compute) {
  auto registers = CreateArmAllRegisters();

  ArmRSC(&registers, REGISTER_R1, 1u, 4u);
  EXPECT_EQ(2u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r15 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmRSC, ComputeWithCarry) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.cpsr.carry = true;
  ArmRSC(&registers, REGISTER_R1, 1u, 6u);
  EXPECT_EQ(5u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmRSCS, Compute) {
  auto registers = CreateArmAllRegisters();

  ArmRSCS(&registers, REGISTER_R1, 1u, 4u);
  EXPECT_EQ(2u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmRSCS, ComputeWithCarry) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.cpsr.carry = true;
  ArmRSCS(&registers, REGISTER_R1, 1u, 5u);
  EXPECT_EQ(4u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmRSCS, R15ToUSR) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;
  registers.current.spsr.mode = MODE_USR;

  registers.current.user.gprs.r15 = 4u;
  ArmRSCS(&registers, REGISTER_R15, 4u, 13u);
  EXPECT_EQ(8u, ArmCurrentInstruction(&registers));
  EXPECT_EQ(MODE_USR, registers.current.user.cpsr.mode);

  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.mode = 0;
  EXPECT_TRUE(ArmPrivilegedRegistersAreZero(registers.current));
}

TEST(ArmRSCS, Zero) {
  auto registers = CreateArmAllRegisters();

  ArmRSCS(&registers, REGISTER_R0, 0u, 1u);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.zero);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.zero = false;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmRSCS, Negative) {
  auto registers = CreateArmAllRegisters();

  ArmRSCS(&registers, REGISTER_R0, 0u, UINT32_MAX);
  EXPECT_EQ(UINT32_MAX - 1u, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.negative);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.negative = false;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmRSCS, NegativeWithCarry) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.cpsr.carry = true;
  ArmRSCS(&registers, REGISTER_R0, 0u, UINT32_MAX);
  EXPECT_EQ(UINT32_MAX, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.negative);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.negative = false;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmRSCS, Carry) {
  auto registers = CreateArmAllRegisters();

  ArmRSCS(&registers, REGISTER_R0, 0u, UINT32_MAX);
  EXPECT_EQ(UINT32_MAX - 1u, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.negative);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.negative = false;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmRSCS, CarryWithCarry) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.cpsr.carry = true;
  ArmRSCS(&registers, REGISTER_R0, 0u, UINT32_MAX);
  EXPECT_EQ(UINT32_MAX, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.negative);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.negative = false;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmRSCS, Overflow) {
  auto registers = CreateArmAllRegisters();

  ArmRSCS(&registers, REGISTER_R0, -2, (uint32_t)INT32_MAX);
  EXPECT_EQ(INT32_MIN, registers.current.user.gprs.r0_s);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.overflow);
  EXPECT_TRUE(registers.current.user.cpsr.negative);

  registers.current.user.gprs.r0_s = 0;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.overflow = false;
  registers.current.user.cpsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmRSCS, Underflow) {
  auto registers = CreateArmAllRegisters();

  ArmRSCS(&registers, REGISTER_R0, 0u, INT32_MIN);
  EXPECT_EQ(INT32_MAX, registers.current.user.gprs.r0_s);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.overflow);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0_s = 0;
  registers.current.user.gprs.r1_s = 0;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.overflow = false;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmRSCS, UnderflowWithCarry) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.cpsr.carry = true;
  ArmRSCS(&registers, REGISTER_R0, 1u, INT32_MIN);
  EXPECT_EQ(INT32_MAX, registers.current.user.gprs.r0_s);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.overflow);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0_s = 0;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.overflow = false;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmSBC, Compute) {
  auto registers = CreateArmAllRegisters();

  ArmSBC(&registers, REGISTER_R1, 4u, 1u);
  EXPECT_EQ(2u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r15 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmSBC, ComputeWithCarry) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.cpsr.carry = true;
  ArmSBC(&registers, REGISTER_R1, 6u, 1u);
  EXPECT_EQ(5u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmSBCS, Compute) {
  auto registers = CreateArmAllRegisters();

  ArmSBCS(&registers, REGISTER_R1, 4u, 1u);
  EXPECT_EQ(2u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmSBCS, ComputeWithCarry) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.cpsr.carry = true;
  ArmSBCS(&registers, REGISTER_R1, 5u, 1u);
  EXPECT_EQ(4u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmSBCS, R15ToUSR) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;
  registers.current.spsr.mode = MODE_USR;

  registers.current.user.gprs.r15 = 13u;
  ArmSBCS(&registers, REGISTER_R15, 13u, 4u);
  EXPECT_EQ(8u, ArmCurrentInstruction(&registers));
  EXPECT_EQ(MODE_USR, registers.current.user.cpsr.mode);

  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.mode = 0;
  EXPECT_TRUE(ArmPrivilegedRegistersAreZero(registers.current));
}

TEST(ArmSBCS, Zero) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.cpsr.carry = true;
  ArmSBCS(&registers, REGISTER_R0, 0u, 0u);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.zero);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.zero = false;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmSBCS, Negative) {
  auto registers = CreateArmAllRegisters();

  ArmSBCS(&registers, REGISTER_R0, UINT32_MAX, 0u);
  EXPECT_EQ(UINT32_MAX - 1u, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.negative);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.negative = false;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmSBCS, NegativeWithCarry) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.cpsr.carry = true;
  ArmSBCS(&registers, REGISTER_R0, UINT32_MAX, 0u);
  EXPECT_EQ(UINT32_MAX, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.negative);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.negative = false;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmSBCS, Carry) {
  auto registers = CreateArmAllRegisters();

  ArmSBCS(&registers, REGISTER_R0, UINT32_MAX, 0u);
  EXPECT_EQ(UINT32_MAX - 1u, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.negative);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.negative = false;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmSBCS, CarryWithCarry) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.cpsr.carry = true;
  ArmSBCS(&registers, REGISTER_R0, UINT32_MAX, 0u);
  EXPECT_EQ(UINT32_MAX, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.negative);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.negative = false;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmSBCS, Overflow) {
  auto registers = CreateArmAllRegisters();

  ArmSBCS(&registers, REGISTER_R0, INT32_MAX, (uint32_t)-2);
  EXPECT_EQ(INT32_MIN, registers.current.user.gprs.r0_s);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.overflow);
  EXPECT_TRUE(registers.current.user.cpsr.negative);

  registers.current.user.gprs.r0_s = 0;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.overflow = false;
  registers.current.user.cpsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmSBCS, Underflow) {
  auto registers = CreateArmAllRegisters();

  ArmSBCS(&registers, REGISTER_R0, INT32_MIN, registers.current.user.gprs.r1);
  EXPECT_EQ(INT32_MAX, registers.current.user.gprs.r0_s);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.overflow);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0_s = 0;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.overflow = false;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmSBCS, UnderflowWithCarry) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.cpsr.carry = true;
  ArmSBCS(&registers, REGISTER_R0, INT32_MIN, 1u);
  EXPECT_EQ(INT32_MAX, registers.current.user.gprs.r0_s);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.overflow);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0_s = 0;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.overflow = false;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmSUB, Compute) {
  auto registers = CreateArmAllRegisters();

  ArmSUB(&registers, REGISTER_R1, 3u, 1u);
  EXPECT_EQ(2u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r15 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmSUBS, Compute) {
  auto registers = CreateArmAllRegisters();

  ArmSUBS(&registers, REGISTER_R1, 3u, 1u);
  EXPECT_EQ(2u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmSUBS, R15ToUSR) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;
  registers.current.spsr.mode = MODE_USR;

  ArmSUBS(&registers, REGISTER_R15, 12u, 4u);
  EXPECT_EQ(8u, ArmCurrentInstruction(&registers));
  EXPECT_EQ(MODE_USR, registers.current.user.cpsr.mode);

  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.mode = 0;
  EXPECT_TRUE(ArmPrivilegedRegistersAreZero(registers.current));
}

TEST(ArmSUBS, Zero) {
  auto registers = CreateArmAllRegisters();

  ArmSUBS(&registers, REGISTER_R0, 0u, 0u);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.zero);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.zero = false;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmSUBS, Negative) {
  auto registers = CreateArmAllRegisters();

  ArmSUBS(&registers, REGISTER_R0, UINT32_MAX, 0u);
  EXPECT_EQ(UINT32_MAX, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.negative);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.negative = false;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmSUBS, NotCarry) {
  auto registers = CreateArmAllRegisters();

  ArmSUBS(&registers, REGISTER_R0, 0u, 1u);
  EXPECT_EQ(UINT32_MAX, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.negative);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmSUBS, Overflow) {
  auto registers = CreateArmAllRegisters();

  ArmSUBS(&registers, REGISTER_R0, INT32_MAX, (uint32_t)-1);
  EXPECT_EQ(INT32_MIN, registers.current.user.gprs.r0_s);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.overflow);
  EXPECT_TRUE(registers.current.user.cpsr.negative);

  registers.current.user.gprs.r0_s = 0;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.overflow = false;
  registers.current.user.cpsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmSUBS, Underflow) {
  auto registers = CreateArmAllRegisters();

  ArmSUBS(&registers, REGISTER_R0, INT32_MIN, 1u);
  EXPECT_EQ(INT32_MAX, registers.current.user.gprs.r0_s);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.overflow);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r0_s = 0;
  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.overflow = false;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmTEQ, Compute) {
  auto registers = CreateArmAllRegisters();

  ArmTEQ(&registers, REGISTER_R0, 5u, 3u, false);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  registers.current.user.gprs.r15 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmTEQ, Zero) {
  auto registers = CreateArmAllRegisters();

  ArmTEQ(&registers, REGISTER_R0, 1u, 1u, false);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.zero);

  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.zero = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmTEQ, Negative) {
  auto registers = CreateArmAllRegisters();

  ArmTEQ(&registers, REGISTER_R0, UINT32_MAX, 0u, false);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.negative);

  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmTEQ, Carry) {
  auto registers = CreateArmAllRegisters();

  ArmTEQ(&registers, REGISTER_R0, UINT32_MAX, UINT32_MAX - 1, true);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmTEQ, R15ToUSR) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;
  registers.current.spsr.mode = MODE_USR;

  ArmTEQ(&registers, REGISTER_R15, 0u, 0u, true);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_EQ(MODE_USR, registers.current.user.cpsr.mode);

  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.mode = 0;
  EXPECT_TRUE(ArmPrivilegedRegistersAreZero(registers.current));
}

TEST(ArmTST, Compute) {
  auto registers = CreateArmAllRegisters();

  ArmTST(&registers, REGISTER_R0, 15u, 23u, false);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  registers.current.user.gprs.r15 = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmTST, Zero) {
  auto registers = CreateArmAllRegisters();

  ArmTST(&registers, REGISTER_R0, 1u, 2u, false);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.zero);

  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.zero = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmTST, Negative) {
  auto registers = CreateArmAllRegisters();

  ArmTST(&registers, REGISTER_R0, UINT32_MAX, UINT32_MAX, false);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.negative);

  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmTST, Carry) {
  auto registers = CreateArmAllRegisters();

  ArmTST(&registers, REGISTER_R0, UINT32_MAX, 1u, true);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_TRUE(registers.current.user.cpsr.carry);

  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.carry = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmTST, R15ToUSR) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;
  registers.current.spsr.mode = MODE_USR;

  ArmTST(&registers, REGISTER_R15, 0u, 0u, true);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_EQ(MODE_USR, registers.current.user.cpsr.mode);

  registers.current.user.gprs.r15 = 0u;
  registers.current.user.cpsr.mode = 0;
  EXPECT_TRUE(ArmPrivilegedRegistersAreZero(registers.current));
}