extern "C" {
#include "emulator/cpu/arm7tdmi/decoders/arm/operand.h"
}

#include <cstring>

#include "googletest/include/gtest/gtest.h"

ArmUserRegisters CreateArmUserRegisters() {
  ArmUserRegisters registers;
  memset(&registers, 0, sizeof(ArmUserRegisters));
  return registers;
}

TEST(ArmOperandMultiply, Compute) {
  uint32_t instruction = 0xE0000291u;  // mul r0, r1, r2
  ArmRegisterIndex rd, rm, rs;
  ArmOperandMultiply(instruction, &rd, &rm, &rs);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rm);
  EXPECT_EQ(REGISTER_R2, rs);
}

TEST(ArmOperandMultiplyAccumulate, Compute) {
  uint32_t instruction = 0xE0203291u;  // mla r0, r1, r2, r3
  ArmRegisterIndex rd, rm, rs, rn;
  ArmOperandMultiplyAccumulate(instruction, &rd, &rm, &rs, &rn);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rm);
  EXPECT_EQ(REGISTER_R2, rs);
  EXPECT_EQ(REGISTER_R3, rn);
}

TEST(ArmOperandMultiplyLong, Compute) {
  uint32_t instruction = 0xE0810392u;  // umull r0, r1, r2, r3
  ArmRegisterIndex rd_lsw, rd_msw, rm, rs;
  ArmOperandMultiplyLong(instruction, &rd_lsw, &rd_msw, &rm, &rs);
  EXPECT_EQ(REGISTER_R0, rd_lsw);
  EXPECT_EQ(REGISTER_R1, rd_msw);
  EXPECT_EQ(REGISTER_R2, rm);
  EXPECT_EQ(REGISTER_R3, rs);
}

TEST(ArmOperandBranchExchange, Compute) {
  uint32_t instruction = 0xE12FFF1Eu;  // bx lr
  ArmRegisterIndex rn;
  ArmOperandBranchExchange(instruction, &rn);
  EXPECT_EQ(REGISTER_R14, rn);
}

TEST(ArmOperandSingleDataSwap, Compute) {
  uint32_t instruction = 0xE1020091u;  // swp r0, r1, [r2]
  ArmRegisterIndex rd, rm, rn;
  ArmOperandSingleDataSwap(instruction, &rd, &rm, &rn);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rm);
  EXPECT_EQ(REGISTER_R2, rn);
}

TEST(ArmOperandHalfwordImmediate, Compute) {
  uint32_t instruction = 0xE1D100B8u;  // ldrh r0, [r1, #8]
  ArmRegisterIndex rd, rn;
  uint_fast8_t offset;
  ArmOperandHalfwordImmediate(instruction, &rd, &rn, &offset);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_EQ(8u, offset);
}

TEST(ArmOperandHalfwordAddressMode, Compute) {
  auto registers = CreateArmUserRegisters();
  registers.gprs.r2 = 100u;
  uint32_t instruction = 0xE19100B2u;  // ldrh r0, [r1, r2]
  ArmRegisterIndex rd, rn;
  uint32_t offset;
  ArmOperandHalfwordAddressMode(instruction, &registers.gprs, &rd, &rn,
                                &offset);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_EQ(100u, offset);
}

TEST(ArmOperandDataProcessingImmediate, NoShiftNoCarry) {
  auto registers = CreateArmUserRegisters();
  registers.cpsr.carry = false;
  uint32_t instruction = 0xE28100FFu;  // add r0, r1, #255
  ArmRegisterIndex rd, rn;
  uint32_t value;
  bool carry_out;
  ArmOperandDataProcessingImmediate(instruction, &registers, &rd, &rn,
                                    &carry_out, &value);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_FALSE(carry_out);
  EXPECT_EQ(255u, value);
}

TEST(ArmOperandDataProcessingImmediate, NoShiftWithCarry) {
  auto registers = CreateArmUserRegisters();
  registers.cpsr.carry = true;
  uint32_t instruction = 0xE28100FFu;  // add r0, r1, #255
  ArmRegisterIndex rd, rn;
  uint32_t value;
  bool carry_out;
  ArmOperandDataProcessingImmediate(instruction, &registers, &rd, &rn,
                                    &carry_out, &value);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_TRUE(carry_out);
  EXPECT_EQ(255u, value);
}

