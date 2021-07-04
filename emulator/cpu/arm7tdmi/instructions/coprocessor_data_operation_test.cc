extern "C" {
#include "emulator/cpu/arm7tdmi/instructions/coprocessor_data_operation.h"
}

#include <cstring>

#include "googletest/include/gtest/gtest.h"

ArmAllRegisters CreateArmAllRegisters() {
  ArmAllRegisters registers;
  memset(&registers, 0, sizeof(ArmAllRegisters));
  registers.current.user.cpsr.mode = MODE_SVC;
  return registers;
}

TEST(ArmCDP, Run) {
  auto registers = CreateArmAllRegisters();
  ArmCDP(&registers);
  EXPECT_EQ(MODE_UND, registers.current.user.cpsr.mode);
}