extern "C" {
#include "emulator/cpu/arm7tdmi/exceptions.h"
}

#include <cstring>
#include <tuple>

#include "googletest/include/gtest/gtest.h"

#define USR_BANK_INDEX 0u
#define SYS_BANK_INDEX 0u
#define FIQ_BANK_INDEX 1u
#define IRQ_BANK_INDEX 2u
#define SVC_BANK_INDEX 3u
#define ABT_BANK_INDEX 4u
#define UND_BANK_INDEX 5u

#define BANKED_R8_INDEX 4u
#define BANKED_R9_INDEX 3u
#define BANKED_R10_INDEX 2u
#define BANKED_R11_INDEX 1u
#define BANKED_R12_INDEX 0u
#define BANKED_R13_INDEX 1u
#define BANKED_R14_INDEX 0u

ArmAllRegisters CreateArmAllRegisters() {
  ArmAllRegisters registers;
  memset(&registers, 0, sizeof(ArmAllRegisters));
  return registers;
}

bool ArmPrivilegedRegistersAreZero(const ArmAllRegisters& regs) {
  auto zero = CreateArmAllRegisters();
  return !memcmp(&zero.current, &regs.current, sizeof(ArmPrivilegedRegisters));
}

TEST(ArmExceptionDataABT, ArmMode) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.pc = 108u;
  registers.current.user.cpsr.mode = MODE_UND;
  registers.current.user.gprs.r13 = 13u;
  registers.current.user.gprs.r14 = 14u;
  auto old_cpsr = registers.current.user.cpsr;

  ArmExceptionDataABT(&registers);
  EXPECT_EQ(108u, registers.current.user.gprs.r14);
  EXPECT_EQ(old_cpsr.value, registers.current.spsr.value);
  EXPECT_EQ(0x10u, registers.current.user.gprs.pc);
  EXPECT_EQ(MODE_ABT, registers.current.user.cpsr.mode);
  EXPECT_FALSE(registers.current.user.cpsr.thumb);
  EXPECT_TRUE(registers.current.user.cpsr.irq_disable);

  registers.current.user.gprs.r13 = 0u;
  registers.current.user.gprs.r14 = 0u;
  registers.current.user.gprs.pc = 0u;
  registers.current.user.cpsr.mode = 0u;
  registers.current.user.cpsr.irq_disable = 0u;
  registers.current.spsr.mode = 0u;
  EXPECT_TRUE(ArmPrivilegedRegistersAreZero(registers));
}

TEST(ArmExceptionDataABT, ThumbMode) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.pc = 104u;
  registers.current.user.cpsr.mode = MODE_UND;
  registers.current.user.cpsr.thumb = true;
  registers.current.user.gprs.r13 = 13u;
  registers.current.user.gprs.r14 = 14u;
  registers.current.spsr.zero = true;
  auto old_cpsr = registers.current.user.cpsr;

  ArmExceptionDataABT(&registers);
  EXPECT_EQ(108u, registers.current.user.gprs.r14);
  EXPECT_EQ(old_cpsr.value, registers.current.spsr.value);
  EXPECT_EQ(0x10u, registers.current.user.gprs.pc);
  EXPECT_EQ(MODE_ABT, registers.current.user.cpsr.mode);
  EXPECT_FALSE(registers.current.user.cpsr.thumb);
  EXPECT_TRUE(registers.current.user.cpsr.irq_disable);

  registers.current.user.gprs.r13 = 0u;
  registers.current.user.gprs.r14 = 0u;
  registers.current.user.gprs.pc = 0u;
  registers.current.user.cpsr.mode = 0u;
  registers.current.user.cpsr.irq_disable = 0u;
  registers.current.spsr.mode = 0u;
  registers.current.spsr.thumb = false;
  EXPECT_TRUE(ArmPrivilegedRegistersAreZero(registers));
}

