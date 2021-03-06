extern "C" {
#include "emulator/cpu/arm7tdmi/decoders/arm/execute.h"
}

#include <cstring>

#include "googletest/include/gtest/gtest.h"

class ExecuteTest : public testing::TestWithParam<uint16_t> {
 public:
  void SetUp() override {
    for (char &c : memory_space_) {
      c = 0;
    }
    memory_ = MemoryAllocate(nullptr, Load32LE, Load16LE, Load8, Store32LE,
                             Store16LE, Store8, nullptr);
    ASSERT_NE(nullptr, memory_);
    memory_fails_ =
        MemoryAllocate(nullptr, Load32LEFails, Load16LEFails, Load8Fails,
                       Store32LEFails, Store16LEFails, Store8Fails, nullptr);
    ASSERT_NE(nullptr, memory_fails_);

    memset(&registers_, 0, sizeof(ArmAllRegisters));
    registers_.current.user.cpsr.mode = MODE_SVC;
    registers_.current.spsr.mode = MODE_USR;
    registers_.current.user.gprs.pc = 0x108;
    registers_.current.user.gprs.sp = 0x200;

    instruction_end_ = 0x100;
    num_instructions_ = 0;
  }

  void TearDown() override {
    MemoryFree(memory_);
    MemoryFree(memory_fails_);
  }

 protected:
  static bool Load32LE(const void *context, uint32_t address, uint32_t *value) {
    if (address + sizeof(uint32_t) - 1 >= memory_space_.size()) {
      return false;
    }
    char *data = memory_space_.data() + address;
    *value = *reinterpret_cast<uint32_t *>(data);
    return true;
  }

  static bool Load32LEFails(const void *context, uint32_t address,
                            uint32_t *value) {
    return false;
  }

  static bool Load16LE(const void *context, uint32_t address, uint16_t *value) {
    if (address + sizeof(uint16_t) - 1 >= memory_space_.size()) {
      return false;
    }
    char *data = memory_space_.data() + address;
    *value = *reinterpret_cast<uint16_t *>(data);
    return true;
  }

  static bool Load16LEFails(const void *context, uint32_t address,
                            uint16_t *value) {
    return false;
  }

  static bool Load8(const void *context, uint32_t address, uint8_t *value) {
    if (address > memory_space_.size()) {
      return false;
    }
    *value = memory_space_[address];
    return true;
  }

  static bool Load8Fails(const void *context, uint32_t address,
                         uint8_t *value) {
    return false;
  }

  static bool Store32LE(void *context, uint32_t address, uint32_t value) {
    if (address + sizeof(uint32_t) - 1 >= memory_space_.size()) {
      return false;
    }
    char *data = memory_space_.data() + address;
    *reinterpret_cast<uint32_t *>(data) = value;
    return true;
  }

  static bool Store32LEFails(void *context, uint32_t address, uint32_t value) {
    return false;
  }

  static bool Store16LE(void *context, uint32_t address, uint16_t value) {
    if (address + sizeof(uint16_t) - 1 >= memory_space_.size()) {
      return false;
    }
    char *data = memory_space_.data() + address;
    *reinterpret_cast<uint16_t *>(data) = value;
    return true;
  }

  static bool Store16LEFails(void *context, uint32_t address, uint16_t value) {
    return false;
  }

  static bool Store8(void *context, uint32_t address, uint8_t value) {
    if (address >= memory_space_.size()) {
      return false;
    }
    memory_space_[address] = value;
    return true;
  }

  static bool Store8Fails(void *context, uint32_t address, uint8_t value) {
    return false;
  }

  bool ArmIsDataAbort(const ArmAllRegisters &regs) {
    return regs.current.user.cpsr.mode == MODE_ABT;
  }

  // Assumes little-endian hex string
  void RunInstruction(std::string instruction_hex) {
    uint32_t next_instruction = ToInstruction(instruction_hex);
    ArmInstructionExecute(next_instruction, &registers_, memory_);
  }

  // Assumes little-endian hex string
  void RunInstructionBadMemory(std::string instruction_hex) {
    uint32_t next_instruction = ToInstruction(instruction_hex);
    ArmInstructionExecute(next_instruction, &registers_, memory_fails_);
  }

  static std::vector<char> memory_space_;
  size_t instruction_end_;
  size_t num_instructions_;
  ArmAllRegisters registers_;
  Memory *memory_;
  Memory *memory_fails_;

 private:
  uint32_t ToInstruction(std::string instruction_hex) {
    if (instruction_hex[0u] == '0' && instruction_hex[1u] == 'x') {
      instruction_hex.erase(0u, 2u);
    }

    assert(instruction_hex.size() == 8u);
    uint32_t instruction = 0u;
    for (size_t i = 0u; i < instruction_hex.size(); i += 2u) {
      std::string hex_byte;
      hex_byte += instruction_hex[i];
      hex_byte += instruction_hex[i + 1u];
      uint32_t byte = std::stoul(hex_byte, nullptr, 16u);
      instruction += byte << (i * 4u);
    }

    return instruction;
  }
};

std::vector<char> ExecuteTest::memory_space_(1024u, 0);

TEST_F(ExecuteTest, ExecutionSkipped) {
  registers_.current.user.gprs.r0 = 1u;
  RunInstruction("0x00008000");  // addeq r0, r0, r0
  EXPECT_EQ(1u, registers_.current.user.gprs.r0);
  EXPECT_EQ(0x104u, ArmCurrentInstruction(&registers_));
}

TEST_F(ExecuteTest, ADC) {
  registers_.current.user.gprs.r0 = 1u;
  registers_.current.user.cpsr.carry = true;
  RunInstruction("0x0000A0E0");  // adc r0, r0, r0
  EXPECT_EQ(3u, registers_.current.user.gprs.r0);

  // Modifies PC
  RunInstruction("0x00F0A0E0");  // adc r15, r0, r0
}

TEST_F(ExecuteTest, ADC_I32) {
  registers_.current.user.gprs.r0 = 1u;
  registers_.current.user.cpsr.carry = true;
  RunInstruction("0x0200A0E2");  // adc r0, r0, #2
  EXPECT_EQ(4u, registers_.current.user.gprs.r0);

  // Modifies PC
  RunInstruction("0x01F0A0E2");  // adc r15, r0, #1
}

TEST_F(ExecuteTest, ADCS) {
  registers_.current.user.gprs.r0 = 0xFFFFFFFFu;
  registers_.current.user.cpsr.carry = true;
  RunInstruction("0x0100B0E0");  // adcs r0, r0, r1
  EXPECT_EQ(0u, registers_.current.user.gprs.r0);
  EXPECT_TRUE(registers_.current.user.cpsr.zero);
  EXPECT_TRUE(registers_.current.user.cpsr.carry);

  // Modifies PC
  RunInstruction("0x01F0B0E0");  // adcs r15, r0, r0
}

TEST_F(ExecuteTest, ADCS_I32) {
  registers_.current.user.gprs.r0 = 0xFFFFFFFFu;
  registers_.current.user.cpsr.carry = true;
  RunInstruction("0x0000B0E2");  // adcs r0, r0, #0
  EXPECT_EQ(0u, registers_.current.user.gprs.r0);
  EXPECT_TRUE(registers_.current.user.cpsr.zero);
  EXPECT_TRUE(registers_.current.user.cpsr.carry);

  // Modifies PC
  RunInstruction("0x00F0B0E2");  // adcs r15, r0, #0
}

TEST_F(ExecuteTest, ADD) {
  registers_.current.user.gprs.r0 = 1u;
  RunInstruction("0x000080E0");  // add r0, r0, r0
  EXPECT_EQ(2u, registers_.current.user.gprs.r0);

  // Modifies PC
  RunInstruction("0x00F080E0");  // add r15, r0, r0
}

TEST_F(ExecuteTest, ADD_I32) {
  registers_.current.user.gprs.r0 = 1u;
  RunInstruction("0x020080E2");  // add r0, r0, #2
  EXPECT_EQ(3u, registers_.current.user.gprs.r0);

  // Modifies PC
  RunInstruction("0x02F080E2");  // add r15, r0, #2
}

TEST_F(ExecuteTest, ADDS) {
  registers_.current.user.gprs.r0 = 0xFFFFFFFFu;
  registers_.current.user.gprs.r1 = 0x1u;
  RunInstruction("0x010090E0");  // adds r0, r0, r1
  EXPECT_EQ(0u, registers_.current.user.gprs.r0);
  EXPECT_TRUE(registers_.current.user.cpsr.zero);
  EXPECT_TRUE(registers_.current.user.cpsr.carry);

  // Modifies PC
  RunInstruction("0x01F090E0");  // adds r15, r0, r1
}

TEST_F(ExecuteTest, ADDS_I32) {
  registers_.current.user.gprs.r0 = 0xFFFFFFFFu;
  RunInstruction("0x010090E2");  // adds r0, r0, #1
  EXPECT_EQ(0u, registers_.current.user.gprs.r0);
  EXPECT_TRUE(registers_.current.user.cpsr.zero);
  EXPECT_TRUE(registers_.current.user.cpsr.carry);

  // Modifies PC
  RunInstruction("0x01F090E2");  // adds r15, r0, #1
}

TEST_F(ExecuteTest, AND) {
  registers_.current.user.gprs.r0 = 0xFF00FF00u;
  registers_.current.user.gprs.r1 = 0x77777777u;
  RunInstruction("0x010000E0");  // and r0, r0, r1
  EXPECT_EQ(0x77007700u, registers_.current.user.gprs.r0);

  // Modifies PC
  RunInstruction("0x01F000E0");  // and r15, r0, r1
}

TEST_F(ExecuteTest, AND_I32) {
  registers_.current.user.gprs.r0 = 0x77777777u;
  RunInstruction("0xFF0000E2");  // and r0, r0, #0xFF
  EXPECT_EQ(0x77u, registers_.current.user.gprs.r0);

  // Modifies PC
  RunInstruction("0xFFF000E2");  // and r15, r0, #0xFF
}

TEST_F(ExecuteTest, ANDS) {
  registers_.current.user.gprs.r0 = 0xFF00FF00u;
  registers_.current.user.gprs.r1 = 0xF7777777u;
  RunInstruction("0x810010E0");  // ands r0, r0, r1, lsl #1
  EXPECT_EQ(0xEE00EE00u, registers_.current.user.gprs.r0);
  EXPECT_TRUE(registers_.current.user.cpsr.carry);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);

  // Modifies PC
  RunInstruction("0x81F010E0");  // ands r15, r0, r1, lsl #1
}

TEST_F(ExecuteTest, ANDS_I32) {
  registers_.current.user.gprs.r0 = 0x77777700u;
  RunInstruction("0xFF0010E2");  // ands r0, r0, #0xFF
  EXPECT_EQ(0u, registers_.current.user.gprs.r0);
  EXPECT_TRUE(registers_.current.user.cpsr.zero);

  // Modifies PC
  RunInstruction("0xFFF010E2");  // ands r15, r0, #0xFF
}

TEST_F(ExecuteTest, B_FWD) {
  RunInstruction("0x3E0000EA");  // b #0x100
  EXPECT_EQ(0x200u, ArmCurrentInstruction(&registers_));
}

TEST_F(ExecuteTest, B_REV) {
  RunInstruction("0xFDFFFFEA");  // b #-4
  EXPECT_EQ(0x0FCu, ArmCurrentInstruction(&registers_));
}

TEST_F(ExecuteTest, BIC) {
  registers_.current.user.gprs.r0 = 0xFF00FF00u;
  registers_.current.user.gprs.r1 = 0x88888888u;
  RunInstruction("0x0100C0E1");  // bic r0, r0, r1
  EXPECT_EQ(0x77007700u, registers_.current.user.gprs.r0);

  // Modifies PC
  RunInstruction("0x01F0C0E1");  // bic r15, r0, r1
}

TEST_F(ExecuteTest, BIC_I32) {
  registers_.current.user.gprs.r0 = 0x77777777u;
  RunInstruction("0xFF00C0E3");  // bic r0, r0, #0xFF
  EXPECT_EQ(0x77777700u, registers_.current.user.gprs.r0);

  // Modifies PC
  RunInstruction("0xFFF0C0E3");  // bic r15, r0, #0xFF
}

TEST_F(ExecuteTest, BICS) {
  registers_.current.user.gprs.r0 = 0xFF00FF00u;
  registers_.current.user.gprs.r1 = 0xF7777777u;
  RunInstruction("0x8100D0E1");  // bics r0, r0, r1, lsl #1
  EXPECT_EQ(0x11001100u, registers_.current.user.gprs.r0);
  EXPECT_TRUE(registers_.current.user.cpsr.carry);

  // Modifies PC
  RunInstruction("0x81F0D0E1");  // bics r15, r0, r1, lsl #1
}

TEST_F(ExecuteTest, BICS_I32) {
  registers_.current.user.gprs.r0 = 0x000000FFu;
  RunInstruction("0xFF00D0E3");  // bics r0, r0, #0xFF
  EXPECT_EQ(0u, registers_.current.user.gprs.r0);
  EXPECT_TRUE(registers_.current.user.cpsr.zero);

  // Modifies PC
  RunInstruction("0xFFF0D0E3");  // bics r15, r0, #0xFF
}

TEST_F(ExecuteTest, BL_FWD) {
  RunInstruction("0x3E0000EB");  // bl #0x100
  EXPECT_EQ(0x200u, ArmCurrentInstruction(&registers_));
  EXPECT_EQ(0x104u, registers_.current.user.gprs.lr);
}

TEST_F(ExecuteTest, BL_REV) {
  RunInstruction("0xFDFFFFEB");  // bl #-4
  EXPECT_EQ(0x0FCu, ArmCurrentInstruction(&registers_));
  EXPECT_EQ(0x104u, registers_.current.user.gprs.lr);
}

TEST_F(ExecuteTest, BX) {
  registers_.current.user.gprs.r0 = 0x80u;
  RunInstruction("0x10FF2FE1");  // bx r0
  EXPECT_EQ(0x80u, ArmCurrentInstruction(&registers_));
}

TEST_F(ExecuteTest, CDP) {
  RunInstruction("0x83C52AEE");  // cdp p5, 2, c12, c10, c3, 4
  EXPECT_EQ(MODE_UND, registers_.current.user.cpsr.mode);
}

TEST_F(ExecuteTest, CMN) {
  registers_.current.user.gprs.r0 = 0xFFFFFFFFu;
  registers_.current.user.gprs.r1 = 0x1u;
  RunInstruction("0x010070E1");  // cmn r0, r1
  EXPECT_TRUE(registers_.current.user.cpsr.carry);
  EXPECT_TRUE(registers_.current.user.cpsr.zero);
}

TEST_F(ExecuteTest, CMN_I32) {
  registers_.current.user.gprs.r0 = 0xFFFFFFFFu;
  RunInstruction("0x010070E3");  // cmn r0, #1
  EXPECT_TRUE(registers_.current.user.cpsr.carry);
  EXPECT_TRUE(registers_.current.user.cpsr.zero);
}

TEST_F(ExecuteTest, CMP) {
  registers_.current.user.gprs.r0 = 0x0u;
  RunInstruction("0x010050E1");  // cmp r0, r1
  EXPECT_TRUE(registers_.current.user.cpsr.zero);
}

TEST_F(ExecuteTest, CMP_I32) {
  registers_.current.user.gprs.r0 = 0x1u;
  RunInstruction("0x010050E3");  // cmp r0, #1
  EXPECT_TRUE(registers_.current.user.cpsr.zero);
}

TEST_F(ExecuteTest, EOR) {
  registers_.current.user.gprs.r0 = 0xFF00FF00u;
  registers_.current.user.gprs.r1 = 0x77777777u;
  RunInstruction("0x010020E0");  // eor r0, r0, r1
  EXPECT_EQ(0x88778877u, registers_.current.user.gprs.r0);

  // Modifies PC
  RunInstruction("0x01F020E0");  // eor r15, r0, r1
}

TEST_F(ExecuteTest, EOR_I32) {
  registers_.current.user.gprs.r0 = 0xFF00FF00u;
  RunInstruction("0xFF0020E2");  // eor r0, r0, #0xFF
  EXPECT_EQ(0xFF00FFFFu, registers_.current.user.gprs.r0);

  // Modifies PC
  RunInstruction("0xFFF020E2");  // eor r15, r0, #0xFF
}

TEST_F(ExecuteTest, EORS) {
  registers_.current.user.gprs.r0 = 0xFF00FF00u;
  registers_.current.user.gprs.r1 = 0xF7777777u;
  RunInstruction("0x810030E0");  // eors r0, r0, r1, lsl #1
  EXPECT_EQ(0x11EE11EEu, registers_.current.user.gprs.r0);
  EXPECT_TRUE(registers_.current.user.cpsr.carry);

  // Modifies PC
  RunInstruction("0x81F030E0");  // eors r15, r0, r1, lsl #1
}

TEST_F(ExecuteTest, EORS_I32) {
  registers_.current.user.gprs.r0 = 0xFF00FF00u;
  RunInstruction("0xFF0030E2");  // eors r0, r0, #0xFF
  EXPECT_EQ(0xFF00FFFFu, registers_.current.user.gprs.r0);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);

  // Modifies PC
  RunInstruction("0xFFF030E2");  // eors r15, r0, #0xFF
}

TEST_F(ExecuteTest, LDC) {
  RunInstruction("0x001694ED");  // ldc p6, cr1, [r4]
  EXPECT_EQ(MODE_UND, registers_.current.user.cpsr.mode);
}

TEST_F(ExecuteTest, LDMDA) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 1u));
  ASSERT_TRUE(Store32LE(nullptr, 0x304, 2u));
  ASSERT_TRUE(Store32LE(nullptr, 0x308, 3u));
  registers_.current.user.gprs.r0 = 0x308u;
  RunInstruction("0x070010E8");  // ldmda r0, {r0-r2}
  EXPECT_EQ(1u, registers_.current.user.gprs.r0);
  EXPECT_EQ(2u, registers_.current.user.gprs.r1);
  EXPECT_EQ(3u, registers_.current.user.gprs.r2);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x308u;
  RunInstruction("0x038010E8");  // ldmda r0, {r0, r1, pc}
}