TEST(ArmOperandDataProcessingImmediate, ShiftNoCarry) {
  auto registers = CreateArmUserRegisters();
  uint32_t instruction = 0xE2810A01u;  // add r0, r1, #4096
  ArmRegisterIndex rd, rn;
  uint32_t value;
  bool carry_out;
  ArmOperandDataProcessingImmediate(instruction, &registers, &rd, &rn,
                                    &carry_out, &value);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_FALSE(carry_out);
  EXPECT_EQ(4096u, value);
}

TEST(ArmOperandDataProcessingImmediate, ShiftWithCarry) {
  auto registers = CreateArmUserRegisters();
  uint32_t instruction = 0xE28104FFu;  // add r0, r1, #0xFF000000
  ArmRegisterIndex rd, rn;
  uint32_t value;
  bool carry_out;
  ArmOperandDataProcessingImmediate(instruction, &registers, &rd, &rn,
                                    &carry_out, &value);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_TRUE(carry_out);
  EXPECT_EQ(0xFF000000u, value);
}

TEST(ArmOperandDataProcessingOperand2, RegisterValueNoCarry) {
  uint32_t instruction = 0xE0810002u;  // add r0, r1, r2

  auto registers = CreateArmUserRegisters();
  registers.gprs.r2 = 64u;
  registers.cpsr.carry = false;

  ArmRegisterIndex rd, rn;
  uint32_t value;
  bool carry_out;
  ArmOperandDataProcessingOperand2(instruction, &registers, &rd, &rn,
                                   &carry_out, &value);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_FALSE(carry_out);
  EXPECT_EQ(64u, value);
}

TEST(ArmOperandDataProcessingOperand2, RegisterValueWithCarry) {
  uint32_t instruction = 0xE0810002u;  // add r0, r1, r2

  auto registers = CreateArmUserRegisters();
  registers.gprs.r2 = 64u;
  registers.cpsr.carry = true;

  ArmRegisterIndex rd, rn;
  uint32_t value;
  bool carry_out;
  ArmOperandDataProcessingOperand2(instruction, &registers, &rd, &rn,
                                   &carry_out, &value);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_TRUE(carry_out);
  EXPECT_EQ(64u, value);
}

TEST(ArmOperandDataProcessingOperand2, ImmediateShiftedOffsetLSLNoCarryOut) {
  uint32_t instruction = 0xE0810082u;  // add r0, r1, r2, lsl #1

  auto registers = CreateArmUserRegisters();
  registers.gprs.r2 = 64u;

  ArmRegisterIndex rd, rn;
  uint32_t value;
  bool carry_out;
  ArmOperandDataProcessingOperand2(instruction, &registers, &rd, &rn,
                                   &carry_out, &value);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_FALSE(carry_out);
  EXPECT_EQ(128u, value);
}

TEST(ArmOperandDataProcessingOperand2, ImmediateShiftedOffsetLSLWithCarryOut) {
  uint32_t instruction = 0xE0810082u;  // add r0, r1, r2, lsl #1

  auto registers = CreateArmUserRegisters();
  registers.gprs.r2 = 0xF0000000u;

  ArmRegisterIndex rd, rn;
  uint32_t value;
  bool carry_out;
  ArmOperandDataProcessingOperand2(instruction, &registers, &rd, &rn,
                                   &carry_out, &value);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_TRUE(carry_out);
  EXPECT_EQ(0xE0000000u, value);
}

TEST(ArmOperandDataProcessingOperand2, ImmediateShiftedOffsetLSRNoCarryOut) {
  uint32_t instruction = 0xE08100A2u;  // add r0, r1, r2, lsr #1

  auto registers = CreateArmUserRegisters();
  registers.gprs.r2 = 64u;

  ArmRegisterIndex rd, rn;
  uint32_t value;
  bool carry_out;
  ArmOperandDataProcessingOperand2(instruction, &registers, &rd, &rn,
                                   &carry_out, &value);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_FALSE(carry_out);
  EXPECT_EQ(32u, value);
}