TEST(ArmExceptionPrefetchABT, ArmMode) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.pc = 108u;
  registers.current.user.cpsr.mode = MODE_UND;
  registers.current.user.gprs.r13 = 13u;
  registers.current.user.gprs.r14 = 14u;
  auto old_cpsr = registers.current.user.cpsr;

  ArmExceptionPrefetchABT(&registers);
  EXPECT_EQ(104u, registers.current.user.gprs.r14);
  EXPECT_EQ(old_cpsr.value, registers.current.spsr.value);
  EXPECT_EQ(0xCu, registers.current.user.gprs.pc);
  EXPECT_EQ(MODE_ABT, registers.current.user.cpsr.mode);
  EXPECT_FALSE(registers.current.user.cpsr.thumb);
  EXPECT_TRUE(registers.current.user.cpsr.irq_disable);

  registers.current.user.gprs.r13 = 0u;
  registers.current.user.gprs.r14 = 0u;
  registers.current.user.gprs.pc = 0u;
  registers.current.user.cpsr.mode = 0u;
  registers.current.user.cpsr.irq_disable = 0u;
  registers.current.spsr.mode = 0u;
  EXPECT_TRUE(ArmPrivilegedRegistersAreZero(registers));
}

TEST(ArmExceptionPrefetchABT, ThumbMode) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.pc = 104u;
  registers.current.user.cpsr.mode = MODE_UND;
  registers.current.user.cpsr.thumb = true;
  registers.current.user.gprs.r13 = 13u;
  registers.current.user.gprs.r14 = 14u;
  registers.current.spsr.zero = true;
  auto old_cpsr = registers.current.user.cpsr;

  ArmExceptionPrefetchABT(&registers);
  EXPECT_EQ(104u, registers.current.user.gprs.r14);
  EXPECT_EQ(old_cpsr.value, registers.current.spsr.value);
  EXPECT_EQ(0xCu, registers.current.user.gprs.pc);
  EXPECT_EQ(MODE_ABT, registers.current.user.cpsr.mode);
  EXPECT_FALSE(registers.current.user.cpsr.thumb);
  EXPECT_TRUE(registers.current.user.cpsr.irq_disable);

  registers.current.user.gprs.r13 = 0u;
  registers.current.user.gprs.r14 = 0u;
  registers.current.user.gprs.pc = 0u;
  registers.current.user.cpsr.mode = 0u;
  registers.current.user.cpsr.irq_disable = 0u;
  registers.current.spsr.mode = 0u;
  registers.current.spsr.thumb = false;
  EXPECT_TRUE(ArmPrivilegedRegistersAreZero(registers));
}

TEST(ArmExceptionFIQ, ArmMode) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.pc = 108u;
  registers.current.user.cpsr.mode = MODE_UND;
  registers.current.user.gprs.r13 = 13u;
  registers.current.user.gprs.r14 = 14u;
  auto old_cpsr = registers.current.user.cpsr;

  ArmExceptionFIQ(&registers);
  EXPECT_EQ(104u, registers.current.user.gprs.r14);
  EXPECT_EQ(old_cpsr.value, registers.current.spsr.value);
  EXPECT_EQ(0x1Cu, registers.current.user.gprs.pc);
  EXPECT_EQ(MODE_FIQ, registers.current.user.cpsr.mode);
  EXPECT_FALSE(registers.current.user.cpsr.thumb);
  EXPECT_TRUE(registers.current.user.cpsr.irq_disable);
  EXPECT_TRUE(registers.current.user.cpsr.fiq_disable);

  registers.current.user.gprs.r13 = 0u;
  registers.current.user.gprs.r14 = 0u;
  registers.current.user.gprs.pc = 0u;
  registers.current.user.cpsr.mode = 0u;
  registers.current.user.cpsr.irq_disable = 0u;
  registers.current.user.cpsr.fiq_disable = 0u;
  registers.current.spsr.mode = 0u;
  EXPECT_TRUE(ArmPrivilegedRegistersAreZero(registers));
}

