#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_ARM_EXECUTE_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_ARM_EXECUTE_

#include "emulator/cpu/arm7tdmi/decoders/arm/branch.h"
#include "emulator/cpu/arm7tdmi/decoders/arm/condition.h"
#include "emulator/cpu/arm7tdmi/decoders/arm/opcode.h"
#include "emulator/cpu/arm7tdmi/decoders/arm/operand.h"
#include "emulator/cpu/arm7tdmi/instructions/block_data_transfer.h"
#include "emulator/cpu/arm7tdmi/instructions/branch_exchange.h"
#include "emulator/cpu/arm7tdmi/instructions/coprocessor_data_operation.h"
#include "emulator/cpu/arm7tdmi/instructions/coprocessor_data_transfer.h"
#include "emulator/cpu/arm7tdmi/instructions/coprocessor_register_transfer.h"
#include "emulator/cpu/arm7tdmi/instructions/data_processing.h"
#include "emulator/cpu/arm7tdmi/instructions/load_store_register_byte.h"
#include "emulator/cpu/arm7tdmi/instructions/move_status_register.h"
#include "emulator/cpu/arm7tdmi/instructions/multiply.h"
#include "emulator/cpu/arm7tdmi/instructions/multiply_long.h"
#include "emulator/cpu/arm7tdmi/instructions/signed_data_transfer.h"
#include "emulator/cpu/arm7tdmi/instructions/single_data_swap.h"
#include "emulator/cpu/arm7tdmi/instructions/swi.h"
#include "emulator/memory/memory.h"