TEST(ArmOperandDataProcessingOperand2, ImmediateShiftedOffsetLSRWithCarryOut) {
  uint32_t instruction = 0xE08100A2u;  // add r0, r1, r2, lsr #1

  auto registers = CreateArmUserRegisters();
  registers.gprs.r2 = 0xFu;

  ArmRegisterIndex rd, rn;
  uint32_t value;
  bool carry_out;
  ArmOperandDataProcessingOperand2(instruction, &registers, &rd, &rn,
                                   &carry_out, &value);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_TRUE(carry_out);
  EXPECT_EQ(0x7u, value);
}

TEST(ArmOperandDataProcessingOperand2,
     ImmediateShiftedOffsetLSRZeroNoCarryOut) {
  uint32_t instruction = 0xE0810022u;  // add r0, r1, r2, lsr #32

  auto registers = CreateArmUserRegisters();
  registers.gprs.r2 = 64u;

  ArmRegisterIndex rd, rn;
  uint32_t value;
  bool carry_out;
  ArmOperandDataProcessingOperand2(instruction, &registers, &rd, &rn,
                                   &carry_out, &value);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_FALSE(carry_out);
  EXPECT_EQ(0u, value);
}

TEST(ArmOperandDataProcessingOperand2,
     ImmediateShiftedOffsetLSRZeroWithCarryOut) {
  uint32_t instruction = 0xE0810022u;  // add r0, r1, r2, lsr #32

  auto registers = CreateArmUserRegisters();
  registers.gprs.r2 = 0xF0000000u;

  ArmRegisterIndex rd, rn;
  uint32_t value;
  bool carry_out;
  ArmOperandDataProcessingOperand2(instruction, &registers, &rd, &rn,
                                   &carry_out, &value);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_TRUE(carry_out);
  EXPECT_EQ(0u, value);
}

TEST(ArmOperandDataProcessingOperand2, ImmediateShiftedOffsetASRNoCarry) {
  uint32_t instruction = 0xE08100C2u;  // add r0, r1, r2, asr #1

  auto registers = CreateArmUserRegisters();
  registers.gprs.r2_s = -64;

  ArmRegisterIndex rd, rn;
  uint32_t value;
  bool carry_out;
  ArmOperandDataProcessingOperand2(instruction, &registers, &rd, &rn,
                                   &carry_out, &value);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_FALSE(carry_out);
  EXPECT_EQ(-32, (int32_t)value);
}

TEST(ArmOperandDataProcessingOperand2, ImmediateShiftedOffsetASRWithCarry) {
  uint32_t instruction = 0xE08100C2u;  // add r0, r1, r2, asr #1

  auto registers = CreateArmUserRegisters();
  registers.gprs.r2_s = -3;

  ArmRegisterIndex rd, rn;
  uint32_t value;
  bool carry_out;
  ArmOperandDataProcessingOperand2(instruction, &registers, &rd, &rn,
                                   &carry_out, &value);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_TRUE(carry_out);
  EXPECT_EQ(-2, (int32_t)value);
}

TEST(ArmOperandDataProcessingOperand2, ImmediateShiftedOffsetASRPositive32) {
  uint32_t instruction = 0xE0810042u;  // add r0, r1, r2, asr #32

  auto registers = CreateArmUserRegisters();
  registers.gprs.r2_s = 3;

  ArmRegisterIndex rd, rn;
  uint32_t value;
  bool carry_out;
  ArmOperandDataProcessingOperand2(instruction, &registers, &rd, &rn,
                                   &carry_out, &value);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_FALSE(carry_out);
  EXPECT_EQ(0, (int32_t)value);
}

TEST(ArmOperandDataProcessingOperand2, ImmediateShiftedOffsetASRNegative32) {
  uint32_t instruction = 0xE0810042u;  // add r0, r1, r2, asr #32

  auto registers = CreateArmUserRegisters();
  registers.gprs.r2_s = -3;

  ArmRegisterIndex rd, rn;
  uint32_t value;
  bool carry_out;
  ArmOperandDataProcessingOperand2(instruction, &registers, &rd, &rn,
                                   &carry_out, &value);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_TRUE(carry_out);
  EXPECT_EQ(-1, (int32_t)value);
}