TEST(ArmExceptionFIQ, ThumbMode) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.pc = 104u;
  registers.current.user.cpsr.mode = MODE_UND;
  registers.current.user.cpsr.thumb = true;
  registers.current.user.gprs.r13 = 13u;
  registers.current.user.gprs.r14 = 14u;
  registers.current.spsr.zero = true;
  auto old_cpsr = registers.current.user.cpsr;

  ArmExceptionFIQ(&registers);
  EXPECT_EQ(104u, registers.current.user.gprs.r14);
  EXPECT_EQ(old_cpsr.value, registers.current.spsr.value);
  EXPECT_EQ(0x1Cu, registers.current.user.gprs.pc);
  EXPECT_EQ(MODE_FIQ, registers.current.user.cpsr.mode);
  EXPECT_FALSE(registers.current.user.cpsr.thumb);
  EXPECT_TRUE(registers.current.user.cpsr.irq_disable);
  EXPECT_TRUE(registers.current.user.cpsr.fiq_disable);

  registers.current.user.gprs.r13 = 0u;
  registers.current.user.gprs.r14 = 0u;
  registers.current.user.gprs.pc = 0u;
  registers.current.user.cpsr.mode = 0u;
  registers.current.user.cpsr.irq_disable = 0u;
  registers.current.user.cpsr.fiq_disable = 0u;
  registers.current.spsr.mode = 0u;
  registers.current.spsr.thumb = false;
  EXPECT_TRUE(ArmPrivilegedRegistersAreZero(registers));
}

TEST(ArmExceptionIRQ, ArmMode) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.pc = 108u;
  registers.current.user.cpsr.mode = MODE_UND;
  registers.current.user.gprs.r13 = 13u;
  registers.current.user.gprs.r14 = 14u;
  auto old_cpsr = registers.current.user.cpsr;

  ArmExceptionIRQ(&registers);
  EXPECT_EQ(104u, registers.current.user.gprs.r14);
  EXPECT_EQ(old_cpsr.value, registers.current.spsr.value);
  EXPECT_EQ(0x18u, registers.current.user.gprs.pc);
  EXPECT_EQ(MODE_IRQ, registers.current.user.cpsr.mode);
  EXPECT_FALSE(registers.current.user.cpsr.thumb);
  EXPECT_TRUE(registers.current.user.cpsr.irq_disable);

  registers.current.user.gprs.r13 = 0u;
  registers.current.user.gprs.r14 = 0u;
  registers.current.user.gprs.pc = 0u;
  registers.current.user.cpsr.mode = 0u;
  registers.current.user.cpsr.irq_disable = 0u;
  registers.current.spsr.mode = 0u;
  EXPECT_TRUE(ArmPrivilegedRegistersAreZero(registers));
}

TEST(ArmExceptionIRQ, ThumbMode) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.pc = 104u;
  registers.current.user.cpsr.mode = MODE_UND;
  registers.current.user.cpsr.thumb = true;
  registers.current.user.gprs.r13 = 13u;
  registers.current.user.gprs.r14 = 14u;
  registers.current.spsr.zero = true;
  auto old_cpsr = registers.current.user.cpsr;

  ArmExceptionIRQ(&registers);
  EXPECT_EQ(104u, registers.current.user.gprs.r14);
  EXPECT_EQ(old_cpsr.value, registers.current.spsr.value);
  EXPECT_EQ(0x18u, registers.current.user.gprs.pc);
  EXPECT_EQ(MODE_IRQ, registers.current.user.cpsr.mode);
  EXPECT_FALSE(registers.current.user.cpsr.thumb);
  EXPECT_TRUE(registers.current.user.cpsr.irq_disable);

  registers.current.user.gprs.r13 = 0u;
  registers.current.user.gprs.r14 = 0u;
  registers.current.user.gprs.pc = 0u;
  registers.current.user.cpsr.mode = 0u;
  registers.current.user.cpsr.irq_disable = 0u;
  registers.current.spsr.mode = 0u;
  registers.current.spsr.thumb = false;
  EXPECT_TRUE(ArmPrivilegedRegistersAreZero(registers));
}

