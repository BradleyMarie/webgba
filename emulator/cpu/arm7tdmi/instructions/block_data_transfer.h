#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTMUCTIONS_BLOCK_DATA_TRANSFER_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTMUCTIONS_BLOCK_DATA_TRANSFER_

#include "emulator/cpu/arm7tdmi/arm7tdmi.h"
#include "emulator/memory.h"

void ArmLDMDA(ArmGeneralPurposeRegisters *registers, const Memory *memory,
              ArmRegisterIndex Rn, uint_fast16_t register_list);

void ArmLDMDB(ArmGeneralPurposeRegisters *registers, const Memory *memory,
              ArmRegisterIndex Rn, uint_fast16_t register_list);

void ArmLDMDAW(ArmGeneralPurposeRegisters *registers, const Memory *memory,
               ArmRegisterIndex Rn, uint_fast16_t register_list);

void ArmLDMDBW(ArmGeneralPurposeRegisters *registers, const Memory *memory,
               ArmRegisterIndex Rn, uint_fast16_t register_list);

void ArmLDMIA(ArmGeneralPurposeRegisters *registers, const Memory *memory,
              ArmRegisterIndex Rn, uint_fast16_t register_list);

void ArmLDMIB(ArmGeneralPurposeRegisters *registers, const Memory *memory,
              ArmRegisterIndex Rn, uint_fast16_t register_list);

void ArmLDMIAW(ArmGeneralPurposeRegisters *registers, const Memory *memory,
               ArmRegisterIndex Rn, uint_fast16_t register_list);

void ArmLDMIBW(ArmGeneralPurposeRegisters *registers, const Memory *memory,
               ArmRegisterIndex Rn, uint_fast16_t register_list);

void ArmLDMSDA(ArmAllRegisters *registers, const Memory *memory,
               ArmRegisterIndex Rn, uint_fast16_t register_list);

void ArmLDMSDB(ArmAllRegisters *registers, const Memory *memory,
               ArmRegisterIndex Rn, uint_fast16_t register_list);

void ArmLDMSDAW(ArmAllRegisters *registers, const Memory *memory,
                ArmRegisterIndex Rn, uint_fast16_t register_list);

void ArmLDMSDBW(ArmAllRegisters *registers, const Memory *memory,
                ArmRegisterIndex Rn, uint_fast16_t register_list);

void ArmLDMSIA(ArmAllRegisters *registers, const Memory *memory,
               ArmRegisterIndex Rn, uint_fast16_t register_list);

void ArmLDMSIB(ArmAllRegisters *registers, const Memory *memory,
               ArmRegisterIndex Rn, uint_fast16_t register_list);

void ArmLDMSIAW(ArmAllRegisters *registers, const Memory *memory,
                ArmRegisterIndex Rn, uint_fast16_t register_list);

void ArmLDMSIBW(ArmAllRegisters *registers, const Memory *memory,
                ArmRegisterIndex Rn, uint_fast16_t register_list);

void ArmSTMDA(const ArmGeneralPurposeRegisters *registers, Memory *memory,
              ArmRegisterIndex Rn, uint_fast16_t register_list);

void ArmSTMDB(const ArmGeneralPurposeRegisters *registers, Memory *memory,
              ArmRegisterIndex Rn, uint_fast16_t register_list);

void ArmSTMDAW(ArmGeneralPurposeRegisters *registers, Memory *memory,
               ArmRegisterIndex Rn, uint_fast16_t register_list);

void ArmSTMDBW(ArmGeneralPurposeRegisters *registers, Memory *memory,
               ArmRegisterIndex Rn, uint_fast16_t register_list);

void ArmSTMIA(const ArmGeneralPurposeRegisters *registers, Memory *memory,
              ArmRegisterIndex Rn, uint_fast16_t register_list);

void ArmSTMIB(const ArmGeneralPurposeRegisters *registers, Memory *memory,
              ArmRegisterIndex Rn, uint_fast16_t register_list);

void ArmSTMIAW(ArmGeneralPurposeRegisters *registers, Memory *memory,
               ArmRegisterIndex Rn, uint_fast16_t register_list);

void ArmSTMIBW(ArmGeneralPurposeRegisters *registers, Memory *memory,
               ArmRegisterIndex Rn, uint_fast16_t register_list);

void ArmSTMSDA(const ArmAllRegisters *registers, Memory *memory,
               ArmRegisterIndex Rn, uint_fast16_t register_list);

void ArmSTMSDB(const ArmAllRegisters *registers, Memory *memory,
               ArmRegisterIndex Rn, uint_fast16_t register_list);

void ArmSTMSDAW(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
                uint_fast16_t register_list);

void ArmSTMSDBW(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
                uint_fast16_t register_list);

void ArmSTMSIA(const ArmAllRegisters *registers, Memory *memory,
               ArmRegisterIndex Rn, uint_fast16_t register_list);

void ArmSTMSIB(const ArmAllRegisters *registers, Memory *memory,
               ArmRegisterIndex Rn, uint_fast16_t register_list);

void ArmSTMSIAW(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
                uint_fast16_t register_list);

void ArmSTMSIBW(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
                uint_fast16_t register_list);

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTMUCTIONS_BLOCK_DATA_TRANSFER_