TEST(ArmOperandDataProcessingOperand2, ImmediateShiftedOffsetRORNoCarry) {
  uint32_t instruction = 0xE0810262u;  // add r0, r1, r2, ror #4

  auto registers = CreateArmUserRegisters();
  registers.gprs.r2 = 0xC0000003u;

  ArmRegisterIndex rd, rn;
  uint32_t value;
  bool carry_out;
  ArmOperandDataProcessingOperand2(instruction, &registers, &rd, &rn,
                                   &carry_out, &value);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_FALSE(carry_out);
  EXPECT_EQ(0x3C000000u, value);
}

TEST(ArmOperandDataProcessingOperand2, ImmediateShiftedOffsetRORWithCarry) {
  uint32_t instruction = 0xE0810262u;  // add r0, r1, r2, ror #4

  auto registers = CreateArmUserRegisters();
  registers.gprs.r2 = 0x000000F8u;

  ArmRegisterIndex rd, rn;
  uint32_t value;
  bool carry_out;
  ArmOperandDataProcessingOperand2(instruction, &registers, &rd, &rn,
                                   &carry_out, &value);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_TRUE(carry_out);
  EXPECT_EQ(0x8000000Fu, value);
}

TEST(ArmOperandDataProcessingOperand2,
     ImmediateShiftedOffsetRRXNoCarryInNoCarryOut) {
  uint32_t instruction = 0xE0810062;  // add r0, r1, r2, rrx

  auto registers = CreateArmUserRegisters();
  registers.gprs.r2 = 0x00000002u;
  registers.cpsr.carry = false;

  ArmRegisterIndex rd, rn;
  uint32_t value;
  bool carry_out;
  ArmOperandDataProcessingOperand2(instruction, &registers, &rd, &rn,
                                   &carry_out, &value);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_FALSE(carry_out);
  EXPECT_EQ(0x00000001u, value);
}

TEST(ArmOperandDataProcessingOperand2,
     ImmediateShiftedOffsetRRXNoCarryInWithCarryOut) {
  uint32_t instruction = 0xE0810062;  // add r0, r1, r2, rrx

  auto registers = CreateArmUserRegisters();
  registers.gprs.r2 = 0x00000003u;
  registers.cpsr.carry = false;

  ArmRegisterIndex rd, rn;
  uint32_t value;
  bool carry_out;
  ArmOperandDataProcessingOperand2(instruction, &registers, &rd, &rn,
                                   &carry_out, &value);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_TRUE(carry_out);
  EXPECT_EQ(0x00000001u, value);
}

TEST(ArmOperandDataProcessingOperand2,
     ImmediateShiftedOffsetRRXWithCarryInNoCarryOut) {
  uint32_t instruction = 0xE0810062;  // add r0, r1, r2, rrx

  auto registers = CreateArmUserRegisters();
  registers.gprs.r2 = 0x00000002u;
  registers.cpsr.carry = true;

  ArmRegisterIndex rd, rn;
  uint32_t value;
  bool carry_out;
  ArmOperandDataProcessingOperand2(instruction, &registers, &rd, &rn,
                                   &carry_out, &value);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_FALSE(carry_out);
  EXPECT_EQ(0x80000001u, value);
}

TEST(ArmOperandDataProcessingOperand2,
     ImmediateShiftedOffsetRRXWithCarryInWithCarryOut) {
  uint32_t instruction = 0xE0810062;  // add r0, r1, r2, rrx

  auto registers = CreateArmUserRegisters();
  registers.gprs.r2 = 0x00000003u;
  registers.cpsr.carry = true;

  ArmRegisterIndex rd, rn;
  uint32_t value;
  bool carry_out;
  ArmOperandDataProcessingOperand2(instruction, &registers, &rd, &rn,
                                   &carry_out, &value);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_TRUE(carry_out);
  EXPECT_EQ(0x80000001u, value);
}

