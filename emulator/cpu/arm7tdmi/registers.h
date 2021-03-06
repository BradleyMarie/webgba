#ifndef _WEBGBA_EMULATOR_CPU_ARM7TDMI_REGISTERS_
#define _WEBGBA_EMULATOR_CPU_ARM7TDMI_REGISTERS_

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

typedef enum {
  REGISTER_R0 = 0,
  REGISTER_R1 = 1,
  REGISTER_R2 = 2,
  REGISTER_R3 = 3,
  REGISTER_R4 = 4,
  REGISTER_R5 = 5,
  REGISTER_R6 = 6,
  REGISTER_R7 = 7,
  REGISTER_R8 = 8,
  REGISTER_R9 = 9,
  REGISTER_R10 = 10,
  REGISTER_R11 = 11,
  REGISTER_R12 = 12,
  REGISTER_R13 = 13,
  REGISTER_SP = 13,
  REGISTER_R14 = 14,
  REGISTER_LR = 14,
  REGISTER_R15 = 15,
  REGISTER_PC = 15
} ArmRegisterIndex;

typedef union {
  struct {
    union {
      uint32_t r0;
      int32_t r0_s;
    };
    union {
      uint32_t r1;
      int32_t r1_s;
    };
    union {
      uint32_t r2;
      int32_t r2_s;
    };
    union {
      uint32_t r3;
      int32_t r3_s;
    };
    union {
      uint32_t r4;
      int32_t r4_s;
    };
    union {
      uint32_t r5;
      int32_t r5_s;
    };
    union {
      uint32_t r6;
      int32_t r6_s;
    };
    union {
      uint32_t r7;
      int32_t r7_s;
    };
    union {
      uint32_t r8;
      int32_t r8_s;
    };
    union {
      uint32_t r9;
      int32_t r9_s;
    };
    union {
      uint32_t r10;
      int32_t r10_s;
    };
    union {
      uint32_t r11;
      int32_t r11_s;
    };
    union {
      uint32_t r12;
      int32_t r12_s;
    };
    union {
      uint32_t r13;
      int32_t r13_s;
      uint32_t sp;
    };
    union {
      uint32_t r14;
      int32_t r14_s;
      uint32_t lr;
    };
    union {
      uint32_t r15;
      int32_t r15_s;
      uint32_t pc;
    };
  };
  uint32_t gprs[16];
  int32_t gprs_s[16];
} ArmGeneralPurposeRegisters;

#define MODE_USR 0x10u
#define MODE_FIQ 0x11u
#define MODE_IRQ 0x12u
#define MODE_SVC 0x13u
#define MODE_ABT 0x15u
#define MODE_UND 0x1Bu
#define MODE_SYS 0x1Fu

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
} ArmProgramStatusRegister;

typedef struct {
  ArmGeneralPurposeRegisters gprs;
  ArmProgramStatusRegister cpsr;
} ArmUserRegisters;

typedef struct {
  ArmUserRegisters user;
  ArmProgramStatusRegister spsr;
} ArmPrivilegedRegisters;

// Non-architectural state used to control execution
typedef struct {
  union {
    struct {
      bool thumb : 1;
      bool irq : 1;
      bool fiq : 1;
      bool rst : 1;
    };
    uint32_t mode;
  };
  bool irq_raised;
  bool fiq_raised;
} ArmExecutionControl;

typedef struct {
  ArmPrivilegedRegisters current;
  uint32_t banked_splrs[6][2];
  uint32_t banked_fiq_gprs[5];
  ArmProgramStatusRegister banked_spsrs[6];
  ArmExecutionControl execution_control;
} ArmAllRegisters;

static inline uint32_t ArmNextInstruction(const ArmAllRegisters* registers) {
  return registers->current.user.gprs.pc -
         (4u >> registers->current.user.cpsr.thumb);
}

static inline uint32_t ArmCurrentInstruction(const ArmAllRegisters* registers) {
  return registers->current.user.gprs.pc -
         (8u >> registers->current.user.cpsr.thumb);
}

static inline void ArmAdvanceProgramCounter(ArmAllRegisters* registers) {
  registers->current.user.gprs.pc += 4u >> registers->current.user.cpsr.thumb;
}

static inline void ArmLoadProgramCounter(ArmAllRegisters* registers,
                                         uint32_t address) {
  assert(registers->current.user.cpsr.thumb || (address & 0x2u) == 0u);
  assert((address & 0x1u) == 0u);
  registers->current.user.gprs.pc =
      address + (8u >> registers->current.user.cpsr.thumb);
}

// This function only needs to be used if it cannot be guaranteed that the
// program counter might be loaded by an instruction
static inline void ArmLoadGPSR(ArmAllRegisters* registers,
                               ArmRegisterIndex register_index,
                               uint32_t value) {
  if (register_index != REGISTER_PC) {
    registers->current.user.gprs.gprs[register_index] = value;
  } else {
    uint_fast8_t shift_size = 2u >> registers->current.user.cpsr.thumb;
    ArmLoadProgramCounter(registers, (value >> shift_size) << shift_size);
  }
}

void ArmLoadCPSR(ArmAllRegisters* registers, ArmProgramStatusRegister cpsr);

#endif  // _WEBGBA_EMULATOR_CPU_ARM7TDMI_REGISTERS_