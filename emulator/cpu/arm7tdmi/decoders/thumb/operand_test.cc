extern "C" {
#include "emulator/cpu/arm7tdmi/decoders/thumb/operand.h"
}

#include <cstring>

#include "googletest/include/gtest/gtest.h"

ArmUserRegisters CreateArmUserRegisters() {
  ArmUserRegisters registers;
  memset(&registers, 0, sizeof(ArmUserRegisters));
  return registers;
}

TEST(ThumbShiftByImmediate, Compute) {
  uint16_t instruction = 0x0148u;  // lsls r0, r1, #5
  ArmRegisterIndex rd, rm;
  uint_fast8_t immediate;
  ThumbOperandShiftByImmediate(instruction, &rd, &rm, &immediate);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rm);
  EXPECT_EQ(5u, immediate);
}

TEST(ThumbAddSubtractRegister, Compute) {
  uint16_t instruction = 0x1888u;  // adds r0, r1, r2
  ArmRegisterIndex rd, rn, rm;
  ThumbOperandAddSubtractRegister(instruction, &rd, &rn, &rm);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_EQ(REGISTER_R2, rm);
}

TEST(ThumbAddSubtractImmediate, Compute) {
  uint16_t instruction = 0x1C88;  // adds r0, r1, #2
  ArmRegisterIndex rd, rn;
  uint_fast8_t immediate;
  ThumbOperandAddSubtractImmediate(instruction, &rd, &rn, &immediate);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_EQ(2u, immediate);
}

TEST(ThumbAddSubtractCompareMoveImmediate, Compute) {
  uint16_t instruction = 0x2802u;  // cmp r0, #2
  ArmRegisterIndex rd_rn;
  uint_fast8_t immediate;
  ThumbOperandAddSubtractCompareMoveImmediate(instruction, &rd_rn, &immediate);
  EXPECT_EQ(REGISTER_R0, rd_rn);
  EXPECT_EQ(2u, immediate);
}

TEST(ThumbDataProcessingRegister, Compute) {
  uint16_t instruction = 0x4348;  // muls r0, r0, r1
  ArmRegisterIndex rd_rn, rm_rs;
  ThumbOperandDataProcessingRegister(instruction, &rd_rn, &rm_rs);
  EXPECT_EQ(REGISTER_R0, rd_rn);
  EXPECT_EQ(REGISTER_R1, rm_rs);
}

TEST(ThumbSpecialDataProcessing, Compute) {
  uint16_t instruction = 0x44C8;  // add r8, r8, r9
  ArmRegisterIndex rd_rn, rm;
  ThumbOperandSpecialDataProcessing(instruction, &rd_rn, &rm);
  EXPECT_EQ(REGISTER_R8, rd_rn);
  EXPECT_EQ(REGISTER_R9, rm);
}

TEST(ThumbBranchExchange, Compute) {
  uint16_t instruction = 0x4738u;  // bx r7
  ArmRegisterIndex rm;
  ThumbOperandBranchExchange(instruction, &rm);
  EXPECT_EQ(REGISTER_R7, rm);
}

TEST(ThumbLoadPCRelative, Compute) {
  uint16_t instruction = 0x4FFFu;  // ldr r7, [pc, #1020]
  ArmRegisterIndex rd;
  uint_fast16_t offset;
  ThumbOperandLoadPCRelative(instruction, &rd, &offset);
  EXPECT_EQ(REGISTER_R7, rd);
  EXPECT_EQ(1020u, offset);
}

TEST(ThumbLoadStoreRegisterOffset, Compute) {
  uint16_t instruction = 0x5888;  // ldr r0, [r1, r2]
  ArmRegisterIndex rd, rn, rm;
  ThumbOperandLoadStoreRegisterOffset(instruction, &rd, &rn, &rm);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_EQ(REGISTER_R2, rm);
}

TEST(ThumbLoadStoreWordImmediateOffset, Compute) {
  uint16_t instruction = 0x6FC8;  // ldr r0, [r1, #124]
  ArmRegisterIndex rd, rn;
  uint_fast8_t offset;
  ThumbOperandLoadStoreWordImmediateOffset(instruction, &rd, &rn, &offset);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_EQ(124u, offset);
}

TEST(ThumbLoadStoreHalfWordImmediateOffset, Compute) {
  uint16_t instruction = 0x8FC8;  // ldrh r0, [r1, #62]
  ArmRegisterIndex rd, rn;
  uint_fast8_t offset;
  ThumbOperandLoadStoreWordImmediateOffset(instruction, &rd, &rn, &offset);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_EQ(124u, offset);
}

