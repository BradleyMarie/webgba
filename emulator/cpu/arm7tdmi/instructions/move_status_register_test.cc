extern "C" {
#include "emulator/cpu/arm7tdmi/instructions/move_status_register.h"
}

#include <strings.h>

#include <vector>

#include "googletest/include/gtest/gtest.h"

ArmAllRegisters CreateArmAllRegisters() {
  ArmAllRegisters registers;
  memset(&registers, 0, sizeof(ArmAllRegisters));
  return registers;
}

bool ArmAllRegistersAreZero(const ArmAllRegisters &regs) {
  auto zero = CreateArmAllRegisters();
  return !memcmp(&zero, &regs, sizeof(ArmAllRegisters));
}

TEST(ArmMRS_CPSR, Move) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.cpsr.thumb = true;
  ArmMRS_CPSR(&registers, REGISTER_R0);
  EXPECT_EQ(registers.current.user.gprs.r0, registers.current.user.cpsr.value);
  EXPECT_TRUE(registers.current.user.cpsr.thumb);

  registers.current.user.gprs.r0 = 0;
  registers.current.user.cpsr.thumb = false;

  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmMRS_SPSR, Move) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.cpsr.mode = MODE_SVC;
  registers.current.spsr.mode = MODE_USR;
  ArmMRS_SPSR(&registers, REGISTER_R0);
  EXPECT_EQ(registers.current.user.gprs.r0, registers.current.spsr.value);
  EXPECT_EQ(MODE_SVC, registers.current.user.cpsr.mode);
  EXPECT_EQ(MODE_USR, registers.current.spsr.mode);

  registers.current.user.gprs.r0 = 0;
  registers.current.user.cpsr.mode = 0u;
  registers.current.spsr.mode = 0u;

  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmMSR_CPSR, FlagsOnly) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_USR;

  ArmProgramStatusRegister next_status;
  memset(&next_status, 0, sizeof(ArmProgramStatusRegister));
  next_status.thumb = true;
  next_status.fiq_disable = true;
  next_status.irq_disable = true;
  next_status.overflow = true;
  next_status.carry = true;
  next_status.zero = true;
  next_status.negative = true;

  ArmMSR_CPSR(&registers, false, true, next_status.value);
  EXPECT_TRUE(registers.current.user.cpsr.overflow);
  EXPECT_TRUE(registers.current.user.cpsr.carry);
  EXPECT_TRUE(registers.current.user.cpsr.zero);
  EXPECT_TRUE(registers.current.user.cpsr.negative);
  EXPECT_EQ(MODE_USR, registers.current.user.cpsr.mode);

  registers.current.user.cpsr.overflow = false;
  registers.current.user.cpsr.carry = false;
  registers.current.user.cpsr.zero = false;
  registers.current.user.cpsr.negative = false;
  registers.current.user.cpsr.mode = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmMSR_CPSR, FromUsr) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_USR;

  ArmProgramStatusRegister next_status;
  memset(&next_status, 0, sizeof(ArmProgramStatusRegister));
  next_status.mode = MODE_SVC;
  next_status.thumb = true;
  next_status.fiq_disable = true;
  next_status.irq_disable = true;
  next_status.overflow = true;
  next_status.carry = true;
  next_status.zero = true;
  next_status.negative = true;

  ArmMSR_CPSR(&registers, true, true, next_status.value);
  EXPECT_EQ(MODE_USR, registers.current.user.cpsr.mode);
  EXPECT_TRUE(registers.current.user.cpsr.overflow);
  EXPECT_TRUE(registers.current.user.cpsr.carry);
  EXPECT_TRUE(registers.current.user.cpsr.zero);
  EXPECT_TRUE(registers.current.user.cpsr.negative);

  registers.current.user.cpsr.mode = 0u;
  registers.current.user.cpsr.overflow = false;
  registers.current.user.cpsr.carry = false;
  registers.current.user.cpsr.zero = false;
  registers.current.user.cpsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmMSR_SPSR, FlagsOnly) {
  auto registers = CreateArmAllRegisters();

  ArmProgramStatusRegister next_status;
  memset(&next_status, 0, sizeof(ArmProgramStatusRegister));
  next_status.mode = MODE_USR;
  next_status.thumb = true;
  next_status.fiq_disable = true;
  next_status.irq_disable = true;
  next_status.overflow = true;
  next_status.carry = true;
  next_status.zero = true;
  next_status.negative = true;

  ArmMSR_SPSR(&registers, false, true, next_status.value);
  EXPECT_TRUE(registers.current.spsr.overflow);
  EXPECT_TRUE(registers.current.spsr.carry);
  EXPECT_TRUE(registers.current.spsr.zero);
  EXPECT_TRUE(registers.current.spsr.negative);

  registers.current.spsr.overflow = false;
  registers.current.spsr.carry = false;
  registers.current.spsr.zero = false;
  registers.current.spsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmMSR_SPSR, ControlOnly) {
  auto registers = CreateArmAllRegisters();

  ArmProgramStatusRegister next_status;
  memset(&next_status, 0, sizeof(ArmProgramStatusRegister));
  next_status.mode = MODE_USR;
  next_status.thumb = true;
  next_status.fiq_disable = true;
  next_status.irq_disable = true;
  next_status.overflow = true;
  next_status.carry = true;
  next_status.zero = true;
  next_status.negative = true;

  ArmMSR_SPSR(&registers, true, false, next_status.value);
  EXPECT_EQ(MODE_USR, registers.current.spsr.mode);
  EXPECT_TRUE(registers.current.spsr.thumb);
  EXPECT_TRUE(registers.current.spsr.fiq_disable);
  EXPECT_TRUE(registers.current.spsr.irq_disable);

  registers.current.spsr.mode = 0u;
  registers.current.spsr.thumb = false;
  registers.current.spsr.fiq_disable = false;
  registers.current.spsr.irq_disable = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST(ArmMSR_SPSR, Both) {
  auto registers = CreateArmAllRegisters();

  ArmProgramStatusRegister next_status;
  memset(&next_status, 0, sizeof(ArmProgramStatusRegister));
  next_status.mode = MODE_USR;
  next_status.thumb = true;
  next_status.fiq_disable = true;
  next_status.irq_disable = true;
  next_status.overflow = true;
  next_status.carry = true;
  next_status.zero = true;
  next_status.negative = true;

  ArmMSR_SPSR(&registers, true, true, next_status.value);
  EXPECT_EQ(MODE_USR, registers.current.spsr.mode);
  EXPECT_TRUE(registers.current.spsr.thumb);
  EXPECT_TRUE(registers.current.spsr.fiq_disable);
  EXPECT_TRUE(registers.current.spsr.irq_disable);
  EXPECT_TRUE(registers.current.spsr.overflow);
  EXPECT_TRUE(registers.current.spsr.carry);
  EXPECT_TRUE(registers.current.spsr.zero);
  EXPECT_TRUE(registers.current.spsr.negative);

  registers.current.spsr.mode = 0u;
  registers.current.spsr.thumb = false;
  registers.current.spsr.fiq_disable = false;
  registers.current.spsr.irq_disable = false;
  registers.current.spsr.overflow = false;
  registers.current.spsr.carry = false;
  registers.current.spsr.zero = false;
  registers.current.spsr.negative = false;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}