TEST_F(ExecuteTest, LDMDA_W) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 1u));
  ASSERT_TRUE(Store32LE(nullptr, 0x304, 2u));
  ASSERT_TRUE(Store32LE(nullptr, 0x308, 3u));
  registers_.current.user.gprs.r0 = 0x308u;
  RunInstruction("0x0E0030E8");  // ldmda r0!, {r1-r3}
  EXPECT_EQ(0x2FCu, registers_.current.user.gprs.r0);
  EXPECT_EQ(1u, registers_.current.user.gprs.r1);
  EXPECT_EQ(2u, registers_.current.user.gprs.r2);
  EXPECT_EQ(3u, registers_.current.user.gprs.r3);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x308u;
  RunInstruction("0x038010E8");  // ldmda r0!, {r1, r2, pc}

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0x07003FE8");  // ldmda pc!, {r0-r2}
}

TEST_F(ExecuteTest, LDMDB) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 1u));
  ASSERT_TRUE(Store32LE(nullptr, 0x304, 2u));
  ASSERT_TRUE(Store32LE(nullptr, 0x308, 3u));
  registers_.current.user.gprs.r0 = 0x30Cu;
  RunInstruction("0x070010E9");  // ldmdb r0, {r0-r2}
  EXPECT_EQ(1u, registers_.current.user.gprs.r0);
  EXPECT_EQ(2u, registers_.current.user.gprs.r1);
  EXPECT_EQ(3u, registers_.current.user.gprs.r2);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x30Cu;
  RunInstruction("0x038010E9");  // ldmdb r0, {r0, r1, pc}
}

TEST_F(ExecuteTest, LDMDB_W) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 1u));
  ASSERT_TRUE(Store32LE(nullptr, 0x304, 2u));
  ASSERT_TRUE(Store32LE(nullptr, 0x308, 3u));
  registers_.current.user.gprs.r0 = 0x30Cu;
  RunInstruction("0x0E0030E9");  // ldmdb r0!, {r1-r3}
  EXPECT_EQ(0x300u, registers_.current.user.gprs.r0);
  EXPECT_EQ(1u, registers_.current.user.gprs.r1);
  EXPECT_EQ(2u, registers_.current.user.gprs.r2);
  EXPECT_EQ(3u, registers_.current.user.gprs.r3);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x30Cu;
  RunInstruction("0x068030E9");  // ldmdb r0!, {r1, r2, pc}

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0x07003FE9");  // ldmdb pc!, {r0-r2}
}

TEST_F(ExecuteTest, LDMIA) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 1u));
  ASSERT_TRUE(Store32LE(nullptr, 0x304, 2u));
  ASSERT_TRUE(Store32LE(nullptr, 0x308, 3u));
  registers_.current.user.gprs.r0 = 0x300u;
  RunInstruction("0x070090E8");  // ldmia r0, {r0-r2}
  EXPECT_EQ(1u, registers_.current.user.gprs.r0);
  EXPECT_EQ(2u, registers_.current.user.gprs.r1);
  EXPECT_EQ(3u, registers_.current.user.gprs.r2);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x300u;
  RunInstruction("0x038010E9");  // ldmia r0, {r0, r1, pc}
}

TEST_F(ExecuteTest, LDMIA_W) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 1u));
  ASSERT_TRUE(Store32LE(nullptr, 0x304, 2u));
  ASSERT_TRUE(Store32LE(nullptr, 0x308, 3u));
  registers_.current.user.gprs.r0 = 0x300u;
  RunInstruction("0x0E00B0E8");  // ldmia r0!, {r1-r3}
  EXPECT_EQ(0x30Cu, registers_.current.user.gprs.r0);
  EXPECT_EQ(1u, registers_.current.user.gprs.r1);
  EXPECT_EQ(2u, registers_.current.user.gprs.r2);
  EXPECT_EQ(3u, registers_.current.user.gprs.r3);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x300u;
  RunInstruction("0x0680B0E8");  // ldmia r0!, {r1, r2, pc}

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0x0700BFE8");  // ldmia pc!, {r0-r2}
}

TEST_F(ExecuteTest, LDMIB) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 1u));
  ASSERT_TRUE(Store32LE(nullptr, 0x304, 2u));
  ASSERT_TRUE(Store32LE(nullptr, 0x308, 3u));
  registers_.current.user.gprs.r0 = 0x2FCu;
  RunInstruction("0x070090E9");  // ldmib r0, {r0-r2}
  EXPECT_EQ(1u, registers_.current.user.gprs.r0);
  EXPECT_EQ(2u, registers_.current.user.gprs.r1);
  EXPECT_EQ(3u, registers_.current.user.gprs.r2);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x2FCu;
  RunInstruction("0x038090E9");  // ldmib r0, {r0, r1, pc}
}

TEST_F(ExecuteTest, LDMIB_W) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 1u));
  ASSERT_TRUE(Store32LE(nullptr, 0x304, 2u));
  ASSERT_TRUE(Store32LE(nullptr, 0x308, 3u));
  registers_.current.user.gprs.r0 = 0x2FCu;
  RunInstruction("0x0E00B0E9");  // ldmib r0!, {r1-r3}
  EXPECT_EQ(0x308u, registers_.current.user.gprs.r0);
  EXPECT_EQ(1u, registers_.current.user.gprs.r1);
  EXPECT_EQ(2u, registers_.current.user.gprs.r2);
  EXPECT_EQ(3u, registers_.current.user.gprs.r3);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x2FCu;
  RunInstruction("0x0680B0E9");  // ldmib r0!, {r1, r2, pc}

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0x0700BFE9");  // ldmib pc!, {r0-r2}
}

TEST_F(ExecuteTest, LDMSDA) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 1u));
  ASSERT_TRUE(Store32LE(nullptr, 0x304, 2u));
  ASSERT_TRUE(Store32LE(nullptr, 0x308, 3u));
  registers_.current.user.gprs.r0 = 0x308u;
  RunInstruction("0x0E0050E8");  // ldmda r0, {r1-r3}^
  EXPECT_EQ(1u, registers_.current.user.gprs.r1);
  EXPECT_EQ(2u, registers_.current.user.gprs.r2);
  EXPECT_EQ(3u, registers_.current.user.gprs.r3);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x308u;
  RunInstruction("0x068050E8");  // ldmda r0, {r1, r2, pc}^
  EXPECT_EQ(MODE_USR, registers_.current.user.cpsr.mode);
}

TEST_F(ExecuteTest, LDMSDA_W) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 1u));
  ASSERT_TRUE(Store32LE(nullptr, 0x304, 2u));
  ASSERT_TRUE(Store32LE(nullptr, 0x308, 3u));
  registers_.current.user.gprs.r0 = 0x308u;
  RunInstruction("0x0E0070E8");  // ldmda r0!, {r1-r3}^
  EXPECT_EQ(0x2FCu, registers_.current.user.gprs.r0);
  EXPECT_EQ(1u, registers_.current.user.gprs.r1);
  EXPECT_EQ(2u, registers_.current.user.gprs.r2);
  EXPECT_EQ(3u, registers_.current.user.gprs.r3);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x308u;
  RunInstruction("0x068070E8");  // ldmda r0!, {r1, r2, pc}^
  EXPECT_EQ(MODE_USR, registers_.current.user.cpsr.mode);

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0x0E007FE8");  // ldmda pc!, {r1-r3}^
}

TEST_F(ExecuteTest, LDMSDB) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 1u));
  ASSERT_TRUE(Store32LE(nullptr, 0x304, 2u));
  ASSERT_TRUE(Store32LE(nullptr, 0x308, 3u));
  registers_.current.user.gprs.r0 = 0x30Cu;
  RunInstruction("0x0E0050E9");  // ldmdb r0, {r1-r3}^
  EXPECT_EQ(1u, registers_.current.user.gprs.r1);
  EXPECT_EQ(2u, registers_.current.user.gprs.r2);
  EXPECT_EQ(3u, registers_.current.user.gprs.r3);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x30Cu;
  RunInstruction("0x068050E9");  // ldmdb r0, {r1, r2, pc}^
  EXPECT_EQ(MODE_USR, registers_.current.user.cpsr.mode);
}

TEST_F(ExecuteTest, LDMSDB_W) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 1u));
  ASSERT_TRUE(Store32LE(nullptr, 0x304, 2u));
  ASSERT_TRUE(Store32LE(nullptr, 0x308, 3u));
  registers_.current.user.gprs.r0 = 0x30Cu;
  RunInstruction("0x0E0070E9");  // ldmdb r0!, {r1-r3}^
  EXPECT_EQ(0x300u, registers_.current.user.gprs.r0);
  EXPECT_EQ(1u, registers_.current.user.gprs.r1);
  EXPECT_EQ(2u, registers_.current.user.gprs.r2);
  EXPECT_EQ(3u, registers_.current.user.gprs.r3);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x30Cu;
  RunInstruction("0x068070E9");  // ldmdb r0!, {r1, r2, pc}^
  EXPECT_EQ(MODE_USR, registers_.current.user.cpsr.mode);

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0x0E007FE9");  // ldmdb pc!, {r1-r3}^
}

TEST_F(ExecuteTest, LDMSIA) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 1u));
  ASSERT_TRUE(Store32LE(nullptr, 0x304, 2u));
  ASSERT_TRUE(Store32LE(nullptr, 0x308, 3u));
  registers_.current.user.gprs.r0 = 0x300u;
  RunInstruction("0x0E00D0E8");  // ldmia r0, {r1-r3}^
  EXPECT_EQ(1u, registers_.current.user.gprs.r1);
  EXPECT_EQ(2u, registers_.current.user.gprs.r2);
  EXPECT_EQ(3u, registers_.current.user.gprs.r3);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x300u;
  RunInstruction("0x0380D0E8");  // ldmia r0, {r0, r1, pc}^
  EXPECT_EQ(MODE_USR, registers_.current.user.cpsr.mode);
}

TEST_F(ExecuteTest, LDMSIA_W) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 1u));
  ASSERT_TRUE(Store32LE(nullptr, 0x304, 2u));
  ASSERT_TRUE(Store32LE(nullptr, 0x308, 3u));
  registers_.current.user.gprs.r0 = 0x300u;
  RunInstruction("0x0E00F0E8");  // ldmia r0!, {r1-r3}^
  EXPECT_EQ(0x30Cu, registers_.current.user.gprs.r0);
  EXPECT_EQ(1u, registers_.current.user.gprs.r1);
  EXPECT_EQ(2u, registers_.current.user.gprs.r2);
  EXPECT_EQ(3u, registers_.current.user.gprs.r3);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x300u;
  RunInstruction("0x0680F0E8");  // ldmia r0!, {r1, r2, pc}^
  EXPECT_EQ(MODE_USR, registers_.current.user.cpsr.mode);

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0x0E00FFE8");  // ldmia pc!, {r1-r3}^
}

TEST_F(ExecuteTest, LDMSIB) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 1u));
  ASSERT_TRUE(Store32LE(nullptr, 0x304, 2u));
  ASSERT_TRUE(Store32LE(nullptr, 0x308, 3u));
  registers_.current.user.gprs.r0 = 0x2FCu;
  RunInstruction("0x0E00D0E9");  // ldmib r0, {r1-r3}^
  EXPECT_EQ(1u, registers_.current.user.gprs.r1);
  EXPECT_EQ(2u, registers_.current.user.gprs.r2);
  EXPECT_EQ(3u, registers_.current.user.gprs.r3);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x2FCu;
  RunInstruction("0x0380D0E9");  // ldmib r0, {r0, r1, pc}^
  EXPECT_EQ(MODE_USR, registers_.current.user.cpsr.mode);
}

TEST_F(ExecuteTest, LDMSIB_W) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 1u));
  ASSERT_TRUE(Store32LE(nullptr, 0x304, 2u));
  ASSERT_TRUE(Store32LE(nullptr, 0x308, 3u));
  registers_.current.user.gprs.r0 = 0x2FCu;
  RunInstruction("0x0E00F0E9");  // ldmib r0!, {r1-r3}^
  EXPECT_EQ(0x308u, registers_.current.user.gprs.r0);
  EXPECT_EQ(1u, registers_.current.user.gprs.r1);
  EXPECT_EQ(2u, registers_.current.user.gprs.r2);
  EXPECT_EQ(3u, registers_.current.user.gprs.r3);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x2FCu;
  RunInstruction("0x0680F0E9");  // ldmib r0!, {r1, r2, pc}^
  EXPECT_EQ(MODE_USR, registers_.current.user.cpsr.mode);

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0x0E00FFE9");  // ldmib pc!, {r1-r3}^
}

TEST_F(ExecuteTest, LDR_DAW) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 1u));
  registers_.current.user.gprs.r0 = 0x300u;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0x012010E6");  // ldr r2, [r0], -r1
  EXPECT_EQ(0x2FCu, registers_.current.user.gprs.r0);
  EXPECT_EQ(1u, registers_.current.user.gprs.r2);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x300u;
  RunInstruction("0x01F010E6");  // ldr pc, [r0], -r1

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0x0E00FFE9");  // ldr r0, [pc], -r1
}

TEST_F(ExecuteTest, LDR_DAW_I12) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 1u));
  registers_.current.user.gprs.r0 = 0x300u;

  RunInstruction("0x042010E4");  // ldr r2, [r0], #-4
  EXPECT_EQ(0x2FCu, registers_.current.user.gprs.r0);
  EXPECT_EQ(1u, registers_.current.user.gprs.r2);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x300u;
  RunInstruction("0x04F010E4");  // ldr pc, [r0], #-4

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0x04001FE4");  // ldr r0, [pc], #-4
}

TEST_F(ExecuteTest, LDR_DB) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 1u));
  registers_.current.user.gprs.r0 = 0x304u;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0x012010E7");  // ldr r2, [r0, -r1]
  EXPECT_EQ(1u, registers_.current.user.gprs.r2);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x304u;
  RunInstruction("0x01F010E7");  // ldr pc, [r0, -r1]
}

TEST_F(ExecuteTest, LDR_DB_I12) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 1u));
  registers_.current.user.gprs.r0 = 0x304u;

  RunInstruction("0x042010E5");  // ldr r2, [r0, #-4]
  EXPECT_EQ(1u, registers_.current.user.gprs.r2);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x304u;
  RunInstruction("0x04F010E5");  // ldr pc, [r0, #-4]
}

TEST_F(ExecuteTest, LDR_DBW) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 1u));
  registers_.current.user.gprs.r0 = 0x304u;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0x012030E7");  // ldr r2, [r0, -r1]!
  EXPECT_EQ(0x300u, registers_.current.user.gprs.r0);
  EXPECT_EQ(1u, registers_.current.user.gprs.r2);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x304u;
  RunInstruction("0x01F030E7");  // ldr pc, [r0, -r1]!

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0x01003FE7");  // ldr r0, [pc, -r1]!
}

TEST_F(ExecuteTest, LDR_DBW_I12) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 1u));
  registers_.current.user.gprs.r0 = 0x304u;

  RunInstruction("0x042030E5");  // ldr r2, [r0, #-4]!
  EXPECT_EQ(0x300u, registers_.current.user.gprs.r0);
  EXPECT_EQ(1u, registers_.current.user.gprs.r2);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x304u;
  RunInstruction("0x04F030E5");  // ldr pc, [r0, #-4]!

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0x04003FE5");  // ldr r0, [pc, #-4]!
}

TEST_F(ExecuteTest, LDR_IAW) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 1u));
  registers_.current.user.gprs.r0 = 0x300u;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0x012090E6");  // ldr r2, [r0], r1
  EXPECT_EQ(0x304u, registers_.current.user.gprs.r0);
  EXPECT_EQ(1u, registers_.current.user.gprs.r2);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x304u;
  RunInstruction("0x01F090E6");  // ldr pc, [r0], r1

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0x01009FE6");  // ldr r0, [pc], r1
}

TEST_F(ExecuteTest, LDR_IAW_I12) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 1u));
  registers_.current.user.gprs.r0 = 0x300u;

  RunInstruction("0x042090E4");  // ldr r2, [r0], #4
  EXPECT_EQ(0x304u, registers_.current.user.gprs.r0);
  EXPECT_EQ(1u, registers_.current.user.gprs.r2);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x300u;
  RunInstruction("0x04F090E4");  // ldr pc, [r0], #4

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0x04009FE4");  // ldr r0, [pc], #4
}

TEST_F(ExecuteTest, LDR_IB) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 1u));
  registers_.current.user.gprs.r0 = 0x2FC;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0x012090E7");  // ldr r2, [r0, r1]
  EXPECT_EQ(1u, registers_.current.user.gprs.r2);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x2FC;
  RunInstruction("0x01F090E7");  // ldr pc, [r0, r1]
}

TEST_F(ExecuteTest, LDR_IB_I12) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 1u));
  registers_.current.user.gprs.r0 = 0x2FC;

  RunInstruction("0x042090E5");  // ldr r2, [r0, #4]
  EXPECT_EQ(1u, registers_.current.user.gprs.r2);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x2FC;
  RunInstruction("0x04F090E5");  // ldr pc, [r0, #4]
}

TEST_F(ExecuteTest, LDR_IBW) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 1u));
  registers_.current.user.gprs.r0 = 0x2FCu;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0x0120B0E7");  // ldr r2, [r0, r1]!
  EXPECT_EQ(0x300u, registers_.current.user.gprs.r0);
  EXPECT_EQ(1u, registers_.current.user.gprs.r2);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x2FCu;
  RunInstruction("0x01F0B0E7");  // ldr pc, [r0, r1]!

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0x0100BFE7");  // ldr r0, [pc, r1]!
}