TEST(ArmOperandDataProcessingOperand2, RegisterShiftedOffsetLSL) {
  uint32_t instruction = 0xE0810312u;  // add r0, r1, r2, lsl r3

  auto registers = CreateArmUserRegisters();
  registers.gprs.r2 = 64u;
  registers.gprs.r3 = 1u;

  ArmRegisterIndex rd, rn;
  uint32_t value;
  bool carry_out;
  ArmOperandDataProcessingOperand2(instruction, &registers, &rd, &rn,
                                   &carry_out, &value);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_FALSE(carry_out);
  EXPECT_EQ(128u, value);
}

TEST(ArmOperandDataProcessingOperand2, RegisterZeroShiftedOffsetLSL) {
  uint32_t instruction = 0xE0810312u;  // add r0, r1, r2, lsl r3

  auto registers = CreateArmUserRegisters();
  registers.gprs.r2 = 64u;
  registers.gprs.r3 = 0u;
  registers.cpsr.carry = true;

  ArmRegisterIndex rd, rn;
  uint32_t value;
  bool carry_out;
  ArmOperandDataProcessingOperand2(instruction, &registers, &rd, &rn,
                                   &carry_out, &value);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_TRUE(carry_out);
  EXPECT_EQ(64u, value);
}

TEST(ArmOperandDataProcessingOperand2, Register32ShiftedOffsetLSLNoCarryOut) {
  uint32_t instruction = 0xE0810312u;  // add r0, r1, r2, lsl r3

  auto registers = CreateArmUserRegisters();
  registers.gprs.r2 = 4u;
  registers.gprs.r3 = 32u;
  registers.cpsr.carry = true;

  ArmRegisterIndex rd, rn;
  uint32_t value;
  bool carry_out;
  ArmOperandDataProcessingOperand2(instruction, &registers, &rd, &rn,
                                   &carry_out, &value);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_FALSE(carry_out);
  EXPECT_EQ(0u, value);
}

TEST(ArmOperandDataProcessingOperand2, Register32ShiftedOffsetLSLWithCarryOut) {
  uint32_t instruction = 0xE0810312u;  // add r0, r1, r2, lsl r3

  auto registers = CreateArmUserRegisters();
  registers.gprs.r2 = 1u;
  registers.gprs.r3 = 32u;
  registers.cpsr.carry = true;

  ArmRegisterIndex rd, rn;
  uint32_t value;
  bool carry_out;
  ArmOperandDataProcessingOperand2(instruction, &registers, &rd, &rn,
                                   &carry_out, &value);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_TRUE(carry_out);
  EXPECT_EQ(0u, value);
}

TEST(ArmOperandDataProcessingOperand2, RegisterLargeShiftedOffsetLSL) {
  uint32_t instruction = 0xE0810312u;  // add r0, r1, r2, lsl r3

  auto registers = CreateArmUserRegisters();
  registers.gprs.r2 = 64u;
  registers.gprs.r3 = 0xFFu;
  registers.cpsr.carry = true;

  ArmRegisterIndex rd, rn;
  uint32_t value;
  bool carry_out;
  ArmOperandDataProcessingOperand2(instruction, &registers, &rd, &rn,
                                   &carry_out, &value);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_FALSE(carry_out);
  EXPECT_EQ(0u, value);
}

TEST(ArmOperandDataProcessingOperand2, RegisterZeroShiftedOffsetLSR) {
  uint32_t instruction = 0xE0810332u;  // add r0, r1, r2, lsr r3

  auto registers = CreateArmUserRegisters();
  registers.gprs.r2 = 64u;
  registers.gprs.r3 = 0u;
  registers.cpsr.carry = true;

  ArmRegisterIndex rd, rn;
  uint32_t value;
  bool carry_out;
  ArmOperandDataProcessingOperand2(instruction, &registers, &rd, &rn,
                                   &carry_out, &value);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_TRUE(carry_out);
  EXPECT_EQ(64u, value);
}

