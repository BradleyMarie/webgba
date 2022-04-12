#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTMUCTIONS_BLOCK_DATA_TRANSFER_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTMUCTIONS_BLOCK_DATA_TRANSFER_

#include "emulator/cpu/arm7tdmi/memory.h"
#include "emulator/cpu/arm7tdmi/registers.h"

void ArmLDMDA(ArmAllRegisters *registers, const Memory *memory,
              ArmRegisterIndex Rn, uint_fast16_t register_list);

void ArmLDMDB(ArmAllRegisters *registers, const Memory *memory,
              ArmRegisterIndex Rn, uint_fast16_t register_list);

void ArmLDMDAW(ArmAllRegisters *registers, const Memory *memory,
               ArmRegisterIndex Rn, uint_fast16_t register_list);

void ArmLDMDBW(ArmAllRegisters *registers, const Memory *memory,
               ArmRegisterIndex Rn, uint_fast16_t register_list);

void ArmLDMIA(ArmAllRegisters *registers, const Memory *memory,
              ArmRegisterIndex Rn, uint_fast16_t register_list);

void ArmLDMIB(ArmAllRegisters *registers, const Memory *memory,
              ArmRegisterIndex Rn, uint_fast16_t register_list);

void ArmLDMIAW(ArmAllRegisters *registers, const Memory *memory,
               ArmRegisterIndex Rn, uint_fast16_t register_list);

void ArmLDMIBW(ArmAllRegisters *registers, const Memory *memory,
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

void ArmSTMDA(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
              uint_fast16_t register_list);

void ArmSTMDB(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
              uint_fast16_t register_list);

void ArmSTMDAW(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
               uint_fast16_t register_list);

void ArmSTMDBW(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
               uint_fast16_t register_list);

void ArmSTMIA(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
              uint_fast16_t register_list);

void ArmSTMIB(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
              uint_fast16_t register_list);

void ArmSTMIAW(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
               uint_fast16_t register_list);

void ArmSTMIBW(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
               uint_fast16_t register_list);

void ArmSTMSDA(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
               uint_fast16_t register_list);

void ArmSTMSDB(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
               uint_fast16_t register_list);

void ArmSTMSDAW(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
                uint_fast16_t register_list);

void ArmSTMSDBW(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
                uint_fast16_t register_list);

void ArmSTMSIA(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
               uint_fast16_t register_list);

void ArmSTMSIB(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
               uint_fast16_t register_list);

void ArmSTMSIAW(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
                uint_fast16_t register_list);

void ArmSTMSIBW(ArmAllRegisters *registers, Memory *memory, ArmRegisterIndex Rn,
                uint_fast16_t register_list);

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_INSTMUCTIONS_BLOCK_DATA_TRANSFER_