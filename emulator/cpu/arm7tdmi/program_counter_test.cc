extern "C" {
#include "emulator/cpu/arm7tdmi/program_counter.h"
}

#include "googletest/include/gtest/gtest.h"

ArmAllRegisters CreateArmAllRegisters() {
  ArmAllRegisters registers;
  memset(&registers, 0, sizeof(ArmAllRegisters));
  return registers;
}

TEST(ProgramCounterTest, ArmNextInstructionArm) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.gprs.pc = 0x108u;
  EXPECT_EQ(0x104u, ArmNextInstruction(&registers));
}

TEST(ProgramCounterTest, ArmNextInstructionThumb) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.gprs.pc = 0x104u;
  registers.current.user.cpsr.thumb = true;
  EXPECT_EQ(0x102u, ArmNextInstruction(&registers));
}

TEST(ProgramCounterTest, ArmCurrentInstructionArm) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.gprs.pc = 0x108u;
  EXPECT_EQ(0x100u, ArmCurrentInstruction(&registers));
}

TEST(ProgramCounterTest, ArmCurrentInstructionThumb) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.gprs.pc = 0x104u;
  registers.current.user.cpsr.thumb = true;
  EXPECT_EQ(0x100u, ArmCurrentInstruction(&registers));
}

TEST(ProgramCounterTest, ArmAdvanceProgramCounterArm) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.gprs.pc = 0x108u;
  ArmAdvanceProgramCounter(&registers);
  EXPECT_EQ(0x104u, ArmCurrentInstruction(&registers));
}

TEST(ProgramCounterTest, ArmAdvanceProgramCounterThumb) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.gprs.pc = 0x104u;
  registers.current.user.cpsr.thumb = true;
  ArmAdvanceProgramCounter(&registers);
  EXPECT_EQ(0x102u, ArmCurrentInstruction(&registers));
}