TEST(ArmOperandDataProcessingOperand2, Register32ShiftedOffsetLSRNoCarryOut) {
  uint32_t instruction = 0xE0810332u;  // add r0, r1, r2, lsr r3

  auto registers = CreateArmUserRegisters();
  registers.gprs.r2 = 4u;
  registers.gprs.r3 = 32u;
  registers.cpsr.carry = true;

  ArmRegisterIndex rd, rn;
  uint32_t value;
  bool carry_out;
  ArmOperandDataProcessingOperand2(instruction, &registers, &rd, &rn,
                                   &carry_out, &value);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_FALSE(carry_out);
  EXPECT_EQ(0u, value);
}

TEST(ArmOperandDataProcessingOperand2, Register32ShiftedOffsetLSRWithCarryOut) {
  uint32_t instruction = 0xE0810332u;  // add r0, r1, r2, lsr r3

  auto registers = CreateArmUserRegisters();
  registers.gprs.r2 = 0x80000000u;
  registers.gprs.r3 = 32u;
  registers.cpsr.carry = true;

  ArmRegisterIndex rd, rn;
  uint32_t value;
  bool carry_out;
  ArmOperandDataProcessingOperand2(instruction, &registers, &rd, &rn,
                                   &carry_out, &value);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_TRUE(carry_out);
  EXPECT_EQ(0u, value);
}

TEST(ArmOperandDataProcessingOperand2, RegisterLargeShiftedOffsetLSR) {
  uint32_t instruction = 0xE0810332u;  // add r0, r1, r2, lsr r3

  auto registers = CreateArmUserRegisters();
  registers.gprs.r2 = 64u;
  registers.gprs.r3 = 0xFFu;
  registers.cpsr.carry = true;

  ArmRegisterIndex rd, rn;
  uint32_t value;
  bool carry_out;
  ArmOperandDataProcessingOperand2(instruction, &registers, &rd, &rn,
                                   &carry_out, &value);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_FALSE(carry_out);
  EXPECT_EQ(0u, value);
}

TEST(ArmOperandDataProcessingOperand2, RegisterZeroShiftedOffsetASR) {
  uint32_t instruction = 0xE0810352u;  // add r0, r1, r2, asr r3

  auto registers = CreateArmUserRegisters();
  registers.gprs.r2 = 64u;
  registers.gprs.r3 = 0u;
  registers.cpsr.carry = true;

  ArmRegisterIndex rd, rn;
  uint32_t value;
  bool carry_out;
  ArmOperandDataProcessingOperand2(instruction, &registers, &rd, &rn,
                                   &carry_out, &value);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_TRUE(carry_out);
  EXPECT_EQ(64u, value);
}

TEST(ArmOperandDataProcessingOperand2, Register32ShiftedOffsetASRPositive) {
  uint32_t instruction = 0xE0810352u;  // add r0, r1, r2, asr r3

  auto registers = CreateArmUserRegisters();
  registers.gprs.r2 = 4u;
  registers.gprs.r3 = 32u;
  registers.cpsr.carry = true;

  ArmRegisterIndex rd, rn;
  uint32_t value;
  bool carry_out;
  ArmOperandDataProcessingOperand2(instruction, &registers, &rd, &rn,
                                   &carry_out, &value);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_FALSE(carry_out);
  EXPECT_EQ(0u, value);
}

TEST(ArmOperandDataProcessingOperand2, Register32ShiftedOffsetASRNegative) {
  uint32_t instruction = 0xE0810352u;  // add r0, r1, r2, asr r3

  auto registers = CreateArmUserRegisters();
  registers.gprs.r2_s = -4;
  registers.gprs.r3 = 32u;
  registers.cpsr.carry = true;

  ArmRegisterIndex rd, rn;
  uint32_t value;
  bool carry_out;
  ArmOperandDataProcessingOperand2(instruction, &registers, &rd, &rn,
                                   &carry_out, &value);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_TRUE(carry_out);
  EXPECT_EQ(-1, (int32_t)value);
}

TEST(ArmOperandDataProcessingOperand2, RegisterZeroShiftedOffsetROR) {
  uint32_t instruction = 0xE0810372u;  // add r0, r1, r2, ror r3

  auto registers = CreateArmUserRegisters();
  registers.gprs.r2 = 64u;
  registers.gprs.r3 = 0u;
  registers.cpsr.carry = true;

  ArmRegisterIndex rd, rn;
  uint32_t value;
  bool carry_out;
  ArmOperandDataProcessingOperand2(instruction, &registers, &rd, &rn,
                                   &carry_out, &value);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_TRUE(carry_out);
  EXPECT_EQ(64u, value);
}