TEST_F(ExecuteTest, LDR_IBW_I12) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 1u));
  registers_.current.user.gprs.r0 = 0x2FCu;

  RunInstruction("0x0420B0E5");  // ldr r2, [r0, #4]!
  EXPECT_EQ(0x300u, registers_.current.user.gprs.r0);
  EXPECT_EQ(1u, registers_.current.user.gprs.r2);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x2FCu;
  RunInstruction("0x04F0B0E5");  // ldr pc, [r0, #4]!

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0x0400BFE5");  // ldr r0, [pc, #4]!
}

TEST_F(ExecuteTest, LDRB_DAW) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 0x01010101u));
  registers_.current.user.gprs.r0 = 0x300u;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0x012050E6");  // ldrb r2, [r0], -r1
  EXPECT_EQ(0x2FCu, registers_.current.user.gprs.r0);
  EXPECT_EQ(1u, registers_.current.user.gprs.r2);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x300u;
  RunInstruction("0x01F050E6");  // ldrb pc, [r0], -r1

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0x01005FE6");  // ldrb r0, [pc], -r1
}

TEST_F(ExecuteTest, LDRB_DAW_I12) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 0x01010101u));
  registers_.current.user.gprs.r0 = 0x300u;

  RunInstruction("0x042050E4");  // ldrb r2, [r0], #-4
  EXPECT_EQ(0x2FCu, registers_.current.user.gprs.r0);
  EXPECT_EQ(1u, registers_.current.user.gprs.r2);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x300u;
  RunInstruction("0x04F050E4");  // ldrb pc, [r0], #-4

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0x04005FE4");  // ldrb r0, [pc], #-4
}

TEST_F(ExecuteTest, LDRB_DB) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 0x01010101u));
  registers_.current.user.gprs.r0 = 0x304u;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0x012050E7");  // ldrb r2, [r0, -r1]
  EXPECT_EQ(1u, registers_.current.user.gprs.r2);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x304u;
  RunInstruction("0x01F050E7");  // ldrb pc, [r0, -r1]
}

TEST_F(ExecuteTest, LDRB_DB_I12) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 0x01010101u));
  registers_.current.user.gprs.r0 = 0x304u;

  RunInstruction("0x042050E5");  // ldrb r2, [r0, #-4]
  EXPECT_EQ(1u, registers_.current.user.gprs.r2);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x304u;
  RunInstruction("0x04F050E5");  // ldrb pc, [r0, #-4]
}

TEST_F(ExecuteTest, LDRB_DBW) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 0x01010101u));
  registers_.current.user.gprs.r0 = 0x304u;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0x012070E7");  // ldrb r2, [r0, -r1]!
  EXPECT_EQ(0x300u, registers_.current.user.gprs.r0);
  EXPECT_EQ(1u, registers_.current.user.gprs.r2);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x304u;
  RunInstruction("0x01F070E7");  // ldrb pc, [r0, -r1]!

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0x01007FE7");  // ldrb r0, [pc, -r1]!
}

TEST_F(ExecuteTest, LDRB_DBW_I12) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 0x01010101u));
  registers_.current.user.gprs.r0 = 0x304u;

  RunInstruction("0x042070E5");  // ldrb r2, [r0, #-4]!
  EXPECT_EQ(0x300u, registers_.current.user.gprs.r0);
  EXPECT_EQ(1u, registers_.current.user.gprs.r2);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x304u;
  RunInstruction("0x04F070E5");  // ldrb pc, [r0, #-4]!

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0x04007FE5");  // ldrb r0, [pc, #-4]!
}

TEST_F(ExecuteTest, LDRB_IAW) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 0x01010101u));
  registers_.current.user.gprs.r0 = 0x300u;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0x0120D0E6");  // ldrb r2, [r0], r1
  EXPECT_EQ(0x304u, registers_.current.user.gprs.r0);
  EXPECT_EQ(1u, registers_.current.user.gprs.r2);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x304u;
  RunInstruction("0x01F0D0E6");  // ldrb pc, [r0], r1

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0x0100DFE6");  // ldrb r0, [pc], r1
}

TEST_F(ExecuteTest, LDRB_IAW_I12) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 0x01010101u));
  registers_.current.user.gprs.r0 = 0x300u;

  RunInstruction("0x0420D0E4");  // ldrb r2, [r0], #4
  EXPECT_EQ(0x304u, registers_.current.user.gprs.r0);
  EXPECT_EQ(1u, registers_.current.user.gprs.r2);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x300u;
  RunInstruction("0x04F0D0E4");  // ldrb pc, [r0], #4

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0x0400DFE4");  // ldrb r0, [pc], #4
}

TEST_F(ExecuteTest, LDRB_IB) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 0x01010101u));
  registers_.current.user.gprs.r0 = 0x2FC;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0x0120D0E7");  // ldrb r2, [r0, r1]
  EXPECT_EQ(1u, registers_.current.user.gprs.r2);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x2FC;
  RunInstruction("0x01F0D0E7");  // ldrb pc, [r0, r1]
}

TEST_F(ExecuteTest, LDRB_IB_I12) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 0x01010101u));
  registers_.current.user.gprs.r0 = 0x2FC;

  RunInstruction("0x0420D0E5");  // ldrb r2, [r0, #4]
  EXPECT_EQ(1u, registers_.current.user.gprs.r2);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x2FC;
  RunInstruction("0x04F0D0E5");  // ldrb pc, [r0, #4]
}

TEST_F(ExecuteTest, LDRB_IBW) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 0x01010101u));
  registers_.current.user.gprs.r0 = 0x2FCu;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0x0120F0E7");  // ldrb r2, [r0, r1]!
  EXPECT_EQ(0x300u, registers_.current.user.gprs.r0);
  EXPECT_EQ(1u, registers_.current.user.gprs.r2);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x2FCu;
  RunInstruction("0x01F0F0E7");  // ldrb pc, [r0, r1]!

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0x0100FFE7");  // ldrb r0, [pc, r1]!
}

TEST_F(ExecuteTest, LDRB_IBW_I12) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 0x01010101u));
  registers_.current.user.gprs.r0 = 0x2FCu;

  RunInstruction("0x0420F0E5");  // ldrb r2, [r0, #4]!
  EXPECT_EQ(0x300u, registers_.current.user.gprs.r0);
  EXPECT_EQ(1u, registers_.current.user.gprs.r2);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x2FCu;
  RunInstruction("0x04F0F0E5");  // ldrb pc, [r0, #4]!

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0x0400FFE5");  // ldrb r0, [pc, #4]!
}

TEST_F(ExecuteTest, LDRBT_DAW) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 0x01010101u));
  registers_.current.user.gprs.r0 = 0x300u;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0x012070E6");  // ldrbt r2, [r0], -r1
  EXPECT_EQ(0x2FCu, registers_.current.user.gprs.r0);
  EXPECT_EQ(1u, registers_.current.user.gprs.r2);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x300u;
  RunInstruction("0x01F070E6");  // ldrbt pc, [r0], -r1

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0x01007FE6");  // ldrbt r0, [pc], -r1
}

TEST_F(ExecuteTest, LDRBT_DAW_I12) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 0x01010101u));
  registers_.current.user.gprs.r0 = 0x300u;

  RunInstruction("0x042070E4");  // ldrbt r2, [r0], #-4
  EXPECT_EQ(0x2FCu, registers_.current.user.gprs.r0);
  EXPECT_EQ(1u, registers_.current.user.gprs.r2);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x300u;
  RunInstruction("0x04F070E4");  // ldrbt pc, [r0], #-4

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0x04007FE4");  // ldrbt r0, [pc], #-4
}

TEST_F(ExecuteTest, LDRBT_IAW) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 0x01010101u));
  registers_.current.user.gprs.r0 = 0x300u;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0x0120F0E6");  // ldrbt r2, [r0], r1
  EXPECT_EQ(0x304u, registers_.current.user.gprs.r0);
  EXPECT_EQ(1u, registers_.current.user.gprs.r2);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x304u;
  RunInstruction("0x01F0F0E6");  // ldrbt pc, [r0], r1

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0x0100FFE6");  // ldrbt r0, [pc], r1
}

TEST_F(ExecuteTest, LDRBT_IAW_I12) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 0x01010101u));
  registers_.current.user.gprs.r0 = 0x300u;

  RunInstruction("0x0420F0E4");  // ldrbt r2, [r0], #4
  EXPECT_EQ(0x304u, registers_.current.user.gprs.r0);
  EXPECT_EQ(1u, registers_.current.user.gprs.r2);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x300u;
  RunInstruction("0x04F0F0E4");  // ldrbt pc, [r0], #4

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0x0400FFE4");  // ldrbt r0, [pc], #4
}

TEST_F(ExecuteTest, LDRH_DAW) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 0x01010101u));
  registers_.current.user.gprs.r0 = 0x300u;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0xB12010E0");  // ldrh r2, [r0], -r1
  EXPECT_EQ(0x2FCu, registers_.current.user.gprs.r0);
  EXPECT_EQ(0x0101u, registers_.current.user.gprs.r2);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x300u;
  RunInstruction("0xB1F010E0");  // ldrh pc, [r0], -r1

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0xB1001FE0");  // ldrh r0, [pc], -r1
}

TEST_F(ExecuteTest, LDRH_DAW_I12) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 0x01010101u));
  registers_.current.user.gprs.r0 = 0x300u;

  RunInstruction("0xB42050E0");  // ldrh r2, [r0], #-4
  EXPECT_EQ(0x2FCu, registers_.current.user.gprs.r0);
  EXPECT_EQ(0x0101u, registers_.current.user.gprs.r2);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x300u;
  RunInstruction("0xB4F050E0");  // ldrh pc, [r0], #-4

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0xB4005FE0");  // ldrh r0, [pc], #-4
}

TEST_F(ExecuteTest, LDRH_DB) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 0x01010101u));
  registers_.current.user.gprs.r0 = 0x304u;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0xB12010E1");  // ldrh r2, [r0, -r1]
  EXPECT_EQ(0x0101u, registers_.current.user.gprs.r2);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x304u;
  RunInstruction("0xB1F010E1");  // ldrh pc, [r0, -r1]
}

TEST_F(ExecuteTest, LDRH_DB_I12) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 0x01010101u));
  registers_.current.user.gprs.r0 = 0x304u;

  RunInstruction("0xB42050E1");  // ldrh r2, [r0, #-4]
  EXPECT_EQ(0x0101u, registers_.current.user.gprs.r2);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x304u;
  RunInstruction("0xB4F050E1");  // ldrh pc, [r0, #-4]
}

TEST_F(ExecuteTest, LDRH_DBW) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 0x01010101u));
  registers_.current.user.gprs.r0 = 0x304u;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0xB12030E1");  // ldrh r2, [r0, -r1]!
  EXPECT_EQ(0x300u, registers_.current.user.gprs.r0);
  EXPECT_EQ(0x0101u, registers_.current.user.gprs.r2);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x304u;
  RunInstruction("0xB1F030E1");  // ldrh pc, [r0, -r1]!

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0xB1003FE1");  // ldrh r0, [pc, -r1]!
}

TEST_F(ExecuteTest, LDRH_DBW_I12) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 0x01010101u));
  registers_.current.user.gprs.r0 = 0x304u;

  RunInstruction("0xB42070E1");  // ldrh r2, [r0, #-4]!
  EXPECT_EQ(0x300u, registers_.current.user.gprs.r0);
  EXPECT_EQ(0x0101u, registers_.current.user.gprs.r2);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x304u;
  RunInstruction("0xB4F070E1");  // ldrh pc, [r0, #-4]!

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0xB4007FE1");  // ldrh r0, [pc, #-4]!
}

TEST_F(ExecuteTest, LDRH_IAW) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 0x01010101u));
  registers_.current.user.gprs.r0 = 0x300u;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0xB12090E0");  // ldrh r2, [r0], r1
  EXPECT_EQ(0x304u, registers_.current.user.gprs.r0);
  EXPECT_EQ(0x0101u, registers_.current.user.gprs.r2);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x304u;
  RunInstruction("0xB1F090E0");  // ldrh pc, [r0], r1

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0xB1009FE0");  // ldrh r0, [pc], r1
}

TEST_F(ExecuteTest, LDRH_IAW_I12) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 0x01010101u));
  registers_.current.user.gprs.r0 = 0x300u;

  RunInstruction("0xB420D0E0");  // ldrh r2, [r0], #4
  EXPECT_EQ(0x304u, registers_.current.user.gprs.r0);
  EXPECT_EQ(0x0101u, registers_.current.user.gprs.r2);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x300u;
  RunInstruction("0xB4F0D0E0");  // ldrh pc, [r0], #4

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0xB400DFE0");  // ldrh r0, [pc], #4
}

TEST_F(ExecuteTest, LDRH_IB) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 0x01010101u));
  registers_.current.user.gprs.r0 = 0x2FC;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0xB12090E1");  // ldrh r2, [r0, r1]
  EXPECT_EQ(0x0101u, registers_.current.user.gprs.r2);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x2FC;
  RunInstruction("0xB1F090E1");  // ldrh pc, [r0, r1]
}

TEST_F(ExecuteTest, LDRH_IB_I12) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 0x01010101u));
  registers_.current.user.gprs.r0 = 0x2FC;

  RunInstruction("0xB420D0E1");  // ldrh r2, [r0, #4]
  EXPECT_EQ(0x0101u, registers_.current.user.gprs.r2);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x2FC;
  RunInstruction("0xB4F0D0E1");  // ldrh pc, [r0, #4]
}

TEST_F(ExecuteTest, LDRH_IBW) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 0x01010101u));
  registers_.current.user.gprs.r0 = 0x2FCu;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0xB120B0E1");  // ldrh r2, [r0, r1]!
  EXPECT_EQ(0x300u, registers_.current.user.gprs.r0);
  EXPECT_EQ(0x0101u, registers_.current.user.gprs.r2);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x2FCu;
  RunInstruction("0xB1F0B0E1");  // ldrh pc, [r0, r1]!

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0xB100BFE1");  // ldrh r0, [pc, r1]!
}

TEST_F(ExecuteTest, LDRH_IBW_I12) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 0x01010101u));
  registers_.current.user.gprs.r0 = 0x2FCu;

  RunInstruction("0xB420F0E1");  // ldrh r2, [r0, #4]!
  EXPECT_EQ(0x300u, registers_.current.user.gprs.r0);
  EXPECT_EQ(0x0101u, registers_.current.user.gprs.r2);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x2FCu;
  RunInstruction("0xB4F0F0E1");  // ldrh pc, [r0, #4]!

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0xB400FFE1");  // ldrh r0, [pc, #4]!
}

TEST_F(ExecuteTest, LDRSB_DAW) {
  ASSERT_TRUE(Store32LE(memory_, 0x300, 0x000000FFu));
  registers_.current.user.gprs.r0 = 0x300u;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0xD12010E0");  // ldrsb r2, [r0], -r1
  EXPECT_EQ(0x2FCu, registers_.current.user.gprs.r0);
  EXPECT_EQ(-1, registers_.current.user.gprs.r2_s);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x300u;
  RunInstruction("0xD1F010E0");  // ldrsb pc, [r0], -r1

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0xD1001FE0");  // ldrsb r0, [pc], -r1
}

TEST_F(ExecuteTest, LDRSB_DAW_I12) {
  ASSERT_TRUE(Store32LE(memory_, 0x300, 0x000000FFu));
  registers_.current.user.gprs.r0 = 0x300u;

  RunInstruction("0xD42050E0");  // ldrsb r2, [r0], #-4
  EXPECT_EQ(0x2FCu, registers_.current.user.gprs.r0);
  EXPECT_EQ(-1, registers_.current.user.gprs.r2_s);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x300u;
  RunInstruction("0xD4F050E0");  // ldrsb pc, [r0], #-4

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0xD4005FE0");  // ldrsb r0, [pc], #-4
}

TEST_F(ExecuteTest, LDRSB_DB) {
  ASSERT_TRUE(Store32LE(memory_, 0x300, 0x000000FFu));
  registers_.current.user.gprs.r0 = 0x304u;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0xD12010E1");  // ldrsb r2, [r0, -r1]
  EXPECT_EQ(-1, registers_.current.user.gprs.r2_s);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x304u;
  RunInstruction("0xD1F010E1");  // ldrsb pc, [r0, -r1]
}

TEST_F(ExecuteTest, LDRSB_DB_I12) {
  ASSERT_TRUE(Store32LE(memory_, 0x300, 0x000000FFu));
  registers_.current.user.gprs.r0 = 0x304u;

  RunInstruction("0xD42050E1");  // ldrsb r2, [r0, #-4]
  EXPECT_EQ(-1, registers_.current.user.gprs.r2_s);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x304u;
  RunInstruction("0xD4F050E1");  // ldrsb pc, [r0, #-4]
}

TEST_F(ExecuteTest, LDRSB_DBW) {
  ASSERT_TRUE(Store32LE(memory_, 0x300, 0x000000FFu));
  registers_.current.user.gprs.r0 = 0x304u;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0xD12030E1");  // ldrsb r2, [r0, -r1]!
  EXPECT_EQ(0x300u, registers_.current.user.gprs.r0);
  EXPECT_EQ(-1, registers_.current.user.gprs.r2_s);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x304u;
  RunInstruction("0xD1F030E1");  // ldrsb pc, [r0, -r1]!

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0xD1003FE1");  // ldrsb r0, [pc, -r1]!
}

TEST_F(ExecuteTest, LDRSB_DBW_I12) {
  ASSERT_TRUE(Store32LE(memory_, 0x300, 0x000000FFu));
  registers_.current.user.gprs.r0 = 0x304u;

  RunInstruction("0xD42070E1");  // ldrsb r2, [r0, #-4]!
  EXPECT_EQ(0x300u, registers_.current.user.gprs.r0);
  EXPECT_EQ(-1, registers_.current.user.gprs.r2_s);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x304u;
  RunInstruction("0xD4F070E1");  // ldrsb pc, [r0, #-4]!

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0xD4007FE1");  // ldrsb r0, [pc, #-4]!
}

