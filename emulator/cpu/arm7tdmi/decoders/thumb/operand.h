#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_THUMB_OPERAND_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_THUMB_OPERAND_

#include "emulator/cpu/arm7tdmi/registers.h"
#include "util/macros.h"

static inline void ThumbOperandShiftByImmediate(uint16_t instruction,
                                                ArmRegisterIndex *Rd,
                                                ArmRegisterIndex *Rm,
                                                uint_fast8_t *immediate) {
  *Rd = (ArmRegisterIndex)((instruction >> 0u) & 0x7u);
  codegen_assert(*Rd != REGISTER_PC);

  *Rm = (ArmRegisterIndex)((instruction >> 3u) & 0x7u);
  codegen_assert(*Rm != REGISTER_PC);

  *immediate = ((instruction >> 6u) & 0x1Fu);
}

static inline void ThumbOperandAddSubtractRegister(uint16_t instruction,
                                                   ArmRegisterIndex *Rd,
                                                   ArmRegisterIndex *Rn,
                                                   ArmRegisterIndex *Rm) {
  *Rd = (ArmRegisterIndex)((instruction >> 0u) & 0x7u);
  codegen_assert(*Rd != REGISTER_PC);

  *Rn = (ArmRegisterIndex)((instruction >> 3u) & 0x7u);
  codegen_assert(*Rn != REGISTER_PC);

  *Rm = (ArmRegisterIndex)((instruction >> 6u) & 0x7u);
  codegen_assert(*Rm != REGISTER_PC);
}

static inline void ThumbOperandAddSubtractImmediate(uint16_t instruction,
                                                    ArmRegisterIndex *Rd,
                                                    ArmRegisterIndex *Rn,
                                                    uint_fast8_t *immediate) {
  *Rd = (ArmRegisterIndex)((instruction >> 0u) & 0x7u);
  codegen_assert(*Rd != REGISTER_PC);

  *Rn = (ArmRegisterIndex)((instruction >> 3u) & 0x7u);
  codegen_assert(*Rn != REGISTER_PC);

  *immediate = (instruction >> 6u) & 0x7u;
}

static inline void ThumbOperandAddSubtractCompareMoveImmediate(
    uint16_t instruction, ArmRegisterIndex *Rd_Rn, uint_fast8_t *immediate) {
  *Rd_Rn = (ArmRegisterIndex)((instruction >> 8u) & 0x7u);
  codegen_assert(*Rd_Rn != REGISTER_PC);

  *immediate = instruction & 0xFFu;
}

static inline void ThumbOperandDataProcessingRegister(uint16_t instruction,
                                                      ArmRegisterIndex *Rd_Rn,
                                                      ArmRegisterIndex *Rm_Rs) {
  *Rd_Rn = (ArmRegisterIndex)((instruction >> 0u) & 0x7u);
  codegen_assert(*Rd_Rn != REGISTER_PC);

  *Rm_Rs = (ArmRegisterIndex)((instruction >> 3u) & 0x7u);
  codegen_assert(*Rm_Rs != REGISTER_PC);
}

static inline void ThumbOperandSpecialDataProcessing(uint16_t instruction,
                                                     ArmRegisterIndex *Rd_Rn,
                                                     ArmRegisterIndex *Rm) {
  *Rd_Rn = (ArmRegisterIndex)(((instruction >> 4u) & 0x8u) |
                              ((instruction >> 0u) & 0x7u));
  *Rm = (ArmRegisterIndex)((instruction >> 3u) & 0xFu);
}

static inline void ThumbOperandBranchExchange(uint16_t instruction,
                                              ArmRegisterIndex *Rm) {
  *Rm = (ArmRegisterIndex)((instruction >> 3u) & 0xFu);
}

static inline void ThumbOperandLoadPCRelative(uint16_t instruction,
                                              ArmRegisterIndex *Rd,
                                              uint_fast16_t *offset) {
  *Rd = (ArmRegisterIndex)((instruction >> 8u) & 0x7u);
  codegen_assert(*Rd != REGISTER_PC);

  *offset = (instruction & 0xFFu) * 4u;
}

static inline void ThumbOperandLoadStoreRegisterOffset(uint16_t instruction,
                                                       ArmRegisterIndex *Rd,
                                                       ArmRegisterIndex *Rn,
                                                       ArmRegisterIndex *Rm) {
  *Rd = (ArmRegisterIndex)((instruction >> 0u) & 0x7u);
  codegen_assert(*Rd != REGISTER_PC);

  *Rn = (ArmRegisterIndex)((instruction >> 3u) & 0x7u);
  codegen_assert(*Rn != REGISTER_PC);

  *Rm = (ArmRegisterIndex)((instruction >> 6u) & 0x7u);
  codegen_assert(*Rm != REGISTER_PC);
}

