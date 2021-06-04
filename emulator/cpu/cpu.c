#include "emulator/cpu/cpu.h"

#include <string.h>

CpuState CreateCpuState() {
    CpuState result;
    memset(&result, 0, sizeof(CpuState));
    result.registers.cpsr.mode = MODE_SYSTEM;
    return result;
}