#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_THUMB_OPERAND_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_THUMB_OPERAND_

#include "emulator/cpu/arm7tdmi/arm7tdmi.h"

static inline void ThumbOperandShift(uint16_t instruction, ArmRegisterIndex *Rd,
                                     ArmRegisterIndex *Rm) {
  *Rd = (ArmRegisterIndex)((instruction >> 0u) & 0x7u);
  *Rm = (ArmRegisterIndex)((instruction >> 3u) & 0x7u);
}

static inline void ThumbOperandAddSubtractRegister(uint16_t instruction,
                                                   ArmRegisterIndex *Rd,
                                                   ArmRegisterIndex *Rm,
                                                   ArmRegisterIndex *Rn) {
  *Rd = (ArmRegisterIndex)((instruction >> 0u) & 0x7u);
  *Rn = (ArmRegisterIndex)((instruction >> 3u) & 0x7u);
  *Rm = (ArmRegisterIndex)((instruction >> 6u) & 0x7u);
}

static inline void ThumbOperandAddSubtractImmediate(uint16_t instruction,
                                                    ArmRegisterIndex *Rd,
                                                    ArmRegisterIndex *Rn,
                                                    uint_fast8_t *immediate) {
  *Rd = (ArmRegisterIndex)((instruction >> 0u) & 0x7u);
  *Rn = (ArmRegisterIndex)((instruction >> 3u) & 0x7u);
  *immediate = (instruction >> 6u) & 0x7u;
}

static inline void ThumbOperandAddSubtractCompareMoveImmediate(
    uint16_t instruction, ArmRegisterIndex *Rd_Rn, uint_fast8_t *immediate) {
  *Rd_Rn = (ArmRegisterIndex)((instruction >> 7u) & 0x7u);
  *immediate = instruction & 0xFFu;
}

static inline void ThumbOperandDataProcessingRegister(uint16_t instruction,
                                                      ArmRegisterIndex *Rd_Rn,
                                                      ArmRegisterIndex *Rm_Rs) {
  *Rd_Rn = (ArmRegisterIndex)((instruction >> 0u) & 0x7u);
  *Rm_Rs = (ArmRegisterIndex)((instruction >> 3u) & 0x7u);
}

static inline void ThumbOperandSpecialDataProcessing(uint16_t instruction,
                                                     ArmRegisterIndex *Rd_Rn,
                                                     ArmRegisterIndex *Rm) {
  bool h1 = (instruction >> 7u) & 1u;
  bool h2 = (instruction >> 6u) & 1u;

  *Rd_Rn = (ArmRegisterIndex)(((instruction >> 0u) & 0x7u) | (h1 << 4u));
  *Rm = (ArmRegisterIndex)(((instruction >> 3u) & 0x7u) | (h2 << 4u));
}

static inline void ThumbOperandBranchExchange(uint16_t instruction,
                                              ArmRegisterIndex *Rm) {
  bool h2 = (instruction >> 6u) & 1u;
  *Rm = (ArmRegisterIndex)(((instruction >> 3u) & 0x7u) | (h2 << 4u));
}

static inline void ThumbOperandLoadPCRelative(uint16_t instruction,
                                              ArmRegisterIndex *Rd,
                                              uint_fast16_t *offset) {
  *Rd = (ArmRegisterIndex)((instruction >> 8u) & 0x7u);
  *offset = (instruction & 0xFFu) * 4u;
}

static inline void ThumbOperandLoadStoreRegisterOffset(uint16_t instruction,
                                                       ArmRegisterIndex *Rd,
                                                       ArmRegisterIndex *Rm,
                                                       ArmRegisterIndex *Rn) {
  *Rd = (ArmRegisterIndex)((instruction >> 0u) & 0x7u);
  *Rn = (ArmRegisterIndex)((instruction >> 3u) & 0x7u);
  *Rm = (ArmRegisterIndex)((instruction >> 6u) & 0x7u);
}

static inline void ThumbOperandLoadStoreWordByteImmediateOffset(
    uint16_t instruction, ArmRegisterIndex *Rd, ArmRegisterIndex *Rn,
    uint_fast8_t *offset) {
  *Rd = (ArmRegisterIndex)((instruction >> 0u) & 0x7u);
  *Rn = (ArmRegisterIndex)((instruction >> 3u) & 0x7u);
  *offset = (instruction >> 6u) & 0x1Fu;
}

static inline void ThumbOperandLoadStoreHalfWordImmediateOffset(
    uint16_t instruction, ArmRegisterIndex *Rd, ArmRegisterIndex *Rn,
    uint_fast8_t *offset) {
  *Rd = (ArmRegisterIndex)((instruction >> 0u) & 0x7u);
  *Rn = (ArmRegisterIndex)((instruction >> 3u) & 0x7u);
  *offset = (instruction >> 6u) & 0x1Fu;
}

static inline void ThumbOperandLoadStoreSPRelative(uint16_t instruction,
                                                   ArmRegisterIndex *Rd,
                                                   uint_fast16_t *offset) {
  *Rd = (ArmRegisterIndex)((instruction >> 8u) & 0x7u);
  *offset = (instruction & 0xFFu) * 4u;
}

static inline void ThumbOperandAddToSPOrPC(uint16_t instruction,
                                           ArmRegisterIndex *Rd,
                                           uint_fast16_t *offset) {
  *Rd = (ArmRegisterIndex)((instruction >> 8u) & 0x7u);
  *offset = (instruction & 0xFFu) * 4u;
}

static inline void ThumbAdjustStackPointer(uint16_t instruction,
                                           uint_fast8_t *immediate) {
  *immediate = instruction & 0x7Fu;
}

static inline void ThumbPushPopRegisterList(uint16_t instruction,
                                            uint_fast16_t *register_list) {
  bool lr = (instruction >> 8u) & 1u;
  *register_list = instruction & 0x7Fu | (lr << REGISTER_R14);
}

static inline void ThumbOperandLoadStoreMultiple(uint16_t instruction,
                                                 ArmRegisterIndex *Rd,
                                                 uint_fast16_t *register_list) {
  *Rd = (ArmRegisterIndex)((instruction >> 8u) & 0x7u);
  *register_list = (instruction & 0xFFu);
}

static inline void ThumbOperandConditionalForwardBranch(uint16_t instruction,
                                                        uint_fast8_t *condition,
                                                        uint_fast32_t *offset) {
  *condition = (ArmRegisterIndex)((instruction >> 8u) & 0xFu);
  *offset = (instruction & 0xFFu) << 1u;
}

static inline void ThumbOperandConditionalReverseBranch(uint16_t instruction,
                                                        uint_fast8_t *condition,
                                                        uint_fast32_t *offset) {
  *condition = (ArmRegisterIndex)((instruction >> 8u) & 0xFu);
  *offset = 0xFFFFFE00u | ((instruction & 0xFFu) << 1u);
}

static inline void ThumbOperandForwardBranch(uint16_t instruction,
                                             uint_fast32_t *offset) {
  *offset = (instruction & 0x7FFu) << 1u;
}

static inline void ThumbOperandReverseBranch(uint16_t instruction,
                                             uint_fast32_t *offset) {
  *offset = 0xFFFFF000u | ((instruction & 0x7FFu) << 1u);
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
                                          uint_fast32_t *offset) {
  *offset = (instruction & 0x7FFu) << 1u;
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_THUMB_OPERAND_