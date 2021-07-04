extern "C" {
#include "emulator/cpu/arm7tdmi/instructions/coprocessor_data_transfer.h"
}

#include <cstring>

#include "googletest/include/gtest/gtest.h"

ArmAllRegisters CreateArmAllRegisters() {
  ArmAllRegisters registers;
  memset(&registers, 0, sizeof(ArmAllRegisters));
  registers.current.user.cpsr.mode = MODE_SVC;
  return registers;
}

TEST(ArmLDC, Run) {
  auto registers = CreateArmAllRegisters();
  ArmLDC(&registers);
  EXPECT_EQ(MODE_UND, registers.current.user.cpsr.mode);
}

TEST(ArmSTC, Run) {
  auto registers = CreateArmAllRegisters();
  ArmSTC(&registers);
  EXPECT_EQ(MODE_UND, registers.current.user.cpsr.mode);
}