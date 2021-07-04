extern "C" {
#include "emulator/cpu/arm7tdmi/instructions/coprocessor_register_transfer.h"
}

#include <cstring>

#include "googletest/include/gtest/gtest.h"

ArmAllRegisters CreateArmAllRegisters() {
  ArmAllRegisters registers;
  memset(&registers, 0, sizeof(ArmAllRegisters));
  registers.current.user.cpsr.mode = MODE_SVC;
  return registers;
}

TEST(ArmMCR, Run) {
  auto registers = CreateArmAllRegisters();
  ArmMCR(&registers);
  EXPECT_EQ(MODE_UND, registers.current.user.cpsr.mode);
}

TEST(ArmMRC, Run) {
  auto registers = CreateArmAllRegisters();
  ArmMRC(&registers);
  EXPECT_EQ(MODE_UND, registers.current.user.cpsr.mode);
}