TEST_F(ExecuteTest, LDRSB_IAW) {
  ASSERT_TRUE(Store32LE(memory_, 0x300, 0x000000FFu));
  registers_.current.user.gprs.r0 = 0x300u;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0xD12090E0");  // ldrsb r2, [r0], r1
  EXPECT_EQ(0x304u, registers_.current.user.gprs.r0);
  EXPECT_EQ(-1, registers_.current.user.gprs.r2_s);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x304u;
  RunInstruction("0xD1F090E0");  // ldrsb pc, [r0], r1

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0xD1009FE0");  // ldrsb r0, [pc], r1
}

TEST_F(ExecuteTest, LDRSB_IAW_I12) {
  ASSERT_TRUE(Store32LE(memory_, 0x300, 0x000000FFu));
  registers_.current.user.gprs.r0 = 0x300u;

  RunInstruction("0xD420D0E0");  // ldrsb r2, [r0], #4
  EXPECT_EQ(0x304u, registers_.current.user.gprs.r0);
  EXPECT_EQ(-1, registers_.current.user.gprs.r2_s);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x300u;
  RunInstruction("0xD4F0D0E0");  // ldrsb pc, [r0], #4

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0xD400DFE0");  // ldrsb r0, [pc], #4
}

TEST_F(ExecuteTest, LDRSB_IB) {
  ASSERT_TRUE(Store32LE(memory_, 0x300, 0x000000FFu));
  registers_.current.user.gprs.r0 = 0x2FC;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0xD12090E1");  // ldrsb r2, [r0, r1]
  EXPECT_EQ(-1, registers_.current.user.gprs.r2_s);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x2FC;
  RunInstruction("0xD1F090E1");  // ldrsb pc, [r0, r1]
}

TEST_F(ExecuteTest, LDRSB_IB_I12) {
  ASSERT_TRUE(Store32LE(memory_, 0x300, 0x000000FFu));
  registers_.current.user.gprs.r0 = 0x2FC;

  RunInstruction("0xD420D0E1");  // ldrsb r2, [r0, #4]
  EXPECT_EQ(-1, registers_.current.user.gprs.r2_s);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x2FC;
  RunInstruction("0xD4F0D0E1");  // ldrsb pc, [r0, #4]
}

TEST_F(ExecuteTest, LDRSB_IBW) {
  ASSERT_TRUE(Store32LE(memory_, 0x300, 0x000000FFu));
  registers_.current.user.gprs.r0 = 0x2FCu;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0xD120B0E1");  // ldrsb r2, [r0, r1]!
  EXPECT_EQ(0x300u, registers_.current.user.gprs.r0);
  EXPECT_EQ(-1, registers_.current.user.gprs.r2_s);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x2FCu;
  RunInstruction("0xD1F0B0E1");  // ldrsb pc, [r0, r1]!

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0xD100BFE1");  // ldrsb r0, [pc, r1]!
}

TEST_F(ExecuteTest, LDRSB_IBW_I12) {
  ASSERT_TRUE(Store32LE(memory_, 0x300, 0x000000FFu));
  registers_.current.user.gprs.r0 = 0x2FCu;

  RunInstruction("0xD420F0E1");  // ldrsb r2, [r0, #4]!
  EXPECT_EQ(0x300u, registers_.current.user.gprs.r0);
  EXPECT_EQ(-1, registers_.current.user.gprs.r2_s);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x2FCu;
  RunInstruction("0xD4F0F0E1");  // ldrsb pc, [r0, #4]!

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0xD400FFE1");  // ldrsb r0, [pc, #4]!
}

TEST_F(ExecuteTest, LDRSH_DAW) {
  ASSERT_TRUE(Store32LE(memory_, 0x300, 0x0000FFFFu));
  registers_.current.user.gprs.r0 = 0x300u;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0xF12010E0");  // ldrsh r2, [r0], -r1
  EXPECT_EQ(0x2FCu, registers_.current.user.gprs.r0);
  EXPECT_EQ(-1, registers_.current.user.gprs.r2_s);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x300u;
  RunInstruction("0xF1F010E0");  // ldrsh pc, [r0], -r1

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0xF1001FE0");  // ldrsh r0, [pc], -r1
}

TEST_F(ExecuteTest, LDRSH_DAW_I12) {
  ASSERT_TRUE(Store32LE(memory_, 0x300, 0x0000FFFFu));
  registers_.current.user.gprs.r0 = 0x300u;

  RunInstruction("0xF42050E0");  // ldrsh r2, [r0], #-4
  EXPECT_EQ(0x2FCu, registers_.current.user.gprs.r0);
  EXPECT_EQ(-1, registers_.current.user.gprs.r2_s);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x300u;
  RunInstruction("0xF4F050E0");  // ldrsh pc, [r0], #-4

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0xF4005FE0");  // ldrsh r0, [pc], #-4
}

TEST_F(ExecuteTest, LDRSH_DB) {
  ASSERT_TRUE(Store32LE(memory_, 0x300, 0x0000FFFFu));
  registers_.current.user.gprs.r0 = 0x304u;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0xF12010E1");  // ldrsh r2, [r0, -r1]
  EXPECT_EQ(-1, registers_.current.user.gprs.r2_s);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x304u;
  RunInstruction("0xF1F010E1");  // ldrsh pc, [r0, -r1]
}

TEST_F(ExecuteTest, LDRSH_DB_I12) {
  ASSERT_TRUE(Store32LE(memory_, 0x300, 0x0000FFFFu));
  registers_.current.user.gprs.r0 = 0x304u;

  RunInstruction("0xF42050E1");  // ldrsh r2, [r0, #-4]
  EXPECT_EQ(-1, registers_.current.user.gprs.r2_s);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x304u;
  RunInstruction("0xF4F050E1");  // ldrsh pc, [r0, #-4]
}

TEST_F(ExecuteTest, LDRSH_DBW) {
  ASSERT_TRUE(Store32LE(memory_, 0x300, 0x0000FFFFu));
  registers_.current.user.gprs.r0 = 0x304u;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0xF12030E1");  // ldrsh r2, [r0, -r1]!
  EXPECT_EQ(0x300u, registers_.current.user.gprs.r0);
  EXPECT_EQ(-1, registers_.current.user.gprs.r2_s);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x304u;
  RunInstruction("0xF1F030E1");  // ldrsh pc, [r0, -r1]!

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0xF1003FE1");  // ldrsh r0, [pc, -r1]!
}

TEST_F(ExecuteTest, LDRSH_DBW_I12) {
  ASSERT_TRUE(Store32LE(memory_, 0x300, 0x0000FFFFu));
  registers_.current.user.gprs.r0 = 0x304u;

  RunInstruction("0xF42070E1");  // ldrsh r2, [r0, #-4]!
  EXPECT_EQ(0x300u, registers_.current.user.gprs.r0);
  EXPECT_EQ(-1, registers_.current.user.gprs.r2_s);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x304u;
  RunInstruction("0xF4F070E1");  // ldrsh pc, [r0, #-4]!

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0xF4007FE1");  // ldrsh r0, [pc, #-4]!
}

TEST_F(ExecuteTest, LDRSH_IAW) {
  ASSERT_TRUE(Store32LE(memory_, 0x300, 0x0000FFFFu));
  registers_.current.user.gprs.r0 = 0x300u;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0xF12090E0");  // ldrsh r2, [r0], r1
  EXPECT_EQ(0x304u, registers_.current.user.gprs.r0);
  EXPECT_EQ(-1, registers_.current.user.gprs.r2_s);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x304u;
  RunInstruction("0xF1F090E0");  // ldrsh pc, [r0], r1

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0xF1009FE0");  // ldrsh r0, [pc], r1
}

TEST_F(ExecuteTest, LDRSH_IAW_I12) {
  ASSERT_TRUE(Store32LE(memory_, 0x300, 0x0000FFFFu));
  registers_.current.user.gprs.r0 = 0x300u;

  RunInstruction("0xF420D0E0");  // ldrsh r2, [r0], #4
  EXPECT_EQ(0x304u, registers_.current.user.gprs.r0);
  EXPECT_EQ(-1, registers_.current.user.gprs.r2_s);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x300u;
  RunInstruction("0xF4F0D0E0");  // ldrsh pc, [r0], #4

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0xF400DFE0");  // ldrsh r0, [pc], #4
}

TEST_F(ExecuteTest, LDRSH_IB) {
  ASSERT_TRUE(Store32LE(memory_, 0x300, 0x0000FFFFu));
  registers_.current.user.gprs.r0 = 0x2FC;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0xF12090E1");  // ldrsh r2, [r0, r1]
  EXPECT_EQ(-1, registers_.current.user.gprs.r2_s);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x2FC;
  RunInstruction("0xF1F090E1");  // ldrsh pc, [r0, r1]
}

TEST_F(ExecuteTest, LDRSH_IB_I12) {
  ASSERT_TRUE(Store32LE(memory_, 0x300, 0x0000FFFFu));
  registers_.current.user.gprs.r0 = 0x2FC;

  RunInstruction("0xF420D0E1");  // ldrsh r2, [r0, #4]
  EXPECT_EQ(-1, registers_.current.user.gprs.r2_s);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x2FC;
  RunInstruction("0xF4F0D0E1");  // ldrsh pc, [r0, #4]
}

TEST_F(ExecuteTest, LDRSH_IBW) {
  ASSERT_TRUE(Store32LE(memory_, 0x300, 0x0000FFFFu));
  registers_.current.user.gprs.r0 = 0x2FCu;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0xF120B0E1");  // ldrsh r2, [r0, r1]!
  EXPECT_EQ(0x300u, registers_.current.user.gprs.r0);
  EXPECT_EQ(-1, registers_.current.user.gprs.r2_s);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x2FCu;
  RunInstruction("0xF1F0B0E1");  // ldrsh pc, [r0, r1]!

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0xF100BFE1");  // ldrsh r0, [pc, r1]!
}

TEST_F(ExecuteTest, LDRSH_IBW_I12) {
  ASSERT_TRUE(Store32LE(memory_, 0x300, 0x0000FFFFu));
  registers_.current.user.gprs.r0 = 0x2FCu;

  RunInstruction("0xF420F0E1");  // ldrsh r2, [r0, #4]!
  EXPECT_EQ(0x300u, registers_.current.user.gprs.r0);
  EXPECT_EQ(-1, registers_.current.user.gprs.r2_s);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x2FCu;
  RunInstruction("0xF4F0F0E1");  // ldrsh pc, [r0, #4]!

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0xF400FFE1");  // ldrsh r0, [pc, #4]!
}

TEST_F(ExecuteTest, LDRT_DAW) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 1u));
  registers_.current.user.gprs.r0 = 0x300u;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0x012030E6");  // ldrt r2, [r0], -r1
  EXPECT_EQ(0x2FCu, registers_.current.user.gprs.r0);
  EXPECT_EQ(1u, registers_.current.user.gprs.r2);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x300u;
  RunInstruction("0x01F030E6");  // ldrt pc, [r0], -r1

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0x01003FE6");  // ldrt r0, [pc], -r1
}

TEST_F(ExecuteTest, LDRT_DAW_I12) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 1u));
  registers_.current.user.gprs.r0 = 0x300u;

  RunInstruction("0x042030E4");  // ldrt r2, [r0], #-4
  EXPECT_EQ(0x2FCu, registers_.current.user.gprs.r0);
  EXPECT_EQ(1u, registers_.current.user.gprs.r2);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x300u;
  RunInstruction("0x04F030E4");  // ldrt pc, [r0], #-4

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0x04003FE4");  // ldrt r0, [pc], #-4
}

TEST_F(ExecuteTest, LDRT_IAW) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 1u));
  registers_.current.user.gprs.r0 = 0x300u;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0x0120B0E6");  // ldrt r2, [r0], r1
  EXPECT_EQ(0x304u, registers_.current.user.gprs.r0);
  EXPECT_EQ(1u, registers_.current.user.gprs.r2);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x304u;
  RunInstruction("0x01F0B0E6");  // ldrt pc, [r0], r1

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0x0100BFE6");  // ldrt r0, [pc], r1
}

TEST_F(ExecuteTest, LDRT_IAW_I12) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 1u));
  registers_.current.user.gprs.r0 = 0x300u;

  RunInstruction("0x0420B0E4");  // ldrt r2, [r0], #4
  EXPECT_EQ(0x304u, registers_.current.user.gprs.r0);
  EXPECT_EQ(1u, registers_.current.user.gprs.r2);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x300u;
  RunInstruction("0x04F0B0E4");  // ldrt pc, [r0], #4

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0x0400BFE4");  // ldrt r0, [pc], #4
}

TEST_F(ExecuteTest, MCR) {
  RunInstruction("0xDC7E27EE");  // mcr p14, 1, r7, c7, c12, 6
  EXPECT_EQ(MODE_UND, registers_.current.user.cpsr.mode);
}

TEST_F(ExecuteTest, MLA) {
  registers_.current.user.gprs.r0 = 0x2u;
  RunInstruction("0x900020E0");  // mla r0, r0, r0, r0
  EXPECT_EQ(0x6u, registers_.current.user.gprs.r0);

  // This is technically unpredictable behavior and ARM assemblers should not
  // allow r15 to be specified as the destination operand for this instruction
  RunInstruction("0x90002FE0");  // mla r15, r0, r0, r0
}

TEST_F(ExecuteTest, MLAS) {
  registers_.current.user.gprs.r0 = 0xFFFFu;
  RunInstruction("0x900030E0");  // mlas r0, r0, r0, r0
  EXPECT_EQ(0xFFFF0000u, registers_.current.user.gprs.r0);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);

  // This is technically unpredictable behavior and ARM assemblers should not
  // allow r15 to be specified as the destination operand for this instruction
  RunInstruction("0x90003FE0");  // mlas r15, r0, r0, r0
}

TEST_F(ExecuteTest, MOV) {
  registers_.current.user.gprs.r0 = 0xFF00FF00u;
  RunInstruction("0x0010A0E1");  // mov r1, r0
  EXPECT_EQ(0xFF00FF00u, registers_.current.user.gprs.r1);

  // Modifies PC
  RunInstruction("0x00F0A0E1");  // mov r15, r0
}

TEST_F(ExecuteTest, MOV_I32) {
  RunInstruction("0xFF02A0E3");  // mov r0, #0xF000000F
  EXPECT_EQ(0xF000000F, registers_.current.user.gprs.r0);

  // Modifies PC
  RunInstruction("0xFFF2A0E3");  // mov r15, #0xF000000F
}

TEST_F(ExecuteTest, MOVS) {
  registers_.current.user.gprs.r0 = 0xFF00FF00u;
  RunInstruction("0x8010B0E1");  // movs r1, r0, lsl #1
  EXPECT_EQ(0xFE01FE00u, registers_.current.user.gprs.r1);
  EXPECT_TRUE(registers_.current.user.cpsr.carry);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);

  // Modifies PC
  RunInstruction("0x00F0B0E1");  // movs r15, r0
}

TEST_F(ExecuteTest, MOVS_I32) {
  RunInstruction("0xFF02B0E3");  // movs r0, #0xF000000F
  EXPECT_EQ(0xF000000F, registers_.current.user.gprs.r0);
  EXPECT_TRUE(registers_.current.user.cpsr.carry);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);

  // Modifies PC
  RunInstruction("0xFFF2B0E3");  // movs r15, #0xF000000F
}

TEST_F(ExecuteTest, MRC) {
  RunInstruction("0x724FB0EE");  // mrc p15, 5, r4, c0, c2, 3
  EXPECT_EQ(MODE_UND, registers_.current.user.cpsr.mode);
}

TEST_F(ExecuteTest, MRS_CPSR) {
  RunInstruction("0x00000FE1");  // mrs r0, cpsr
  EXPECT_EQ(registers_.current.user.cpsr.value,
            registers_.current.user.gprs.r0);
}

TEST_F(ExecuteTest, MRS_SPSR) {
  RunInstruction("0x00004FE1");  // mrs r0, spsr
  EXPECT_EQ(registers_.current.spsr.value, registers_.current.user.gprs.r0);
}

TEST_F(ExecuteTest, MSR_CPSR) {
  ArmProgramStatusRegister status;
  status.value = registers_.current.user.cpsr.value;
  status.mode = MODE_FIQ;
  registers_.current.user.gprs.r0 = status.value;
  RunInstruction("0x00F02FE1");  // msr cpsr_cxsf, r0
  EXPECT_EQ(status.value, registers_.current.user.cpsr.value);
}

TEST_F(ExecuteTest, MSR_CPSR_I8) {
  RunInstruction("0xFFF428E3");  // msr cpsr_f, #0xFF000000
  EXPECT_TRUE(registers_.current.user.cpsr.overflow);
  EXPECT_TRUE(registers_.current.user.cpsr.carry);
  EXPECT_TRUE(registers_.current.user.cpsr.zero);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);
}

TEST_F(ExecuteTest, MSR_SPSR) {
  ArmProgramStatusRegister status;
  status.value = registers_.current.user.cpsr.value;
  status.mode = MODE_FIQ;
  registers_.current.user.gprs.r0 = status.value;
  RunInstruction("0x00F06FE1");  // msr spsr_cxsf, r0
  EXPECT_EQ(status.value, registers_.current.spsr.value);
}

TEST_F(ExecuteTest, MSR_SPSR_I8) {
  RunInstruction("0xFFF468E3");  // msr spsr_f, #0xFF000000
  EXPECT_TRUE(registers_.current.spsr.overflow);
  EXPECT_TRUE(registers_.current.spsr.carry);
  EXPECT_TRUE(registers_.current.spsr.zero);
  EXPECT_TRUE(registers_.current.spsr.negative);
}