TEST(ArmExceptionRST, ArmMode) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.pc = 108u;
  registers.current.user.cpsr.mode = MODE_UND;
  registers.current.user.gprs.r13 = 13u;
  registers.current.user.gprs.r14 = 14u;

  ArmExceptionRST(&registers);
  EXPECT_EQ(0x0u, registers.current.user.gprs.pc);
  EXPECT_EQ(MODE_SVC, registers.current.user.cpsr.mode);
  EXPECT_FALSE(registers.current.user.cpsr.thumb);
  EXPECT_TRUE(registers.current.user.cpsr.irq_disable);
  EXPECT_TRUE(registers.current.user.cpsr.fiq_disable);

  registers.current.user.gprs.r13 = 0u;
  registers.current.user.gprs.r14 = 0u;
  registers.current.user.gprs.pc = 0u;
  registers.current.user.cpsr.mode = 0u;
  registers.current.user.cpsr.irq_disable = 0u;
  registers.current.user.cpsr.fiq_disable = 0u;
  registers.current.spsr.mode = 0u;
  EXPECT_TRUE(ArmPrivilegedRegistersAreZero(registers));
}

TEST(ArmExceptionRST, ThumbMode) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.pc = 104u;
  registers.current.user.cpsr.mode = MODE_UND;
  registers.current.user.cpsr.thumb = true;
  registers.current.user.gprs.r13 = 13u;
  registers.current.user.gprs.r14 = 14u;
  registers.current.spsr.zero = true;

  ArmExceptionRST(&registers);
  EXPECT_EQ(0x0u, registers.current.user.gprs.pc);
  EXPECT_EQ(MODE_SVC, registers.current.user.cpsr.mode);
  EXPECT_FALSE(registers.current.user.cpsr.thumb);
  EXPECT_TRUE(registers.current.user.cpsr.irq_disable);
  EXPECT_TRUE(registers.current.user.cpsr.fiq_disable);

  registers.current.user.gprs.r13 = 0u;
  registers.current.user.gprs.r14 = 0u;
  registers.current.user.gprs.pc = 0u;
  registers.current.user.cpsr.mode = 0u;
  registers.current.user.cpsr.irq_disable = 0u;
  registers.current.user.cpsr.fiq_disable = 0u;
  registers.current.spsr.mode = 0u;
  registers.current.spsr.thumb = false;
  EXPECT_TRUE(ArmPrivilegedRegistersAreZero(registers));
}

TEST(ArmExceptionSWI, ArmMode) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.pc = 108u;
  registers.current.user.cpsr.mode = MODE_IRQ;
  registers.current.user.gprs.r13 = 13u;
  registers.current.user.gprs.r14 = 14u;
  auto old_cpsr = registers.current.user.cpsr;

  ArmExceptionSWI(&registers);
  EXPECT_EQ(104u, registers.current.user.gprs.r14);
  EXPECT_EQ(old_cpsr.value, registers.current.spsr.value);
  EXPECT_EQ(8u, registers.current.user.gprs.pc);
  EXPECT_EQ(MODE_SVC, registers.current.user.cpsr.mode);
  EXPECT_FALSE(registers.current.user.cpsr.thumb);
  EXPECT_TRUE(registers.current.user.cpsr.irq_disable);

  registers.current.user.gprs.r13 = 0u;
  registers.current.user.gprs.r14 = 0u;
  registers.current.user.gprs.pc = 0u;
  registers.current.user.cpsr.mode = 0u;
  registers.current.user.cpsr.irq_disable = 0u;
  registers.current.spsr.mode = 0u;
  EXPECT_TRUE(ArmPrivilegedRegistersAreZero(registers));
}

