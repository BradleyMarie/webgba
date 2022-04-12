#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_THUMB_EXECUTE_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_THUMB_EXECUTE_

#include "emulator/cpu/arm7tdmi/decoders/thumb/branch_link.h"
#include "emulator/cpu/arm7tdmi/decoders/thumb/condition.h"
#include "emulator/cpu/arm7tdmi/decoders/thumb/load.h"
#include "emulator/cpu/arm7tdmi/decoders/thumb/operand.h"
#include "emulator/cpu/arm7tdmi/decoders/thumb/shift.h"
#include "emulator/cpu/arm7tdmi/exceptions.h"
#include "emulator/cpu/arm7tdmi/instructions/block_data_transfer.h"
#include "emulator/cpu/arm7tdmi/instructions/branch.h"
#include "emulator/cpu/arm7tdmi/instructions/branch_exchange.h"
#include "emulator/cpu/arm7tdmi/instructions/data_processing.h"
#include "emulator/cpu/arm7tdmi/instructions/load_store_register_byte.h"
#include "emulator/cpu/arm7tdmi/instructions/multiply.h"
#include "emulator/cpu/arm7tdmi/instructions/signed_data_transfer.h"
#include "emulator/cpu/arm7tdmi/instructions/swi.h"
#include "emulator/memory/memory.h"
#include "tools/thumb_opcode_decoder/decoder.h"