TEST_F(ExecuteTest, MUL) {
  registers_.current.user.gprs.r0 = 0x2u;
  RunInstruction("0x900000E0");  // mul r0, r0, r0
  EXPECT_EQ(0x4u, registers_.current.user.gprs.r0);

  // This is technically unpredictable behavior and ARM assemblers should not
  // allow r15 to be specified as the destination operand for this instruction
  RunInstruction("0x90000FE0");  // mul r15, r0, r0
}

TEST_F(ExecuteTest, MULS) {
  registers_.current.user.gprs.r0 = 0xFFFFu;
  RunInstruction("0x900010E0");  // muls r0, r0, r0
  EXPECT_EQ(0xFFFE0001, registers_.current.user.gprs.r0);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);

  // This is technically unpredictable behavior and ARM assemblers should not
  // allow r15 to be specified as the destination operand for this instruction
  RunInstruction("0x90001FE0");  // muls r15, r0, r0
}

TEST_F(ExecuteTest, MVN) {
  registers_.current.user.gprs.r0 = 0xFF00FF00u;
  RunInstruction("0x0010E0E1");  // mvn r1, r0
  EXPECT_EQ(0x00FF00FFu, registers_.current.user.gprs.r1);

  // Modifies PC
  RunInstruction("0x00F0E0E1");  // mvn r15, r0
}

TEST_F(ExecuteTest, MVN_I32) {
  RunInstruction("0xFF10E0E3");  // mvn r1, #0xFF
  EXPECT_EQ(0xFFFFFF00u, registers_.current.user.gprs.r1);

  // Modifies PC
  RunInstruction("0xFFF0E0E3");  // mvn r15, #0xFF
}

TEST_F(ExecuteTest, MVNS) {
  registers_.current.user.gprs.r0 = 0x80FF00FFu;
  RunInstruction("0x8010F0E1");  // mvns r1, r0, lsl #1
  EXPECT_EQ(0xFE01FE01, registers_.current.user.gprs.r1);
  EXPECT_TRUE(registers_.current.user.cpsr.carry);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);

  // Modifies PC
  RunInstruction("0x80F0F0E1");  // mvns r15, r0, lsl #1
}

TEST_F(ExecuteTest, MVNS_I32) {
  RunInstruction("0xFF02F0E3");  // mvns r0, #0xF000000F
  EXPECT_EQ(0x0FFFFFF0u, registers_.current.user.gprs.r0);
  EXPECT_TRUE(registers_.current.user.cpsr.carry);

  // Modifies PC
  RunInstruction("0xFFF2B0E3");  // movs r15, #0xF000000F
}

TEST_F(ExecuteTest, ORR) {
  registers_.current.user.gprs.r0 = 0xFF00FF00u;
  registers_.current.user.gprs.r1 = 0x00FFF0F0u;
  RunInstruction("0x010080E1");  // orr r0, r0, r1
  EXPECT_EQ(0xFFFFFFF0u, registers_.current.user.gprs.r0);

  // Modifies PC
  RunInstruction("0x01F080E1");  // orr r15, r0, r1
}

TEST_F(ExecuteTest, ORR_I32) {
  registers_.current.user.gprs.r0 = 0xFF00FF00u;
  RunInstruction("0xFF0880E3");  // orr r0, r0, #0x00FF0000
  EXPECT_EQ(0xFFFFFF00u, registers_.current.user.gprs.r0);

  // Modifies PC
  RunInstruction("0xFFF880E3");  // orr r15, r0, #0x00FF0000
}

TEST_F(ExecuteTest, ORRS) {
  registers_.current.user.gprs.r0 = 0xFF00FF00u;
  registers_.current.user.gprs.r1 = 0x80FFF0F0u;
  RunInstruction("0x810090E1");  // orrs r0, r0, r1, lsl #1
  EXPECT_EQ(0xFFFFFFE0u, registers_.current.user.gprs.r0);
  EXPECT_TRUE(registers_.current.user.cpsr.carry);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);

  // Modifies PC
  RunInstruction("0x81F090E1");  // orrs r15, r0, r1, lsl #1
}

TEST_F(ExecuteTest, ORRS_I32) {
  registers_.current.user.gprs.r0 = 0xFF00FF00u;
  RunInstruction("0xFF0290E3");  // orrs r0, r0, #0xF000000F
  EXPECT_EQ(0xFF00FF0Fu, registers_.current.user.gprs.r0);
  EXPECT_TRUE(registers_.current.user.cpsr.carry);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);

  // Modifies PC
  RunInstruction("0xFFF290E3");  // orrs r15, r0, #0xF000000F
}

TEST_F(ExecuteTest, RSB) {
  registers_.current.user.gprs.r0 = 1u;
  registers_.current.user.gprs.r1 = 3u;
  RunInstruction("0x010060E0");  // rsb r0, r0, r1
  EXPECT_EQ(2u, registers_.current.user.gprs.r0);

  // Modifies PC
  RunInstruction("0x01F060E0");  // rsb r15, r0, r1
}

TEST_F(ExecuteTest, RSB_I32) {
  registers_.current.user.gprs.r0 = 1u;
  RunInstruction("0x030060E2");  // rsb r0, r0, #3
  EXPECT_EQ(2u, registers_.current.user.gprs.r0);

  // Modifies PC
  RunInstruction("0x03F060E2");  // rsb r15, r0, #3
}

TEST_F(ExecuteTest, RSBS) {
  registers_.current.user.gprs.r0 = 6u;
  registers_.current.user.gprs.r1 = 3u;
  RunInstruction("0x010070E0");  // rsbs r0, r0, r1
  EXPECT_EQ(-3, registers_.current.user.gprs.r0_s);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);

  // Modifies PC
  RunInstruction("0x01F070E0");  // rsbs r15, r0, r1
}

TEST_F(ExecuteTest, RSBS_I32) {
  registers_.current.user.gprs.r0 = 6u;
  RunInstruction("0x030070E2");  // rsbs r0, r0, #3
  EXPECT_EQ(-3, registers_.current.user.gprs.r0_s);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);

  // Modifies PC
  RunInstruction("0x03F070E2");  // rsbs r15, r0, #3
}

TEST_F(ExecuteTest, RSC) {
  registers_.current.user.gprs.r0 = 1u;
  registers_.current.user.gprs.r1 = 4u;
  RunInstruction("0x0100E0E0");  // rsc r0, r0, r1
  EXPECT_EQ(2u, registers_.current.user.gprs.r0);

  // Modifies PC
  RunInstruction("0x01F0E0E0");  // rsc r15, r0, r1
}

TEST_F(ExecuteTest, RSC_I32) {
  registers_.current.user.gprs.r0 = 1u;
  RunInstruction("0x0400E0E2");  // rsc r0, r0, #4
  EXPECT_EQ(2u, registers_.current.user.gprs.r0);

  // Modifies PC
  RunInstruction("0x03F0E0E2");  // rsc r15, r0, #3
}

TEST_F(ExecuteTest, RSCS) {
  registers_.current.user.gprs.r0 = 6u;
  registers_.current.user.gprs.r1 = 3u;
  RunInstruction("0x0100F0E0");  // rscs r0, r0, r1
  EXPECT_EQ(-4, registers_.current.user.gprs.r0_s);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);

  // Modifies PC
  RunInstruction("0x01F0F0E0");  // rscs r15, r0, r1
}

TEST_F(ExecuteTest, RSCS_I32) {
  registers_.current.user.gprs.r0 = 6u;
  RunInstruction("0x0300F0E2");  // rscs r0, r0, #3
  EXPECT_EQ(-4, registers_.current.user.gprs.r0_s);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);

  // Modifies PC
  RunInstruction("0x03F0F0E2");  // rscs r15, r0, #3
}

TEST_F(ExecuteTest, SBC) {
  registers_.current.user.gprs.r0 = 4u;
  registers_.current.user.gprs.r1 = 1u;
  RunInstruction("0x0100C0E0");  // sbc r0, r0, r1
  EXPECT_EQ(2u, registers_.current.user.gprs.r0);

  // Modifies PC
  RunInstruction("0x01F0C0E0");  // sbc r15, r0, r1
}

TEST_F(ExecuteTest, SBC_I32) {
  registers_.current.user.gprs.r0 = 4u;
  RunInstruction("0x0100C0E2");  // sbc r0, r0, #1
  EXPECT_EQ(2u, registers_.current.user.gprs.r0);

  // Modifies PC
  RunInstruction("0x01F0C0E2");  // sbc r15, r0, #1
}

TEST_F(ExecuteTest, SBCS) {
  registers_.current.user.gprs.r0 = 3u;
  registers_.current.user.gprs.r1 = 6u;
  RunInstruction("0x0100D0E0");  // sbcs r0, r0, r1
  EXPECT_EQ(-4, registers_.current.user.gprs.r0_s);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);

  // Modifies PC
  RunInstruction("0x01F0D0E0");  // sbcs r15, r0, r1
}

TEST_F(ExecuteTest, SBCS_I32) {
  registers_.current.user.gprs.r0 = 3u;
  RunInstruction("0x0600D0E2");  // sbcs r0, r0, #6
  EXPECT_EQ(-4, registers_.current.user.gprs.r0_s);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);

  // Modifies PC
  RunInstruction("0x06F0D0E2");  // sbcs r15, r0, #6
}

TEST_F(ExecuteTest, SMLAL) {
  registers_.current.user.gprs.r0_s = -2147483648;
  registers_.current.user.gprs.r1_s = -1;
  registers_.current.user.gprs.r2_s = 2147483647;
  RunInstruction("0x9002E1E0");  // smlal r0, r1, r0, r2
  EXPECT_EQ(0, registers_.current.user.gprs.r0_s);
  EXPECT_EQ(-1073741824, registers_.current.user.gprs.r1_s);

  // This is technically unpredictable behavior and ARM assemblers should not
  // allow r15 to be specified as the destination operand for this instruction
  uint32_t pc = registers_.current.user.gprs.pc;
  RunInstruction("0x90F0E1E0");  // smlal r15, r1, r0, r0

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0x9000EFE0");  // smlal r0, r15, r0, r0
}

TEST_F(ExecuteTest, SMLALS) {
  registers_.current.user.gprs.r0_s = -2147483648;
  registers_.current.user.gprs.r1_s = -1;
  registers_.current.user.gprs.r2_s = 2147483647;
  RunInstruction("0x9002F1E0");  // smlals r0, r1, r0, r2
  EXPECT_EQ(0, registers_.current.user.gprs.r0_s);
  EXPECT_EQ(-1073741824, registers_.current.user.gprs.r1_s);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);

  // This is technically unpredictable behavior and ARM assemblers should not
  // allow r15 to be specified as the destination operand for this instruction
  uint32_t pc = registers_.current.user.gprs.pc;
  RunInstruction("0x90F2F1E0");  // smlals r15, r1, r0, r2

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0x9002FFE0");  // smlals r0, r15, r0, r2
}

TEST_F(ExecuteTest, SMULL) {
  registers_.current.user.gprs.r0_s = -2147483648;
  registers_.current.user.gprs.r1_s = 2147483647;
  RunInstruction("0x9001C1E0");  // smull r0, r1, r0, r1
  EXPECT_EQ(-2147483648, registers_.current.user.gprs.r0_s);
  EXPECT_EQ(-1073741824, registers_.current.user.gprs.r1_s);

  // This is technically unpredictable behavior and ARM assemblers should not
  // allow r15 to be specified as the destination operand for this instruction
  uint32_t pc = registers_.current.user.gprs.pc;
  RunInstruction("0x90F0C1E0");  // smull r15, r1, r0, r0

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0x9000CFE0");  // smull r0, r15, r0, r0
}

TEST_F(ExecuteTest, SMULLS) {
  registers_.current.user.gprs.r0_s = -2147483648;
  registers_.current.user.gprs.r1_s = 2147483647;
  RunInstruction("0x9001D1E0");  // smulls r0, r1, r0, r1
  EXPECT_EQ(-2147483648, registers_.current.user.gprs.r0_s);
  EXPECT_EQ(-1073741824, registers_.current.user.gprs.r1_s);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);

  // This is technically unpredictable behavior and ARM assemblers should not
  // allow r15 to be specified as the destination operand for this instruction
  uint32_t pc = registers_.current.user.gprs.pc;
  RunInstruction("0x90F0D1E0");  // smulls r15, r1, r0, r0

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0x9000DFE0");  // smulls r0, r15, r0, r0
}

TEST_F(ExecuteTest, STC) {
  RunInstruction("0x01888CED");  // stc p8, cr8, [r12, #4]
  EXPECT_EQ(MODE_UND, registers_.current.user.cpsr.mode);
}

TEST_F(ExecuteTest, STMDA) {
  registers_.current.user.gprs.r0 = 0x308u;
  registers_.current.user.gprs.r1 = 1u;
  registers_.current.user.gprs.r2 = 2u;
  registers_.current.user.gprs.r3 = 3u;
  RunInstruction("0x0E0000E8");  // stmda r0, {r1-r3}
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(1u, value);
  ASSERT_TRUE(Load32LE(nullptr, 0x304, &value));
  EXPECT_EQ(2u, value);
  ASSERT_TRUE(Load32LE(nullptr, 0x308, &value));
  EXPECT_EQ(3u, value);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x308u;
  RunInstruction("0x038000E8");  // stmda r0, {r0, r1, pc}
}

TEST_F(ExecuteTest, STMDA_W) {
  registers_.current.user.gprs.r0 = 0x308u;
  registers_.current.user.gprs.r1 = 1u;
  registers_.current.user.gprs.r2 = 2u;
  registers_.current.user.gprs.r3 = 3u;
  RunInstruction("0x0E0020E8");  // stmda r0!, {r1-r3}
  EXPECT_EQ(0x2FCu, registers_.current.user.gprs.r0);
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(1u, value);
  ASSERT_TRUE(Load32LE(nullptr, 0x304, &value));
  EXPECT_EQ(2u, value);
  ASSERT_TRUE(Load32LE(nullptr, 0x308, &value));
  EXPECT_EQ(3u, value);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x308u;
  RunInstruction("0x068020E8");  // stmda r0!, {r1, r2, pc}

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0x0E002FE8");  // stmda pc!, {r1-r3}
}

TEST_F(ExecuteTest, STMDB) {
  registers_.current.user.gprs.r0 = 0x30Cu;
  registers_.current.user.gprs.r1 = 1u;
  registers_.current.user.gprs.r2 = 2u;
  registers_.current.user.gprs.r3 = 3u;
  RunInstruction("0x0E0000E9");  // stmdb r0, {r1-r3}
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(1u, value);
  ASSERT_TRUE(Load32LE(nullptr, 0x304, &value));
  EXPECT_EQ(2u, value);
  ASSERT_TRUE(Load32LE(nullptr, 0x308, &value));
  EXPECT_EQ(3u, value);

  registers_.current.user.gprs.r0 = 0x30Cu;
  RunInstruction("0x038000E9");  // stmdb r0, {r0, r1, pc}
}

TEST_F(ExecuteTest, STMDB_W) {
  registers_.current.user.gprs.r0 = 0x30Cu;
  registers_.current.user.gprs.r1 = 1u;
  registers_.current.user.gprs.r2 = 2u;
  registers_.current.user.gprs.r3 = 3u;
  RunInstruction("0x0E0020E9");  // stmdb r0!, {r1-r3}
  EXPECT_EQ(0x300u, registers_.current.user.gprs.r0);
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(1u, value);
  ASSERT_TRUE(Load32LE(nullptr, 0x304, &value));
  EXPECT_EQ(2u, value);
  ASSERT_TRUE(Load32LE(nullptr, 0x308, &value));
  EXPECT_EQ(3u, value);

  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x30Cu;
  RunInstruction("0x068020E9");  // stmdb r0!, {r1, r2, pc}

  // Modifies PC
  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0x0E002FE9");  // stmdb pc!, {r1-r3}
}

TEST_F(ExecuteTest, STMIA) {
  registers_.current.user.gprs.r0 = 0x300u;
  registers_.current.user.gprs.r1 = 1u;
  registers_.current.user.gprs.r2 = 2u;
  registers_.current.user.gprs.r3 = 3u;
  RunInstruction("0x0E0080E8");  // stmia r0, {r1-r3}
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(1u, value);
  ASSERT_TRUE(Load32LE(nullptr, 0x304, &value));
  EXPECT_EQ(2u, value);
  ASSERT_TRUE(Load32LE(nullptr, 0x308, &value));
  EXPECT_EQ(3u, value);

  registers_.current.user.gprs.r0 = 0x300u;
  RunInstruction("0x038080E8");  // stmia r0, {r0, r1, pc}
}

TEST_F(ExecuteTest, STMIA_W) {
  registers_.current.user.gprs.r0 = 0x300u;
  registers_.current.user.gprs.r1 = 1u;
  registers_.current.user.gprs.r2 = 2u;
  registers_.current.user.gprs.r3 = 3u;
  RunInstruction("0x0E00A0E8");  // stmia r0!, {r1-r3}
  EXPECT_EQ(0x30Cu, registers_.current.user.gprs.r0);
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(1u, value);
  ASSERT_TRUE(Load32LE(nullptr, 0x304, &value));
  EXPECT_EQ(2u, value);
  ASSERT_TRUE(Load32LE(nullptr, 0x308, &value));
  EXPECT_EQ(3u, value);

  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x300u;
  RunInstruction("0x0680A0E8");  // stmia r0!, {r1, r2, pc}

  // Modifies PC
  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0x0E00AFE8");  // stmia pc!, {r1-r3}
}

TEST_F(ExecuteTest, STMIB) {
  registers_.current.user.gprs.r0 = 0x2FCu;
  registers_.current.user.gprs.r1 = 1u;
  registers_.current.user.gprs.r2 = 2u;
  registers_.current.user.gprs.r3 = 3u;
  RunInstruction("0x0E0080E9");  // stmib r0, {r1-r3}
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(1u, value);
  ASSERT_TRUE(Load32LE(nullptr, 0x304, &value));
  EXPECT_EQ(2u, value);
  ASSERT_TRUE(Load32LE(nullptr, 0x308, &value));
  EXPECT_EQ(3u, value);

  registers_.current.user.gprs.r0 = 0x2FCu;
  RunInstruction("0x038080E9");  // stmib r0, {r0, r1, pc}
}

