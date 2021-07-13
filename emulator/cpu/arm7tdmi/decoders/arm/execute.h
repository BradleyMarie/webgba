#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_ARM_EXECUTE_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_DECODERS_ARM_EXECUTE_

#include "emulator/cpu/arm7tdmi/decoders/arm/branch_link.h"
#include "emulator/cpu/arm7tdmi/decoders/arm/condition.h"
#include "emulator/cpu/arm7tdmi/decoders/arm/opcode.h"
#include "emulator/cpu/arm7tdmi/decoders/arm/operand.h"
#include "emulator/cpu/arm7tdmi/exceptions.h"
#include "emulator/cpu/arm7tdmi/instructions/block_data_transfer.h"
#include "emulator/cpu/arm7tdmi/instructions/branch.h"
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

  bool modified_pc, load_store_success;
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
      load_store_success = ArmLDMDA(registers, memory, rn, register_list);
      modified_pc =
          !load_store_success || !!((register_list >> REGISTER_R15) & 0x1u);
      break;
    case ARM_OPCODE_LDMDA_W:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      load_store_success = ArmLDMDAW(registers, memory, rn, register_list);
      modified_pc = !load_store_success ||
                    !!((register_list >> REGISTER_R15) & 0x1u) ||
                    (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDMDB:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      load_store_success = ArmLDMDB(registers, memory, rn, register_list);
      modified_pc =
          !load_store_success || !!((register_list >> REGISTER_R15) & 0x1u);
      break;
    case ARM_OPCODE_LDMDB_W:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      load_store_success = ArmLDMDBW(registers, memory, rn, register_list);
      modified_pc = !load_store_success ||
                    !!((register_list >> REGISTER_R15) & 0x1u) ||
                    (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDMIA:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      load_store_success = ArmLDMIA(registers, memory, rn, register_list);
      modified_pc =
          !load_store_success || !!((register_list >> REGISTER_R15) & 0x1u);
      break;
    case ARM_OPCODE_LDMIA_W:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      load_store_success = ArmLDMIAW(registers, memory, rn, register_list);
      modified_pc = !load_store_success ||
                    !!((register_list >> REGISTER_R15) & 0x1u) ||
                    (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDMIB:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      load_store_success = ArmLDMIB(registers, memory, rn, register_list);
      modified_pc =
          !load_store_success || !!((register_list >> REGISTER_R15) & 0x1u);
      break;
    case ARM_OPCODE_LDMIB_W:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      load_store_success = ArmLDMIBW(registers, memory, rn, register_list);
      modified_pc = !load_store_success ||
                    !!((register_list >> REGISTER_R15) & 0x1u) ||
                    (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDMSDA:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      load_store_success = ArmLDMSDA(registers, memory, rn, register_list);
      modified_pc =
          !load_store_success || !!((register_list >> REGISTER_R15) & 0x1u);
      break;
    case ARM_OPCODE_LDMSDA_W:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      load_store_success = ArmLDMSDAW(registers, memory, rn, register_list);
      modified_pc = !load_store_success ||
                    !!((register_list >> REGISTER_R15) & 0x1u) ||
                    (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDMSDB:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      load_store_success = ArmLDMSDB(registers, memory, rn, register_list);
      modified_pc =
          !load_store_success || !!((register_list >> REGISTER_R15) & 0x1u);
      break;
    case ARM_OPCODE_LDMSDB_W:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      load_store_success = ArmLDMSDBW(registers, memory, rn, register_list);
      modified_pc = !load_store_success ||
                    !!((register_list >> REGISTER_R15) & 0x1u) ||
                    (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDMSIA:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      load_store_success = ArmLDMSIA(registers, memory, rn, register_list);
      modified_pc =
          !load_store_success || !!((register_list >> REGISTER_R15) & 0x1u);
      break;
    case ARM_OPCODE_LDMSIA_W:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      load_store_success = ArmLDMSIAW(registers, memory, rn, register_list);
      modified_pc = !load_store_success ||
                    !!((register_list >> REGISTER_R15) & 0x1u) ||
                    (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDMSIB:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      load_store_success = ArmLDMSIB(registers, memory, rn, register_list);
      modified_pc =
          !load_store_success || !!((register_list >> REGISTER_R15) & 0x1u);
      break;
    case ARM_OPCODE_LDMSIB_W:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      load_store_success = ArmLDMSIBW(registers, memory, rn, register_list);
      modified_pc = !load_store_success ||
                    !!((register_list >> REGISTER_R15) & 0x1u) ||
                    (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDR_DAW:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      load_store_success = ArmLDR_DAW(registers, memory, rd, rn, offset_32);
      modified_pc =
          !load_store_success || (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDR_DAW_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      load_store_success = ArmLDR_DAW(registers, memory, rd, rn, offset_16);
      modified_pc =
          !load_store_success || (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDR_DB:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      load_store_success = ArmLDR_DB(registers, memory, rd, rn, offset_32);
      modified_pc = !load_store_success || (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_LDR_DB_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      load_store_success = ArmLDR_DB(registers, memory, rd, rn, offset_16);
      modified_pc = !load_store_success || (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_LDR_DBW:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      load_store_success = ArmLDR_DBW(registers, memory, rd, rn, offset_32);
      modified_pc =
          !load_store_success || (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDR_DBW_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      load_store_success = ArmLDR_DBW(registers, memory, rd, rn, offset_16);
      modified_pc =
          !load_store_success || (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDR_IAW:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      load_store_success = ArmLDR_IAW(registers, memory, rd, rn, offset_32);
      modified_pc =
          !load_store_success || (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDR_IAW_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      load_store_success = ArmLDR_IAW(registers, memory, rd, rn, offset_16);
      modified_pc =
          !load_store_success || (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDR_IB:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      load_store_success = ArmLDR_IB(registers, memory, rd, rn, offset_32);
      modified_pc = !load_store_success || (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_LDR_IB_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      load_store_success = ArmLDR_IB(registers, memory, rd, rn, offset_16);
      modified_pc = !load_store_success || (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_LDR_IBW:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      load_store_success = ArmLDR_IBW(registers, memory, rd, rn, offset_32);
      modified_pc =
          !load_store_success || (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDR_IBW_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      load_store_success = ArmLDR_IBW(registers, memory, rd, rn, offset_16);
      modified_pc =
          !load_store_success || (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRB_DAW:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      load_store_success = ArmLDRB_DAW(registers, memory, rd, rn, offset_32);
      modified_pc =
          !load_store_success || (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRB_DAW_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      load_store_success = ArmLDRB_DAW(registers, memory, rd, rn, offset_16);
      modified_pc =
          !load_store_success || (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRB_DB:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      load_store_success = ArmLDRB_DB(registers, memory, rd, rn, offset_32);
      modified_pc = !load_store_success || (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRB_DB_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      load_store_success = ArmLDRB_DB(registers, memory, rd, rn, offset_16);
      modified_pc = !load_store_success || (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRB_DBW:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      load_store_success = ArmLDRB_DBW(registers, memory, rd, rn, offset_32);
      modified_pc =
          !load_store_success || (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRB_DBW_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      load_store_success = ArmLDR_DBW(registers, memory, rd, rn, offset_16);
      modified_pc =
          !load_store_success || (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRB_IAW:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      load_store_success = ArmLDRB_IAW(registers, memory, rd, rn, offset_32);
      modified_pc =
          !load_store_success || (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRB_IAW_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      load_store_success = ArmLDRB_IAW(registers, memory, rd, rn, offset_16);
      modified_pc =
          !load_store_success || (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRB_IB:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      load_store_success = ArmLDRB_IB(registers, memory, rd, rn, offset_32);
      modified_pc = !load_store_success || (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRB_IB_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      load_store_success = ArmLDRB_IB(registers, memory, rd, rn, offset_16);
      modified_pc = !load_store_success || (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRB_IBW:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      load_store_success = ArmLDRB_IBW(registers, memory, rd, rn, offset_32);
      modified_pc =
          !load_store_success || (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRB_IBW_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      load_store_success = ArmLDRB_IBW(registers, memory, rd, rn, offset_16);
      modified_pc =
          !load_store_success || (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRBT_DAW:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      load_store_success = ArmLDRBT_DAW(registers, memory, rd, rn, offset_32);
      modified_pc =
          !load_store_success || (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRBT_DAW_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      load_store_success = ArmLDRBT_DAW(registers, memory, rd, rn, offset_16);
      modified_pc =
          !load_store_success || (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRBT_IAW:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      load_store_success = ArmLDRBT_IAW(registers, memory, rd, rn, offset_32);
      modified_pc =
          !load_store_success || (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRBT_IAW_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      load_store_success = ArmLDRBT_IAW(registers, memory, rd, rn, offset_16);
      modified_pc =
          !load_store_success || (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRH_DAW:
      ArmOperandHalfwordAddressMode(next_instruction,
                                    &registers->current.user.gprs, &rd, &rn,
                                    &offset_32);
      load_store_success = ArmLDRH_DAW(registers, memory, rd, rn, offset_32);
      modified_pc =
          !load_store_success || (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRH_DAW_I8:
      ArmOperandHalfwordImmediate(next_instruction, &rd, &rn, &offset_8);
      load_store_success = ArmLDRH_DAW(registers, memory, rd, rn, offset_8);
      modified_pc =
          !load_store_success || (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRH_DB:
      ArmOperandHalfwordAddressMode(next_instruction,
                                    &registers->current.user.gprs, &rd, &rn,
                                    &offset_32);
      load_store_success = ArmLDRH_DB(registers, memory, rd, rn, offset_32);
      modified_pc = !load_store_success || (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRH_DB_I8:
      ArmOperandHalfwordImmediate(next_instruction, &rd, &rn, &offset_8);
      load_store_success = ArmLDRH_DB(registers, memory, rd, rn, offset_8);
      modified_pc = !load_store_success || (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRH_DBW:
      ArmOperandHalfwordAddressMode(next_instruction,
                                    &registers->current.user.gprs, &rd, &rn,
                                    &offset_32);
      load_store_success = ArmLDRH_DBW(registers, memory, rd, rn, offset_32);
      modified_pc =
          !load_store_success || (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRH_DBW_I8:
      ArmOperandHalfwordImmediate(next_instruction, &rd, &rn, &offset_8);
      load_store_success = ArmLDRH_DBW(registers, memory, rd, rn, offset_8);
      modified_pc =
          !load_store_success || (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRH_IAW:
      ArmOperandHalfwordAddressMode(next_instruction,
                                    &registers->current.user.gprs, &rd, &rn,
                                    &offset_32);
      load_store_success = ArmLDRH_IAW(registers, memory, rd, rn, offset_32);
      modified_pc =
          !load_store_success || (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRH_IAW_I8:
      ArmOperandHalfwordImmediate(next_instruction, &rd, &rn, &offset_8);
      load_store_success = ArmLDRH_IAW(registers, memory, rd, rn, offset_8);
      modified_pc =
          !load_store_success || (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRH_IB:
      ArmOperandHalfwordAddressMode(next_instruction,
                                    &registers->current.user.gprs, &rd, &rn,
                                    &offset_32);
      load_store_success = ArmLDRH_IB(registers, memory, rd, rn, offset_32);
      modified_pc = !load_store_success || (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRH_IB_I8:
      ArmOperandHalfwordImmediate(next_instruction, &rd, &rn, &offset_8);
      load_store_success = ArmLDRH_IB(registers, memory, rd, rn, offset_8);
      modified_pc = !load_store_success || (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRH_IBW:
      ArmOperandHalfwordAddressMode(next_instruction,
                                    &registers->current.user.gprs, &rd, &rn,
                                    &offset_32);
      load_store_success = ArmLDRH_IBW(registers, memory, rd, rn, offset_32);
      modified_pc =
          !load_store_success || (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRH_IBW_I8:
      ArmOperandHalfwordImmediate(next_instruction, &rd, &rn, &offset_8);
      load_store_success = ArmLDRH_IBW(registers, memory, rd, rn, offset_8);
      modified_pc =
          !load_store_success || (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRSB_DAW:
      ArmOperandHalfwordAddressMode(next_instruction,
                                    &registers->current.user.gprs, &rd, &rn,
                                    &offset_32);
      load_store_success = ArmLDRSB_DAW(registers, memory, rd, rn, offset_32);
      modified_pc =
          !load_store_success || (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRSB_DAW_I8:
      ArmOperandHalfwordImmediate(next_instruction, &rd, &rn, &offset_8);
      load_store_success = ArmLDRSB_DAW(registers, memory, rd, rn, offset_8);
      modified_pc =
          !load_store_success || (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRSB_DB:
      ArmOperandHalfwordAddressMode(next_instruction,
                                    &registers->current.user.gprs, &rd, &rn,
                                    &offset_32);
      load_store_success = ArmLDRSB_DB(registers, memory, rd, rn, offset_32);
      modified_pc = !load_store_success || (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRSB_DB_I8:
      ArmOperandHalfwordImmediate(next_instruction, &rd, &rn, &offset_8);
      load_store_success = ArmLDRSB_DB(registers, memory, rd, rn, offset_8);
      modified_pc = !load_store_success || (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRSB_DBW:
      ArmOperandHalfwordAddressMode(next_instruction,
                                    &registers->current.user.gprs, &rd, &rn,
                                    &offset_32);
      load_store_success = ArmLDRSB_DBW(registers, memory, rd, rn, offset_32);
      modified_pc =
          !load_store_success || (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRSB_DBW_I8:
      ArmOperandHalfwordImmediate(next_instruction, &rd, &rn, &offset_8);
      load_store_success = ArmLDRSB_DBW(registers, memory, rd, rn, offset_8);
      modified_pc =
          !load_store_success || (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRSB_IAW:
      ArmOperandHalfwordAddressMode(next_instruction,
                                    &registers->current.user.gprs, &rd, &rn,
                                    &offset_32);
      load_store_success = ArmLDRSB_IAW(registers, memory, rd, rn, offset_32);
      modified_pc =
          !load_store_success || (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRSB_IAW_I8:
      ArmOperandHalfwordImmediate(next_instruction, &rd, &rn, &offset_8);
      load_store_success = ArmLDRSB_IAW(registers, memory, rd, rn, offset_8);
      modified_pc =
          !load_store_success || (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRSB_IB:
      ArmOperandHalfwordAddressMode(next_instruction,
                                    &registers->current.user.gprs, &rd, &rn,
                                    &offset_32);
      load_store_success = ArmLDRSB_IB(registers, memory, rd, rn, offset_32);
      modified_pc = !load_store_success || (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRSB_IB_I8:
      ArmOperandHalfwordImmediate(next_instruction, &rd, &rn, &offset_8);
      load_store_success = ArmLDRSB_IB(registers, memory, rd, rn, offset_8);
      modified_pc = !load_store_success || (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRSB_IBW:
      ArmOperandHalfwordAddressMode(next_instruction,
                                    &registers->current.user.gprs, &rd, &rn,
                                    &offset_32);
      load_store_success = ArmLDRSB_IBW(registers, memory, rd, rn, offset_32);
      modified_pc =
          !load_store_success || (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRSB_IBW_I8:
      ArmOperandHalfwordImmediate(next_instruction, &rd, &rn, &offset_8);
      load_store_success = ArmLDRSB_IBW(registers, memory, rd, rn, offset_8);
      modified_pc =
          !load_store_success || (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRSH_DAW:
      ArmOperandHalfwordAddressMode(next_instruction,
                                    &registers->current.user.gprs, &rd, &rn,
                                    &offset_32);
      load_store_success = ArmLDRSH_DAW(registers, memory, rd, rn, offset_32);
      modified_pc =
          !load_store_success || (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRSH_DAW_I8:
      ArmOperandHalfwordImmediate(next_instruction, &rd, &rn, &offset_8);
      load_store_success = ArmLDRSH_DAW(registers, memory, rd, rn, offset_8);
      modified_pc =
          !load_store_success || (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRSH_DB:
      ArmOperandHalfwordAddressMode(next_instruction,
                                    &registers->current.user.gprs, &rd, &rn,
                                    &offset_32);
      load_store_success = ArmLDRSH_DB(registers, memory, rd, rn, offset_32);
      modified_pc = !load_store_success || (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRSH_DB_I8:
      ArmOperandHalfwordImmediate(next_instruction, &rd, &rn, &offset_8);
      load_store_success = ArmLDRSH_DB(registers, memory, rd, rn, offset_8);
      modified_pc = !load_store_success || (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRSH_DBW:
      ArmOperandHalfwordAddressMode(next_instruction,
                                    &registers->current.user.gprs, &rd, &rn,
                                    &offset_32);
      load_store_success = ArmLDRSH_DBW(registers, memory, rd, rn, offset_32);
      modified_pc =
          !load_store_success || (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRSH_DBW_I8:
      ArmOperandHalfwordImmediate(next_instruction, &rd, &rn, &offset_8);
      load_store_success = ArmLDRSH_DBW(registers, memory, rd, rn, offset_8);
      modified_pc =
          !load_store_success || (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRSH_IAW:
      ArmOperandHalfwordAddressMode(next_instruction,
                                    &registers->current.user.gprs, &rd, &rn,
                                    &offset_32);
      load_store_success = ArmLDRSH_IAW(registers, memory, rd, rn, offset_32);
      modified_pc =
          !load_store_success || (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRSH_IAW_I8:
      ArmOperandHalfwordImmediate(next_instruction, &rd, &rn, &offset_8);
      load_store_success = ArmLDRSH_IAW(registers, memory, rd, rn, offset_8);
      modified_pc =
          !load_store_success || (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRSH_IB:
      ArmOperandHalfwordAddressMode(next_instruction,
                                    &registers->current.user.gprs, &rd, &rn,
                                    &offset_32);
      load_store_success = ArmLDRSH_IB(registers, memory, rd, rn, offset_32);
      modified_pc = !load_store_success || (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRSH_IB_I8:
      ArmOperandHalfwordImmediate(next_instruction, &rd, &rn, &offset_8);
      load_store_success = ArmLDRSH_IB(registers, memory, rd, rn, offset_8);
      modified_pc = !load_store_success || (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRSH_IBW:
      ArmOperandHalfwordAddressMode(next_instruction,
                                    &registers->current.user.gprs, &rd, &rn,
                                    &offset_32);
      load_store_success = ArmLDRSH_IBW(registers, memory, rd, rn, offset_32);
      modified_pc =
          !load_store_success || (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRSH_IBW_I8:
      ArmOperandHalfwordImmediate(next_instruction, &rd, &rn, &offset_8);
      load_store_success = ArmLDRSH_IBW(registers, memory, rd, rn, offset_8);
      modified_pc =
          !load_store_success || (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRT_DAW:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      load_store_success = ArmLDRT_DAW(registers, memory, rd, rn, offset_32);
      modified_pc =
          !load_store_success || (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRT_DAW_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      load_store_success = ArmLDRT_DAW(registers, memory, rd, rn, offset_16);
      modified_pc =
          !load_store_success || (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRT_IAW:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      load_store_success = ArmLDRT_IAW(registers, memory, rd, rn, offset_32);
      modified_pc =
          !load_store_success || (rd == REGISTER_R15 || rn == REGISTER_R15);
      break;
    case ARM_OPCODE_LDRT_IAW_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      load_store_success = ArmLDRT_IAW(registers, memory, rd, rn, offset_16);
      modified_pc =
          !load_store_success || (rd == REGISTER_R15 || rn == REGISTER_R15);
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
      load_store_success = ArmSTMDA(registers, memory, rn, register_list);
      modified_pc = !load_store_success;
      break;
    case ARM_OPCODE_STMDA_W:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      load_store_success = ArmSTMDAW(registers, memory, rn, register_list);
      modified_pc = !load_store_success || (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STMDB:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      load_store_success = ArmSTMDB(registers, memory, rn, register_list);
      modified_pc = !load_store_success;
      break;
    case ARM_OPCODE_STMDB_W:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      load_store_success = ArmSTMDBW(registers, memory, rn, register_list);
      modified_pc = !load_store_success || (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STMIA:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      load_store_success = ArmSTMIA(registers, memory, rn, register_list);
      modified_pc = !load_store_success;
      break;
    case ARM_OPCODE_STMIA_W:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      load_store_success = ArmSTMIAW(registers, memory, rn, register_list);
      modified_pc = !load_store_success || (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STMIB:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      load_store_success = ArmSTMIB(registers, memory, rn, register_list);
      modified_pc = !load_store_success;
      break;
    case ARM_OPCODE_STMIB_W:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      load_store_success = ArmSTMIBW(registers, memory, rn, register_list);
      modified_pc = !load_store_success || (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STMSDA:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      load_store_success = ArmSTMSDA(registers, memory, rn, register_list);
      modified_pc = !load_store_success;
      break;
    case ARM_OPCODE_STMSDA_W:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      load_store_success = ArmSTMSDAW(registers, memory, rn, register_list);
      modified_pc = !load_store_success || (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STMSDB:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      load_store_success = ArmSTMSDB(registers, memory, rn, register_list);
      modified_pc = !load_store_success;
      break;
    case ARM_OPCODE_STMSDB_W:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      load_store_success = ArmSTMSDBW(registers, memory, rn, register_list);
      modified_pc = !load_store_success || (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STMSIA:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      load_store_success = ArmSTMSIA(registers, memory, rn, register_list);
      modified_pc = !load_store_success;
      break;
    case ARM_OPCODE_STMSIA_W:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      load_store_success = ArmSTMSIAW(registers, memory, rn, register_list);
      modified_pc = !load_store_success || (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STMSIB:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      load_store_success = ArmSTMSIB(registers, memory, rn, register_list);
      modified_pc = !load_store_success;
      break;
    case ARM_OPCODE_STMSIB_W:
      ArmOperandRegisterAndRegisterList(next_instruction, &rn, &register_list);
      load_store_success = ArmSTMSIBW(registers, memory, rn, register_list);
      modified_pc = !load_store_success || (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STR_DAW:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      load_store_success = ArmSTR_DAW(registers, memory, rd, rn, offset_32);
      modified_pc = !load_store_success || (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STR_DAW_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      load_store_success = ArmSTR_DAW(registers, memory, rd, rn, offset_16);
      modified_pc = !load_store_success || (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STR_DB:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      load_store_success = ArmSTR_DB(registers, memory, rd, rn, offset_32);
      modified_pc = !load_store_success;
      break;
    case ARM_OPCODE_STR_DB_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      load_store_success = ArmSTR_DB(registers, memory, rd, rn, offset_16);
      modified_pc = !load_store_success;
      break;
    case ARM_OPCODE_STR_DBW:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      load_store_success = ArmSTR_DBW(registers, memory, rd, rn, offset_32);
      modified_pc = !load_store_success || (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STR_DBW_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      load_store_success = ArmSTR_DBW(registers, memory, rd, rn, offset_16);
      modified_pc = !load_store_success || (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STR_IAW:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      load_store_success = ArmSTR_IAW(registers, memory, rd, rn, offset_32);
      modified_pc = !load_store_success || (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STR_IAW_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      load_store_success = ArmSTR_IAW(registers, memory, rd, rn, offset_16);
      modified_pc = !load_store_success || (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STR_IB:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      load_store_success = ArmSTR_IB(registers, memory, rd, rn, offset_32);
      modified_pc = !load_store_success;
      break;
    case ARM_OPCODE_STR_IB_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      load_store_success = ArmSTR_IB(registers, memory, rd, rn, offset_16);
      modified_pc = !load_store_success;
      break;
    case ARM_OPCODE_STR_IBW:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      load_store_success = ArmSTR_IBW(registers, memory, rd, rn, offset_32);
      modified_pc = !load_store_success || (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STR_IBW_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      load_store_success = ArmSTR_IBW(registers, memory, rd, rn, offset_16);
      modified_pc = !load_store_success || (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STRB_DAW:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      load_store_success = ArmSTRB_DAW(registers, memory, rd, rn, offset_32);
      modified_pc = !load_store_success || (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STRB_DAW_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      load_store_success = ArmSTRB_DAW(registers, memory, rd, rn, offset_16);
      modified_pc = !load_store_success || (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STRB_DB:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      load_store_success = ArmSTRB_DB(registers, memory, rd, rn, offset_32);
      modified_pc = !load_store_success;
      break;
    case ARM_OPCODE_STRB_DB_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      load_store_success = ArmSTRB_DB(registers, memory, rd, rn, offset_16);
      modified_pc = !load_store_success;
      break;
    case ARM_OPCODE_STRB_DBW:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      load_store_success = ArmSTRB_DBW(registers, memory, rd, rn, offset_32);
      modified_pc = !load_store_success || (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STRB_DBW_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      load_store_success = ArmSTR_DBW(registers, memory, rd, rn, offset_16);
      modified_pc = !load_store_success || (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STRB_IAW:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      load_store_success = ArmSTRB_IAW(registers, memory, rd, rn, offset_32);
      modified_pc = !load_store_success || (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STRB_IAW_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      load_store_success = ArmSTRB_IAW(registers, memory, rd, rn, offset_16);
      modified_pc = !load_store_success || (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STRB_IB:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      load_store_success = ArmSTRB_IB(registers, memory, rd, rn, offset_32);
      modified_pc = !load_store_success;
      break;
    case ARM_OPCODE_STRB_IB_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      load_store_success = ArmSTRB_IB(registers, memory, rd, rn, offset_16);
      modified_pc = !load_store_success;
      break;
    case ARM_OPCODE_STRB_IBW:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      load_store_success = ArmSTRB_IBW(registers, memory, rd, rn, offset_32);
      modified_pc = !load_store_success || (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STRB_IBW_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      load_store_success = ArmSTRB_IBW(registers, memory, rd, rn, offset_16);
      modified_pc = !load_store_success || (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STRBT_DAW:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      load_store_success = ArmSTRBT_DAW(registers, memory, rd, rn, offset_32);
      modified_pc = !load_store_success || (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STRBT_DAW_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      load_store_success = ArmSTRBT_DAW(registers, memory, rd, rn, offset_16);
      modified_pc = !load_store_success || (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STRBT_IAW:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      load_store_success = ArmSTRBT_IAW(registers, memory, rd, rn, offset_32);
      modified_pc = !load_store_success || (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STRBT_IAW_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      load_store_success = ArmSTRBT_IAW(registers, memory, rd, rn, offset_16);
      modified_pc = !load_store_success || (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STRH_DAW:
      ArmOperandHalfwordAddressMode(next_instruction,
                                    &registers->current.user.gprs, &rd, &rn,
                                    &offset_32);
      load_store_success = ArmSTRH_DAW(registers, memory, rd, rn, offset_32);
      modified_pc = !load_store_success || (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STRH_DAW_I8:
      ArmOperandHalfwordImmediate(next_instruction, &rd, &rn, &offset_8);
      load_store_success = ArmSTRH_DAW(registers, memory, rd, rn, offset_8);
      modified_pc = !load_store_success || (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STRH_DB:
      ArmOperandHalfwordAddressMode(next_instruction,
                                    &registers->current.user.gprs, &rd, &rn,
                                    &offset_32);
      load_store_success = ArmSTRH_DB(registers, memory, rd, rn, offset_32);
      modified_pc = !load_store_success;
      break;
    case ARM_OPCODE_STRH_DB_I8:
      ArmOperandHalfwordImmediate(next_instruction, &rd, &rn, &offset_8);
      load_store_success = ArmSTRH_DB(registers, memory, rd, rn, offset_8);
      modified_pc = !load_store_success;
      break;
    case ARM_OPCODE_STRH_DBW:
      ArmOperandHalfwordAddressMode(next_instruction,
                                    &registers->current.user.gprs, &rd, &rn,
                                    &offset_32);
      load_store_success = ArmSTRH_DBW(registers, memory, rd, rn, offset_32);
      modified_pc = !load_store_success || (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STRH_DBW_I8:
      ArmOperandHalfwordImmediate(next_instruction, &rd, &rn, &offset_8);
      load_store_success = ArmSTRH_DBW(registers, memory, rd, rn, offset_8);
      modified_pc = !load_store_success || (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STRH_IAW:
      ArmOperandHalfwordAddressMode(next_instruction,
                                    &registers->current.user.gprs, &rd, &rn,
                                    &offset_32);
      load_store_success = ArmSTRH_IAW(registers, memory, rd, rn, offset_32);
      modified_pc = !load_store_success || (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STRH_IAW_I8:
      ArmOperandHalfwordImmediate(next_instruction, &rd, &rn, &offset_8);
      load_store_success = ArmSTRH_IAW(registers, memory, rd, rn, offset_8);
      modified_pc = !load_store_success || (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STRH_IB:
      ArmOperandHalfwordAddressMode(next_instruction,
                                    &registers->current.user.gprs, &rd, &rn,
                                    &offset_32);
      load_store_success = ArmSTRH_IB(registers, memory, rd, rn, offset_32);
      modified_pc = !load_store_success;
      break;
    case ARM_OPCODE_STRH_IB_I8:
      ArmOperandHalfwordImmediate(next_instruction, &rd, &rn, &offset_8);
      load_store_success = ArmSTRH_IB(registers, memory, rd, rn, offset_8);
      modified_pc = !load_store_success;
      break;
    case ARM_OPCODE_STRH_IBW:
      ArmOperandHalfwordAddressMode(next_instruction,
                                    &registers->current.user.gprs, &rd, &rn,
                                    &offset_32);
      load_store_success = ArmSTRH_IBW(registers, memory, rd, rn, offset_32);
      modified_pc = !load_store_success || (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STRH_IBW_I8:
      ArmOperandHalfwordImmediate(next_instruction, &rd, &rn, &offset_8);
      load_store_success = ArmSTRH_IBW(registers, memory, rd, rn, offset_8);
      modified_pc = !load_store_success || (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STRT_DAW:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      load_store_success = ArmSTRT_DAW(registers, memory, rd, rn, offset_32);
      modified_pc = !load_store_success || (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STRT_DAW_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      load_store_success = ArmSTRT_DAW(registers, memory, rd, rn, offset_16);
      modified_pc = !load_store_success || (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STRT_IAW:
      ArmOperandLoadStoreAddressMode(next_instruction, &registers->current.user,
                                     &rd, &rn, &offset_32);
      load_store_success = ArmSTRT_IAW(registers, memory, rd, rn, offset_32);
      modified_pc = !load_store_success || (rn == REGISTER_R15);
      break;
    case ARM_OPCODE_STRT_IAW_I12:
      ArmOperandLoadStoreImmediate(next_instruction, &rd, &rn, &offset_16);
      load_store_success = ArmSTRT_IAW(registers, memory, rd, rn, offset_16);
      modified_pc = !load_store_success || (rn == REGISTER_R15);
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
      load_store_success = ArmSWP(registers, memory, rd, rm, rn);
      modified_pc = !load_store_success || (rd == REGISTER_R15);
      break;
    case ARM_OPCODE_SWPB:
      ArmOperandSingleDataSwap(next_instruction, &rd, &rm, &rn);
      load_store_success = ArmSWPB(registers, memory, rd, rm, rn);
      modified_pc = !load_store_success || (rd == REGISTER_R15);
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