#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_THUMB_EXECUTE_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_THUMB_EXECUTE_

#include "emulator/cpu/arm7tdmi/decoders/thumb/branch.h"
#include "emulator/cpu/arm7tdmi/decoders/thumb/opcode.h"
#include "emulator/cpu/arm7tdmi/decoders/thumb/operand.h"
#include "emulator/cpu/arm7tdmi/instructions/block_data_transfer.h"
#include "emulator/cpu/arm7tdmi/instructions/branch_exchange.h"
#include "emulator/cpu/arm7tdmi/instructions/data_processing.h"
#include "emulator/cpu/arm7tdmi/instructions/multiply.h"
#include "emulator/cpu/arm7tdmi/instructions/swi.h"
#include "emulator/memory/memory.h"

static inline bool ThumbInstructionExecute(uint16_t next_instruction,
                                           ArmAllRegisters* registers,
                                           Memory* memory) {
  ArmRegisterIndex rd, rn, rm;
  uint_fast32_t branch_offset_32;
  uint_fast8_t condition, immediate_8, offset_8;
  uint_fast16_t branch_offset_16, immediate_16, offset_16, register_list;

  bool modified_pc;
  ThumbOpcode opcode = ThumbDecodeOpcode(next_instruction);
  switch (opcode) {
    case THUMB_OPCODE_ADCS:
      ThumbOperandDataProcessingRegister(next_instruction, &rd, &rm);
      ArmADCS(registers, rd, rd, registers->current.user.gprs.gprs[rm]);
      modified_pc = false;
      break;
    case THUMB_OPCODE_ADD_ANY:
      ThumbOperandSpecialDataProcessing(next_instruction, &rd, &rm);
      ArmADD(&registers->current.user.gprs, rd, rd,
             registers->current.user.gprs.gprs[rm]);
      modified_pc = (rd == REGISTER_R15);
      break;
    case THUMB_OPCODE_ADD_PC:
      ThumbOperandAddToSPOrPC(next_instruction, &rd, &immediate_16);
      ArmADD(&registers->current.user.gprs, rd, REGISTER_R15, immediate_16);
      modified_pc = false;
      break;
    case THUMB_OPCODE_ADD_SP:
      ThumbOperandAddToSPOrPC(next_instruction, &rd, &immediate_16);
      ArmADD(&registers->current.user.gprs, rd, REGISTER_R13, immediate_16);
      modified_pc = false;
      break;
    case THUMB_OPCODE_ADD_SP_I7:
      ThumbOperandAdjustStackPointer(next_instruction, &immediate_16);
      ArmADD(&registers->current.user.gprs, REGISTER_R13, REGISTER_R13,
             immediate_16);
      modified_pc = false;
      break;
    case THUMB_OPCODE_ADDS:
      ThumbOperandAddSubtractRegister(next_instruction, &rd, &rn, &rm);
      ArmADDS(registers, rd, rn, registers->current.user.gprs.gprs[rm]);
      modified_pc = false;
      break;
    case THUMB_OPCODE_ADDS_I3:
      ThumbOperandAddSubtractImmediate(next_instruction, &rd, &rn,
                                       &immediate_8);
      ArmADDS(registers, rd, rn, immediate_8);
      modified_pc = false;
      break;
    case THUMB_OPCODE_ADDS_I8:
      ThumbOperandAddSubtractCompareMoveImmediate(next_instruction, &rd,
                                                  &immediate_8);
      ArmADDS(registers, rd, rd, immediate_8);
      modified_pc = false;
      break;
    case THUMB_OPCODE_ANDS:
      ThumbOperandDataProcessingRegister(next_instruction, &rd, &rm);
      ArmANDS(registers, rd, rd, registers->current.user.gprs.gprs[rm],
              registers->current.user.cpsr.carry);
      modified_pc = false;
      break;
    case THUMB_OPCODE_ASRS:
      // TODO
      break;
    case THUMB_OPCODE_ASRS_I5:
      // TODO
      break;
    case THUMB_OPCODE_B_FWD:
      ThumbOperandForwardBranch(next_instruction, &branch_offset_32);
      ThumbB(&registers->current.user.gprs, branch_offset_32);
      modified_pc = true;
      break;
    case THUMB_OPCODE_B_FWD_COND:
      // TODO
      break;
    case THUMB_OPCODE_B_REV:
      ThumbOperandReverseBranch(next_instruction, &branch_offset_32);
      ThumbB(&registers->current.user.gprs, branch_offset_32);
      modified_pc = true;
      break;
    case THUMB_OPCODE_B_REV_COND:
      // TODO
      break;
    case THUMB_OPCODE_BICS:
      ThumbOperandDataProcessingRegister(next_instruction, &rd, &rm);
      ArmBICS(registers, rd, rd, registers->current.user.gprs.gprs[rm],
              registers->current.user.cpsr.carry);
      modified_pc = false;
      break;
    case THUMB_OPCODE_BL:
      ThumbOperandBranchLink(next_instruction, &branch_offset_16);
      ThumbBL2(&registers->current.user.gprs, branch_offset_16);
      modified_pc = true;
      break;
    case THUMB_OPCODE_BL_FWD:
      ThumbOperandForwardBranchLink(next_instruction, &branch_offset_32);
      ThumbBL1(&registers->current.user.gprs, branch_offset_32);
      modified_pc = false;
      break;
    case THUMB_OPCODE_BL_REV:
      ThumbOperandReverseBranchLink(next_instruction, &branch_offset_32);
      ThumbBL1(&registers->current.user.gprs, branch_offset_32);
      modified_pc = false;
      break;
    case THUMB_OPCODE_BX:
      ThumbOperandBranchExchange(next_instruction, &rm);
      ArmBX(&registers->current.user, rm);
      modified_pc = true;
      break;
    case THUMB_OPCODE_CMN:
      ThumbOperandDataProcessingRegister(next_instruction, &rn, &rm);
      ArmCMN(&registers->current.user, rn,
             registers->current.user.gprs.gprs[rm]);
      modified_pc = false;
      break;
    case THUMB_OPCODE_CMP:
      ThumbOperandDataProcessingRegister(next_instruction, &rn, &rm);
      ArmCMP(&registers->current.user, rn,
             registers->current.user.gprs.gprs[rm]);
      modified_pc = false;
      break;
    case THUMB_OPCODE_CMP_I8:
      ThumbOperandAddSubtractCompareMoveImmediate(next_instruction, &rn,
                                                  &immediate_8);
      ArmCMP(&registers->current.user, rn, immediate_8);
      modified_pc = false;
      break;
    case THUMB_OPCODE_CMP_ANY:
      ThumbOperandSpecialDataProcessing(next_instruction, &rn, &rm);
      ArmCMP(&registers->current.user, rn,
             registers->current.user.gprs.gprs[rm]);
      modified_pc = false;
      break;
    case THUMB_OPCODE_EORS:
      ThumbOperandDataProcessingRegister(next_instruction, &rd, &rm);
      ArmEORS(registers, rd, rd, registers->current.user.gprs.gprs[rm],
              registers->current.user.cpsr.carry);
      modified_pc = false;
      break;
    case THUMB_OPCODE_LDMIA:
      ThumbOperandLoadStoreMultiple(next_instruction, &rd, &register_list);
      ArmLDMIAW(&registers->current.user.gprs, memory, rd, register_list);
      modified_pc = false;
      break;
    case THUMB_OPCODE_LDR:
      break;
    case THUMB_OPCODE_LDR_I5:
      break;
    case THUMB_OPCODE_LDR_PC_OFFSET_I8:
      break;
    case THUMB_OPCODE_LDR_SP_OFFSET_I8:
      break;
    case THUMB_OPCODE_LDRB:
      break;
    case THUMB_OPCODE_LDRB_I5:
      break;
    case THUMB_OPCODE_LDRH:
      break;
    case THUMB_OPCODE_LDRH_I5:
      break;
    case THUMB_OPCODE_LDRSB:
      break;
    case THUMB_OPCODE_LDRSH:
      break;
    case THUMB_OPCODE_LSLS:
      // TODO
      break;
    case THUMB_OPCODE_LSLS_I5:
      // TODO
      break;
    case THUMB_OPCODE_LSRS:
      // TODO
      break;
    case THUMB_OPCODE_LSRS_I5:
      // TODO
      break;
    case THUMB_OPCODE_MOV_ANY:
      ThumbOperandSpecialDataProcessing(next_instruction, &rd, &rm);
      ArmMOVS(registers, rd, registers->current.user.gprs.gprs[rm],
              registers->current.user.cpsr.carry);
      modified_pc = (rd == REGISTER_R15);
      break;
    case THUMB_OPCODE_MOVS_I8:
      ThumbOperandAddSubtractCompareMoveImmediate(next_instruction, &rd,
                                                  &immediate_8);
      ArmMOVS(registers, rd, immediate_8, registers->current.user.cpsr.carry);
      modified_pc = false;
      break;
    case THUMB_OPCODE_MULS:
      ThumbOperandDataProcessingRegister(next_instruction, &rd, &rm);
      ArmMULS(&registers->current.user, rd, rd, rm);
      modified_pc = false;
      break;
    case THUMB_OPCODE_MVNS:
      ThumbOperandDataProcessingRegister(next_instruction, &rd, &rm);
      ArmMVNS(registers, rd, registers->current.user.gprs.gprs[rm],
              registers->current.user.cpsr.carry);
      modified_pc = false;
      break;
    case THUMB_OPCODE_NEGS:
      // TODO
      break;
    case THUMB_OPCODE_ORRS:
      ThumbOperandDataProcessingRegister(next_instruction, &rd, &rm);
      ArmORRS(registers, rd, rd, registers->current.user.gprs.gprs[rm],
              registers->current.user.cpsr.carry);
      modified_pc = false;
      break;
    case THUMB_OPCODE_POP:
      ThumbOperandPushPopRegisterList(next_instruction, &register_list);
      ArmLDMIA(&registers->current.user.gprs, memory, REGISTER_R14,
               register_list);
      modified_pc = false;
      break;
    case THUMB_OPCODE_PUSH:
      ThumbOperandPushPopRegisterList(next_instruction, &register_list);
      ArmSTMDBW(&registers->current.user.gprs, memory, REGISTER_R14,
                register_list);
      modified_pc = false;
      break;
    case THUMB_OPCODE_RORS:
      // TODO
      break;
    case THUMB_OPCODE_SBCS:
      ThumbOperandDataProcessingRegister(next_instruction, &rd, &rm);
      ArmSBCS(registers, rd, rd, registers->current.user.gprs.gprs[rm]);
      modified_pc = false;
      break;
    case THUMB_OPCODE_STMIA:
      ThumbOperandLoadStoreMultiple(next_instruction, &rd, &register_list);
      ArmSTMIAW(&registers->current.user.gprs, memory, rd, register_list);
      modified_pc = false;
      break;
    case THUMB_OPCODE_STR:
      break;
    case THUMB_OPCODE_STR_I5:
      break;
    case THUMB_OPCODE_STR_SP_OFFSET_I8:
      break;
    case THUMB_OPCODE_STRB:
      break;
    case THUMB_OPCODE_STRB_I5:
      break;
    case THUMB_OPCODE_STRH:
      break;
    case THUMB_OPCODE_STRH_I5:
      break;
    case THUMB_OPCODE_SUB_SP_I7:
      ThumbOperandAdjustStackPointer(next_instruction, &immediate_16);
      ArmSUB(&registers->current.user.gprs, REGISTER_R13, REGISTER_R13,
             immediate_16);
      modified_pc = false;
      break;
    case THUMB_OPCODE_SUBS:
      ThumbOperandDataProcessingRegister(next_instruction, &rd, &rm);
      ArmSUBS(registers, rd, rd, registers->current.user.gprs.gprs[rm]);
      modified_pc = false;
      break;
    case THUMB_OPCODE_SUBS_I3:
      ThumbOperandAddSubtractImmediate(next_instruction, &rd, &rn,
                                       &immediate_8);
      ArmSUBS(registers, rd, rn, immediate_8);
      modified_pc = false;
      break;
    case THUMB_OPCODE_SUBS_I8:
      ThumbOperandAddSubtractCompareMoveImmediate(next_instruction, &rd,
                                                  &immediate_8);
      ArmSUBS(registers, rd, rd, immediate_8);
      modified_pc = false;
      break;
    case THUMB_OPCODE_SWI:
      ArmSWI(registers);
      modified_pc = true;
      break;
    case THUMB_OPCODE_TST:
      ThumbOperandDataProcessingRegister(next_instruction, &rn, &rm);
      ArmTST(&registers->current.user, rn,
             registers->current.user.gprs.gprs[rm],
             registers->current.user.cpsr.carry);
      modified_pc = false;
      break;
    default:
      assert(false);
    case THUMB_OPCODE_UNDEF:
      ArmExceptionUND(registers);
      modified_pc = true;
      break;
  }

  return modified_pc;
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_THUMB_EXECUTE_