TEST_F(ExecuteTest, STMIB_W) {
  registers_.current.user.gprs.r0 = 0x2FCu;
  registers_.current.user.gprs.r1 = 1u;
  registers_.current.user.gprs.r2 = 2u;
  registers_.current.user.gprs.r3 = 3u;
  RunInstruction("0x0E00A0E9");  // stmib r0!, {r1-r3}
  EXPECT_EQ(0x308u, registers_.current.user.gprs.r0);
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(1u, value);
  ASSERT_TRUE(Load32LE(nullptr, 0x304, &value));
  EXPECT_EQ(2u, value);
  ASSERT_TRUE(Load32LE(nullptr, 0x308, &value));
  EXPECT_EQ(3u, value);

  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x2FCu;
  RunInstruction("0x0680A0E9");  // stmib r0!, {r1, r2, pc}

  // Modifies PC
  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0x0E00AFE9");  // stmib pc!, {r1-r3}
}

TEST_F(ExecuteTest, STMSDA) {
  registers_.current.user.gprs.r0 = 0x308u;
  registers_.current.user.gprs.r1 = 1u;
  registers_.current.user.gprs.r2 = 2u;
  registers_.current.user.gprs.r3 = 3u;
  RunInstruction("0x0E0040E8");  // stmda r0, {r1-r3}^
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(1u, value);
  ASSERT_TRUE(Load32LE(nullptr, 0x304, &value));
  EXPECT_EQ(2u, value);
  ASSERT_TRUE(Load32LE(nullptr, 0x308, &value));
  EXPECT_EQ(3u, value);

  registers_.current.user.gprs.r0 = 0x308u;
  RunInstruction("0x068040E8");  // stmda r0, {r1, r2, pc}^
}

TEST_F(ExecuteTest, STMSDA_W) {
  registers_.current.user.gprs.r0 = 0x308u;
  registers_.current.user.gprs.r1 = 1u;
  registers_.current.user.gprs.r2 = 2u;
  registers_.current.user.gprs.r3 = 3u;
  RunInstruction("0x0E0060E8");  // stmda r0!, {r1-r3}^
  EXPECT_EQ(0x2FCu, registers_.current.user.gprs.r0);
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(1u, value);
  ASSERT_TRUE(Load32LE(nullptr, 0x304, &value));
  EXPECT_EQ(2u, value);
  ASSERT_TRUE(Load32LE(nullptr, 0x308, &value));
  EXPECT_EQ(3u, value);

  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x308u;
  RunInstruction("0x068060E8");  // stmda r0!, {r1, r2, pc}^

  // Modifies PC
  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0x0E006FE8");  // stmda pc!, {r1-r3}^
}

TEST_F(ExecuteTest, STMSDB) {
  registers_.current.user.gprs.r0 = 0x30Cu;
  registers_.current.user.gprs.r1 = 1u;
  registers_.current.user.gprs.r2 = 2u;
  registers_.current.user.gprs.r3 = 3u;
  RunInstruction("0x0E0040E9");  // stmdb r0, {r1-r3}^
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(1u, value);
  ASSERT_TRUE(Load32LE(nullptr, 0x304, &value));
  EXPECT_EQ(2u, value);
  ASSERT_TRUE(Load32LE(nullptr, 0x308, &value));
  EXPECT_EQ(3u, value);

  registers_.current.user.gprs.r0 = 0x30Cu;
  RunInstruction("0x068040E9");  // stmdb r0, {r1, r2, pc}^
}

TEST_F(ExecuteTest, STMSDB_W) {
  registers_.current.user.gprs.r0 = 0x30Cu;
  registers_.current.user.gprs.r1 = 1u;
  registers_.current.user.gprs.r2 = 2u;
  registers_.current.user.gprs.r3 = 3u;
  RunInstruction("0x0E0060E9");  // stmdb r0!, {r1-r3}^
  EXPECT_EQ(0x300u, registers_.current.user.gprs.r0);
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(1u, value);
  ASSERT_TRUE(Load32LE(nullptr, 0x304, &value));
  EXPECT_EQ(2u, value);
  ASSERT_TRUE(Load32LE(nullptr, 0x308, &value));
  EXPECT_EQ(3u, value);

  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x30Cu;
  RunInstruction("0x068060E9");  // stmdb r0!, {r1, r2, pc}^

  // Modifies PC
  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0x0E006FE9");  // stmdb pc!, {r1-r3}^
}

TEST_F(ExecuteTest, STMSIA) {
  registers_.current.user.gprs.r0 = 0x300u;
  registers_.current.user.gprs.r1 = 1u;
  registers_.current.user.gprs.r2 = 2u;
  registers_.current.user.gprs.r3 = 3u;
  RunInstruction("0x0E00C0E8");  // stmia r0, {r1-r3}^
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(1u, value);
  ASSERT_TRUE(Load32LE(nullptr, 0x304, &value));
  EXPECT_EQ(2u, value);
  ASSERT_TRUE(Load32LE(nullptr, 0x308, &value));
  EXPECT_EQ(3u, value);

  registers_.current.user.gprs.r0 = 0x300u;
  RunInstruction("0x0380C0E8");  // stmia r0, {r0, r1, pc}^
}

TEST_F(ExecuteTest, STMSIA_W) {
  registers_.current.user.gprs.r0 = 0x300u;
  registers_.current.user.gprs.r1 = 1u;
  registers_.current.user.gprs.r2 = 2u;
  registers_.current.user.gprs.r3 = 3u;
  RunInstruction("0x0E00E0E8");  // stmia r0!, {r1-r3}^
  EXPECT_EQ(0x30Cu, registers_.current.user.gprs.r0);
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(1u, value);
  ASSERT_TRUE(Load32LE(nullptr, 0x304, &value));
  EXPECT_EQ(2u, value);
  ASSERT_TRUE(Load32LE(nullptr, 0x308, &value));
  EXPECT_EQ(3u, value);

  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x300u;
  RunInstruction("0x0680E0E8");  // stmia r0!, {r1, r2, pc}^

  // Modifies PC
  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0x0E00EFE8");  // stmia pc!, {r1-r3}^
}

TEST_F(ExecuteTest, STMSIB) {
  registers_.current.user.gprs.r0 = 0x2FCu;
  registers_.current.user.gprs.r1 = 1u;
  registers_.current.user.gprs.r2 = 2u;
  registers_.current.user.gprs.r3 = 3u;
  RunInstruction("0x0E00C0E9");  // stmib r0, {r1-r3}^
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(1u, value);
  ASSERT_TRUE(Load32LE(nullptr, 0x304, &value));
  EXPECT_EQ(2u, value);
  ASSERT_TRUE(Load32LE(nullptr, 0x308, &value));
  EXPECT_EQ(3u, value);

  registers_.current.user.gprs.r0 = 0x2FCu;
  RunInstruction("0x0380C0E9");  // stmib r0, {r0, r1, pc}^
}

TEST_F(ExecuteTest, STMSIB_W) {
  registers_.current.user.gprs.r0 = 0x2FCu;
  registers_.current.user.gprs.r1 = 1u;
  registers_.current.user.gprs.r2 = 2u;
  registers_.current.user.gprs.r3 = 3u;
  RunInstruction("0x0E00E0E9");  // stmib r0!, {r1-r3}^
  EXPECT_EQ(0x308u, registers_.current.user.gprs.r0);
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(1u, value);
  ASSERT_TRUE(Load32LE(nullptr, 0x304, &value));
  EXPECT_EQ(2u, value);
  ASSERT_TRUE(Load32LE(nullptr, 0x308, &value));
  EXPECT_EQ(3u, value);

  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x2FCu;
  RunInstruction("0x0680E0E9");  // stmib r0!, {r1, r2, pc}^

  // Modifies PC
  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0x0E00EFE9");  // stmib pc!, {r1-r3}^
}

TEST_F(ExecuteTest, STR_DAW) {
  registers_.current.user.gprs.r2 = 0x01010101u;
  registers_.current.user.gprs.r0 = 0x300u;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0x012000E6");  // str r2, [r0], -r1
  EXPECT_EQ(0x2FCu, registers_.current.user.gprs.r0);
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(0x01010101u, value);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x300u;
  RunInstruction("0x01000FE6");  // str r0, [pc], -r1
}

TEST_F(ExecuteTest, STR_DAW_I12) {
  registers_.current.user.gprs.r2 = 0x01010101u;
  registers_.current.user.gprs.r0 = 0x300u;

  RunInstruction("0x042000E4");  // str r2, [r0], #-4
  EXPECT_EQ(0x2FCu, registers_.current.user.gprs.r0);
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(0x01010101u, value);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x300u;
  RunInstruction("0x04001FE4");  // str r0, [pc], #-4
}

TEST_F(ExecuteTest, STR_DB) {
  registers_.current.user.gprs.r2 = 0x01010101u;
  registers_.current.user.gprs.r0 = 0x304u;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0x012000E7");  // str r2, [r0, -r1]
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(0x01010101u, value);
}

TEST_F(ExecuteTest, STR_DB_I12) {
  registers_.current.user.gprs.r2 = 0x01010101u;
  registers_.current.user.gprs.r0 = 0x304u;

  RunInstruction("0x042000E5");  // str r2, [r0, #-4]
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(0x01010101u, value);
}

TEST_F(ExecuteTest, STR_DBW) {
  registers_.current.user.gprs.r2 = 0x01010101u;
  registers_.current.user.gprs.r0 = 0x304u;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0x012020E7");  // str r2, [r0, -r1]!
  EXPECT_EQ(0x300u, registers_.current.user.gprs.r0);
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(0x01010101u, value);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x304u;
  RunInstruction("0x01002FE7");  // str r0, [pc, -r1]!
}

TEST_F(ExecuteTest, STR_DBW_I12) {
  registers_.current.user.gprs.r2 = 0x01010101u;
  registers_.current.user.gprs.r0 = 0x304u;

  RunInstruction("0x042020E5");  // str r2, [r0, #-4]!
  EXPECT_EQ(0x300u, registers_.current.user.gprs.r0);
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(0x01010101u, value);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x304u;
  RunInstruction("0x04002FE5");  // str r0, [pc, #-4]!
}

TEST_F(ExecuteTest, STR_IAW) {
  registers_.current.user.gprs.r2 = 0x01010101u;
  registers_.current.user.gprs.r0 = 0x300u;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0x012080E6");  // str r2, [r0], r1
  EXPECT_EQ(0x304u, registers_.current.user.gprs.r0);
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(0x01010101u, value);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x304u;
  RunInstruction("0x01008FE6");  // str r0, [pc], r1
}

TEST_F(ExecuteTest, STR_IAW_I12) {
  registers_.current.user.gprs.r2 = 0x01010101u;
  registers_.current.user.gprs.r0 = 0x300u;

  RunInstruction("0x042080E4");  // str r2, [r0], #4
  EXPECT_EQ(0x304u, registers_.current.user.gprs.r0);
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(0x01010101u, value);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x300u;
  RunInstruction("0x04008FE4");  // str r0, [pc], #4
}

TEST_F(ExecuteTest, STR_IB) {
  registers_.current.user.gprs.r2 = 0x01010101u;
  registers_.current.user.gprs.r0 = 0x2FC;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0x012080E7");  // str r2, [r0, r1]
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(0x01010101u, value);
}

TEST_F(ExecuteTest, STR_IB_I12) {
  registers_.current.user.gprs.r2 = 0x01010101u;
  registers_.current.user.gprs.r0 = 0x2FC;

  RunInstruction("0x042080E5");  // str r2, [r0, #4]
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(0x01010101u, value);
}

TEST_F(ExecuteTest, STR_IBW) {
  registers_.current.user.gprs.r2 = 0x01010101u;
  registers_.current.user.gprs.r0 = 0x2FCu;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0x0120A0E7");  // str r2, [r0, r1]!
  EXPECT_EQ(0x300u, registers_.current.user.gprs.r0);
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(0x01010101u, value);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x2FCu;
  RunInstruction("0x01F0A0E7");  // str pc, [r0, r1]!

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0x0100AFE7");  // str r0, [pc, r1]!
}

TEST_F(ExecuteTest, STR_IBW_I12) {
  registers_.current.user.gprs.r2 = 0x01010101u;
  registers_.current.user.gprs.r0 = 0x2FCu;

  RunInstruction("0x0420A0E5");  // str r2, [r0, #4]!
  EXPECT_EQ(0x300u, registers_.current.user.gprs.r0);
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(0x01010101u, value);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x2FCu;
  RunInstruction("0x0400AFE5");  // str r0, [pc, #4]!
}

TEST_F(ExecuteTest, STRB_DAW) {
  registers_.current.user.gprs.r2 = 0x01010101u;
  registers_.current.user.gprs.r0 = 0x300u;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0x012040E6");  // strb r2, [r0], -r1
  EXPECT_EQ(0x2FCu, registers_.current.user.gprs.r0);
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(1u, value);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x300u;
  RunInstruction("0x01004FE6");  // strb r0, [pc], -r1
}

TEST_F(ExecuteTest, STRB_DAW_I12) {
  registers_.current.user.gprs.r2 = 0x01010101u;
  registers_.current.user.gprs.r0 = 0x300u;

  RunInstruction("0x042040E4");  // strb r2, [r0], #-4
  EXPECT_EQ(0x2FCu, registers_.current.user.gprs.r0);
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(1u, value);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x300u;
  RunInstruction("0x04004FE4");  // strb r0, [pc], #-4
}

TEST_F(ExecuteTest, STRB_DB) {
  registers_.current.user.gprs.r2 = 0x01010101u;
  registers_.current.user.gprs.r0 = 0x304u;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0x012040E7");  // strb r2, [r0, -r1]
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(1u, value);
}

TEST_F(ExecuteTest, STRB_DB_I12) {
  registers_.current.user.gprs.r2 = 0x01010101u;
  registers_.current.user.gprs.r0 = 0x304u;

  RunInstruction("0x042040E5");  // strb r2, [r0, #-4]
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(1u, value);
}

TEST_F(ExecuteTest, STRB_DBW) {
  registers_.current.user.gprs.r2 = 0x01010101u;
  registers_.current.user.gprs.r0 = 0x304u;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0x012060E7");  // strb r2, [r0, -r1]!
  EXPECT_EQ(0x300u, registers_.current.user.gprs.r0);
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(1u, value);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x304u;
  RunInstruction("0x01006FE7");  // strb r0, [pc, -r1]!
}

TEST_F(ExecuteTest, STRB_DBW_I12) {
  registers_.current.user.gprs.r2 = 0x01010101u;
  registers_.current.user.gprs.r0 = 0x304u;

  RunInstruction("0x042060E5");  // strb r2, [r0, #-4]!
  EXPECT_EQ(0x300u, registers_.current.user.gprs.r0);
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(1u, value);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x304u;
  RunInstruction("0x04006FE5");  // strb r0, [pc, #-4]!
}

TEST_F(ExecuteTest, STRB_IAW) {
  registers_.current.user.gprs.r2 = 0x01010101u;
  registers_.current.user.gprs.r0 = 0x300u;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0x0120C0E6");  // strb r2, [r0], r1
  EXPECT_EQ(0x304u, registers_.current.user.gprs.r0);
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(1u, value);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x304u;
  RunInstruction("0x0100CFE6");  // strb r0, [pc], r1
}

TEST_F(ExecuteTest, STRB_IAW_I12) {
  registers_.current.user.gprs.r2 = 0x01010101u;
  registers_.current.user.gprs.r0 = 0x300u;

  RunInstruction("0x0420C0E4");  // strb r2, [r0], #4
  EXPECT_EQ(0x304u, registers_.current.user.gprs.r0);
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(1u, value);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x300u;
  RunInstruction("0x0400CFE4");  // strb r0, [pc], #4
}

TEST_F(ExecuteTest, STRB_IB) {
  registers_.current.user.gprs.r2 = 0x01010101u;
  registers_.current.user.gprs.r0 = 0x2FC;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0x0120C0E7");  // strb r2, [r0, r1]
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(1u, value);
}

TEST_F(ExecuteTest, STRB_IB_I12) {
  registers_.current.user.gprs.r2 = 0x01010101u;
  registers_.current.user.gprs.r0 = 0x2FC;

  RunInstruction("0x0420C0E5");  // strb r2, [r0, #4]
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(1u, value);
}

TEST_F(ExecuteTest, STRB_IBW) {
  registers_.current.user.gprs.r2 = 0x01010101u;
  registers_.current.user.gprs.r0 = 0x2FCu;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0x0120E0E7");  // strb r2, [r0, r1]!
  EXPECT_EQ(0x300u, registers_.current.user.gprs.r0);
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(1u, value);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x2FCu;
  RunInstruction("0x01F0E0E7");  // strb pc, [r0, r1]!

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0x0100EFE7");  // strb r0, [pc, r1]!
}

TEST_F(ExecuteTest, STRB_IBW_I12) {
  registers_.current.user.gprs.r2 = 0x01010101u;
  registers_.current.user.gprs.r0 = 0x2FCu;

  RunInstruction("0x0420E0E5");  // strb r2, [r0, #4]!
  EXPECT_EQ(0x300u, registers_.current.user.gprs.r0);
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(1u, value);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x2FCu;
  RunInstruction("0x0400EFE5");  // strb r0, [pc, #4]!
}

TEST_F(ExecuteTest, STRBT_DAW) {
  registers_.current.user.gprs.r2 = 0x01010101u;
  registers_.current.user.gprs.r0 = 0x300u;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0x012060E6");  // strbt r2, [r0], -r1
  EXPECT_EQ(0x2FCu, registers_.current.user.gprs.r0);
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(1u, value);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x300u;
  RunInstruction("0x01006FE6");  // strbt r0, [pc], -r1
}