TEST(ArmOperandDataProcessingOperand2, Register32ShiftedOffsetRORNoCarryOut) {
  uint32_t instruction = 0xE0810372u;  // add r0, r1, r2, ror r3

  auto registers = CreateArmUserRegisters();
  registers.gprs.r2 = 0x70000000u;
  registers.gprs.r3 = 32u;
  registers.cpsr.carry = true;

  ArmRegisterIndex rd, rn;
  uint32_t value;
  bool carry_out;
  ArmOperandDataProcessingOperand2(instruction, &registers, &rd, &rn,
                                   &carry_out, &value);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_FALSE(carry_out);
  EXPECT_EQ(0x70000000u, value);
}

TEST(ArmOperandDataProcessingOperand2, Register32ShiftedOffsetRORWithCarryOut) {
  uint32_t instruction = 0xE0810372u;  // add r0, r1, r2, ror r3

  auto registers = CreateArmUserRegisters();
  registers.gprs.r2 = 0x80000000u;
  registers.gprs.r3 = 32u;
  registers.cpsr.carry = true;

  ArmRegisterIndex rd, rn;
  uint32_t value;
  bool carry_out;
  ArmOperandDataProcessingOperand2(instruction, &registers, &rd, &rn,
                                   &carry_out, &value);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_TRUE(carry_out);
  EXPECT_EQ(0x80000000u, value);
}

TEST(ArmOperandLoadStoreImmediate, Compute) {
  uint32_t instruction = 0xE5910FFFu;  // ldr r0, [r1, 4095]
  ArmRegisterIndex rd, rn;
  uint_fast16_t offset;
  ArmOperandLoadStoreImmediate(instruction, &rd, &rn, &offset);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_EQ(4095u, offset);
}

TEST(ArmOperandLoadStoreAddressMode, RegisterOffset) {
  uint32_t instruction = 0xE7910002u;  // ldr r0, [r1, r2]

  auto registers = CreateArmUserRegisters();
  registers.gprs.r2 = 100u;

  ArmRegisterIndex rd, rn;
  uint32_t offset;
  ArmOperandLoadStoreAddressMode(instruction, &registers, &rd, &rn, &offset);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_EQ(100u, offset);
}

TEST(ArmOperandLoadStoreAddressMode, ImmediateShiftedOffsetLSL) {
  uint32_t instruction = 0xE7910082u;  // ldr r0, [r1, r2, lsl #1]

  auto registers = CreateArmUserRegisters();
  registers.gprs.r2 = 64u;

  ArmRegisterIndex rd, rn;
  uint32_t offset;
  ArmOperandLoadStoreAddressMode(instruction, &registers, &rd, &rn, &offset);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_EQ(128u, offset);
}

TEST(ArmOperandLoadStoreAddressMode, ImmediateShiftedOffsetLSR) {
  uint32_t instruction = 0xE79100A2u;  // ldr r0, [r1, r2, lsr #1]

  auto registers = CreateArmUserRegisters();
  registers.gprs.r2 = 64u;

  ArmRegisterIndex rd, rn;
  uint32_t offset;
  ArmOperandLoadStoreAddressMode(instruction, &registers, &rd, &rn, &offset);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_EQ(32u, offset);
}

TEST(ArmOperandLoadStoreAddressMode, ImmediateShiftedOffsetLSR32) {
  uint32_t instruction = 0xE7910022u;  // ldr r0, [r1, r2, lsr #32]

  auto registers = CreateArmUserRegisters();
  registers.gprs.r2 = 64u;

  ArmRegisterIndex rd, rn;
  uint32_t offset;
  ArmOperandLoadStoreAddressMode(instruction, &registers, &rd, &rn, &offset);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_EQ(0u, offset);
}