static inline void ThumbInstructionExecute(uint16_t next_instruction,
                                           ArmAllRegisters* registers,
                                           Memory* memory) {
  ArmRegisterIndex rd, rn, rm;
  uint_fast32_t branch_offset_32;
  uint_fast8_t condition, immediate_8, offset_8;
  uint_fast16_t branch_offset_16, immediate_16, offset_16, register_list;

  ThumbOpcode opcode = ThumbDecodeOpcode(next_instruction);
  switch (opcode) {
    case THUMB_OPCODE_ADCS:
      ThumbOperandDataProcessingRegister(next_instruction, &rd, &rm);
      ArmADCS(registers, rd, rd, registers->current.user.gprs.gprs[rm]);
      break;
    case THUMB_OPCODE_ADD_ANY:
      ThumbOperandSpecialDataProcessing(next_instruction, &rd, &rm);
      ArmADD(registers, rd, rd, registers->current.user.gprs.gprs[rm]);
      break;
    case THUMB_OPCODE_ADD_PC:
      ThumbOperandAddToSPOrPC(next_instruction, &rd, &immediate_16);
      ArmADD(registers, rd, REGISTER_R15, immediate_16);
      break;
    case THUMB_OPCODE_ADD_SP:
      ThumbOperandAddToSPOrPC(next_instruction, &rd, &immediate_16);
      ArmADD(registers, rd, REGISTER_R13, immediate_16);
      break;
    case THUMB_OPCODE_ADD_SP_I7:
      ThumbOperandAdjustStackPointer(next_instruction, &immediate_16);
      ArmADD(registers, REGISTER_R13, REGISTER_R13, immediate_16);
      break;
    case THUMB_OPCODE_ADDS:
      ThumbOperandAddSubtractRegister(next_instruction, &rd, &rn, &rm);
      ArmADDS(registers, rd, rn, registers->current.user.gprs.gprs[rm]);
      break;
    case THUMB_OPCODE_ADDS_I3:
      ThumbOperandAddSubtractImmediate(next_instruction, &rd, &rn,
                                       &immediate_8);
      ArmADDS(registers, rd, rn, immediate_8);
      break;
    case THUMB_OPCODE_ADDS_I8:
      ThumbOperandAddSubtractCompareMoveImmediate(next_instruction, &rd,
                                                  &immediate_8);
      ArmADDS(registers, rd, rd, immediate_8);
      break;
    case THUMB_OPCODE_ANDS:
      ThumbOperandDataProcessingRegister(next_instruction, &rd, &rm);
      ArmANDS(registers, rd, rd, registers->current.user.gprs.gprs[rm],
              registers->current.user.cpsr.carry);
      break;
    case THUMB_OPCODE_ASRS:
      ThumbOperandDataProcessingRegister(next_instruction, &rd, &rm);
      ThumbASRS_R(&registers->current.user, rd, rm);
      break;
    case THUMB_OPCODE_ASRS_I5:
      ThumbOperandShiftByImmediate(next_instruction, &rd, &rm, &immediate_8);
      ThumbASRS_I(&registers->current.user, rd, rm, immediate_8);
      break;
    case THUMB_OPCODE_B_FWD:
      ThumbOperandForwardBranch(next_instruction, &branch_offset_32);
      ArmB(registers, branch_offset_32);
      break;
    case THUMB_OPCODE_B_FWD_COND:
      ThumbOperandConditionalForwardBranch(next_instruction, &condition,
                                           &branch_offset_32);
      if (ThumbShouldBranch(registers->current.user.cpsr, condition)) {
        ArmB(registers, branch_offset_32);
      }
      break;
    case THUMB_OPCODE_B_REV:
      ThumbOperandReverseBranch(next_instruction, &branch_offset_32);
      ArmB(registers, branch_offset_32);
      break;
    case THUMB_OPCODE_B_REV_COND:
      ThumbOperandConditionalReverseBranch(next_instruction, &condition,
                                           &branch_offset_32);
      if (ThumbShouldBranch(registers->current.user.cpsr, condition)) {
        ArmB(registers, branch_offset_32);
      }
      break;
    case THUMB_OPCODE_BICS:
      ThumbOperandDataProcessingRegister(next_instruction, &rd, &rm);
      ArmBICS(registers, rd, rd, registers->current.user.gprs.gprs[rm],
              registers->current.user.cpsr.carry);
      break;
    case THUMB_OPCODE_BL:
      ThumbOperandBranchLink(next_instruction, &branch_offset_16);
      ThumbBL2(&registers->current.user.gprs, branch_offset_16);
      break;
    case THUMB_OPCODE_BL_FWD:
      ThumbOperandForwardBranchLink(next_instruction, &branch_offset_32);
      ThumbBL1(&registers->current.user.gprs, branch_offset_32);
      break;
    case THUMB_OPCODE_BL_REV:
      ThumbOperandReverseBranchLink(next_instruction, &branch_offset_32);
      ThumbBL1(&registers->current.user.gprs, branch_offset_32);
      break;
    case THUMB_OPCODE_BX:
      ThumbOperandBranchExchange(next_instruction, &rm);
      ArmBX(registers, rm);
      break;
    case THUMB_OPCODE_CMN:
      ThumbOperandDataProcessingRegister(next_instruction, &rn, &rm);
      ArmCMN(registers, rn, registers->current.user.gprs.gprs[rm]);
      break;
    case THUMB_OPCODE_CMP:
      ThumbOperandDataProcessingRegister(next_instruction, &rn, &rm);
      ArmCMP(registers, rn, registers->current.user.gprs.gprs[rm]);
      break;
    case THUMB_OPCODE_CMP_I8:
      ThumbOperandAddSubtractCompareMoveImmediate(next_instruction, &rn,
                                                  &immediate_8);
      ArmCMP(registers, rn, immediate_8);
      break;
    case THUMB_OPCODE_CMP_ANY:
      ThumbOperandSpecialDataProcessing(next_instruction, &rn, &rm);
      ArmCMP(registers, rn, registers->current.user.gprs.gprs[rm]);
      break;
    case THUMB_OPCODE_EORS:
      ThumbOperandDataProcessingRegister(next_instruction, &rd, &rm);
      ArmEORS(registers, rd, rd, registers->current.user.gprs.gprs[rm],
              registers->current.user.cpsr.carry);
      break;
    case THUMB_OPCODE_LDMIA:
      ThumbOperandLoadStoreMultiple(next_instruction, &rn, &register_list);
      if (register_list & (1u << rn)) {
        ArmLDMIA(registers, memory, rn, register_list);
      } else {
        ArmLDMIAW(registers, memory, rn, register_list);
      }
      break;
    case THUMB_OPCODE_LDR:
      ThumbOperandLoadStoreRegisterOffset(next_instruction, &rd, &rn, &rm);
      ArmLDR_IB(registers, memory, rd, rn,
                registers->current.user.gprs.gprs[rm]);
      break;
    case THUMB_OPCODE_LDR_I5:
      ThumbOperandLoadStoreWordImmediateOffset(next_instruction, &rd, &rn,
                                               &offset_8);
      ArmLDR_IB(registers, memory, rd, rn, offset_8);
      break;
    case THUMB_OPCODE_LDR_PC_OFFSET_I8:
      ThumbOperandLoadPCRelative(next_instruction, &rd, &offset_16);
      ThumbLDR_PC_IB(registers, memory, rd, offset_16);
      break;
    case THUMB_OPCODE_LDR_SP_OFFSET_I8:
      ThumbOperandLoadStoreSPRelative(next_instruction, &rd, &offset_16);
      ArmLDR_IB(registers, memory, rd, REGISTER_R13, offset_16);
      break;
    case THUMB_OPCODE_LDRB:
      ThumbOperandLoadStoreRegisterOffset(next_instruction, &rd, &rn, &rm);
      ArmLDRB_IB(registers, memory, rd, rn,
                 registers->current.user.gprs.gprs[rm]);
      break;
    case THUMB_OPCODE_LDRB_I5:
      ThumbOperandLoadStoreByteImmediateOffset(next_instruction, &rd, &rn,
                                               &offset_8);
      ArmLDRB_IB(registers, memory, rd, rn, offset_8);
      break;
    case THUMB_OPCODE_LDRH:
      ThumbOperandLoadStoreRegisterOffset(next_instruction, &rd, &rn, &rm);
      ArmLDRH_IB(registers, memory, rd, rn,
                 registers->current.user.gprs.gprs[rm]);
      break;
    case THUMB_OPCODE_LDRH_I5:
      ThumbOperandLoadStoreHalfWordImmediateOffset(next_instruction, &rd, &rn,
                                                   &offset_8);
      ArmLDRH_IB(registers, memory, rd, rn, offset_8);
      break;
    case THUMB_OPCODE_LDRSB:
      ThumbOperandLoadStoreRegisterOffset(next_instruction, &rd, &rn, &rm);
      ArmLDRSB_IB(registers, memory, rd, rn,
                  registers->current.user.gprs.gprs[rm]);
      break;
    case THUMB_OPCODE_LDRSH:
      ThumbOperandLoadStoreRegisterOffset(next_instruction, &rd, &rn, &rm);
      ArmLDRSH_IB(registers, memory, rd, rn,
                  registers->current.user.gprs.gprs[rm]);
      break;
    case THUMB_OPCODE_LSLS:
      ThumbOperandDataProcessingRegister(next_instruction, &rd, &rm);
      ThumbLSLS_R(&registers->current.user, rd, rm);
      break;
    case THUMB_OPCODE_LSLS_I5:
      ThumbOperandShiftByImmediate(next_instruction, &rd, &rm, &immediate_8);
      ThumbLSLS_I(&registers->current.user, rd, rm, immediate_8);
      break;
    case THUMB_OPCODE_LSRS:
      ThumbOperandDataProcessingRegister(next_instruction, &rd, &rm);
      ThumbLSRS_R(&registers->current.user, rd, rm);
      break;
    case THUMB_OPCODE_LSRS_I5:
      ThumbOperandShiftByImmediate(next_instruction, &rd, &rm, &immediate_8);
      ThumbLSRS_I(&registers->current.user, rd, rm, immediate_8);
      break;
    case THUMB_OPCODE_MOV_ANY:
      ThumbOperandSpecialDataProcessing(next_instruction, &rd, &rm);
      ArmMOV(registers, rd, registers->current.user.gprs.gprs[rm]);
      break;
    case THUMB_OPCODE_MOVS_I8:
      ThumbOperandAddSubtractCompareMoveImmediate(next_instruction, &rd,
                                                  &immediate_8);
      ArmMOVS(registers, rd, immediate_8, registers->current.user.cpsr.carry);
      break;
    case THUMB_OPCODE_MULS:
      ThumbOperandDataProcessingRegister(next_instruction, &rd, &rm);
      ArmMULS(registers, rd, rd, rm);
      break;
    case THUMB_OPCODE_MVNS:
      ThumbOperandDataProcessingRegister(next_instruction, &rd, &rm);
      ArmMVNS(registers, rd, registers->current.user.gprs.gprs[rm],
              registers->current.user.cpsr.carry);
      break;
    case THUMB_OPCODE_NEGS:
      ThumbOperandDataProcessingRegister(next_instruction, &rd, &rm);
      ArmRSBS(registers, rd, rm, 0u);
      break;
    case THUMB_OPCODE_ORRS:
      ThumbOperandDataProcessingRegister(next_instruction, &rd, &rm);
      ArmORRS(registers, rd, rd, registers->current.user.gprs.gprs[rm],
              registers->current.user.cpsr.carry);
      break;
    case THUMB_OPCODE_POP:
      ThumbOperandPopRegisterList(next_instruction, &register_list);
      ArmLDMIAW(registers, memory, REGISTER_R13, register_list);
      break;
    case THUMB_OPCODE_PUSH:
      ThumbOperandPushRegisterList(next_instruction, &register_list);
      ArmSTMDBW(registers, memory, REGISTER_R13, register_list);
      break;
    case THUMB_OPCODE_RORS:
      ThumbOperandDataProcessingRegister(next_instruction, &rd, &rm);
      ThumbRORS(&registers->current.user, rd, rm);
      break;
    case THUMB_OPCODE_SBCS:
      ThumbOperandDataProcessingRegister(next_instruction, &rd, &rm);
      ArmSBCS(registers, rd, rd, registers->current.user.gprs.gprs[rm]);
      break;
    case THUMB_OPCODE_STMIA:
      ThumbOperandLoadStoreMultiple(next_instruction, &rd, &register_list);
      ArmSTMIAW(registers, memory, rd, register_list);
      break;
    case THUMB_OPCODE_STR:
      ThumbOperandLoadStoreRegisterOffset(next_instruction, &rd, &rn, &rm);
      ArmSTR_IB(registers, memory, rd, rn,
                registers->current.user.gprs.gprs[rm]);
      break;
    case THUMB_OPCODE_STR_I5:
      ThumbOperandLoadStoreWordImmediateOffset(next_instruction, &rd, &rn,
                                               &offset_8);
      ArmSTR_IB(registers, memory, rd, rn, offset_8);
      break;
    case THUMB_OPCODE_STR_SP_OFFSET_I8:
      ThumbOperandLoadStoreSPRelative(next_instruction, &rd, &offset_16);
      ArmSTR_IB(registers, memory, rd, REGISTER_R13, offset_16);
      break;
    case THUMB_OPCODE_STRB:
      ThumbOperandLoadStoreRegisterOffset(next_instruction, &rd, &rn, &rm);
      ArmSTRB_IB(registers, memory, rd, rn,
                 registers->current.user.gprs.gprs[rm]);
      break;
    case THUMB_OPCODE_STRB_I5:
      ThumbOperandLoadStoreByteImmediateOffset(next_instruction, &rd, &rn,
                                               &offset_8);
      ArmSTRB_IB(registers, memory, rd, rn, offset_8);
      break;
    case THUMB_OPCODE_STRH:
      ThumbOperandLoadStoreRegisterOffset(next_instruction, &rd, &rn, &rm);
      ArmSTRH_IB(registers, memory, rd, rn,
                 registers->current.user.gprs.gprs[rm]);
      break;
    case THUMB_OPCODE_STRH_I5:
      ThumbOperandLoadStoreHalfWordImmediateOffset(next_instruction, &rd, &rn,
                                                   &offset_8);
      ArmSTRH_IB(registers, memory, rd, rn, offset_8);
      break;
    case THUMB_OPCODE_SUB_SP_I7:
      ThumbOperandAdjustStackPointer(next_instruction, &immediate_16);
      ArmSUB(registers, REGISTER_R13, REGISTER_R13, immediate_16);
      break;
    case THUMB_OPCODE_SUBS:
      ThumbOperandAddSubtractRegister(next_instruction, &rd, &rn, &rm);
      ArmSUBS(registers, rd, rn, registers->current.user.gprs.gprs[rm]);
      break;
    case THUMB_OPCODE_SUBS_I3:
      ThumbOperandAddSubtractImmediate(next_instruction, &rd, &rn,
                                       &immediate_8);
      ArmSUBS(registers, rd, rn, immediate_8);
      break;
    case THUMB_OPCODE_SUBS_I8:
      ThumbOperandAddSubtractCompareMoveImmediate(next_instruction, &rd,
                                                  &immediate_8);
      ArmSUBS(registers, rd, rd, immediate_8);
      break;
    case THUMB_OPCODE_SWI:
      ArmSWI(registers);
      break;
    case THUMB_OPCODE_TST:
      ThumbOperandDataProcessingRegister(next_instruction, &rn, &rm);
      ArmTST(registers, rn, registers->current.user.gprs.gprs[rm],
             registers->current.user.cpsr.carry);
      break;
    default:
      assert(false);
    case THUMB_OPCODE_UNDEF:
      ArmExceptionUND(registers);
      break;
  }
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_THUMB_EXECUTE_