TEST_F(ExecuteTest, STRBT_DAW_I12) {
  registers_.current.user.gprs.r2 = 0x01010101u;
  registers_.current.user.gprs.r0 = 0x300u;

  RunInstruction("0x042060E4");  // strbt r2, [r0], #-4
  EXPECT_EQ(0x2FCu, registers_.current.user.gprs.r0);
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(1u, value);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x300u;
  RunInstruction("0x04006FE4");  // strbt r0, [pc], #-4
}

TEST_F(ExecuteTest, STRBT_IAW) {
  registers_.current.user.gprs.r2 = 0x01010101u;
  registers_.current.user.gprs.r0 = 0x300u;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0x0120E0E6");  // strbt r2, [r0], r1
  EXPECT_EQ(0x304u, registers_.current.user.gprs.r0);
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(1u, value);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x304u;
  RunInstruction("0x0100EFE6");  // strbt r0, [pc], r1
}

TEST_F(ExecuteTest, STRBT_IAW_I8) {
  registers_.current.user.gprs.r2 = 0x01010101u;
  registers_.current.user.gprs.r0 = 0x300u;

  RunInstruction("0x0420E0E4");  // strbt r2, [r0], #4
  EXPECT_EQ(0x304u, registers_.current.user.gprs.r0);
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(1u, value);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x300u;
  RunInstruction("0x0400EFE4");  // strbt r0, [pc], #4
}

TEST_F(ExecuteTest, STRH_DAW) {
  registers_.current.user.gprs.r2 = 0x01010101u;
  registers_.current.user.gprs.r0 = 0x300u;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0xB12000E0");  // strh r2, [r0], -r1
  EXPECT_EQ(0x2FCu, registers_.current.user.gprs.r0);
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(0x0101u, value);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x300u;
  RunInstruction("0xB1000FE0");  // strh r0, [pc], -r1
}

TEST_F(ExecuteTest, STRH_DAW_I8) {
  registers_.current.user.gprs.r2 = 0x01010101u;
  registers_.current.user.gprs.r0 = 0x300u;

  RunInstruction("0xB42040E0");  // strh r2, [r0], #-4
  EXPECT_EQ(0x2FCu, registers_.current.user.gprs.r0);
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(0x0101u, value);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x300u;
  RunInstruction("0xB4004FE0");  // strh r0, [pc], #-4
}

TEST_F(ExecuteTest, STRH_DB) {
  registers_.current.user.gprs.r2 = 0x01010101u;
  registers_.current.user.gprs.r0 = 0x304u;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0xB12000E1");  // strh r2, [r0, -r1]
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(0x0101u, value);
}

TEST_F(ExecuteTest, STRH_DB_I8) {
  registers_.current.user.gprs.r2 = 0x01010101u;
  registers_.current.user.gprs.r0 = 0x304u;

  RunInstruction("0xB42040E1");  // strh r2, [r0, #-4]
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(0x0101u, value);
}

TEST_F(ExecuteTest, STRH_DBW) {
  registers_.current.user.gprs.r2 = 0x01010101u;
  registers_.current.user.gprs.r0 = 0x304u;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0xB12020E1");  // strh r2, [r0, -r1]!
  EXPECT_EQ(0x300u, registers_.current.user.gprs.r0);
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(0x0101u, value);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x304u;
  RunInstruction("0xB1002FE1");  // strh r0, [pc, -r1]!
}

TEST_F(ExecuteTest, STRH_DBW_I8) {
  registers_.current.user.gprs.r2 = 0x01010101u;
  registers_.current.user.gprs.r0 = 0x304u;

  RunInstruction("0xB42060E1");  // strh r2, [r0, #-4]!
  EXPECT_EQ(0x300u, registers_.current.user.gprs.r0);
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(0x0101u, value);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x304u;
  RunInstruction("0xB4006FE1");  // strh r0, [pc, #-4]!
}

TEST_F(ExecuteTest, STRH_IAW) {
  registers_.current.user.gprs.r2 = 0x01010101u;
  registers_.current.user.gprs.r0 = 0x300u;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0xB12080E0");  // strh r2, [r0], r1
  EXPECT_EQ(0x304u, registers_.current.user.gprs.r0);
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(0x0101u, value);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x304u;
  RunInstruction("0xB1008FE0");  // strh r0, [pc], r1
}

TEST_F(ExecuteTest, STRH_IAW_I8) {
  registers_.current.user.gprs.r2 = 0x01010101u;
  registers_.current.user.gprs.r0 = 0x300u;

  RunInstruction("0xB420C0E0");  // strh r2, [r0], #4
  EXPECT_EQ(0x304u, registers_.current.user.gprs.r0);
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(0x0101u, value);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x300u;
  RunInstruction("0xB400CFE0");  // strh r0, [pc], #4
}

TEST_F(ExecuteTest, STRH_IB) {
  registers_.current.user.gprs.r2 = 0x01010101u;
  registers_.current.user.gprs.r0 = 0x2FC;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0xB12080E1");  // strh r2, [r0, r1]
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(0x0101u, value);
}

TEST_F(ExecuteTest, STRH_IB_I8) {
  registers_.current.user.gprs.r2 = 0x01010101u;
  registers_.current.user.gprs.r0 = 0x2FC;

  RunInstruction("0xB420C0E1");  // strh r2, [r0, #4]
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(0x0101u, value);
}

TEST_F(ExecuteTest, STRH_IBW) {
  registers_.current.user.gprs.r2 = 0x01010101u;
  registers_.current.user.gprs.r0 = 0x2FCu;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0xB120A0E1");  // strh r2, [r0, r1]!
  EXPECT_EQ(0x300u, registers_.current.user.gprs.r0);
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(0x0101u, value);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x2FCu;
  RunInstruction("0xB1F0A0E1");  // strh pc, [r0, r1]!

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0xB100AFE1");  // strh r0, [pc, r1]!
}

TEST_F(ExecuteTest, STRH_IBW_I8) {
  registers_.current.user.gprs.r2 = 0x01010101u;
  registers_.current.user.gprs.r0 = 0x2FCu;

  RunInstruction("0xB420E0E1");  // strh r2, [r0, #4]!
  EXPECT_EQ(0x300u, registers_.current.user.gprs.r0);
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(0x0101u, value);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x2FCu;
  RunInstruction("0xB400EFE1");  // strh r0, [pc, #4]!
}

TEST_F(ExecuteTest, STRT_DAW) {
  registers_.current.user.gprs.r2 = 0x01010101u;
  registers_.current.user.gprs.r0 = 0x300u;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0x012020E6");  // strt r2, [r0], -r1
  EXPECT_EQ(0x2FCu, registers_.current.user.gprs.r0);
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(0x01010101u, value);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x300u;
  RunInstruction("0x01002FE6");  // strt r0, [pc], -r1
}

TEST_F(ExecuteTest, STRT_DAW_I12) {
  registers_.current.user.gprs.r2 = 0x01010101u;
  registers_.current.user.gprs.r0 = 0x300u;

  RunInstruction("0x042020E4");  // strt r2, [r0], #-4
  EXPECT_EQ(0x2FCu, registers_.current.user.gprs.r0);
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(0x01010101u, value);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x300u;
  RunInstruction("0x04002FE4");  // strt r0, [pc], #-4
}

TEST_F(ExecuteTest, STRT_IAW) {
  registers_.current.user.gprs.r2 = 0x01010101u;
  registers_.current.user.gprs.r0 = 0x300u;
  registers_.current.user.gprs.r1 = 4u;

  RunInstruction("0x012080E6");  // strt r2, [r0], r1
  EXPECT_EQ(0x304u, registers_.current.user.gprs.r0);
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(0x01010101u, value);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x304u;
  RunInstruction("0x0100AFE6");  // strt r0, [pc], r1
}

TEST_F(ExecuteTest, STRT_IAW_I12) {
  registers_.current.user.gprs.r2 = 0x01010101u;
  registers_.current.user.gprs.r0 = 0x300u;

  RunInstruction("0x0420A0E4");  // strt r2, [r0], #4
  EXPECT_EQ(0x304u, registers_.current.user.gprs.r0);
  uint32_t value;
  ASSERT_TRUE(Load32LE(nullptr, 0x300, &value));
  EXPECT_EQ(0x01010101u, value);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x300u;
  RunInstruction("0x0400AFE4");  // strt r0, [pc], #4
}

TEST_F(ExecuteTest, SUB) {
  registers_.current.user.gprs.r0 = 4u;
  registers_.current.user.gprs.r1 = 1u;
  RunInstruction("0x010040E0");  // sub r0, r0, r1
  EXPECT_EQ(3u, registers_.current.user.gprs.r0);

  // Modifies PC
  RunInstruction("0x01F040E0");  // sub r15, r0, r1
}

TEST_F(ExecuteTest, SUB_I32) {
  registers_.current.user.gprs.r0 = 4u;
  RunInstruction("0x010040E2");  // sub r0, r0, #1
  EXPECT_EQ(3u, registers_.current.user.gprs.r0);

  // Modifies PC
  RunInstruction("0x01F040E2");  // sub r15, r0, #1
}

TEST_F(ExecuteTest, SUBS) {
  registers_.current.user.gprs.r0 = 3u;
  registers_.current.user.gprs.r1 = 6u;
  RunInstruction("0x010050E0");  // subs r0, r0, r1
  EXPECT_EQ(-3, registers_.current.user.gprs.r0_s);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);

  // Modifies PC
  RunInstruction("0x01F050E0");  // subs r15, r0, r1
}

TEST_F(ExecuteTest, SUBS_I32) {
  registers_.current.user.gprs.r0 = 3u;
  RunInstruction("0x060050E2");  // subs r0, r0, #6
  EXPECT_EQ(-3, registers_.current.user.gprs.r0_s);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);

  // Modifies PC
  RunInstruction("0x06F050E2");  // subs r15, r0, #6
}

TEST_F(ExecuteTest, SWI) {
  RunInstruction("0x640000EF");  // swi #100
  EXPECT_EQ(MODE_SVC, registers_.current.user.cpsr.mode);
}

TEST_F(ExecuteTest, SWP) {
  ASSERT_TRUE(Store32LE(nullptr, 0x50u, 0xDEADBEEFu));
  registers_.current.user.gprs.r0 = 0xCAFEBABEu;
  registers_.current.user.gprs.r1 = 0x50u;
  RunInstruction("0x900001E1");  // swp r0, r0, [r1]
  EXPECT_EQ(0xDEADBEEFu, registers_.current.user.gprs.r0);
  uint32_t memory_value;
  ASSERT_TRUE(Load32LE(nullptr, 0x50u, &memory_value));
  EXPECT_EQ(0xCAFEBABEu, memory_value);

  // This is technically unpredictable behavior and ARM assemblers should not
  // allow r15 to be specified as the destination operand for this instruction
  RunInstruction("0x90F001E1");  // subs swp r15, r0, [r1]
}

TEST_F(ExecuteTest, SWPB) {
  ASSERT_TRUE(Store8(nullptr, 0x50u, 0xCCu));
  registers_.current.user.gprs.r0 = 0xBBu;
  registers_.current.user.gprs.r1 = 0x50u;
  RunInstruction("0x900041E1");  // swpb r0, r0, [r1]
  EXPECT_EQ(0xCCu, registers_.current.user.gprs.r0);
  uint8_t memory_value;
  ASSERT_TRUE(Load8(nullptr, 0x50u, &memory_value));
  EXPECT_EQ(0xBBu, memory_value);

  // This is technically unpredictable behavior and ARM assemblers should not
  // allow r15 to be specified as the destination operand for this instruction
  RunInstruction("0x90F041E1");  // subs swpb r15, r0, [r1]
}

TEST_F(ExecuteTest, TEQ) {
  registers_.current.user.gprs.r0 = 0xFFFFFFFEu;
  registers_.current.user.gprs.r1 = 0xFFFFFFFFu;
  RunInstruction("0x810030E1");  // teq r0, r1, lsl #1
  EXPECT_TRUE(registers_.current.user.cpsr.carry);
  EXPECT_TRUE(registers_.current.user.cpsr.zero);
}

TEST_F(ExecuteTest, TEQ_I32) {
  registers_.current.user.gprs.r0 = 0xF000000Fu;
  RunInstruction("0xFF0230E3");  // teq r0, #0xF000000F
  EXPECT_TRUE(registers_.current.user.cpsr.carry);
  EXPECT_TRUE(registers_.current.user.cpsr.zero);
}

TEST_F(ExecuteTest, TST) {
  registers_.current.user.gprs.r0 = 0xFFFFFFFEu;
  registers_.current.user.gprs.r1 = 0xFFFFFFFFu;
  RunInstruction("0x810010E1");  // tst r0, r1, lsl #1
  EXPECT_TRUE(registers_.current.user.cpsr.negative);
}

TEST_F(ExecuteTest, TST_I32) {
  registers_.current.user.gprs.r0 = 0xF000000Fu;
  RunInstruction("0xFF0210E3");  // tst r0, #0xF000000F
  EXPECT_TRUE(registers_.current.user.cpsr.negative);
}

TEST_F(ExecuteTest, UMLAL) {
  registers_.current.user.gprs.r0 = 0xFFFFFFFFu;
  RunInstruction("0x9000A1E0");  // umlal r0, r1, r0, r0
  EXPECT_EQ(0u, registers_.current.user.gprs.r0);
  EXPECT_EQ(0xFFFFFFFFu, registers_.current.user.gprs.r1);

  // This is technically unpredictable behavior and ARM assemblers should not
  // allow r15 to be specified as the destination operand for this instruction
  uint32_t pc = registers_.current.user.gprs.pc;
  RunInstruction("0x90F0A1E0");  // umlal r15, r1, r0, r0

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0x9000AFE0");  // umlal r0, r15, r0, r0
}

TEST_F(ExecuteTest, UMLALS) {
  registers_.current.user.gprs.r0 = 0xFFFFFFFFu;
  RunInstruction("0x9000B1E0");  // umlals r0, r1, r0, r0
  EXPECT_EQ(0u, registers_.current.user.gprs.r0);
  EXPECT_EQ(0xFFFFFFFFu, registers_.current.user.gprs.r1);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);

  // This is technically unpredictable behavior and ARM assemblers should not
  // allow r15 to be specified as the destination operand for this instruction
  uint32_t pc = registers_.current.user.gprs.pc;
  RunInstruction("0x90F0B1E0");  // umlals r15, r1, r0, r0

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0x9000BFE0");  // umlals r0, r15, r0, r0
}

TEST_F(ExecuteTest, UMULL) {
  registers_.current.user.gprs.r0 = 0xFFFFFFFFu;
  RunInstruction("0x900081E0");  // umull r0, r1, r0, r0
  EXPECT_EQ(1u, registers_.current.user.gprs.r0);
  EXPECT_EQ(0xFFFFFFFEu, registers_.current.user.gprs.r1);

  // This is technically unpredictable behavior and ARM assemblers should not
  // allow r15 to be specified as the destination operand for this instruction
  uint32_t pc = registers_.current.user.gprs.pc;
  RunInstruction("0x90F081E0");  // umull r15, r1, r0, r0

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0x90008FE0");  // umull r0, r15, r0, r0
}

TEST_F(ExecuteTest, UMULLS) {
  registers_.current.user.gprs.r0 = 0xFFFFFFFFu;
  RunInstruction("0x900091E0");  // umulls r0, r1, r0, r0
  EXPECT_EQ(1u, registers_.current.user.gprs.r0);
  EXPECT_EQ(0xFFFFFFFEu, registers_.current.user.gprs.r1);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);

  // This is technically unpredictable behavior and ARM assemblers should not
  // allow r15 to be specified as the destination operand for this instruction
  uint32_t pc = registers_.current.user.gprs.pc;
  RunInstruction("0x90F091E0");  // umulls r15, r1, r0, r0

  registers_.current.user.gprs.pc = pc + 4u;
  RunInstruction("0x90009FE0");  // umulls r0, r15, r0, r0
}

TEST_F(ExecuteTest, UNDEF) {
  // This is techically a BKPT instruction which is not present in ARMv4
  RunInstruction("0x700020E1");
  EXPECT_EQ(MODE_UND, registers_.current.user.cpsr.mode);
}

//
// Data Abort Exceptions
//