TEST(ThumbLoadStoreBYteImmediateOffset, Compute) {
  uint16_t instruction = 0x7FC8;  // ldrb r0, [r1, #31]
  ArmRegisterIndex rd, rn;
  uint_fast8_t offset;
  ThumbOperandLoadStoreWordImmediateOffset(instruction, &rd, &rn, &offset);
  EXPECT_EQ(REGISTER_R0, rd);
  EXPECT_EQ(REGISTER_R1, rn);
  EXPECT_EQ(124u, offset);
}

TEST(ThumbLoadStoreSPRelative, Compute) {
  uint16_t instruction = 0x9FFF;  // ldr r7, [sp, #1020]
  ArmRegisterIndex rd;
  uint_fast16_t offset;
  ThumbOperandLoadStoreSPRelative(instruction, &rd, &offset);
  EXPECT_EQ(REGISTER_R7, rd);
  EXPECT_EQ(1020u, offset);
}

TEST(ThumbLoadStoreAddToSPOrPc, Compute) {
  uint16_t instruction = 0xAFFF;  // add r7, sp, #1020
  ArmRegisterIndex rd;
  uint_fast16_t offset;
  ThumbOperandAddToSPOrPC(instruction, &rd, &offset);
  EXPECT_EQ(REGISTER_R7, rd);
  EXPECT_EQ(1020u, offset);
}

TEST(ThumbAdjustStackPointer, Compute) {
  uint16_t instruction = 0xB07Fu;  // add sp, sp, #508
  uint_fast16_t immediate;
  ThumbOperandAdjustStackPointer(instruction, &immediate);
  EXPECT_EQ(508u, immediate);
}

TEST(ThumbPushRegisterList, Compute) {
  uint16_t instruction = 0xB5FF;  // push {r0-r7, lr}
  uint_fast16_t register_list;
  ThumbOperandPushRegisterList(instruction, &register_list);
  EXPECT_EQ(0x40FFu, register_list);
}

TEST(ThumbPopRegisterList, Compute) {
  uint16_t instruction = 0xBDFFu;  // pop {r0-r7, pc}
  uint_fast16_t register_list;
  ThumbOperandPopRegisterList(instruction, &register_list);
  EXPECT_EQ(0x80FFu, register_list);
}

TEST(ThumbLoadStoreMultiple, Compute) {
  uint16_t instruction = 0xCFFFu;  // ldm r7, {r0-r7}
  ArmRegisterIndex rd;
  uint_fast16_t register_list;
  ThumbOperandLoadStoreMultiple(instruction, &rd, &register_list);
  EXPECT_EQ(REGISTER_R7, rd);
  EXPECT_EQ(0xFFu, register_list);
}

TEST(ThumbConditionalForwardBranch, Compute) {
  uint16_t instruction = 0xDD7Du;  // ble #254
  uint_fast8_t condition;
  uint_fast32_t offset;
  ThumbOperandConditionalForwardBranch(instruction, &condition, &offset);
  EXPECT_EQ(13u, condition);
  EXPECT_EQ(250u, offset);
}

TEST(ThumbConditionalReverseBranch, Compute) {
  uint16_t instruction = 0xDDFFu;  // ble #2
  uint_fast8_t condition;
  uint_fast32_t offset;
  ThumbOperandConditionalReverseBranch(instruction, &condition, &offset);
  EXPECT_EQ(13u, condition);
  EXPECT_EQ(0xFFFFFFFEu, offset);
}

TEST(ThumbForwardBranch, Compute) {
  uint16_t instruction = 0xE3FDu;  // b #2046
  uint_fast32_t offset;
  ThumbOperandForwardBranch(instruction, &offset);
  EXPECT_EQ(2042u, offset);
}

TEST(ThumbReverseBranch, Compute) {
  uint16_t instruction = 0xE7FF;  // b #2
  uint_fast32_t offset;
  ThumbOperandReverseBranch(instruction, &offset);
  EXPECT_EQ(0xFFFFFFFEu, offset);
}

TEST(ThumbForwardBranchLink, Compute) {
  // bl #0xFFFE
  uint_fast32_t offset;
  uint16_t first_instruction = 0xF00Fu;
  ThumbOperandForwardBranchLink(first_instruction, &offset);
  EXPECT_EQ(0xF000u, offset);

  uint16_t second_instruction = 0xFFFDu;
  ThumbOperandBranchLink(second_instruction, &offset);
  EXPECT_EQ(0xFFAu, offset);
}

TEST(ThumbReverseBranchLink, Compute) {
  // bl #2
  uint_fast32_t offset;
  uint16_t first_instruction = 0xF7FFu;
  ThumbOperandReverseBranchLink(first_instruction, &offset);
  EXPECT_EQ(0xFFFFF000u, offset);

  uint16_t second_instruction = 0xFFFFu;
  ThumbOperandBranchLink(second_instruction, &offset);
  EXPECT_EQ(0xFFEu, offset);
}