#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_MEMORY_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_MEMORY_

#include "emulator/memory/memory.h"

bool ArmLoad32LEWithRotation(const Memory *memory, uint32_t address,
                             uint32_t *value);
bool ArmLoad32LE(const Memory *memory, uint32_t address, uint32_t *value);
bool ArmLoad16LEWithRotation(const Memory *memory, uint32_t address,
                             uint32_t *value);

bool ArmLoad32SLEWithRotation(const Memory *memory, uint32_t address,
                              int32_t *value);
bool ArmLoad32SLE(const Memory *memory, uint32_t address, int32_t *value);
bool ArmLoad16SLEWithRotation(const Memory *memory, uint32_t address,
                              int32_t *value);

bool ArmStore32LE(Memory *memory, uint32_t address, uint32_t value);
bool ArmStore16LE(Memory *memory, uint32_t address, uint16_t value);

bool ArmStore32SLE(Memory *memory, uint32_t address, int32_t value);
bool ArmStore16SLE(Memory *memory, uint32_t address, int16_t value);

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_MEMORY_