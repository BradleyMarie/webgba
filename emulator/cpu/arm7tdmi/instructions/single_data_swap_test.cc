extern "C" {
#include "emulator/cpu/arm7tdmi/instructions/single_data_swap.h"
}

#include <strings.h>

#include "googletest/include/gtest/gtest.h"

ArmGeneralPurposeRegisters CreateArmGeneralPurposeRegistersRegisters() {
  ArmGeneralPurposeRegisters registers;
  memset(&registers, 0, sizeof(ArmGeneralPurposeRegisters));
  return registers;
}

bool ArmGeneralPurposeRegistersAreZero(const ArmGeneralPurposeRegisters& regs) {
  auto zero = CreateArmGeneralPurposeRegistersRegisters();
  return !memcmp(&zero, &regs, sizeof(ArmGeneralPurposeRegisters));
}

ArmUserRegisters CreateArmUserRegisters() {
  ArmUserRegisters registers;
  memset(&registers, 0, sizeof(ArmUserRegisters));
  return registers;
}

bool ArmUserRegistersAreZero(const ArmUserRegisters& regs) {
  auto zero = CreateArmUserRegisters();
  return !memcmp(&zero, &regs, sizeof(ArmUserRegisters));
}

ArmPrivilegedRegisters CreateArmPrivilegedRegisters() {
  ArmPrivilegedRegisters registers;
  memset(&registers, 0, sizeof(ArmPrivilegedRegisters));
  return registers;
}

bool ArmArmPrivilegedRegistersAreZero(const ArmPrivilegedRegisters& regs) {
  auto zero = CreateArmPrivilegedRegisters();
  return !memcmp(&zero, &regs, sizeof(ArmPrivilegedRegisters));
}

TEST(ArmMRS, Move) {
  auto registers = CreateArmUserRegisters();

  registers.cpsr.thumb = true;
  ArmMRS(&registers, REGISTER_R0);
  EXPECT_EQ(registers.gprs.r0, registers.cpsr.value);
  EXPECT_TRUE(registers.cpsr.thumb);

  registers.gprs.r0 = 0;
  registers.cpsr.thumb = false;

  EXPECT_TRUE(ArmUserRegistersAreZero(registers));
}

TEST(ArmMRSR, Move) {
  auto registers = CreateArmPrivilegedRegisters();

  registers.user.cpsr.mode = MODE_SVC;
  registers.spsr.mode = MODE_USR;
  ArmMRSR(&registers, REGISTER_R0);
  EXPECT_EQ(registers.user.gprs.r0, registers.spsr.value);
  EXPECT_EQ(MODE_SVC, registers.user.cpsr.mode);
  EXPECT_EQ(MODE_USR, registers.spsr.mode);

  registers.user.gprs.r0 = 0;
  registers.user.cpsr.mode = 0u;
  registers.spsr.mode = 0u;

  EXPECT_TRUE(ArmArmPrivilegedRegistersAreZero(registers));
}