static inline bool ArmInstructionExecute(uint32_t next_instruction,
                                         ArmAllRegisters* registers,
                                         Memory* memory) {
  bool should_execute = ArmInstructionShouldExecute(
      registers->current.user.cpsr, next_instruction);
  if (!should_execute) {
    return false;
  }

  ArmRegisterIndex rd_msw, rd_lsw, rd, rn, rm, rs;
  uint32_t operand2, offset_32;
  uint_fast32_t branch_offset;
  uint_fast16_t register_list, offset_16;
  uint_fast8_t offset_8;
  bool shifter_carry_out, control, flags;

  bool modified_pc;
  ArmOpcode opcode = ArmDecodeOpcode(next_instruction);
  switch (opcode) {
    case ARM_OPCODE_ADC:
      ArmOperandDataProcessingOperand2(next_instruction,
                                       &registers->current.user, &rd, &rn,
                                       &shifter_carry_out, &operand2);
      ArmADC(&registers->current.user, rd, rn, operand2);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_ADC_I32:
      ArmOperandDataProcessingImmediate(next_instruction,
                                        &registers->current.user, &rd, &rn,
                                        &shifter_carry_out, &operand2);
      ArmADC(&registers->current.user, rd, rn, operand2);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_ADCS:
      ArmOperandDataProcessingOperand2(next_instruction,
                                       &registers->current.user, &rd, &rn,
                                       &shifter_carry_out, &operand2);
      ArmADCS(registers, rd, rn, operand2);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_ADCS_I32:
      ArmOperandDataProcessingImmediate(next_instruction,
                                        &registers->current.user, &rd, &rn,
                                        &shifter_carry_out, &operand2);
      ArmADCS(registers, rd, rn, operand2);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_ADD:
      ArmOperandDataProcessingOperand2(next_instruction,
                                       &registers->current.user, &rd, &rn,
                                       &shifter_carry_out, &operand2);
      ArmADD(&registers->current.user.gprs, rd, rn, operand2);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_ADD_I32:
      ArmOperandDataProcessingImmediate(next_instruction,
                                        &registers->current.user, &rd, &rn,
                                        &shifter_carry_out, &operand2);
      ArmADD(&registers->current.user.gprs, rd, rn, operand2);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_ADDS:
      ArmOperandDataProcessingOperand2(next_instruction,
                                       &registers->current.user, &rd, &rn,
                                       &shifter_carry_out, &operand2);
      ArmADDS(registers, rd, rn, operand2);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_ADDS_I32:
      ArmOperandDataProcessingImmediate(next_instruction,
                                        &registers->current.user, &rd, &rn,
                                        &shifter_carry_out, &operand2);
      ArmADDS(registers, rd, rn, operand2);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_AND:
      ArmOperandDataProcessingOperand2(next_instruction,
                                       &registers->current.user, &rd, &rn,
                                       &shifter_carry_out, &operand2);
      ArmAND(&registers->current.user.gprs, rd, rn, operand2);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_AND_I32:
      ArmOperandDataProcessingImmediate(next_instruction,
                                        &registers->current.user, &rd, &rn,
                                        &shifter_carry_out, &operand2);
      ArmAND(&registers->current.user.gprs, rd, rn, operand2);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_ANDS:
      ArmOperandDataProcessingOperand2(next_instruction,
                                       &registers->current.user, &rd, &rn,
                                       &shifter_carry_out, &operand2);
      ArmANDS(registers, rd, rn, operand2, shifter_carry_out);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_ANDS_I32:
      ArmOperandDataProcessingImmediate(next_instruction,
                                        &registers->current.user, &rd, &rn,
                                        &shifter_carry_out, &operand2);
      ArmANDS(registers, rd, rn, operand2, shifter_carry_out);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_B_FWD:
      ArmOperandBranchForward(next_instruction, &branch_offset);
      ArmB(&registers->current.user.gprs, branch_offset);
      modified_pc = true;
      break;
    case ARM_OPCODE_B_REV:
      ArmOperandBranchReverse(next_instruction, &branch_offset);
      ArmB(&registers->current.user.gprs, branch_offset);
      modified_pc = true;
      break;
    case ARM_OPCODE_BIC:
      ArmOperandDataProcessingOperand2(next_instruction,
                                       &registers->current.user, &rd, &rn,
                                       &shifter_carry_out, &operand2);
      ArmBIC(&registers->current.user.gprs, rd, rn, operand2);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_BIC_I32:
      ArmOperandDataProcessingImmediate(next_instruction,
                                        &registers->current.user, &rd, &rn,
                                        &shifter_carry_out, &operand2);
      ArmBIC(&registers->current.user.gprs, rd, rn, operand2);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_BICS:
      ArmOperandDataProcessingOperand2(next_instruction,
                                       &registers->current.user, &rd, &rn,
                                       &shifter_carry_out, &operand2);
      ArmBICS(registers, rd, rn, operand2, shifter_carry_out);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_BICS_I32:
      ArmOperandDataProcessingImmediate(next_instruction,
                                        &registers->current.user, &rd, &rn,
                                        &shifter_carry_out, &operand2);
      ArmBICS(registers, rd, rn, operand2, shifter_carry_out);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_BL_FWD:
      ArmOperandBranchForward(next_instruction, &branch_offset);
      ArmBL(&registers->current.user.gprs, branch_offset);
      modified_pc = true;
      break;
    case ARM_OPCODE_BL_REV:
      ArmOperandBranchReverse(next_instruction, &branch_offset);
      ArmBL(&registers->current.user.gprs, branch_offset);
      modified_pc = true;
      break;
    case ARM_OPCODE_BX:
      ArmOperandBranchExchange(next_instruction, &rm);
      ArmBX(&registers->current.user, rm);
      modified_pc = true;
      break;
    case ARM_OPCODE_CDP:
      ArmCDP(registers);
      modified_pc = true;
      break;
    case ARM_OPCODE_CMN:
      ArmOperandDataProcessingOperand2(next_instruction,
                                       &registers->current.user, &rd, &rn,
                                       &shifter_carry_out, &operand2);
      ArmCMN(&registers->current.user, rn, operand2);
      modified_pc = false;
      break;
    case ARM_OPCODE_CMN_I32:
      ArmOperandDataProcessingImmediate(next_instruction,
                                        &registers->current.user, &rd, &rn,
                                        &shifter_carry_out, &operand2);
      ArmCMN(&registers->current.user, rn, operand2);
      modified_pc = false;
      break;
    case ARM_OPCODE_CMP:
      ArmOperandDataProcessingOperand2(next_instruction,
                                       &registers->current.user, &rd, &rn,
                                       &shifter_carry_out, &operand2);
      ArmCMP(&registers->current.user, rn, operand2);
      modified_pc = false;
      break;
    case ARM_OPCODE_CMP_I32:
      ArmOperandDataProcessingImmediate(next_instruction,
                                        &registers->current.user, &rd, &rn,
                                        &shifter_carry_out, &operand2);
      ArmCMP(&registers->current.user, rn, operand2);
      modified_pc = false;
      break;
    case ARM_OPCODE_EOR:
      ArmOperandDataProcessingOperand2(next_instruction,
                                       &registers->current.user, &rd, &rn,
                                       &shifter_carry_out, &operand2);
      ArmEOR(&registers->current.user.gprs, rd, rn, operand2);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_EOR_I32:
      ArmOperandDataProcessingImmediate(next_instruction,
                                        &registers->current.user, &rd, &rn,
                                        &shifter_carry_out, &operand2);
      ArmEOR(&registers->current.user.gprs, rd, rn, operand2);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_EORS:
      ArmOperandDataProcessingOperand2(next_instruction,
                                       &registers->current.user, &rd, &rn,
                                       &shifter_carry_out, &operand2);
      ArmEORS(registers, rd, rn, operand2, shifter_carry_out);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_EORS_I32:
      ArmOperandDataProcessingImmediate(next_instruction,
                                        &registers->current.user, &rd, &rn,
                                        &shifter_carry_out, &operand2);
      ArmEORS(registers, rd, rn, operand2, shifter_carry_out);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_LDC:
      ArmLDC(registers);
      modified_pc = true;
      break;
    case ARM_OPCODE_LDMDA:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      ArmLDMDA(&registers->current.user.gprs, memory, rn, register_list);
      modified_pc = !!((register_list >> REGISTER_R15) & 0x1u);
      break;
    case ARM_OPCODE_LDMDA_W:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      ArmLDMDAW(&registers->current.user.gprs, memory, rn, register_list);
      modified_pc =
          !!((register_list >> REGISTER_R15) & 0x1u) || (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDMDB:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      ArmLDMDB(&registers->current.user.gprs, memory, rn, register_list);
      modified_pc = !!((register_list >> REGISTER_R15) & 0x1u);
      break;
    case ARM_OPCODE_LDMDB_W:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      ArmLDMDBW(&registers->current.user.gprs, memory, rn, register_list);
      modified_pc =
          !!((register_list >> REGISTER_R15) & 0x1u) || (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDMIA:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      ArmLDMIA(&registers->current.user.gprs, memory, rn, register_list);
      modified_pc = !!((register_list >> REGISTER_R15) & 0x1u);
      break;
    case ARM_OPCODE_LDMIA_W:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      ArmLDMIAW(&registers->current.user.gprs, memory, rn, register_list);
      modified_pc =
          !!((register_list >> REGISTER_R15) & 0x1u) || (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDMIB:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      ArmLDMIB(&registers->current.user.gprs, memory, rn, register_list);
      modified_pc = !!((register_list >> REGISTER_R15) & 0x1u);
      break;
    case ARM_OPCODE_LDMIB_W:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      ArmLDMIBW(&registers->current.user.gprs, memory, rn, register_list);
      modified_pc =
          !!((register_list >> REGISTER_R15) & 0x1u) || (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDMSDA:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      ArmLDMSDA(registers, memory, rn, register_list);
      modified_pc = !!((register_list >> REGISTER_R15) & 0x1u);
      break;
    case ARM_OPCODE_LDMSDA_W:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      ArmLDMSDAW(registers, memory, rn, register_list);
      modified_pc =
          !!((register_list >> REGISTER_R15) & 0x1u) || (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDMSDB:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      ArmLDMSDB(registers, memory, rn, register_list);
      modified_pc = !!((register_list >> REGISTER_R15) & 0x1u);
      break;
    case ARM_OPCODE_LDMSDB_W:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      ArmLDMSDBW(registers, memory, rn, register_list);
      modified_pc =
          !!((register_list >> REGISTER_R15) & 0x1u) || (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDMSIA:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      ArmLDMSIA(registers, memory, rn, register_list);
      modified_pc = !!((register_list >> REGISTER_R15) & 0x1u);
      break;
    case ARM_OPCODE_LDMSIA_W:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      ArmLDMSIAW(registers, memory, rn, register_list);
      modified_pc =
          !!((register_list >> REGISTER_R15) & 0x1u) || (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDMSIB:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      ArmLDMSIB(registers, memory, rn, register_list);
      modified_pc = !!((register_list >> REGISTER_R15) & 0x1u);
      break;
    case ARM_OPCODE_LDMSIB_W:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      ArmLDMSIBW(registers, memory, rn, register_list);
      modified_pc =
          !!((register_list >> REGISTER_R15) & 0x1u) || (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDR_DAW:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      ArmLDR_DAW(&registers->current.user.gprs, memory, rd, rn, offset_32);
      modified_pc = (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDR_DAW_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      ArmLDR_DAW(&registers->current.user.gprs, memory, rd, rn, offset_16);
      modified_pc = (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDR_DB:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      ArmLDR_DB(&registers->current.user.gprs, memory, rd, rn, offset_32);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_LDR_DB_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      ArmLDR_DB(&registers->current.user.gprs, memory, rd, rn, offset_16);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_LDR_DBW:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      ArmLDR_DBW(&registers->current.user.gprs, memory, rd, rn, offset_32);
      modified_pc = (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDR_DBW_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      ArmLDR_DBW(&registers->current.user.gprs, memory, rd, rn, offset_16);
      modified_pc = (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDR_IAW:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      ArmLDR_IAW(&registers->current.user.gprs, memory, rd, rn, offset_32);
      modified_pc = (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDR_IAW_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      ArmLDR_IAW(&registers->current.user.gprs, memory, rd, rn, offset_16);
      modified_pc = (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDR_IB:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      ArmLDR_IB(&registers->current.user.gprs, memory, rd, rn, offset_32);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_LDR_IB_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      ArmLDR_IB(&registers->current.user.gprs, memory, rd, rn, offset_16);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_LDR_IBW:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      ArmLDR_IBW(&registers->current.user.gprs, memory, rd, rn, offset_32);
      modified_pc = (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDR_IBW_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      ArmLDR_IBW(&registers->current.user.gprs, memory, rd, rn, offset_16);
      modified_pc = (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRB_DAW:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      ArmLDRB_DAW(&registers->current.user.gprs, memory, rd, rn, offset_32);
      modified_pc = (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRB_DAW_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      ArmLDRB_DAW(&registers->current.user.gprs, memory, rd, rn, offset_16);
      modified_pc = (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRB_DB:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      ArmLDRB_DB(&registers->current.user.gprs, memory, rd, rn, offset_32);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRB_DB_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      ArmLDRB_DB(&registers->current.user.gprs, memory, rd, rn, offset_16);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRB_DBW:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      ArmLDRB_DBW(&registers->current.user.gprs, memory, rd, rn, offset_32);
      modified_pc = (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRB_DBW_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      ArmLDR_DBW(&registers->current.user.gprs, memory, rd, rn, offset_16);
      modified_pc = (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRB_IAW:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      ArmLDRB_IAW(&registers->current.user.gprs, memory, rd, rn, offset_32);
      modified_pc = (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRB_IAW_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      ArmLDRB_IAW(&registers->current.user.gprs, memory, rd, rn, offset_16);
      modified_pc = (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRB_IB:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      ArmLDRB_IB(&registers->current.user.gprs, memory, rd, rn, offset_32);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRB_IB_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      ArmLDRB_IB(&registers->current.user.gprs, memory, rd, rn, offset_16);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRB_IBW:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      ArmLDRB_IBW(&registers->current.user.gprs, memory, rd, rn, offset_32);
      modified_pc = (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRB_IBW_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      ArmLDRB_IBW(&registers->current.user.gprs, memory, rd, rn, offset_16);
      modified_pc = (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRBT_DAW:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      ArmLDRBT_DAW(registers, memory, rd, rn, offset_32);
      modified_pc = (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRBT_DAW_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      ArmLDRBT_DAW(registers, memory, rd, rn, offset_16);
      modified_pc = (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRBT_IAW:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      ArmLDRBT_IAW(registers, memory, rd, rn, offset_32);
      modified_pc = (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRBT_IAW_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      ArmLDRBT_IAW(registers, memory, rd, rn, offset_16);
      modified_pc = (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRH_DAW:
      ArmOperandHalfwordAddressMode(next_instruction,
                                    &registers->current.user.gprs, &rd, &rn,
                                    &offset_32);
      ArmLDRH_DAW(&registers->current.user.gprs, memory, rd, rn, offset_32);
      modified_pc = (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRH_DAW_I8:
      ArmOperandHalfwordImmediate(next_instruction, &rd, &rn, &offset_8);
      ArmLDRH_DAW(&registers->current.user.gprs, memory, rd, rn, offset_8);
      modified_pc = (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRH_DB:
      ArmOperandHalfwordAddressMode(next_instruction,
                                    &registers->current.user.gprs, &rd, &rn,
                                    &offset_32);
      ArmLDRH_DB(&registers->current.user.gprs, memory, rd, rn, offset_32);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRH_DB_I8:
      ArmOperandHalfwordImmediate(next_instruction, &rd, &rn, &offset_8);
      ArmLDRH_DB(&registers->current.user.gprs, memory, rd, rn, offset_8);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRH_DBW:
      ArmOperandHalfwordAddressMode(next_instruction,
                                    &registers->current.user.gprs, &rd, &rn,
                                    &offset_32);
      ArmLDRH_DBW(&registers->current.user.gprs, memory, rd, rn, offset_32);
      modified_pc = (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRH_DBW_I8:
      ArmOperandHalfwordImmediate(next_instruction, &rd, &rn, &offset_8);
      ArmLDRH_DBW(&registers->current.user.gprs, memory, rd, rn, offset_8);
      modified_pc = (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRH_IAW:
      ArmOperandHalfwordAddressMode(next_instruction,
                                    &registers->current.user.gprs, &rd, &rn,
                                    &offset_32);
      ArmLDRH_IAW(&registers->current.user.gprs, memory, rd, rn, offset_32);
      modified_pc = (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRH_IAW_I8:
      ArmOperandHalfwordImmediate(next_instruction, &rd, &rn, &offset_8);
      ArmLDRH_IAW(&registers->current.user.gprs, memory, rd, rn, offset_8);
      modified_pc = (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRH_IB:
      ArmOperandHalfwordAddressMode(next_instruction,
                                    &registers->current.user.gprs, &rd, &rn,
                                    &offset_32);
      ArmLDRH_IB(&registers->current.user.gprs, memory, rd, rn, offset_32);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRH_IB_I8:
      ArmOperandHalfwordImmediate(next_instruction, &rd, &rn, &offset_8);
      ArmLDRH_IB(&registers->current.user.gprs, memory, rd, rn, offset_8);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRH_IBW:
      ArmOperandHalfwordAddressMode(next_instruction,
                                    &registers->current.user.gprs, &rd, &rn,
                                    &offset_32);
      ArmLDRH_IBW(&registers->current.user.gprs, memory, rd, rn, offset_32);
      modified_pc = (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRH_IBW_I8:
      ArmOperandHalfwordImmediate(next_instruction, &rd, &rn, &offset_8);
      ArmLDRH_IBW(&registers->current.user.gprs, memory, rd, rn, offset_8);
      modified_pc = (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRSB_DAW:
      ArmOperandHalfwordAddressMode(next_instruction,
                                    &registers->current.user.gprs, &rd, &rn,
                                    &offset_32);
      ArmLDRSB_DAW(&registers->current.user.gprs, memory, rd, rn, offset_32);
      modified_pc = (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRSB_DAW_I8:
      ArmOperandHalfwordImmediate(next_instruction, &rd, &rn, &offset_8);
      ArmLDRSB_DAW(&registers->current.user.gprs, memory, rd, rn, offset_8);
      modified_pc = (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRSB_DB:
      ArmOperandHalfwordAddressMode(next_instruction,
                                    &registers->current.user.gprs, &rd, &rn,
                                    &offset_32);
      ArmLDRSB_DB(&registers->current.user.gprs, memory, rd, rn, offset_32);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRSB_DB_I8:
      ArmOperandHalfwordImmediate(next_instruction, &rd, &rn, &offset_8);
      ArmLDRSB_DB(&registers->current.user.gprs, memory, rd, rn, offset_8);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRSB_DBW:
      ArmOperandHalfwordAddressMode(next_instruction,
                                    &registers->current.user.gprs, &rd, &rn,
                                    &offset_32);
      ArmLDRSB_DBW(&registers->current.user.gprs, memory, rd, rn, offset_32);
      modified_pc = (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRSB_DBW_I8:
      ArmOperandHalfwordImmediate(next_instruction, &rd, &rn, &offset_8);
      ArmLDRSB_DBW(&registers->current.user.gprs, memory, rd, rn, offset_8);
      modified_pc = (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRSB_IAW:
      ArmOperandHalfwordAddressMode(next_instruction,
                                    &registers->current.user.gprs, &rd, &rn,
                                    &offset_32);
      ArmLDRSB_IAW(&registers->current.user.gprs, memory, rd, rn, offset_32);
      modified_pc = (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRSB_IAW_I8:
      ArmOperandHalfwordImmediate(next_instruction, &rd, &rn, &offset_8);
      ArmLDRSB_IAW(&registers->current.user.gprs, memory, rd, rn, offset_8);
      modified_pc = (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRSB_IB:
      ArmOperandHalfwordAddressMode(next_instruction,
                                    &registers->current.user.gprs, &rd, &rn,
                                    &offset_32);
      ArmLDRSB_IB(&registers->current.user.gprs, memory, rd, rn, offset_32);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRSB_IB_I8:
      ArmOperandHalfwordImmediate(next_instruction, &rd, &rn, &offset_8);
      ArmLDRSB_IB(&registers->current.user.gprs, memory, rd, rn, offset_8);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRSB_IBW:
      ArmOperandHalfwordAddressMode(next_instruction,
                                    &registers->current.user.gprs, &rd, &rn,
                                    &offset_32);
      ArmLDRSB_IBW(&registers->current.user.gprs, memory, rd, rn, offset_32);
      modified_pc = (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRSB_IBW_I8:
      ArmOperandHalfwordImmediate(next_instruction, &rd, &rn, &offset_8);
      ArmLDRSB_IBW(&registers->current.user.gprs, memory, rd, rn, offset_8);
      modified_pc = (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRSH_DAW:
      ArmOperandHalfwordAddressMode(next_instruction,
                                    &registers->current.user.gprs, &rd, &rn,
                                    &offset_32);
      ArmLDRSH_DAW(&registers->current.user.gprs, memory, rd, rn, offset_32);
      modified_pc = (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRSH_DAW_I8:
      ArmOperandHalfwordImmediate(next_instruction, &rd, &rn, &offset_8);
      ArmLDRSH_DAW(&registers->current.user.gprs, memory, rd, rn, offset_8);
      modified_pc = (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRSH_DB:
      ArmOperandHalfwordAddressMode(next_instruction,
                                    &registers->current.user.gprs, &rd, &rn,
                                    &offset_32);
      ArmLDRSH_DB(&registers->current.user.gprs, memory, rd, rn, offset_32);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRSH_DB_I8:
      ArmOperandHalfwordImmediate(next_instruction, &rd, &rn, &offset_8);
      ArmLDRSH_DB(&registers->current.user.gprs, memory, rd, rn, offset_8);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRSH_DBW:
      ArmOperandHalfwordAddressMode(next_instruction,
                                    &registers->current.user.gprs, &rd, &rn,
                                    &offset_32);
      ArmLDRSH_DBW(&registers->current.user.gprs, memory, rd, rn, offset_32);
      modified_pc = (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRSH_DBW_I8:
      ArmOperandHalfwordImmediate(next_instruction, &rd, &rn, &offset_8);
      ArmLDRSH_DBW(&registers->current.user.gprs, memory, rd, rn, offset_8);
      modified_pc = (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRSH_IAW:
      ArmOperandHalfwordAddressMode(next_instruction,
                                    &registers->current.user.gprs, &rd, &rn,
                                    &offset_32);
      ArmLDRSH_IAW(&registers->current.user.gprs, memory, rd, rn, offset_32);
      modified_pc = (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRSH_IAW_I8:
      ArmOperandHalfwordImmediate(next_instruction, &rd, &rn, &offset_8);
      ArmLDRSH_IAW(&registers->current.user.gprs, memory, rd, rn, offset_8);
      modified_pc = (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRSH_IB:
      ArmOperandHalfwordAddressMode(next_instruction,
                                    &registers->current.user.gprs, &rd, &rn,
                                    &offset_32);
      ArmLDRSH_IB(&registers->current.user.gprs, memory, rd, rn, offset_32);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRSH_IB_I8:
      ArmOperandHalfwordImmediate(next_instruction, &rd, &rn, &offset_8);
      ArmLDRSH_IB(&registers->current.user.gprs, memory, rd, rn, offset_8);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRSH_IBW:
      ArmOperandHalfwordAddressMode(next_instruction,
                                    &registers->current.user.gprs, &rd, &rn,
                                    &offset_32);
      ArmLDRSH_IBW(&registers->current.user.gprs, memory, rd, rn, offset_32);
      modified_pc = (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRSH_IBW_I8:
      ArmOperandHalfwordImmediate(next_instruction, &rd, &rn, &offset_8);
      ArmLDRSH_IBW(&registers->current.user.gprs, memory, rd, rn, offset_8);
      modified_pc = (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRT_DAW:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      ArmLDRT_DAW(registers, memory, rd, rn, offset_32);
      modified_pc = (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRT_DAW_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      ArmLDRT_DAW(registers, memory, rd, rn, offset_16);
      modified_pc = (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRT_IAW:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      ArmLDRT_IAW(registers, memory, rd, rn, offset_32);
      modified_pc = (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRT_IAW_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      ArmLDRT_IAW(registers, memory, rd, rn, offset_16);
      modified_pc = (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_MCR:
      ArmMCR(registers);
      modified_pc = true;
      break;
    case ARM_OPCODE_MLA:
      ArmOperandMultiplyAccumulate(next_instruction, &rd, &rm, &rs, &rn);
      ArmMLA(&registers->current.user.gprs, rd, rm, rs, rn);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_MLAS:
      ArmOperandMultiplyAccumulate(next_instruction, &rd, &rm, &rs, &rn);
      ArmMLAS(&registers->current.user, rd, rm, rs, rn);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_MOV:
      ArmOperandDataProcessingOperand2(next_instruction,
                                       &registers->current.user, &rd, &rn,
                                       &shifter_carry_out, &operand2);
      ArmMOV(&registers->current.user.gprs, rd, operand2);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_MOV_I32:
      ArmOperandDataProcessingImmediate(next_instruction,
                                        &registers->current.user, &rd, &rn,
                                        &shifter_carry_out, &operand2);
      ArmMOV(&registers->current.user.gprs, rd, operand2);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_MOVS:
      ArmOperandDataProcessingOperand2(next_instruction,
                                       &registers->current.user, &rd, &rn,
                                       &shifter_carry_out, &operand2);
      ArmMOVS(registers, rd, operand2, shifter_carry_out);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_MOVS_I32:
      ArmOperandDataProcessingImmediate(next_instruction,
                                        &registers->current.user, &rd, &rn,
                                        &shifter_carry_out, &operand2);
      ArmMOVS(registers, rd, operand2, shifter_carry_out);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_MRC:
      ArmMRC(registers);
      modified_pc = true;
      break;
    case ARM_OPCODE_MRS_CPSR:
      ArmOperandMoveFromStatusRegister(next_instruction, &rd);
      ArmMRS_CPSR(&registers->current.user, rd);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_MRS_SPSR:
      ArmOperandMoveFromStatusRegister(next_instruction, &rd);
      ArmMRS_SPSR(&registers->current, rd);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_MSR_CPSR:
      ArmOperandMoveToStatusRegisterRegister(next_instruction,
                                             &registers->current.user.gprs,
                                             &control, &flags, &operand2);
      ArmMSR_CPSR(registers, control, flags, operand2);
      modified_pc = false;
      break;
    case ARM_OPCODE_MSR_CPSR_I8:
      ArmOperandMoveToStatusRegisterImmediate(next_instruction, &control,
                                              &flags, &operand2);
      ArmMSR_CPSR(registers, control, flags, operand2);
      modified_pc = false;
      break;
    case ARM_OPCODE_MSR_SPSR:
      ArmOperandMoveToStatusRegisterRegister(next_instruction,
                                             &registers->current.user.gprs,
                                             &control, &flags, &operand2);
      ArmMSR_SPSR(&registers->current, control, flags, operand2);
      modified_pc = false;
      break;
    case ARM_OPCODE_MSR_SPSR_I8:
      ArmOperandMoveToStatusRegisterImmediate(next_instruction, &control,
                                              &flags, &operand2);
      ArmMSR_SPSR(&registers->current, control, flags, operand2);
      modified_pc = false;
      break;
    case ARM_OPCODE_MUL:
      ArmOperandMultiply(next_instruction, &rd, &rm, &rs);
      ArmMUL(&registers->current.user.gprs, rd, rm, rs);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_MULS:
      ArmOperandMultiply(next_instruction, &rd, &rm, &rs);
      ArmMULS(&registers->current.user, rd, rm, rs);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_MVN:
      ArmOperandDataProcessingOperand2(next_instruction,
                                       &registers->current.user, &rd, &rn,
                                       &shifter_carry_out, &operand2);
      ArmMVN(&registers->current.user.gprs, rd, operand2);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_MVN_I32:
      ArmOperandDataProcessingImmediate(next_instruction,
                                        &registers->current.user, &rd, &rn,
                                        &shifter_carry_out, &operand2);
      ArmMVN(&registers->current.user.gprs, rd, operand2);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_MVNS:
      ArmOperandDataProcessingOperand2(next_instruction,
                                       &registers->current.user, &rd, &rn,
                                       &shifter_carry_out, &operand2);
      ArmMVNS(registers, rd, operand2, shifter_carry_out);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_MVNS_I32:
      ArmOperandDataProcessingImmediate(next_instruction,
                                        &registers->current.user, &rd, &rn,
                                        &shifter_carry_out, &operand2);
      ArmMVNS(registers, rd, operand2, shifter_carry_out);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_ORR:
      ArmOperandDataProcessingOperand2(next_instruction,
                                       &registers->current.user, &rd, &rn,
                                       &shifter_carry_out, &operand2);
      ArmORR(&registers->current.user.gprs, rd, rn, operand2);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_ORR_I32:
      ArmOperandDataProcessingImmediate(next_instruction,
                                        &registers->current.user, &rd, &rn,
                                        &shifter_carry_out, &operand2);
      ArmORR(&registers->current.user.gprs, rd, rn, operand2);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_ORRS:
      ArmOperandDataProcessingOperand2(next_instruction,
                                       &registers->current.user, &rd, &rn,
                                       &shifter_carry_out, &operand2);
      ArmORRS(registers, rd, rn, operand2, shifter_carry_out);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_ORRS_I32:
      ArmOperandDataProcessingImmediate(next_instruction,
                                        &registers->current.user, &rd, &rn,
                                        &shifter_carry_out, &operand2);
      ArmORRS(registers, rd, rn, operand2, shifter_carry_out);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_RSB:
      ArmOperandDataProcessingOperand2(next_instruction,
                                       &registers->current.user, &rd, &rn,
                                       &shifter_carry_out, &operand2);
      ArmRSB(&registers->current.user.gprs, rd, rn, operand2);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_RSB_I32:
      ArmOperandDataProcessingImmediate(next_instruction,
                                        &registers->current.user, &rd, &rn,
                                        &shifter_carry_out, &operand2);
      ArmRSB(&registers->current.user.gprs, rd, rn, operand2);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_RSBS:
      ArmOperandDataProcessingOperand2(next_instruction,
                                       &registers->current.user, &rd, &rn,
                                       &shifter_carry_out, &operand2);
      ArmRSBS(registers, rd, rn, operand2);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_RSBS_I32:
      ArmOperandDataProcessingImmediate(next_instruction,
                                        &registers->current.user, &rd, &rn,
                                        &shifter_carry_out, &operand2);
      ArmRSBS(registers, rd, rn, operand2);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_RSC:
      ArmOperandDataProcessingOperand2(next_instruction,
                                       &registers->current.user, &rd, &rn,
                                       &shifter_carry_out, &operand2);
      ArmRSC(&registers->current.user, rd, rn, operand2);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_RSC_I32:
      ArmOperandDataProcessingImmediate(next_instruction,
                                        &registers->current.user, &rd, &rn,
                                        &shifter_carry_out, &operand2);
      ArmRSC(&registers->current.user, rd, rn, operand2);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_RSCS:
      ArmOperandDataProcessingOperand2(next_instruction,
                                       &registers->current.user, &rd, &rn,
                                       &shifter_carry_out, &operand2);
      ArmRSCS(registers, rd, rn, operand2);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_RSCS_I32:
      ArmOperandDataProcessingImmediate(next_instruction,
                                        &registers->current.user, &rd, &rn,
                                        &shifter_carry_out, &operand2);
      ArmRSCS(registers, rd, rn, operand2);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_SBC:
      ArmOperandDataProcessingOperand2(next_instruction,
                                       &registers->current.user, &rd, &rn,
                                       &shifter_carry_out, &operand2);
      ArmSBC(&registers->current.user, rd, rn, operand2);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_SBC_I32:
      ArmOperandDataProcessingImmediate(next_instruction,
                                        &registers->current.user, &rd, &rn,
                                        &shifter_carry_out, &operand2);
      ArmSBC(&registers->current.user, rd, rn, operand2);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_SBCS:
      ArmOperandDataProcessingOperand2(next_instruction,
                                       &registers->current.user, &rd, &rn,
                                       &shifter_carry_out, &operand2);
      ArmSBCS(registers, rd, rn, operand2);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_SBCS_I32:
      ArmOperandDataProcessingImmediate(next_instruction,
                                        &registers->current.user, &rd, &rn,
                                        &shifter_carry_out, &operand2);
      ArmSBCS(registers, rd, rn, operand2);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_SMLAL:
      ArmOperandMultiplyLong(next_instruction, &rd_lsw, &rd_msw, &rm, &rs);
      ArmSMLAL(&registers->current.user.gprs, rd_lsw, rd_msw, rm, rs);
      modified_pc = (rd_lsw == REGISTER_R15 || rd_msw == REGISTER_R15);
      break;
    case ARM_OPCODE_SMLALS:
      ArmOperandMultiplyLong(next_instruction, &rd_lsw, &rd_msw, &rm, &rs);
      ArmSMLALS(&registers->current.user, rd_lsw, rd_msw, rm, rs);
      modified_pc = (rd_lsw == REGISTER_R15 || rd_msw == REGISTER_R15);
      break;
    case ARM_OPCODE_SMULL:
      ArmOperandMultiplyLong(next_instruction, &rd_lsw, &rd_msw, &rm, &rs);
      ArmSMULL(&registers->current.user.gprs, rd_lsw, rd_msw, rm, rs);
      modified_pc = (rd_lsw == REGISTER_R15 || rd_msw == REGISTER_R15);
      break;
    case ARM_OPCODE_SMULLS:
      ArmOperandMultiplyLong(next_instruction, &rd_lsw, &rd_msw, &rm, &rs);
      ArmSMULLS(&registers->current.user, rd_lsw, rd_msw, rm, rs);
      modified_pc = (rd_lsw == REGISTER_R15 || rd_msw == REGISTER_R15);
      break;
    case ARM_OPCODE_STC:
      ArmSTC(registers);
      modified_pc = true;
      break;
    case ARM_OPCODE_STMDA:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      ArmSTMDA(&registers->current.user.gprs, memory, rn, register_list);
      modified_pc = false;
      break;
    case ARM_OPCODE_STMDA_W:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      ArmSTMDAW(&registers->current.user.gprs, memory, rn, register_list);
      modified_pc = (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STMDB:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      ArmSTMDB(&registers->current.user.gprs, memory, rn, register_list);
      modified_pc = false;
      break;
    case ARM_OPCODE_STMDB_W:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      ArmSTMDBW(&registers->current.user.gprs, memory, rn, register_list);
      modified_pc = (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STMIA:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      ArmSTMIA(&registers->current.user.gprs, memory, rn, register_list);
      modified_pc = false;
      break;
    case ARM_OPCODE_STMIA_W:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      ArmSTMIAW(&registers->current.user.gprs, memory, rn, register_list);
      modified_pc = (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STMIB:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      ArmSTMIB(&registers->current.user.gprs, memory, rn, register_list);
      modified_pc = false;
      break;
    case ARM_OPCODE_STMIB_W:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      ArmSTMIBW(&registers->current.user.gprs, memory, rn, register_list);
      modified_pc = (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STMSDA:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      ArmSTMSDA(registers, memory, rn, register_list);
      modified_pc = false;
      break;
    case ARM_OPCODE_STMSDA_W:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      ArmSTMSDAW(registers, memory, rn, register_list);
      modified_pc = (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STMSDB:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      ArmSTMSDB(registers, memory, rn, register_list);
      modified_pc = false;
      break;
    case ARM_OPCODE_STMSDB_W:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      ArmSTMSDBW(registers, memory, rn, register_list);
      modified_pc = (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STMSIA:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      ArmSTMSIA(registers, memory, rn, register_list);
      modified_pc = false;
      break;
    case ARM_OPCODE_STMSIA_W:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      ArmSTMSIAW(registers, memory, rn, register_list);
      modified_pc = (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STMSIB:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      ArmSTMSIB(registers, memory, rn, register_list);
      modified_pc = false;
      break;
    case ARM_OPCODE_STMSIB_W:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      ArmSTMSIBW(registers, memory, rn, register_list);
      modified_pc = (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STR_DAW:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      ArmSTR_DAW(&registers->current.user.gprs, memory, rd, rn, offset_32);
      modified_pc = (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STR_DAW_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      ArmSTR_DAW(&registers->current.user.gprs, memory, rd, rn, offset_16);
      modified_pc = (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STR_DB:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      ArmSTR_DB(&registers->current.user.gprs, memory, rd, rn, offset_32);
      modified_pc = false;
      break;
    case ARM_OPCODE_STR_DB_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      ArmSTR_DB(&registers->current.user.gprs, memory, rd, rn, offset_16);
      modified_pc = false;
      break;
    case ARM_OPCODE_STR_DBW:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      ArmSTR_DBW(&registers->current.user.gprs, memory, rd, rn, offset_32);
      modified_pc = (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STR_DBW_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      ArmSTR_DBW(&registers->current.user.gprs, memory, rd, rn, offset_16);
      modified_pc = (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STR_IAW:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      ArmSTR_IAW(&registers->current.user.gprs, memory, rd, rn, offset_32);
      modified_pc = (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STR_IAW_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      ArmSTR_IAW(&registers->current.user.gprs, memory, rd, rn, offset_16);
      modified_pc = (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STR_IB:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      ArmSTR_IB(&registers->current.user.gprs, memory, rd, rn, offset_32);
      modified_pc = false;
      break;
    case ARM_OPCODE_STR_IB_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      ArmSTR_IB(&registers->current.user.gprs, memory, rd, rn, offset_16);
      modified_pc = false;
      break;
    case ARM_OPCODE_STR_IBW:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      ArmSTR_IBW(&registers->current.user.gprs, memory, rd, rn, offset_32);
      modified_pc = (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STR_IBW_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      ArmSTR_IBW(&registers->current.user.gprs, memory, rd, rn, offset_16);
      modified_pc = (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STRB_DAW:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      ArmSTRB_DAW(&registers->current.user.gprs, memory, rd, rn, offset_32);
      modified_pc = (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STRB_DAW_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      ArmSTRB_DAW(&registers->current.user.gprs, memory, rd, rn, offset_16);
      modified_pc = (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STRB_DB:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      ArmSTRB_DB(&registers->current.user.gprs, memory, rd, rn, offset_32);
      modified_pc = false;
      break;
    case ARM_OPCODE_STRB_DB_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      ArmSTRB_DB(&registers->current.user.gprs, memory, rd, rn, offset_16);
      modified_pc = false;
      break;
    case ARM_OPCODE_STRB_DBW:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      ArmSTRB_DBW(&registers->current.user.gprs, memory, rd, rn, offset_32);
      modified_pc = (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STRB_DBW_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      ArmSTR_DBW(&registers->current.user.gprs, memory, rd, rn, offset_16);
      modified_pc = (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STRB_IAW:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      ArmSTRB_IAW(&registers->current.user.gprs, memory, rd, rn, offset_32);
      modified_pc = (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STRB_IAW_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      ArmSTRB_IAW(&registers->current.user.gprs, memory, rd, rn, offset_16);
      modified_pc = (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STRB_IB:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      ArmSTRB_IB(&registers->current.user.gprs, memory, rd, rn, offset_32);
      modified_pc = false;
      break;
    case ARM_OPCODE_STRB_IB_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      ArmSTRB_IB(&registers->current.user.gprs, memory, rd, rn, offset_16);
      modified_pc = false;
      break;
    case ARM_OPCODE_STRB_IBW:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      ArmSTRB_IBW(&registers->current.user.gprs, memory, rd, rn, offset_32);
      modified_pc = (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STRB_IBW_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      ArmSTRB_IBW(&registers->current.user.gprs, memory, rd, rn, offset_16);
      modified_pc = (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STRBT_DAW:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      ArmSTRBT_DAW(registers, memory, rd, rn, offset_32);
      modified_pc = (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STRBT_DAW_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      ArmSTRBT_DAW(registers, memory, rd, rn, offset_16);
      modified_pc = (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STRBT_IAW:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      ArmSTRBT_IAW(registers, memory, rd, rn, offset_32);
      modified_pc = (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STRBT_IAW_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      ArmSTRBT_IAW(registers, memory, rd, rn, offset_16);
      modified_pc = (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STRH_DAW:
      ArmOperandHalfwordAddressMode(next_instruction,
                                    &registers->current.user.gprs, &rd, &rn,
                                    &offset_32);
      ArmSTRH_DAW(&registers->current.user.gprs, memory, rd, rn, offset_32);
      modified_pc = (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STRH_DAW_I8:
      ArmOperandHalfwordImmediate(next_instruction, &rd, &rn, &offset_8);
      ArmSTRH_DAW(&registers->current.user.gprs, memory, rd, rn, offset_8);
      modified_pc = (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STRH_DB:
      ArmOperandHalfwordAddressMode(next_instruction,
                                    &registers->current.user.gprs, &rd, &rn,
                                    &offset_32);
      ArmSTRH_DB(&registers->current.user.gprs, memory, rd, rn, offset_32);
      modified_pc = false;
      break;
    case ARM_OPCODE_STRH_DB_I8:
      ArmOperandHalfwordImmediate(next_instruction, &rd, &rn, &offset_8);
      ArmSTRH_DB(&registers->current.user.gprs, memory, rd, rn, offset_8);
      modified_pc = false;
      break;
    case ARM_OPCODE_STRH_DBW:
      ArmOperandHalfwordAddressMode(next_instruction,
                                    &registers->current.user.gprs, &rd, &rn,
                                    &offset_32);
      ArmSTRH_DBW(&registers->current.user.gprs, memory, rd, rn, offset_32);
      modified_pc = (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STRH_DBW_I8:
      ArmOperandHalfwordImmediate(next_instruction, &rd, &rn, &offset_8);
      ArmSTRH_DBW(&registers->current.user.gprs, memory, rd, rn, offset_8);
      modified_pc = (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STRH_IAW:
      ArmOperandHalfwordAddressMode(next_instruction,
                                    &registers->current.user.gprs, &rd, &rn,
                                    &offset_32);
      ArmSTRH_IAW(&registers->current.user.gprs, memory, rd, rn, offset_32);
      modified_pc = (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STRH_IAW_I8:
      ArmOperandHalfwordImmediate(next_instruction, &rd, &rn, &offset_8);
      ArmSTRH_IAW(&registers->current.user.gprs, memory, rd, rn, offset_8);
      modified_pc = (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STRH_IB:
      ArmOperandHalfwordAddressMode(next_instruction,
                                    &registers->current.user.gprs, &rd, &rn,
                                    &offset_32);
      ArmSTRH_IB(&registers->current.user.gprs, memory, rd, rn, offset_32);
      modified_pc = false;
      break;
    case ARM_OPCODE_STRH_IB_I8:
      ArmOperandHalfwordImmediate(next_instruction, &rd, &rn, &offset_8);
      ArmSTRH_IB(&registers->current.user.gprs, memory, rd, rn, offset_8);
      modified_pc = false;
      break;
    case ARM_OPCODE_STRH_IBW:
      ArmOperandHalfwordAddressMode(next_instruction,
                                    &registers->current.user.gprs, &rd, &rn,
                                    &offset_32);
      ArmSTRH_IBW(&registers->current.user.gprs, memory, rd, rn, offset_32);
      modified_pc = (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STRH_IBW_I8:
      ArmOperandHalfwordImmediate(next_instruction, &rd, &rn, &offset_8);
      ArmSTRH_IBW(&registers->current.user.gprs, memory, rd, rn, offset_8);
      modified_pc = (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STRT_DAW:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      ArmSTRT_DAW(registers, memory, rd, rn, offset_32);
      modified_pc = (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STRT_DAW_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      ArmSTRT_DAW(registers, memory, rd, rn, offset_16);
      modified_pc = (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STRT_IAW:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      ArmSTRT_IAW(registers, memory, rd, rn, offset_32);
      modified_pc = (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STRT_IAW_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      ArmSTRT_IAW(registers, memory, rd, rn, offset_16);
      modified_pc = (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_SUB:
      ArmOperandDataProcessingOperand2(next_instruction,
                                       &registers->current.user, &rd, &rn,
                                       &shifter_carry_out, &operand2);
      ArmSUB(&registers->current.user.gprs, rd, rn, operand2);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_SUB_I32:
      ArmOperandDataProcessingImmediate(next_instruction,
                                        &registers->current.user, &rd, &rn,
                                        &shifter_carry_out, &operand2);
      ArmSUB(&registers->current.user.gprs, rd, rn, operand2);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_SUBS:
      ArmOperandDataProcessingOperand2(next_instruction,
                                       &registers->current.user, &rd, &rn,
                                       &shifter_carry_out, &operand2);
      ArmSUBS(registers, rd, rn, operand2);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_SUBS_I32:
      ArmOperandDataProcessingImmediate(next_instruction,
                                        &registers->current.user, &rd, &rn,
                                        &shifter_carry_out, &operand2);
      ArmSUBS(registers, rd, rn, operand2);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_SWI:
      ArmSWI(registers);
      modified_pc = true;
      break;
    case ARM_OPCODE_SWP:
      ArmOperandSingleDataSwap(next_instruction, &rd, &rm, &rn);
      ArmSWP(&registers->current.user.gprs, memory, rd, rm, rn);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_SWPB:
      ArmOperandSingleDataSwap(next_instruction, &rd, &rm, &rn);
      ArmSWPB(&registers->current.user.gprs, memory, rd, rm, rn);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_TEQ:
      ArmOperandDataProcessingOperand2(next_instruction,
                                       &registers->current.user, &rd, &rn,
                                       &shifter_carry_out, &operand2);
      ArmTEQ(&registers->current.user, rn, operand2, shifter_carry_out);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_TEQ_I32:
      ArmOperandDataProcessingImmediate(next_instruction,
                                        &registers->current.user, &rd, &rn,
                                        &shifter_carry_out, &operand2);
      ArmTEQ(&registers->current.user, rn, operand2, shifter_carry_out);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_TST:
      ArmOperandDataProcessingOperand2(next_instruction,
                                       &registers->current.user, &rd, &rn,
                                       &shifter_carry_out, &operand2);
      ArmTST(&registers->current.user, rn, operand2, shifter_carry_out);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_TST_I32:
      ArmOperandDataProcessingImmediate(next_instruction,
                                        &registers->current.user, &rd, &rn,
                                        &shifter_carry_out, &operand2);
      ArmTST(&registers->current.user, rn, operand2, shifter_carry_out);
      modified_pc = (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_UMLAL:
      ArmOperandMultiplyLong(next_instruction, &rd_lsw, &rd_msw, &rm, &rs);
      ArmUMLAL(&registers->current.user.gprs, rd_lsw, rd_msw, rm, rs);
      modified_pc = (rd_lsw == REGISTER_R15 || rd_msw == REGISTER_R15);
      break;
    case ARM_OPCODE_UMLALS:
      ArmOperandMultiplyLong(next_instruction, &rd_lsw, &rd_msw, &rm, &rs);
      ArmUMLALS(&registers->current.user, rd_lsw, rd_msw, rm, rs);
      modified_pc = (rd_lsw == REGISTER_R15 || rd_msw == REGISTER_R15);
      break;
    case ARM_OPCODE_UMULL:
      ArmOperandMultiplyLong(next_instruction, &rd_lsw, &rd_msw, &rm, &rs);
      ArmUMULL(&registers->current.user.gprs, rd_lsw, rd_msw, rm, rs);
      modified_pc = (rd_lsw == REGISTER_R15 || rd_msw == REGISTER_R15);
      break;
    case ARM_OPCODE_UMULLS:
      ArmOperandMultiplyLong(next_instruction, &rd_lsw, &rd_msw, &rm, &rs);
      ArmUMULLS(&registers->current.user, rd_lsw, rd_msw, rm, rs);
      modified_pc = (rd_lsw == REGISTER_R15 || rd_msw == REGISTER_R15);
      break;
    default:
      assert(false);
    case ARM_OPCODE_UNDEF:
      ArmExceptionUND(registers);
      modified_pc = true;
      break;
  }

  return modified_pc;
}

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_ARM_EXECUTE_