static inline void ThumbOperandLoadStoreWordImmediateOffset(
    uint16_t instruction, ArmRegisterIndex *Rd, ArmRegisterIndex *Rn,
    uint_fast8_t *offset) {
  *Rd = (ArmRegisterIndex)((instruction >> 0u) & 0x7u);
  codegen_assert(*Rd != REGISTER_PC);

  *Rn = (ArmRegisterIndex)((instruction >> 3u) & 0x7u);
  codegen_assert(*Rn != REGISTER_PC);

  *offset = ((instruction >> 6u) & 0x1Fu) * 4u;
}

static inline void ThumbOperandLoadStoreHalfWordImmediateOffset(
    uint16_t instruction, ArmRegisterIndex *Rd, ArmRegisterIndex *Rn,
    uint_fast8_t *offset) {
  *Rd = (ArmRegisterIndex)((instruction >> 0u) & 0x7u);
  codegen_assert(*Rd != REGISTER_PC);

  *Rn = (ArmRegisterIndex)((instruction >> 3u) & 0x7u);
  codegen_assert(*Rn != REGISTER_PC);

  *offset = ((instruction >> 6u) & 0x1Fu) * 2u;
}

static inline void ThumbOperandLoadStoreByteImmediateOffset(
    uint16_t instruction, ArmRegisterIndex *Rd, ArmRegisterIndex *Rn,
    uint_fast8_t *offset) {
  *Rd = (ArmRegisterIndex)((instruction >> 0u) & 0x7u);
  codegen_assert(*Rd != REGISTER_PC);

  *Rn = (ArmRegisterIndex)((instruction >> 3u) & 0x7u);
  codegen_assert(*Rn != REGISTER_PC);

  *offset = ((instruction >> 6u) & 0x1Fu);
}

static inline void ThumbOperandLoadStoreSPRelative(uint16_t instruction,
                                                   ArmRegisterIndex *Rd,
                                                   uint_fast16_t *offset) {
  ThumbOperandLoadPCRelative(instruction, Rd, offset);
}

static inline void ThumbOperandAddToSPOrPC(uint16_t instruction,
                                           ArmRegisterIndex *Rd,
                                           uint_fast16_t *immediate) {
  ThumbOperandLoadPCRelative(instruction, Rd, immediate);
}

static inline void ThumbOperandAdjustStackPointer(uint16_t instruction,
                                                  uint_fast16_t *immediate) {
  *immediate = (instruction & 0x7Fu) * 4u;
}

static inline void ThumbOperandPushRegisterList(uint16_t instruction,
                                                uint_fast16_t *register_list) {
  *register_list = ((instruction & 0x100) << 6u) | (instruction & 0xFFu);
  codegen_assert((*register_list & (1u << REGISTER_PC)) == 0);
}

static inline void ThumbOperandPopRegisterList(uint16_t instruction,
                                               uint_fast16_t *register_list) {
  *register_list = ((instruction & 0x100) << 7u) | (instruction & 0xFFu);
}

static inline void ThumbOperandLoadStoreMultiple(uint16_t instruction,
                                                 ArmRegisterIndex *Rn,
                                                 uint_fast16_t *register_list) {
  *Rn = (ArmRegisterIndex)((instruction >> 8u) & 0x7u);
  codegen_assert(*Rn != REGISTER_PC);

  *register_list = (instruction & 0xFFu);
  codegen_assert((*register_list & (1u << REGISTER_PC)) == 0);
}

static inline void ThumbOperandConditionalForwardBranch(uint16_t instruction,
                                                        uint_fast8_t *condition,
                                                        uint_fast32_t *offset) {
  *condition = (ArmRegisterIndex)((instruction >> 8u) & 0xFu);
  instruction <<= 8u;
  *offset = instruction >> 7u;
}

static inline void ThumbOperandConditionalReverseBranch(uint16_t instruction,
                                                        uint_fast8_t *condition,
                                                        uint_fast32_t *offset) {
  *condition = (ArmRegisterIndex)((instruction >> 8u) & 0xFu);
  *offset = 0xFFFFFE00u | (instruction << 1u);
}

static inline void ThumbOperandForwardBranch(uint16_t instruction,
                                             uint_fast32_t *offset) {
  instruction <<= 5u;
  *offset = instruction >> 4u;
}

static inline void ThumbOperandReverseBranch(uint16_t instruction,
                                             uint_fast32_t *offset) {
  *offset = 0xFFFFF000u | (instruction << 1u);
}

static inline void ThumbOperandForwardBranchLink(uint16_t instruction,
                                                 uint_fast32_t *offset) {
  *offset = (instruction & 0x7FFu) << 12u;
}

static inline void ThumbOperandReverseBranchLink(uint16_t instruction,
                                                 uint_fast32_t *offset) {
  *offset = 0xFF800000u | ((instruction & 0x7FFu) << 12u);
}

static inline void ThumbOperandBranchLink(uint16_t instruction,
                                          uint_fast16_t *offset) {
  *offset = (instruction & 0x7FFu) << 1u;
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_THUMB_OPERAND_