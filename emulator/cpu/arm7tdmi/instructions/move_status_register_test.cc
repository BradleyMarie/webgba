extern "C" {
#include "emulator/cpu/arm7tdmi/instructions/move_status_register.h"
}

#include <strings.h>

#include <vector>

#include "googletest/include/gtest/gtest.h"

ArmGeneralPurposeRegisters CreateArmGeneralPurposeRegisters() {
  ArmGeneralPurposeRegisters registers;
  memset(&registers, 0, sizeof(ArmGeneralPurposeRegisters));
  return registers;
}

bool ArmGeneralPurposeRegistersAreZero(const ArmGeneralPurposeRegisters &regs) {
  auto zero = CreateArmGeneralPurposeRegisters();
  return !memcmp(&zero, &regs, sizeof(ArmGeneralPurposeRegisters));
}

ArmUserRegisters CreateArmUserRegisters() {
  ArmUserRegisters registers;
  memset(&registers, 0, sizeof(ArmUserRegisters));
  return registers;
}

bool ArmUserRegistersAreZero(const ArmUserRegisters &regs) {
  auto zero = CreateArmUserRegisters();
  return !memcmp(&zero, &regs, sizeof(ArmUserRegisters));
}

ArmPrivilegedRegisters CreateArmPrivilegedRegisters() {
  ArmPrivilegedRegisters registers;
  memset(&registers, 0, sizeof(ArmPrivilegedRegisters));
  return registers;
}

bool ArmPrivilegedRegistersAreZero(const ArmPrivilegedRegisters &regs) {
  auto zero = CreateArmPrivilegedRegisters();
  return !memcmp(&zero, &regs, sizeof(ArmPrivilegedRegisters));
}

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
  auto registers = CreateArmUserRegisters();

  registers.cpsr.thumb = true;
  ArmMRS_CPSR(&registers, REGISTER_R0);
  EXPECT_EQ(registers.gprs.r0, registers.cpsr.value);
  EXPECT_TRUE(registers.cpsr.thumb);

  registers.gprs.r0 = 0;
  registers.cpsr.thumb = false;

  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmMRS_SPSR, Move) {
  auto registers = CreateArmPrivilegedRegisters();

  registers.user.cpsr.mode = MODE_SVC;
  registers.spsr.mode = MODE_USR;
  ArmMRS_SPSR(&registers, REGISTER_R0);
  EXPECT_EQ(registers.user.gprs.r0, registers.spsr.value);
  EXPECT_EQ(MODE_SVC, registers.user.cpsr.mode);
  EXPECT_EQ(MODE_USR, registers.spsr.mode);

  registers.user.gprs.r0 = 0;
  registers.user.cpsr.mode = 0u;
  registers.spsr.mode = 0u;

  EXPECT_TRUE(ArmPrivilegedRegistersAreZero(registers));
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
  auto registers = CreateArmPrivilegedRegisters();

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
  EXPECT_TRUE(registers.spsr.overflow);
  EXPECT_TRUE(registers.spsr.carry);
  EXPECT_TRUE(registers.spsr.zero);
  EXPECT_TRUE(registers.spsr.negative);

  registers.spsr.overflow = false;
  registers.spsr.carry = false;
  registers.spsr.zero = false;
  registers.spsr.negative = false;
  EXPECT_TRUE(ArmPrivilegedRegistersAreZero(registers));
}

TEST(ArmMSR_SPSR, ControlOnly) {
  auto registers = CreateArmPrivilegedRegisters();

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
  EXPECT_EQ(MODE_USR, registers.spsr.mode);
  EXPECT_TRUE(registers.spsr.thumb);
  EXPECT_TRUE(registers.spsr.fiq_disable);
  EXPECT_TRUE(registers.spsr.irq_disable);

  registers.spsr.mode = 0u;
  registers.spsr.thumb = false;
  registers.spsr.fiq_disable = false;
  registers.spsr.irq_disable = false;
  EXPECT_TRUE(ArmPrivilegedRegistersAreZero(registers));
}

TEST(ArmMSR_SPSR, Both) {
  auto registers = CreateArmPrivilegedRegisters();

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
  EXPECT_EQ(MODE_USR, registers.spsr.mode);
  EXPECT_TRUE(registers.spsr.thumb);
  EXPECT_TRUE(registers.spsr.fiq_disable);
  EXPECT_TRUE(registers.spsr.irq_disable);
  EXPECT_TRUE(registers.spsr.overflow);
  EXPECT_TRUE(registers.spsr.carry);
  EXPECT_TRUE(registers.spsr.zero);
  EXPECT_TRUE(registers.spsr.negative);

  registers.spsr.mode = 0u;
  registers.spsr.thumb = false;
  registers.spsr.fiq_disable = false;
  registers.spsr.irq_disable = false;
  registers.spsr.overflow = false;
  registers.spsr.carry = false;
  registers.spsr.zero = false;
  registers.spsr.negative = false;
  EXPECT_TRUE(ArmPrivilegedRegistersAreZero(registers));
}