#ifndef _WEBGBA_EMULATOR_CPU_CPU_
#define _WEBGBA_EMULATOR_CPU_CPU_

#include <stdbool.h>
#include <stdint.h>

#define MODE_USER 0x10
#define MODE_FIQ 0x11
#define MODE_IRQ 0x12
#define MODE_SUPERVISOR 0x13
#define MODE_ABORT 0x15
#define MODE_UNDEFINED 0x1B
#define MODE_SYSTEM 0x1F

#define MODE_USER_BANK_INDEX 0
#define MODE_FIQ_INDEX 1
#define MODE_IRQ_INDEX 2
#define MODE_SUPERVISOR_INDEX 3
#define MODE_ABORT_INDEX 4
#define MODE_UNDEFINED_INDEX 5
#define MODE_SYSTEM_BANK_INDEX 0

typedef union {
    struct {
        unsigned mode : 5;
        bool thumb : 1;
        bool fiq_disable : 1;
        bool irq_disable : 1;
        unsigned reserved : 20;
        bool overflow : 1;
        bool carry : 1;
        bool zero : 1;
        bool negative : 1;
    };
    uint32_t value;
} PSR;

typedef struct {
    union {
        uint32_t r0;
        uint32_t r1;
        uint32_t r2;
        uint32_t r3;
        uint32_t r4;
        uint32_t r5;
        uint32_t r6;
        uint32_t r7;
        uint32_t r8;
        uint32_t r9;
        uint32_t r10;
        uint32_t r11;
        uint32_t r12;
        union {
            uint32_t r13;
            uint32_t sp;
        };
        union {
            uint32_t r14;
            uint32_t lr;
        };
        union {
            uint32_t r15;
            uint32_t pc;
        };
        uint32_t gpsrs[16];
    };
    PSR cpsr;
    PSR spsr;
} Registers;

typedef struct {
    Registers registers;
    uint32_t banked_registers[6][7];
} CpuState;

CpuState CreateCpuState();

#endif // _WEBGBA_EMULATOR_CPU_CPU_