TEST(ArmOperandLoadStoreAddressMode, ImmediateShiftedOffsetASR) {
  uint32_t instruction = 0xE79100C2u;  // ldr r0, [r1, r2, asr #1]

  auto registers = CreateArmUserRegisters();
  registers.gprs.r2_s = -64u;

  ArmRegisterIndex rd, rn;
  uint32_t offset;
  ArmOperandLoadStoreAddressMode(instruction, &registers, &rd, &rn, &offset);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_EQ(-32, (int32_t)offset);
}

TEST(ArmOperandLoadStoreAddressMode, ImmediateShiftedOffset32ASRPositive) {
  uint32_t instruction = 0xE7910042u;  // ldr r0, [r1, r2, asr #32]

  auto registers = CreateArmUserRegisters();
  registers.gprs.r2_s = 64u;

  ArmRegisterIndex rd, rn;
  uint32_t offset;
  ArmOperandLoadStoreAddressMode(instruction, &registers, &rd, &rn, &offset);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_EQ(0u, offset);
}

TEST(ArmOperandLoadStoreAddressMode, ImmediateShiftedOffset32ASRNegative) {
  uint32_t instruction = 0xE7910042u;  // ldr r0, [r1, r2, asr #32]

  auto registers = CreateArmUserRegisters();
  registers.gprs.r2_s = -64u;

  ArmRegisterIndex rd, rn;
  uint32_t offset;
  ArmOperandLoadStoreAddressMode(instruction, &registers, &rd, &rn, &offset);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_EQ(-1, (int32_t)offset);
}

TEST(ArmOperandLoadStoreAddressMode, ImmediateShiftedOffsetROR) {
  uint32_t instruction = 0xE7910262;  // ldr r0, [r1, r2, ror #4]

  auto registers = CreateArmUserRegisters();
  registers.gprs.r2_s = 0x000000FFu;

  ArmRegisterIndex rd, rn;
  uint32_t offset;
  ArmOperandLoadStoreAddressMode(instruction, &registers, &rd, &rn, &offset);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_EQ(0xF000000Fu, offset);
}

TEST(ArmOperandLoadStoreAddressMode, ImmediateShiftedOffsetRRXNoCarry) {
  uint32_t instruction = 0xE7910062;  // ldr r0, [r1, r2, rrx]

  auto registers = CreateArmUserRegisters();
  registers.gprs.r2 = 0x00000002u;
  registers.cpsr.carry = false;

  ArmRegisterIndex rd, rn;
  uint32_t offset;
  ArmOperandLoadStoreAddressMode(instruction, &registers, &rd, &rn, &offset);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_EQ(0x00000001u, offset);
}

TEST(ArmOperandLoadStoreAddressMode, ImmediateShiftedOffsetRRXWithCarry) {
  uint32_t instruction = 0xE7910062;  // ldr r0, [r1, r2, rrx]

  auto registers = CreateArmUserRegisters();
  registers.gprs.r2 = 0x00000002u;
  registers.cpsr.carry = true;

  ArmRegisterIndex rd, rn;
  uint32_t offset;
  ArmOperandLoadStoreAddressMode(instruction, &registers, &rd, &rn, &offset);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_EQ(0x80000001u, offset);
}

TEST(ArmOperandRegisterAndRegisterList, Compute) {
  uint32_t instruction = 0xE8AD400Du;  // stmea r13!, {r0, r2-r3, lr}
  ArmRegisterIndex rn;
  uint_fast16_t register_list;
  ArmOperandRegisterAndRegisterList(instruction, &rn, &register_list);
  EXPECT_EQ(REGISTER_R13, rn);
  EXPECT_EQ(0x400Du, register_list);
}

TEST(ArmOperandBranchReverse, Compute) {
  uint32_t instruction = 0xEAF63269u;  // b #-2569812
  uint_fast32_t offset;
  ArmOperandBranchReverse(instruction, &offset);
  EXPECT_EQ(0xFFD8C9ACu, offset + 8u);
}

TEST(ArmOperandBranchForward, Compute) {
  uint32_t instruction = 0xEA09CD93;  // b #2569812
  uint_fast32_t offset;
  ArmOperandBranchForward(instruction, &offset);
  EXPECT_EQ(2569812u, offset + 8u);
}