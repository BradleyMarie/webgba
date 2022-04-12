extern "C" {
#include "emulator/cpu/arm7tdmi/instructions/swi.h"
#include "emulator/cpu/arm7tdmi/program_counter.h"
}

#include <strings.h>

#include "googletest/include/gtest/gtest.h"

ArmAllRegisters CreateArmAllRegisters() {
  ArmAllRegisters registers;
  memset(&registers, 0, sizeof(ArmAllRegisters));
  return registers;
}

bool ArmPrivilegedRegistersAreZero(const ArmAllRegisters& regs) {
  auto zero = CreateArmAllRegisters();
  return !memcmp(&zero.current, &regs.current, sizeof(ArmPrivilegedRegisters));
}

TEST(ArmSWI, SoftwareInterrupt) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.pc = 108u;
  registers.current.user.cpsr.mode = MODE_IRQ;
  registers.current.user.gprs.r13 = 13u;
  registers.current.user.gprs.r14 = 14u;
  auto old_cpsr = registers.current.user.cpsr;

  ArmSWI(&registers);
  EXPECT_EQ(104u, registers.current.user.gprs.r14);
  EXPECT_EQ(old_cpsr.value, registers.current.spsr.value);
  EXPECT_EQ(8u, ArmNextInstruction(&registers));
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

TEST(ArmSWI, SoftwareInterruptThumb) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.pc = 104u;
  registers.current.user.cpsr.mode = MODE_IRQ;
  registers.current.user.cpsr.thumb = true;
  registers.current.user.gprs.r13 = 13u;
  registers.current.user.gprs.r14 = 14u;
  registers.current.spsr.zero = true;
  auto old_cpsr = registers.current.user.cpsr;

  ArmSWI(&registers);
  EXPECT_EQ(102u, registers.current.user.gprs.r14);
  EXPECT_EQ(old_cpsr.value, registers.current.spsr.value);
  EXPECT_EQ(8u, ArmNextInstruction(&registers));
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