TEST(ArmExceptionSWI, ThumbMode) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.pc = 104u;
  registers.current.user.cpsr.mode = MODE_IRQ;
  registers.current.user.cpsr.thumb = true;
  registers.current.user.gprs.r13 = 13u;
  registers.current.user.gprs.r14 = 14u;
  registers.current.spsr.zero = true;
  auto old_cpsr = registers.current.user.cpsr;

  ArmExceptionSWI(&registers);
  EXPECT_EQ(102u, registers.current.user.gprs.r14);
  EXPECT_EQ(old_cpsr.value, registers.current.spsr.value);
  EXPECT_EQ(8u, registers.current.user.gprs.pc);
  EXPECT_EQ(MODE_SVC, registers.current.user.cpsr.mode);
  EXPECT_FALSE(registers.current.user.cpsr.thumb);
  EXPECT_TRUE(registers.current.user.cpsr.irq_disable);

  registers.current.user.gprs.r13 = 0u;
  registers.current.user.gprs.r14 = 0u;
  registers.current.user.gprs.pc = 0u;
  registers.current.user.cpsr.mode = 0u;
  registers.current.user.cpsr.irq_disable = 0u;
  registers.current.spsr.mode = 0u;
  registers.current.spsr.thumb = false;
  EXPECT_TRUE(ArmPrivilegedRegistersAreZero(registers));
}

TEST(ArmExceptionUND, ArmMode) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.pc = 108u;
  registers.current.user.cpsr.mode = MODE_IRQ;
  registers.current.user.gprs.r13 = 13u;
  registers.current.user.gprs.r14 = 14u;
  auto old_cpsr = registers.current.user.cpsr;

  ArmExceptionUND(&registers);
  EXPECT_EQ(104u, registers.current.user.gprs.r14);
  EXPECT_EQ(old_cpsr.value, registers.current.spsr.value);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_EQ(MODE_UND, registers.current.user.cpsr.mode);
  EXPECT_FALSE(registers.current.user.cpsr.thumb);
  EXPECT_TRUE(registers.current.user.cpsr.irq_disable);

  registers.current.user.gprs.r13 = 0u;
  registers.current.user.gprs.r14 = 0u;
  registers.current.user.gprs.pc = 0u;
  registers.current.user.cpsr.mode = 0u;
  registers.current.user.cpsr.irq_disable = 0u;
  registers.current.spsr.mode = 0u;
  EXPECT_TRUE(ArmPrivilegedRegistersAreZero(registers));
}

TEST(ArmExceptionUND, ThumbMode) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.pc = 104u;
  registers.current.user.cpsr.mode = MODE_IRQ;
  registers.current.user.cpsr.thumb = true;
  registers.current.user.gprs.r13 = 13u;
  registers.current.user.gprs.r14 = 14u;
  registers.current.spsr.zero = true;
  auto old_cpsr = registers.current.user.cpsr;

  ArmExceptionUND(&registers);
  EXPECT_EQ(102u, registers.current.user.gprs.r14);
  EXPECT_EQ(old_cpsr.value, registers.current.spsr.value);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);
  EXPECT_EQ(MODE_UND, registers.current.user.cpsr.mode);
  EXPECT_FALSE(registers.current.user.cpsr.thumb);
  EXPECT_TRUE(registers.current.user.cpsr.irq_disable);

  registers.current.user.gprs.r13 = 0u;
  registers.current.user.gprs.r14 = 0u;
  registers.current.user.gprs.pc = 0u;
  registers.current.user.cpsr.mode = 0u;
  registers.current.user.cpsr.irq_disable = 0u;
  registers.current.spsr.mode = 0u;
  registers.current.spsr.thumb = false;
  EXPECT_TRUE(ArmPrivilegedRegistersAreZero(registers));
}