TEST_F(ExecuteTest, LDMDA_FAILS) {
  RunInstructionBadMemory("0x070010E8");  // ldmda r0, {r0-r2}
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDMDA_W_FAILS) {
  RunInstructionBadMemory("0x0E0030E8");  // ldmda r0!, {r1-r3}
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDMDB_FAILS) {
  RunInstructionBadMemory("0x070010E9");  // ldmdb r0, {r0-r2}
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDMDB_W_FAILS) {
  RunInstructionBadMemory("0x0E0030E9");  // ldmdb r0!, {r1-r3}
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDMIA_FAILS) {
  RunInstructionBadMemory("0x070090E8");  // ldmia r0, {r0-r2}
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDMIA_W_FAILS) {
  RunInstructionBadMemory("0x0E00B0E8");  // ldmia r0!, {r1-r3}
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDMIB_FAILS) {
  RunInstructionBadMemory("0x070090E9");  // ldmib r0, {r0-r2}
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDMIB_W_FAILS) {
  RunInstructionBadMemory("0x0E00B0E9");  // ldmib r0!, {r1-r3}
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDMSDA_FAILS) {
  RunInstructionBadMemory("0x0E0050E8");  // ldmda r0, {r1-r3}^
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDMSDA_W_FAILS) {
  RunInstructionBadMemory("0x0E0070E8");  // ldmda r0!, {r1-r3}^
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDMSDB_FAILS) {
  RunInstructionBadMemory("0x0E0050E9");  // ldmdb r0, {r1-r3}^
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDMSDB_W_FAILS) {
  RunInstructionBadMemory("0x0E0070E9");  // ldmdb r0!, {r1-r3}^
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDMSIA_FAILS) {
  RunInstructionBadMemory("0x0E00D0E8");  // ldmia r0, {r1-r3}^
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDMSIA_W_FAILS) {
  RunInstructionBadMemory("0x0E00F0E8");  // ldmia r0!, {r1-r3}^
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDMSIB_FAILS) {
  RunInstructionBadMemory("0x0E00D0E9");  // ldmib r0, {r1-r3}^
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDMSIB_W_FAILS) {
  RunInstructionBadMemory("0x0E00F0E9");  // ldmib r0!, {r1-r3}^
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDR_DAW_FAILS) {
  RunInstructionBadMemory("0x012010E6");  // ldr r2, [r0], -r1
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDR_DAW_I12_FAILS) {
  RunInstructionBadMemory("0x042010E4");  // ldr r2, [r0], #-4
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDR_DB_FAILS) {
  RunInstructionBadMemory("0x012010E7");  // ldr r2, [r0, -r1]
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDR_DB_I12_FAILS) {
  RunInstructionBadMemory("0x042010E5");  // ldr r2, [r0, #-4]
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDR_DBW_FAILS) {
  RunInstructionBadMemory("0x012030E7");  // ldr r2, [r0, -r1]!
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDR_DBW_I12_FAILS) {
  RunInstructionBadMemory("0x042030E5");  // ldr r2, [r0, #-4]!
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDR_IAW_FAILS) {
  RunInstructionBadMemory("0x012090E6");  // ldr r2, [r0], r1
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDR_IAW_I12_FAILS) {
  RunInstructionBadMemory("0x042090E4");  // ldr r2, [r0], #4
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDR_IB_FAILS) {
  RunInstructionBadMemory("0x012090E7");  // ldr r2, [r0, r1]
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDR_IB_I12_FAILS) {
  RunInstructionBadMemory("0x042090E5");  // ldr r2, [r0, #4]
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDR_IBW_FAILS) {
  RunInstructionBadMemory("0x0120B0E7");  // ldr r2, [r0, r1]!
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDR_IBW_I12_FAILS) {
  RunInstructionBadMemory("0x0420B0E5");  // ldr r2, [r0, #4]!
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRB_DAW_FAILS) {
  RunInstructionBadMemory("0x012050E6");  // ldrb r2, [r0], -r1
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRB_DAW_I12_FAILS) {
  RunInstructionBadMemory("0x042050E4");  // ldrb r2, [r0], #-4
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRB_DB_FAILS) {
  RunInstructionBadMemory("0x012050E7");  // ldrb r2, [r0, -r1]
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRB_DB_I12_FAILS) {
  RunInstructionBadMemory("0x042050E5");  // ldrb r2, [r0, #-4]
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRB_DBW_FAILS) {
  RunInstructionBadMemory("0x012070E7");  // ldrb r2, [r0, -r1]!
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRB_DBW_I12_FAILS) {
  RunInstructionBadMemory("0x042070E5");  // ldrb r2, [r0, #-4]!
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRB_IAW_FAILS) {
  RunInstructionBadMemory("0x0120D0E6");  // ldrb r2, [r0], r1
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRB_IAW_I12_FAILS) {
  RunInstructionBadMemory("0x0420D0E4");  // ldrb r2, [r0], #4
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRB_IB_FAILS) {
  RunInstructionBadMemory("0x0120D0E7");  // ldrb r2, [r0, r1]
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRB_IB_I12_FAILS) {
  RunInstructionBadMemory("0x0420D0E5");  // ldrb r2, [r0, #4]
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRB_IBW_FAILS) {
  RunInstructionBadMemory("0x0120F0E7");  // ldrb r2, [r0, r1]!
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRB_IBW_I12_FAILS) {
  RunInstructionBadMemory("0x0420F0E5");  // ldrb r2, [r0, #4]!
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRBT_DAW_FAILS) {
  RunInstructionBadMemory("0x012070E6");  // ldrbt r2, [r0], -r1
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRBT_DAW_I12_FAILS) {
  RunInstructionBadMemory("0x042070E4");  // ldrbt r2, [r0], #-4
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRBT_IAW_FAILS) {
  RunInstructionBadMemory("0x0120F0E6");  // ldrbt r2, [r0], r1
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRBT_IAW_I12_FAILS) {
  RunInstructionBadMemory("0x0420F0E4");  // ldrbt r2, [r0], #4
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRH_DAW_FAILS) {
  RunInstructionBadMemory("0xB12010E0");  // ldrh r2, [r0], -r1
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRH_DAW_I12_FAILS) {
  RunInstructionBadMemory("0xB42050E0");  // ldrh r2, [r0], #-4
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRH_DB_FAILS) {
  RunInstructionBadMemory("0xB12010E1");  // ldrh r2, [r0, -r1]
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRH_DB_I12_FAILS) {
  RunInstructionBadMemory("0xB42050E1");  // ldrh r2, [r0, #-4]
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRH_DBW_FAILS) {
  RunInstructionBadMemory("0xB12030E1");  // ldrh r2, [r0, -r1]!
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRH_DBW_I12_FAILS) {
  RunInstructionBadMemory("0xB42070E1");  // ldrh r2, [r0, #-4]!
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRH_IAW_FAILS) {
  RunInstructionBadMemory("0xB12090E0");  // ldrh r2, [r0], r1
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRH_IAW_I12_FAILS) {
  RunInstructionBadMemory("0xB420D0E0");  // ldrh r2, [r0], #4
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRH_IB_FAILS) {
  RunInstructionBadMemory("0xB12090E1");  // ldrh r2, [r0, r1]
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRH_IB_I12_FAILS) {
  RunInstructionBadMemory("0xB420D0E1");  // ldrh r2, [r0, #4]
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRH_IBW_FAILS) {
  RunInstructionBadMemory("0xB120B0E1");  // ldrh r2, [r0, r1]!
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRH_IBW_I12_FAILS) {
  RunInstructionBadMemory("0xB420F0E1");  // ldrh r2, [r0, #4]!
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRSB_DAW_FAILS) {
  RunInstructionBadMemory("0xD12010E0");  // ldrsb r2, [r0], -r1
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRSB_DAW_I12_FAILS) {
  RunInstructionBadMemory("0xD42050E0");  // ldrsb r2, [r0], #-4
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRSB_DB_FAILS) {
  RunInstructionBadMemory("0xD12010E1");  // ldrsb r2, [r0, -r1]
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRSB_DB_I12_FAILS) {
  RunInstructionBadMemory("0xD42050E1");  // ldrsb r2, [r0, #-4]
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRSB_DBW_FAILS) {
  RunInstructionBadMemory("0xD12030E1");  // ldrsb r2, [r0, -r1]!
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRSB_DBW_I12_FAILS) {
  RunInstructionBadMemory("0xD42070E1");  // ldrsb r2, [r0, #-4]!
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRSB_IAW_FAILS) {
  RunInstructionBadMemory("0xD12090E0");  // ldrsb r2, [r0], r1
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRSB_IAW_I12_FAILS) {
  RunInstructionBadMemory("0xD420D0E0");  // ldrsb r2, [r0], #4
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRSB_IB_FAILS) {
  RunInstructionBadMemory("0xD12090E1");  // ldrsb r2, [r0, r1]
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRSB_IB_I12_FAILS) {
  RunInstructionBadMemory("0xD420D0E1");  // ldrsb r2, [r0, #4]
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRSB_IBW_FAILS) {
  RunInstructionBadMemory("0xD120B0E1");  // ldrsb r2, [r0, r1]!
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRSB_IBW_I12_FAILS) {
  RunInstructionBadMemory("0xD420F0E1");  // ldrsb r2, [r0, #4]!
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRSH_DAW_FAILS) {
  RunInstructionBadMemory("0xF12010E0");  // ldrsh r2, [r0], -r1
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRSH_DAW_I12_FAILS) {
  RunInstructionBadMemory("0xF42050E0");  // ldrsh r2, [r0], #-4
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRSH_DB_FAILS) {
  RunInstructionBadMemory("0xF12010E1");  // ldrsh r2, [r0, -r1]
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRSH_DB_I12_FAILS) {
  RunInstructionBadMemory("0xF42050E1");  // ldrsh r2, [r0, #-4]
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRSH_DBW_FAILS) {
  RunInstructionBadMemory("0xF12030E1");  // ldrsh r2, [r0, -r1]!
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRSH_DBW_I12_FAILS) {
  RunInstructionBadMemory("0xF42070E1");  // ldrsh r2, [r0, #-4]!
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRSH_IAW_FAILS) {
  RunInstructionBadMemory("0xF12090E0");  // ldrsh r2, [r0], r1
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRSH_IAW_I12_FAILS) {
  RunInstructionBadMemory("0xF420D0E0");  // ldrsh r2, [r0], #4
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRSH_IB_FAILS) {
  RunInstructionBadMemory("0xF12090E1");  // ldrsh r2, [r0, r1]
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRSH_IB_I12_FAILS) {
  RunInstructionBadMemory("0xF420D0E1");  // ldrsh r2, [r0, #4]
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRSH_IBW_FAILS) {
  RunInstructionBadMemory("0xF120B0E1");  // ldrsh r2, [r0, r1]!
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRSH_IBW_I12_FAILS) {
  RunInstructionBadMemory("0xF420F0E1");  // ldrsh r2, [r0, #4]!
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRT_DAW_FAILS) {
  RunInstructionBadMemory("0x012030E6");  // ldrt r2, [r0], -r1
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRT_DAW_I12_FAILS) {
  RunInstructionBadMemory("0x042030E4");  // ldrt r2, [r0], #-4
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRT_IAW_FAILS) {
  RunInstructionBadMemory("0x0120B0E6");  // ldrt r2, [r0], r1
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, LDRT_IAW_I12_FAILS) {
  RunInstructionBadMemory("0x0420B0E4");  // ldrt r2, [r0], #4
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STMDA_FAILS) {
  RunInstructionBadMemory("0x0E0000E8");  // stmda r0, {r1-r3}
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STMDA_W_FAILS) {
  RunInstructionBadMemory("0x0E0020E8");  // stmda r0!, {r1-r3}
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STMDB_FAILS) {
  RunInstructionBadMemory("0x0E0000E9");  // stmdb r0, {r1-r3}
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STMDB_W_FAILS) {
  RunInstructionBadMemory("0x0E0020E9");  // stmdb r0!, {r1-r3}
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STMIA_FAILS) {
  RunInstructionBadMemory("0x0E0080E8");  // stmia r0, {r1-r3}
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STMIA_W_FAILS) {
  RunInstructionBadMemory("0x0E00A0E8");  // stmia r0!, {r1-r3}
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STMIB_FAILS) {
  RunInstructionBadMemory("0x0E0080E9");  // stmib r0, {r1-r3}
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STMIB_W_FAILS) {
  RunInstructionBadMemory("0x0E00A0E9");  // stmib r0!, {r1-r3}
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STMSDA_FAILS) {
  RunInstructionBadMemory("0x0E0040E8");  // stmda r0, {r1-r3}^
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STMSDA_W_FAILS) {
  RunInstructionBadMemory("0x0E0060E8");  // stmda r0!, {r1-r3}^
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STMSDB_FAILS) {
  RunInstructionBadMemory("0x0E0040E9");  // stmdb r0, {r1-r3}^
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STMSDB_W_FAILS) {
  RunInstructionBadMemory("0x0E0060E9");  // stmdb r0!, {r1-r3}^
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STMSIA_FAILS) {
  RunInstructionBadMemory("0x0E00C0E8");  // stmia r0, {r1-r3}^
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STMSIA_W_FAILS) {
  RunInstructionBadMemory("0x0E00E0E8");  // stmia r0!, {r1-r3}^
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STMSIB_FAILS) {
  RunInstructionBadMemory("0x0E00C0E9");  // stmib r0, {r1-r3}^
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STMSIB_W_FAILS) {
  RunInstructionBadMemory("0x0E00E0E9");  // stmib r0!, {r1-r3}^
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STR_DAW_FAILS) {
  RunInstructionBadMemory("0x012000E6");  // str r2, [r0], -r1
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STR_DAW_I12_FAILS) {
  RunInstructionBadMemory("0x042000E4");  // str r2, [r0], #-4
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STR_DB_FAILS) {
  RunInstructionBadMemory("0x012000E7");  // str r2, [r0, -r1]
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STR_DB_I12_FAILS) {
  RunInstructionBadMemory("0x042000E5");  // str r2, [r0, #-4]
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STR_DBW_FAILS) {
  RunInstructionBadMemory("0x012020E7");  // str r2, [r0, -r1]!
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STR_DBW_I12_FAILS) {
  RunInstructionBadMemory("0x042020E5");  // str r2, [r0, #-4]!
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STR_IAW_FAILS) {
  RunInstructionBadMemory("0x012080E6");  // str r2, [r0], r1
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STR_IAW_I12_FAILS) {
  RunInstructionBadMemory("0x042080E4");  // str r2, [r0], #4
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STR_IB_FAILS) {
  RunInstructionBadMemory("0x012080E7");  // str r2, [r0, r1]
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STR_IB_I12_FAILS) {
  RunInstructionBadMemory("0x042080E5");  // str r2, [r0, #4]
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STR_IBW_FAILS) {
  RunInstructionBadMemory("0x0120A0E7");  // str r2, [r0, r1]!
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STR_IBW_I12_FAILS) {
  RunInstructionBadMemory("0x0420A0E5");  // str r2, [r0, #4]!
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STRB_DAW_FAILS) {
  RunInstructionBadMemory("0x012040E6");  // strb r2, [r0], -r1
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STRB_DAW_I12_FAILS) {
  RunInstructionBadMemory("0x042040E4");  // strb r2, [r0], #-4
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STRB_DB_FAILS) {
  RunInstructionBadMemory("0x012040E7");  // strb r2, [r0, -r1]
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STRB_DB_I12_FAILS) {
  RunInstructionBadMemory("0x042040E5");  // strb r2, [r0, #-4]
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STRB_DBW_FAILS) {
  RunInstructionBadMemory("0x012060E7");  // strb r2, [r0, -r1]!
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STRB_DBW_I12_FAILS) {
  RunInstructionBadMemory("0x042060E5");  // strb r2, [r0, #-4]!
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STRB_IAW_FAILS) {
  RunInstructionBadMemory("0x0120C0E6");  // strb r2, [r0], r1
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STRB_IAW_I12_FAILS) {
  RunInstructionBadMemory("0x0420C0E4");  // strb r2, [r0], #4
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STRB_IB_FAILS) {
  RunInstructionBadMemory("0x0120C0E7");  // strb r2, [r0, r1]
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STRB_IB_I12_FAILS) {
  RunInstructionBadMemory("0x0420C0E5");  // strb r2, [r0, #4]
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STRB_IBW_FAILS) {
  RunInstructionBadMemory("0x0120E0E7");  // strb r2, [r0, r1]!
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STRB_IBW_I12_FAILS) {
  RunInstructionBadMemory("0x0420E0E5");  // strb r2, [r0, #4]!
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STRBT_DAW_FAILS) {
  RunInstructionBadMemory("0x012060E6");  // strbt r2, [r0], -r1
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STRBT_DAW_I12_FAILS) {
  RunInstructionBadMemory("0x042060E4");  // strbt r2, [r0], #-4
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STRBT_IAW_FAILS) {
  RunInstructionBadMemory("0x0120E0E6");  // strbt r2, [r0], r1
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STRBT_IAW_I12_FAILS) {
  RunInstructionBadMemory("0x0420E0E4");  // strbt r2, [r0], #4
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STRH_DAW_FAILS) {
  RunInstructionBadMemory("0xB12000E0");  // strh r2, [r0], -r1
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STRH_DAW_I12_FAILS) {
  RunInstructionBadMemory("0xB42040E0");  // strh r2, [r0], #-4
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STRH_DB_FAILS) {
  RunInstructionBadMemory("0xB12000E1");  // strh r2, [r0, -r1]
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STRH_DB_I12_FAILS) {
  RunInstructionBadMemory("0xB42040E1");  // strh r2, [r0, #-4]
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STRH_DBW_FAILS) {
  RunInstructionBadMemory("0xB12020E1");  // strh r2, [r0, -r1]!
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STRH_DBW_I12_FAILS) {
  RunInstructionBadMemory("0xB42060E1");  // strh r2, [r0, #-4]!
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STRH_IAW_FAILS) {
  RunInstructionBadMemory("0xB12080E0");  // strh r2, [r0], r1
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STRH_IAW_I12_FAILS) {
  RunInstructionBadMemory("0xB420C0E0");  // strh r2, [r0], #4
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STRH_IB_FAILS) {
  RunInstructionBadMemory("0xB12080E1");  // strh r2, [r0, r1]
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STRH_IB_I12_FAILS) {
  RunInstructionBadMemory("0xB420C0E1");  // strh r2, [r0, #4]
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STRH_IBW_FAILS) {
  RunInstructionBadMemory("0xB120A0E1");  // strh r2, [r0, r1]!
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STRH_IBW_I12_FAILS) {
  RunInstructionBadMemory("0xB420E0E1");  // strh r2, [r0, #4]!
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STRT_DAW_FAILS) {
  RunInstructionBadMemory("0x012020E6");  // strt r2, [r0], -r1
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STRT_DAW_I12_FAILS) {
  RunInstructionBadMemory("0x042020E4");  // strt r2, [r0], #-4
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STRT_IAW_FAILS) {
  RunInstructionBadMemory("0x012080E6");  // strt r2, [r0], r1
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, STRT_IAW_I12_FAILS) {
  RunInstructionBadMemory("0x0420A0E4");  // strt r2, [r0], #4
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, SWP_FAILS) {
  RunInstructionBadMemory("0x900001E1");  // swp r0, r0, [r1]
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, SWPB_FAILS) {
  RunInstructionBadMemory("0x900041E1");  // swpb r0, r0, [r1]
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}