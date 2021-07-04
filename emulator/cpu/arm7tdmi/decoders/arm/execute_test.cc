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

    memset(&registers_, 0, sizeof(ArmAllRegisters));
    registers_.current.user.cpsr.mode = MODE_SVC;
    registers_.current.spsr.mode = MODE_USR;
    registers_.current.user.gprs.pc = 0x108;
    registers_.current.user.gprs.sp = 0x200;

    instruction_end_ = 0x100;
    num_instructions_ = 0;
  }

  void TearDown() override { MemoryFree(memory_); }

 protected:
  static bool Load32LE(const void *context, uint32_t address, uint32_t *value) {
    if (address + sizeof(uint32_t) - 1 >= memory_space_.size()) {
      return false;
    }
    char *data = memory_space_.data() + address;
    *value = *reinterpret_cast<uint32_t *>(data);
    return true;
  }

  static bool Load16LE(const void *context, uint32_t address, uint16_t *value) {
    if (address + sizeof(uint16_t) - 1 >= memory_space_.size()) {
      return false;
    }
    char *data = memory_space_.data() + address;
    *value = *reinterpret_cast<uint16_t *>(data);
    return true;
  }

  static bool Load8(const void *context, uint32_t address, uint8_t *value) {
    if (address > memory_space_.size()) {
      return false;
    }
    *value = memory_space_[address];
    return true;
  }

  static bool Store32LE(void *context, uint32_t address, uint32_t value) {
    if (address + sizeof(uint32_t) - 1 >= memory_space_.size()) {
      return false;
    }
    char *data = memory_space_.data() + address;
    *reinterpret_cast<uint32_t *>(data) = value;
    return true;
  }

  static bool Store16LE(void *context, uint32_t address, uint16_t value) {
    if (address + sizeof(uint16_t) - 1 >= memory_space_.size()) {
      return false;
    }
    char *data = memory_space_.data() + address;
    *reinterpret_cast<uint16_t *>(data) = value;
    return true;
  }

  static bool Store8(void *context, uint32_t address, uint8_t value) {
    if (address >= memory_space_.size()) {
      return false;
    }
    memory_space_[address] = value;
    return true;
  }

  // Assumes little-endian hex string
  bool RunInstruction(std::string instruction_hex) {
    StoreInstruction(instruction_hex);
    bool result = ArmInstructionExecute(&registers_, memory_);
    if (result) {
      registers_.current.user.gprs.pc += 8u;
    } else {
      registers_.current.user.gprs.pc += 4u;
    }
    return result;
  }

  static std::vector<char> memory_space_;
  size_t instruction_end_;
  size_t num_instructions_;
  ArmAllRegisters registers_;
  Memory *memory_;

 private:
  void StoreInstruction(std::string instruction_hex) {
    if (instruction_hex[0u] == '0' && instruction_hex[1u] == 'x') {
      instruction_hex.erase(0u, 2u);
    }

    assert(instruction_hex.size() % 2u == 0u);
    for (size_t i = 0u; i < instruction_hex.size(); i += 2u) {
      std::string hex_byte;
      hex_byte += instruction_hex[i];
      hex_byte += instruction_hex[i + 1u];
      unsigned long byte = std::stoul(hex_byte, nullptr, 16u);
      EXPECT_TRUE(Store8(nullptr, instruction_end_++, (uint8_t)byte));
    }
  }
};

std::vector<char> ExecuteTest::memory_space_(1024u, 0);

TEST_F(ExecuteTest, ExecutionSkipped) {
  registers_.current.user.gprs.r0 = 1u;
  EXPECT_FALSE(RunInstruction("0x00008000"));  // addeq r0, r0, r0
  EXPECT_EQ(1u, registers_.current.user.gprs.r0);
}

TEST_F(ExecuteTest, ADC) {
  registers_.current.user.gprs.r0 = 1u;
  registers_.current.user.cpsr.carry = true;
  EXPECT_FALSE(RunInstruction("0x0000A0E0"));  // adc r0, r0, r0
  EXPECT_EQ(3u, registers_.current.user.gprs.r0);

  // Modifies PC
  EXPECT_TRUE(RunInstruction("0x00F0A0E0"));  // adc r15, r0, r0
}

TEST_F(ExecuteTest, ADC_I32) {
  registers_.current.user.gprs.r0 = 1u;
  registers_.current.user.cpsr.carry = true;
  EXPECT_FALSE(RunInstruction("0x0200A0E2"));  // adc r0, r0, #2
  EXPECT_EQ(4u, registers_.current.user.gprs.r0);

  // Modifies PC
  EXPECT_TRUE(RunInstruction("0x01F0A0E2"));  // adc r15, r0, #1
}

TEST_F(ExecuteTest, ADCS) {
  registers_.current.user.gprs.r0 = 0xFFFFFFFFu;
  registers_.current.user.cpsr.carry = true;
  EXPECT_FALSE(RunInstruction("0x0100B0E0"));  // adcs r0, r0, r1
  EXPECT_EQ(0u, registers_.current.user.gprs.r0);
  EXPECT_TRUE(registers_.current.user.cpsr.zero);
  EXPECT_TRUE(registers_.current.user.cpsr.carry);

  // Modifies PC
  EXPECT_TRUE(RunInstruction("0x01F0B0E0"));  // adcs r15, r0, r0
}

TEST_F(ExecuteTest, ADCS_I32) {
  registers_.current.user.gprs.r0 = 0xFFFFFFFFu;
  registers_.current.user.cpsr.carry = true;
  EXPECT_FALSE(RunInstruction("0x0000B0E2"));  // adcs r0, r0, #0
  EXPECT_EQ(0u, registers_.current.user.gprs.r0);
  EXPECT_TRUE(registers_.current.user.cpsr.zero);
  EXPECT_TRUE(registers_.current.user.cpsr.carry);

  // Modifies PC
  EXPECT_TRUE(RunInstruction("0x00F0B0E2"));  // adcs r15, r0, #0
}

TEST_F(ExecuteTest, ADD) {
  registers_.current.user.gprs.r0 = 1u;
  EXPECT_FALSE(RunInstruction("0x000080E0"));  // add r0, r0, r0
  EXPECT_EQ(2u, registers_.current.user.gprs.r0);

  // Modifies PC
  EXPECT_TRUE(RunInstruction("0x00F080E0"));  // add r15, r0, r0
}

TEST_F(ExecuteTest, ADD_I32) {
  registers_.current.user.gprs.r0 = 1u;
  EXPECT_FALSE(RunInstruction("0x020080E2"));  // add r0, r0, #2
  EXPECT_EQ(3u, registers_.current.user.gprs.r0);

  // Modifies PC
  EXPECT_TRUE(RunInstruction("0x02F080E2"));  // add r15, r0, #2
}

TEST_F(ExecuteTest, ADDS) {
  registers_.current.user.gprs.r0 = 0xFFFFFFFFu;
  registers_.current.user.gprs.r1 = 0x1u;
  EXPECT_FALSE(RunInstruction("0x010090E0"));  // adds r0, r0, r1
  EXPECT_EQ(0u, registers_.current.user.gprs.r0);
  EXPECT_TRUE(registers_.current.user.cpsr.zero);
  EXPECT_TRUE(registers_.current.user.cpsr.carry);

  // Modifies PC
  EXPECT_TRUE(RunInstruction("0x01F090E0"));  // adds r15, r0, r1
}

TEST_F(ExecuteTest, ADDS_I32) {
  registers_.current.user.gprs.r0 = 0xFFFFFFFFu;
  EXPECT_FALSE(RunInstruction("0x010090E2"));  // adds r0, r0, #1
  EXPECT_EQ(0u, registers_.current.user.gprs.r0);
  EXPECT_TRUE(registers_.current.user.cpsr.zero);
  EXPECT_TRUE(registers_.current.user.cpsr.carry);

  // Modifies PC
  EXPECT_TRUE(RunInstruction("0x01F090E2"));  // adds r15, r0, #1
}

TEST_F(ExecuteTest, AND) {
  registers_.current.user.gprs.r0 = 0xFF00FF00u;
  registers_.current.user.gprs.r1 = 0x77777777u;
  EXPECT_FALSE(RunInstruction("0x010000E0"));  // and r0, r0, r1
  EXPECT_EQ(0x77007700u, registers_.current.user.gprs.r0);

  // Modifies PC
  EXPECT_TRUE(RunInstruction("0x01F000E0"));  // and r15, r0, r1
}

TEST_F(ExecuteTest, AND_I32) {
  registers_.current.user.gprs.r0 = 0x77777777u;
  EXPECT_FALSE(RunInstruction("0xFF0000E2"));  // and r0, r0, #0xFF
  EXPECT_EQ(0x77u, registers_.current.user.gprs.r0);

  // Modifies PC
  EXPECT_TRUE(RunInstruction("0xFFF000E2"));  // and r15, r0, #0xFF
}

TEST_F(ExecuteTest, ANDS) {
  registers_.current.user.gprs.r0 = 0xFF00FF00u;
  registers_.current.user.gprs.r1 = 0xF7777777u;
  EXPECT_FALSE(RunInstruction("0x810010E0"));  // ands r0, r0, r1, lsl #1
  EXPECT_EQ(0xEE00EE00u, registers_.current.user.gprs.r0);
  EXPECT_TRUE(registers_.current.user.cpsr.carry);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);

  // Modifies PC
  EXPECT_TRUE(RunInstruction("0x81F010E0"));  // ands r15, r0, r1, lsl #1
}

TEST_F(ExecuteTest, ANDS_I32) {
  registers_.current.user.gprs.r0 = 0x77777700u;
  EXPECT_FALSE(RunInstruction("0xFF0010E2"));  // ands r0, r0, #0xFF
  EXPECT_EQ(0u, registers_.current.user.gprs.r0);
  EXPECT_TRUE(registers_.current.user.cpsr.zero);

  // Modifies PC
  EXPECT_TRUE(RunInstruction("0xFFF010E2"));  // ands r15, r0, #0xFF
}

TEST_F(ExecuteTest, B) {
  EXPECT_TRUE(RunInstruction("0x3E0000EA"));  // b #0x100
  EXPECT_EQ(0x208u, registers_.current.user.gprs.pc);
}

TEST_F(ExecuteTest, BIC) {
  registers_.current.user.gprs.r0 = 0xFF00FF00u;
  registers_.current.user.gprs.r1 = 0x88888888u;
  EXPECT_FALSE(RunInstruction("0x0100C0E1"));  // bic r0, r0, r1
  EXPECT_EQ(0x77007700u, registers_.current.user.gprs.r0);

  // Modifies PC
  EXPECT_TRUE(RunInstruction("0x01F0C0E1"));  // bic r15, r0, r1
}

TEST_F(ExecuteTest, BIC_I32) {
  registers_.current.user.gprs.r0 = 0x77777777u;
  EXPECT_FALSE(RunInstruction("0xFF00C0E3"));  // bic r0, r0, #0xFF
  EXPECT_EQ(0x77777700u, registers_.current.user.gprs.r0);

  // Modifies PC
  EXPECT_TRUE(RunInstruction("0xFFF0C0E3"));  // bic r15, r0, #0xFF
}

TEST_F(ExecuteTest, BICS) {
  registers_.current.user.gprs.r0 = 0xFF00FF00u;
  registers_.current.user.gprs.r1 = 0xF7777777u;
  EXPECT_FALSE(RunInstruction("0x8100D0E1"));  // bics r0, r0, r1, lsl #1
  EXPECT_EQ(0x11001100u, registers_.current.user.gprs.r0);
  EXPECT_TRUE(registers_.current.user.cpsr.carry);

  // Modifies PC
  EXPECT_TRUE(RunInstruction("0x81F0D0E1"));  // bics r15, r0, r1, lsl #1
}

TEST_F(ExecuteTest, BICS_I32) {
  registers_.current.user.gprs.r0 = 0x000000FFu;
  EXPECT_FALSE(RunInstruction("0xFF00D0E3"));  // bics r0, r0, #0xFF
  EXPECT_EQ(0u, registers_.current.user.gprs.r0);
  EXPECT_TRUE(registers_.current.user.cpsr.zero);

  // Modifies PC
  EXPECT_TRUE(RunInstruction("0xFFF0D0E3"));  // bics r15, r0, #0xFF
}

TEST_F(ExecuteTest, BL) {
  EXPECT_TRUE(RunInstruction("0x3E0000EB"));  // bl #0x100
  EXPECT_EQ(0x208u, registers_.current.user.gprs.pc);
  EXPECT_EQ(0x104u, registers_.current.user.gprs.lr);
}

TEST_F(ExecuteTest, BX) {
  registers_.current.user.gprs.r0 = 0x80u;
  EXPECT_TRUE(RunInstruction("0x10FF2FE1"));  // bx r0
  EXPECT_EQ(0x88u, registers_.current.user.gprs.pc);
}

TEST_F(ExecuteTest, CDP) {
  // NOP
}

TEST_F(ExecuteTest, CMN) {
  registers_.current.user.gprs.r0 = 0xFFFFFFFFu;
  registers_.current.user.gprs.r1 = 0x1u;
  EXPECT_FALSE(RunInstruction("0x010070E1"));  // cmn r0, r1
  EXPECT_TRUE(registers_.current.user.cpsr.carry);
  EXPECT_TRUE(registers_.current.user.cpsr.zero);
}

TEST_F(ExecuteTest, CMN_I32) {
  registers_.current.user.gprs.r0 = 0xFFFFFFFFu;
  EXPECT_FALSE(RunInstruction("0x010070E3"));  // cmn r0, #1
  EXPECT_TRUE(registers_.current.user.cpsr.carry);
  EXPECT_TRUE(registers_.current.user.cpsr.zero);
}

TEST_F(ExecuteTest, CMP) {
  registers_.current.user.gprs.r0 = 0x0u;
  EXPECT_FALSE(RunInstruction("0x010050E1"));  // cmp r0, r1
  EXPECT_TRUE(registers_.current.user.cpsr.zero);
}

TEST_F(ExecuteTest, CMP_I32) {
  registers_.current.user.gprs.r0 = 0x1u;
  EXPECT_FALSE(RunInstruction("0x010050E3"));  // cmp r0, #1
  EXPECT_TRUE(registers_.current.user.cpsr.zero);
}

TEST_F(ExecuteTest, EOR) {
  registers_.current.user.gprs.r0 = 0xFF00FF00u;
  registers_.current.user.gprs.r1 = 0x77777777u;
  EXPECT_FALSE(RunInstruction("0x010020E0"));  // eor r0, r0, r1
  EXPECT_EQ(0x88778877u, registers_.current.user.gprs.r0);

  // Modifies PC
  EXPECT_TRUE(RunInstruction("0x01F020E0"));  // eor r15, r0, r1
}

TEST_F(ExecuteTest, EOR_I32) {
  registers_.current.user.gprs.r0 = 0xFF00FF00u;
  EXPECT_FALSE(RunInstruction("0xFF0020E2"));  // eor r0, r0, #0xFF
  EXPECT_EQ(0xFF00FFFFu, registers_.current.user.gprs.r0);

  // Modifies PC
  EXPECT_TRUE(RunInstruction("0xFFF020E2"));  // eor r15, r0, #0xFF
}

TEST_F(ExecuteTest, EORS) {
  registers_.current.user.gprs.r0 = 0xFF00FF00u;
  registers_.current.user.gprs.r1 = 0xF7777777u;
  EXPECT_FALSE(RunInstruction("0x810030E0"));  // eors r0, r0, r1, lsl #1
  EXPECT_EQ(0x11EE11EEu, registers_.current.user.gprs.r0);
  EXPECT_TRUE(registers_.current.user.cpsr.carry);

  // Modifies PC
  EXPECT_TRUE(RunInstruction("0x81F030E0"));  // eors r15, r0, r1, lsl #1
}

TEST_F(ExecuteTest, EORS_I32) {
  registers_.current.user.gprs.r0 = 0xFF00FF00u;
  EXPECT_FALSE(RunInstruction("0xFF0030E2"));  // eors r0, r0, #0xFF
  EXPECT_EQ(0xFF00FFFFu, registers_.current.user.gprs.r0);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);

  // Modifies PC
  EXPECT_TRUE(RunInstruction("0xFFF030E2"));  // eors r15, r0, #0xFF
}

TEST_F(ExecuteTest, LDC) {
  // NOP
}

TEST_F(ExecuteTest, LDMDA) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 1u));
  ASSERT_TRUE(Store32LE(nullptr, 0x304, 2u));
  ASSERT_TRUE(Store32LE(nullptr, 0x308, 3u));
  registers_.current.user.gprs.r0 = 0x308u;
  EXPECT_FALSE(RunInstruction("0x070010E8"));  // ldmda r0, {r0-r2}
  EXPECT_EQ(1u, registers_.current.user.gprs.r0);
  EXPECT_EQ(2u, registers_.current.user.gprs.r1);
  EXPECT_EQ(3u, registers_.current.user.gprs.r2);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x308u;
  EXPECT_TRUE(RunInstruction("0x038010E8"));  // ldmda r0, {r0, r1, pc}
}

TEST_F(ExecuteTest, LDMDA_W) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 1u));
  ASSERT_TRUE(Store32LE(nullptr, 0x304, 2u));
  ASSERT_TRUE(Store32LE(nullptr, 0x308, 3u));
  registers_.current.user.gprs.r0 = 0x308u;
  EXPECT_FALSE(RunInstruction("0x0E0030E8"));  // ldmda r0!, {r1-r3}
  EXPECT_EQ(0x2FCu, registers_.current.user.gprs.r0);
  EXPECT_EQ(1u, registers_.current.user.gprs.r1);
  EXPECT_EQ(2u, registers_.current.user.gprs.r2);
  EXPECT_EQ(3u, registers_.current.user.gprs.r3);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x308u;
  EXPECT_TRUE(RunInstruction("0x038010E8"));  // ldmda r0!, {r1, r2, pc}

  registers_.current.user.gprs.pc = pc + 4u;
  EXPECT_TRUE(RunInstruction("0x07003FE8"));  // ldmda pc!, {r0-r2}
}

TEST_F(ExecuteTest, LDMDB) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 1u));
  ASSERT_TRUE(Store32LE(nullptr, 0x304, 2u));
  ASSERT_TRUE(Store32LE(nullptr, 0x308, 3u));
  registers_.current.user.gprs.r0 = 0x30Cu;
  EXPECT_FALSE(RunInstruction("0x070010E9"));  // ldmdb r0, {r0-r2}
  EXPECT_EQ(1u, registers_.current.user.gprs.r0);
  EXPECT_EQ(2u, registers_.current.user.gprs.r1);
  EXPECT_EQ(3u, registers_.current.user.gprs.r2);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x30Cu;
  EXPECT_TRUE(RunInstruction("0x038010E9"));  // ldmdb r0, {r0, r1, pc}
}

TEST_F(ExecuteTest, LDMDB_W) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 1u));
  ASSERT_TRUE(Store32LE(nullptr, 0x304, 2u));
  ASSERT_TRUE(Store32LE(nullptr, 0x308, 3u));
  registers_.current.user.gprs.r0 = 0x30Cu;
  EXPECT_FALSE(RunInstruction("0x0E0030E9"));  // ldmdb r0!, {r1-r3}
  EXPECT_EQ(0x300u, registers_.current.user.gprs.r0);
  EXPECT_EQ(1u, registers_.current.user.gprs.r1);
  EXPECT_EQ(2u, registers_.current.user.gprs.r2);
  EXPECT_EQ(3u, registers_.current.user.gprs.r3);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x30Cu;
  EXPECT_TRUE(RunInstruction("0x068030E9"));  // ldmdb r0!, {r1, r2, pc}

  registers_.current.user.gprs.pc = pc + 4u;
  EXPECT_TRUE(RunInstruction("0x07003FE9"));  // ldmdb pc!, {r0-r2}
}

TEST_F(ExecuteTest, LDMIA) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 1u));
  ASSERT_TRUE(Store32LE(nullptr, 0x304, 2u));
  ASSERT_TRUE(Store32LE(nullptr, 0x308, 3u));
  registers_.current.user.gprs.r0 = 0x300u;
  EXPECT_FALSE(RunInstruction("0x070090E8"));  // ldmia r0, {r0-r2}
  EXPECT_EQ(1u, registers_.current.user.gprs.r0);
  EXPECT_EQ(2u, registers_.current.user.gprs.r1);
  EXPECT_EQ(3u, registers_.current.user.gprs.r2);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x300u;
  EXPECT_TRUE(RunInstruction("0x038010E9"));  // ldmia r0, {r0, r1, pc}
}

TEST_F(ExecuteTest, LDMIA_W) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 1u));
  ASSERT_TRUE(Store32LE(nullptr, 0x304, 2u));
  ASSERT_TRUE(Store32LE(nullptr, 0x308, 3u));
  registers_.current.user.gprs.r0 = 0x300u;
  EXPECT_FALSE(RunInstruction("0x0E00B0E8"));  // ldmia r0!, {r1-r3}
  EXPECT_EQ(0x30Cu, registers_.current.user.gprs.r0);
  EXPECT_EQ(1u, registers_.current.user.gprs.r1);
  EXPECT_EQ(2u, registers_.current.user.gprs.r2);
  EXPECT_EQ(3u, registers_.current.user.gprs.r3);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x300u;
  EXPECT_TRUE(RunInstruction("0x0680B0E8"));  // ldmia r0!, {r1, r2, pc}

  registers_.current.user.gprs.pc = pc + 4u;
  EXPECT_TRUE(RunInstruction("0x0700BFE8"));  // ldmia pc!, {r0-r2}
}

TEST_F(ExecuteTest, LDMIB) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 1u));
  ASSERT_TRUE(Store32LE(nullptr, 0x304, 2u));
  ASSERT_TRUE(Store32LE(nullptr, 0x308, 3u));
  registers_.current.user.gprs.r0 = 0x2FCu;
  EXPECT_FALSE(RunInstruction("0x070090E9"));  // ldmib r0, {r0-r2}
  EXPECT_EQ(1u, registers_.current.user.gprs.r0);
  EXPECT_EQ(2u, registers_.current.user.gprs.r1);
  EXPECT_EQ(3u, registers_.current.user.gprs.r2);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x2FCu;
  EXPECT_TRUE(RunInstruction("0x038090E9"));  // ldmib r0, {r0, r1, pc}
}

TEST_F(ExecuteTest, LDMIB_W) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 1u));
  ASSERT_TRUE(Store32LE(nullptr, 0x304, 2u));
  ASSERT_TRUE(Store32LE(nullptr, 0x308, 3u));
  registers_.current.user.gprs.r0 = 0x2FCu;
  EXPECT_FALSE(RunInstruction("0x0E00B0E9"));  // ldmib r0!, {r1-r3}
  EXPECT_EQ(0x308u, registers_.current.user.gprs.r0);
  EXPECT_EQ(1u, registers_.current.user.gprs.r1);
  EXPECT_EQ(2u, registers_.current.user.gprs.r2);
  EXPECT_EQ(3u, registers_.current.user.gprs.r3);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x2FCu;
  EXPECT_TRUE(RunInstruction("0x0680B0E9"));  // ldmib r0!, {r1, r2, pc}

  registers_.current.user.gprs.pc = pc + 4u;
  EXPECT_TRUE(RunInstruction("0x0700BFE9"));  // ldmib pc!, {r0-r2}
}

TEST_F(ExecuteTest, LDMSDA) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 1u));
  ASSERT_TRUE(Store32LE(nullptr, 0x304, 2u));
  ASSERT_TRUE(Store32LE(nullptr, 0x308, 3u));
  registers_.current.user.gprs.r0 = 0x308u;
  EXPECT_FALSE(RunInstruction("0x0E0050E8"));  // ldmda r0, {r1-r3}^
  EXPECT_EQ(1u, registers_.current.user.gprs.r1);
  EXPECT_EQ(2u, registers_.current.user.gprs.r2);
  EXPECT_EQ(3u, registers_.current.user.gprs.r3);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x308u;
  EXPECT_TRUE(RunInstruction("0x068050E8"));  // ldmda r0, {r1, r2, pc}^
  EXPECT_EQ(MODE_USR, registers_.current.user.cpsr.mode);
}

TEST_F(ExecuteTest, LDMSDA_W) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 1u));
  ASSERT_TRUE(Store32LE(nullptr, 0x304, 2u));
  ASSERT_TRUE(Store32LE(nullptr, 0x308, 3u));
  registers_.current.user.gprs.r0 = 0x308u;
  EXPECT_FALSE(RunInstruction("0x0E0070E8"));  // ldmda r0!, {r1-r3}^
  EXPECT_EQ(0x2FCu, registers_.current.user.gprs.r0);
  EXPECT_EQ(1u, registers_.current.user.gprs.r1);
  EXPECT_EQ(2u, registers_.current.user.gprs.r2);
  EXPECT_EQ(3u, registers_.current.user.gprs.r3);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x308u;
  EXPECT_TRUE(RunInstruction("0x068070E8"));  // ldmda r0!, {r1, r2, pc}^
  EXPECT_EQ(MODE_USR, registers_.current.user.cpsr.mode);

  registers_.current.user.gprs.pc = pc + 4u;
  EXPECT_TRUE(RunInstruction("0x0E007FE8"));  // ldmda pc!, {r1-r3}^
}

TEST_F(ExecuteTest, LDMSDB) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 1u));
  ASSERT_TRUE(Store32LE(nullptr, 0x304, 2u));
  ASSERT_TRUE(Store32LE(nullptr, 0x308, 3u));
  registers_.current.user.gprs.r0 = 0x30Cu;
  EXPECT_FALSE(RunInstruction("0x0E0050E9"));  // ldmdb r0, {r1-r3}^
  EXPECT_EQ(1u, registers_.current.user.gprs.r1);
  EXPECT_EQ(2u, registers_.current.user.gprs.r2);
  EXPECT_EQ(3u, registers_.current.user.gprs.r3);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x30Cu;
  EXPECT_TRUE(RunInstruction("0x068050E9"));  // ldmdb r0, {r1, r2, pc}^
  EXPECT_EQ(MODE_USR, registers_.current.user.cpsr.mode);
}

TEST_F(ExecuteTest, LDMSDB_W) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 1u));
  ASSERT_TRUE(Store32LE(nullptr, 0x304, 2u));
  ASSERT_TRUE(Store32LE(nullptr, 0x308, 3u));
  registers_.current.user.gprs.r0 = 0x30Cu;
  EXPECT_FALSE(RunInstruction("0x0E0070E9"));  // ldmdb r0!, {r1-r3}^
  EXPECT_EQ(0x300u, registers_.current.user.gprs.r0);
  EXPECT_EQ(1u, registers_.current.user.gprs.r1);
  EXPECT_EQ(2u, registers_.current.user.gprs.r2);
  EXPECT_EQ(3u, registers_.current.user.gprs.r3);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x30Cu;
  EXPECT_TRUE(RunInstruction("0x068070E9"));  // ldmdb r0!, {r1, r2, pc}^
  EXPECT_EQ(MODE_USR, registers_.current.user.cpsr.mode);

  registers_.current.user.gprs.pc = pc + 4u;
  EXPECT_TRUE(RunInstruction("0x0E007FE9"));  // ldmdb pc!, {r1-r3}^
}

TEST_F(ExecuteTest, LDMSIA) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 1u));
  ASSERT_TRUE(Store32LE(nullptr, 0x304, 2u));
  ASSERT_TRUE(Store32LE(nullptr, 0x308, 3u));
  registers_.current.user.gprs.r0 = 0x300u;
  EXPECT_FALSE(RunInstruction("0x0E00D0E8"));  // ldmia r0, {r1-r3}^
  EXPECT_EQ(1u, registers_.current.user.gprs.r1);
  EXPECT_EQ(2u, registers_.current.user.gprs.r2);
  EXPECT_EQ(3u, registers_.current.user.gprs.r3);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x300u;
  EXPECT_TRUE(RunInstruction("0x0380D0E8"));  // ldmia r0, {r0, r1, pc}^
  EXPECT_EQ(MODE_USR, registers_.current.user.cpsr.mode);
}

TEST_F(ExecuteTest, LDMSIA_W) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 1u));
  ASSERT_TRUE(Store32LE(nullptr, 0x304, 2u));
  ASSERT_TRUE(Store32LE(nullptr, 0x308, 3u));
  registers_.current.user.gprs.r0 = 0x300u;
  EXPECT_FALSE(RunInstruction("0x0E00F0E8"));  // ldmia r0!, {r1-r3}^
  EXPECT_EQ(0x30Cu, registers_.current.user.gprs.r0);
  EXPECT_EQ(1u, registers_.current.user.gprs.r1);
  EXPECT_EQ(2u, registers_.current.user.gprs.r2);
  EXPECT_EQ(3u, registers_.current.user.gprs.r3);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x300u;
  EXPECT_TRUE(RunInstruction("0x0680F0E8"));  // ldmia r0!, {r1, r2, pc}^
  EXPECT_EQ(MODE_USR, registers_.current.user.cpsr.mode);

  registers_.current.user.gprs.pc = pc + 4u;
  EXPECT_TRUE(RunInstruction("0x0E00FFE8"));  // ldmia pc!, {r1-r3}^
}

TEST_F(ExecuteTest, LDMSIB) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 1u));
  ASSERT_TRUE(Store32LE(nullptr, 0x304, 2u));
  ASSERT_TRUE(Store32LE(nullptr, 0x308, 3u));
  registers_.current.user.gprs.r0 = 0x2FCu;
  EXPECT_FALSE(RunInstruction("0x0E00D0E9"));  // ldmib r0, {r1-r3}^
  EXPECT_EQ(1u, registers_.current.user.gprs.r1);
  EXPECT_EQ(2u, registers_.current.user.gprs.r2);
  EXPECT_EQ(3u, registers_.current.user.gprs.r3);

  // Modifies PC
  registers_.current.user.gprs.r0 = 0x2FCu;
  EXPECT_TRUE(RunInstruction("0x0380D0E9"));  // ldmib r0, {r0, r1, pc}^
  EXPECT_EQ(MODE_USR, registers_.current.user.cpsr.mode);
}

TEST_F(ExecuteTest, LDMSIB_W) {
  ASSERT_TRUE(Store32LE(nullptr, 0x300, 1u));
  ASSERT_TRUE(Store32LE(nullptr, 0x304, 2u));
  ASSERT_TRUE(Store32LE(nullptr, 0x308, 3u));
  registers_.current.user.gprs.r0 = 0x2FCu;
  EXPECT_FALSE(RunInstruction("0x0E00F0E9"));  // ldmib r0!, {r1-r3}^
  EXPECT_EQ(0x308u, registers_.current.user.gprs.r0);
  EXPECT_EQ(1u, registers_.current.user.gprs.r1);
  EXPECT_EQ(2u, registers_.current.user.gprs.r2);
  EXPECT_EQ(3u, registers_.current.user.gprs.r3);

  // Modifies PC
  uint32_t pc = registers_.current.user.gprs.pc;
  registers_.current.user.gprs.r0 = 0x2FCu;
  EXPECT_TRUE(RunInstruction("0x0680F0E9"));  // ldmib r0!, {r1, r2, pc}^
  EXPECT_EQ(MODE_USR, registers_.current.user.cpsr.mode);

  registers_.current.user.gprs.pc = pc + 4u;
  EXPECT_TRUE(RunInstruction("0x0E00FFE9"));  // ldmib pc!, {r1-r3}^
}

TEST_F(ExecuteTest, LDR_DAW) {}

TEST_F(ExecuteTest, LDR_DAW_I12) {}

TEST_F(ExecuteTest, LDR_DB) {}

TEST_F(ExecuteTest, LDR_DB_I12) {}

TEST_F(ExecuteTest, LDR_DBW) {}

TEST_F(ExecuteTest, LDR_DBW_I12) {}

TEST_F(ExecuteTest, LDR_IAW) {}

TEST_F(ExecuteTest, LDR_IAW_I12) {}

TEST_F(ExecuteTest, LDR_IB) {}

TEST_F(ExecuteTest, LDR_IB_I12) {}

TEST_F(ExecuteTest, LDR_IBW) {}

TEST_F(ExecuteTest, LDR_IBW_I12) {}

TEST_F(ExecuteTest, LDRB_DAW) {}

TEST_F(ExecuteTest, LDRB_DAW_I12) {}

TEST_F(ExecuteTest, LDRB_DB) {}

TEST_F(ExecuteTest, LDRB_DB_I12) {}

TEST_F(ExecuteTest, LDRB_DBW) {}

TEST_F(ExecuteTest, LDRB_DBW_I12) {}

TEST_F(ExecuteTest, LDRB_IAW) {}

TEST_F(ExecuteTest, LDRB_IAW_I12) {}

TEST_F(ExecuteTest, LDRB_IB) {}

TEST_F(ExecuteTest, LDRB_IB_I12) {}

TEST_F(ExecuteTest, LDRB_IBW) {}

TEST_F(ExecuteTest, LDRB_IBW_I12) {}

TEST_F(ExecuteTest, LDRBT_DAW) {}

TEST_F(ExecuteTest, LDRBT_DAW_I12) {}

TEST_F(ExecuteTest, LDRBT_IAW) {}

TEST_F(ExecuteTest, LDRBT_IAW_I12) {}

TEST_F(ExecuteTest, LDRH_DAW) {}

TEST_F(ExecuteTest, LDRH_DAW_I8) {}

TEST_F(ExecuteTest, LDRH_DB) {}

TEST_F(ExecuteTest, LDRH_DB_I8) {}

TEST_F(ExecuteTest, LDRH_DBW) {}

TEST_F(ExecuteTest, LDRH_DBW_I8) {}

TEST_F(ExecuteTest, LDRH_IAW) {}

TEST_F(ExecuteTest, LDRH_IAW_I8) {}

TEST_F(ExecuteTest, LDRH_IB) {}

TEST_F(ExecuteTest, LDRH_IB_I8) {}

TEST_F(ExecuteTest, LDRH_IBW) {}

TEST_F(ExecuteTest, LDRH_IBW_I8) {}

TEST_F(ExecuteTest, LDRSB_DAW) {}

TEST_F(ExecuteTest, LDRSB_DAW_I8) {}

TEST_F(ExecuteTest, LDRSB_DB) {}

TEST_F(ExecuteTest, LDRSB_DB_I8) {}

TEST_F(ExecuteTest, LDRSB_DBW) {}

TEST_F(ExecuteTest, LDRSB_DBW_I8) {}

TEST_F(ExecuteTest, LDRSB_IAW) {}

TEST_F(ExecuteTest, LDRSB_IAW_I8) {}

TEST_F(ExecuteTest, LDRSB_IB) {}

TEST_F(ExecuteTest, LDRSB_IB_I8) {}

TEST_F(ExecuteTest, LDRSB_IBW) {}

TEST_F(ExecuteTest, LDRSB_IBW_I8) {}

TEST_F(ExecuteTest, LDRSH_DAW) {}

TEST_F(ExecuteTest, LDRSH_DAW_I8) {}

TEST_F(ExecuteTest, LDRSH_DB) {}

TEST_F(ExecuteTest, LDRSH_DB_I8) {}

TEST_F(ExecuteTest, LDRSH_DBW) {}

TEST_F(ExecuteTest, LDRSH_DBW_I8) {}

TEST_F(ExecuteTest, LDRSH_IAW) {}

TEST_F(ExecuteTest, LDRSH_IAW_I8) {}

TEST_F(ExecuteTest, LDRSH_IB) {}

TEST_F(ExecuteTest, LDRSH_IB_I8) {}

TEST_F(ExecuteTest, LDRSH_IBW) {}

TEST_F(ExecuteTest, LDRSH_IBW_I8) {}

TEST_F(ExecuteTest, LDRT_DAW) {}

TEST_F(ExecuteTest, LDRT_DAW_I12) {}

TEST_F(ExecuteTest, LDRT_IAW) {}

TEST_F(ExecuteTest, LDRT_IAW_I12) {}

TEST_F(ExecuteTest, MCR) {
  // NOP
}

TEST_F(ExecuteTest, MLA) {
  registers_.current.user.gprs.r0 = 0x2u;
  EXPECT_FALSE(RunInstruction("0x900020E0"));  // mla r0, r0, r0, r0
  EXPECT_EQ(0x6u, registers_.current.user.gprs.r0);

  // This is technically unpredictable behavior and ARM assemblers should not
  // allow r15 to be specified as the destination operand for this instruction
  EXPECT_TRUE(RunInstruction("0x90002FE0"));  // mla r15, r0, r0, r0
}

TEST_F(ExecuteTest, MLAS) {
  registers_.current.user.gprs.r0 = 0xFFFFu;
  EXPECT_FALSE(RunInstruction("0x900030E0"));  // mlas r0, r0, r0, r0
  EXPECT_EQ(0xFFFF0000u, registers_.current.user.gprs.r0);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);

  // This is technically unpredictable behavior and ARM assemblers should not
  // allow r15 to be specified as the destination operand for this instruction
  EXPECT_TRUE(RunInstruction("0x90003FE0"));  // mlas r15, r0, r0, r0
}

TEST_F(ExecuteTest, MOV) {
  registers_.current.user.gprs.r0 = 0xFF00FF00u;
  EXPECT_FALSE(RunInstruction("0x0010A0E1"));  // mov r1, r0
  EXPECT_EQ(0xFF00FF00u, registers_.current.user.gprs.r1);

  // Modifies PC
  EXPECT_TRUE(RunInstruction("0x00F0A0E1"));  // mov r15, r0
}

TEST_F(ExecuteTest, MOV_I32) {
  EXPECT_FALSE(RunInstruction("0xFF02A0E3"));  // mov r0, #0xF000000F
  EXPECT_EQ(0xF000000F, registers_.current.user.gprs.r0);

  // Modifies PC
  EXPECT_TRUE(RunInstruction("0xFFF2A0E3"));  // mov r15, #0xF000000F
}

TEST_F(ExecuteTest, MOVS) {
  registers_.current.user.gprs.r0 = 0xFF00FF00u;
  EXPECT_FALSE(RunInstruction("0x8010B0E1"));  // movs r1, r0, lsl #1
  EXPECT_EQ(0xFE01FE00u, registers_.current.user.gprs.r1);
  EXPECT_TRUE(registers_.current.user.cpsr.carry);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);

  // Modifies PC
  EXPECT_TRUE(RunInstruction("0x00F0B0E1"));  // movs r15, r0
}

TEST_F(ExecuteTest, MOVS_I32) {
  EXPECT_FALSE(RunInstruction("0xFF02B0E3"));  // movs r0, #0xF000000F
  EXPECT_EQ(0xF000000F, registers_.current.user.gprs.r0);
  EXPECT_TRUE(registers_.current.user.cpsr.carry);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);

  // Modifies PC
  EXPECT_TRUE(RunInstruction("0xFFF2B0E3"));  // movs r15, #0xF000000F
}

TEST_F(ExecuteTest, MRC) {
  // NOP
}

TEST_F(ExecuteTest, MRS_CPSR) {
  EXPECT_FALSE(RunInstruction("0x00000FE1"));  // mrs r0, cpsr
  EXPECT_EQ(registers_.current.user.cpsr.value,
            registers_.current.user.gprs.r0);
}

TEST_F(ExecuteTest, MRS_SPSR) {
  EXPECT_FALSE(RunInstruction("0x00004FE1"));  // mrs r0, spsr
  EXPECT_EQ(registers_.current.spsr.value, registers_.current.user.gprs.r0);
}

TEST_F(ExecuteTest, MSR_CPSR) {
  ArmProgramStatusRegister status;
  status.value = registers_.current.user.cpsr.value;
  status.mode = MODE_FIQ;
  registers_.current.user.gprs.r0 = status.value;
  EXPECT_FALSE(RunInstruction("0x00F02FE1"));  // msr cpsr_cxsf, r0
  EXPECT_EQ(status.value, registers_.current.user.cpsr.value);
}

TEST_F(ExecuteTest, MSR_CPSR_I8) {
  EXPECT_FALSE(RunInstruction("0xFFF428E3"));  // msr cpsr_f, #0xFF000000
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
  EXPECT_FALSE(RunInstruction("0x00F06FE1"));  // msr spsr_cxsf, r0
  EXPECT_EQ(status.value, registers_.current.spsr.value);
}

TEST_F(ExecuteTest, MSR_SPSR_I8) {
  EXPECT_FALSE(RunInstruction("0xFFF468E3"));  // msr spsr_f, #0xFF000000
  EXPECT_TRUE(registers_.current.spsr.overflow);
  EXPECT_TRUE(registers_.current.spsr.carry);
  EXPECT_TRUE(registers_.current.spsr.zero);
  EXPECT_TRUE(registers_.current.spsr.negative);
}

TEST_F(ExecuteTest, MUL) {
  registers_.current.user.gprs.r0 = 0x2u;
  EXPECT_FALSE(RunInstruction("0x900000E0"));  // mul r0, r0, r0
  EXPECT_EQ(0x4u, registers_.current.user.gprs.r0);

  // This is technically unpredictable behavior and ARM assemblers should not
  // allow r15 to be specified as the destination operand for this instruction
  EXPECT_TRUE(RunInstruction("0x90000FE0"));  // mul r15, r0, r0
}

TEST_F(ExecuteTest, MULS) {
  registers_.current.user.gprs.r0 = 0xFFFFu;
  EXPECT_FALSE(RunInstruction("0x900010E0"));  // muls r0, r0, r0
  EXPECT_EQ(0xFFFE0001, registers_.current.user.gprs.r0);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);

  // This is technically unpredictable behavior and ARM assemblers should not
  // allow r15 to be specified as the destination operand for this instruction
  EXPECT_TRUE(RunInstruction("0x90001FE0"));  // muls r15, r0, r0
}

TEST_F(ExecuteTest, MVN) {
  registers_.current.user.gprs.r0 = 0xFF00FF00u;
  EXPECT_FALSE(RunInstruction("0x0010E0E1"));  // mvn r1, r0
  EXPECT_EQ(0x00FF00FFu, registers_.current.user.gprs.r1);

  // Modifies PC
  EXPECT_TRUE(RunInstruction("0x00F0E0E1"));  // mvn r15, r0
}

TEST_F(ExecuteTest, MVN_I32) {
  EXPECT_FALSE(RunInstruction("0xFF10E0E3"));  // mvn r1, #0xFF
  EXPECT_EQ(0xFFFFFF00u, registers_.current.user.gprs.r1);

  // Modifies PC
  EXPECT_TRUE(RunInstruction("0xFFF0E0E3"));  // mvn r15, #0xFF
}

TEST_F(ExecuteTest, MVNS) {
  registers_.current.user.gprs.r0 = 0x80FF00FFu;
  EXPECT_FALSE(RunInstruction("0x8010F0E1"));  // mvns r1, r0, lsl #1
  EXPECT_EQ(0xFE01FE01, registers_.current.user.gprs.r1);
  EXPECT_TRUE(registers_.current.user.cpsr.carry);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);

  // Modifies PC
  EXPECT_TRUE(RunInstruction("0x80F0F0E1"));  // mvns r15, r0, lsl #1
}

TEST_F(ExecuteTest, MVNS_I32) {
  EXPECT_FALSE(RunInstruction("0xFF02F0E3"));  // mvns r0, #0xF000000F
  EXPECT_EQ(0x0FFFFFF0u, registers_.current.user.gprs.r0);
  EXPECT_TRUE(registers_.current.user.cpsr.carry);

  // Modifies PC
  EXPECT_TRUE(RunInstruction("0xFFF2B0E3"));  // movs r15, #0xF000000F
}

TEST_F(ExecuteTest, ORR) {
  registers_.current.user.gprs.r0 = 0xFF00FF00u;
  registers_.current.user.gprs.r1 = 0x00FFF0F0u;
  EXPECT_FALSE(RunInstruction("0x010080E1"));  // orr r0, r0, r1
  EXPECT_EQ(0xFFFFFFF0u, registers_.current.user.gprs.r0);

  // Modifies PC
  EXPECT_TRUE(RunInstruction("0x01F080E1"));  // orr r15, r0, r1
}

TEST_F(ExecuteTest, ORR_I32) {
  registers_.current.user.gprs.r0 = 0xFF00FF00u;
  EXPECT_FALSE(RunInstruction("0xFF0880E3"));  // orr r0, r0, #0x00FF0000
  EXPECT_EQ(0xFFFFFF00u, registers_.current.user.gprs.r0);

  // Modifies PC
  EXPECT_TRUE(RunInstruction("0xFFF880E3"));  // orr r15, r0, #0x00FF0000
}

TEST_F(ExecuteTest, ORRS) {
  registers_.current.user.gprs.r0 = 0xFF00FF00u;
  registers_.current.user.gprs.r1 = 0x80FFF0F0u;
  EXPECT_FALSE(RunInstruction("0x810090E1"));  // orrs r0, r0, r1, lsl #1
  EXPECT_EQ(0xFFFFFFE0u, registers_.current.user.gprs.r0);
  EXPECT_TRUE(registers_.current.user.cpsr.carry);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);

  // Modifies PC
  EXPECT_TRUE(RunInstruction("0x81F090E1"));  // orrs r15, r0, r1, lsl #1
}

TEST_F(ExecuteTest, ORRS_I32) {
  registers_.current.user.gprs.r0 = 0xFF00FF00u;
  EXPECT_FALSE(RunInstruction("0xFF0290E3"));  // orrs r0, r0, #0xF000000F
  EXPECT_EQ(0xFF00FF0Fu, registers_.current.user.gprs.r0);
  EXPECT_TRUE(registers_.current.user.cpsr.carry);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);

  // Modifies PC
  EXPECT_TRUE(RunInstruction("0xFFF290E3"));  // orrs r15, r0, #0xF000000F
}

TEST_F(ExecuteTest, RSB) {
  registers_.current.user.gprs.r0 = 1u;
  registers_.current.user.gprs.r1 = 3u;
  EXPECT_FALSE(RunInstruction("0x010060E0"));  // rsb r0, r0, r1
  EXPECT_EQ(2u, registers_.current.user.gprs.r0);

  // Modifies PC
  EXPECT_TRUE(RunInstruction("0x01F060E0"));  // rsb r15, r0, r1
}

TEST_F(ExecuteTest, RSB_I32) {
  registers_.current.user.gprs.r0 = 1u;
  EXPECT_FALSE(RunInstruction("0x030060E2"));  // rsb r0, r0, #3
  EXPECT_EQ(2u, registers_.current.user.gprs.r0);

  // Modifies PC
  EXPECT_TRUE(RunInstruction("0x03F060E2"));  // rsb r15, r0, #3
}

TEST_F(ExecuteTest, RSBS) {
  registers_.current.user.gprs.r0 = 6u;
  registers_.current.user.gprs.r1 = 3u;
  EXPECT_FALSE(RunInstruction("0x010070E0"));  // rsbs r0, r0, r1
  EXPECT_EQ(-3, registers_.current.user.gprs.r0_s);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);

  // Modifies PC
  EXPECT_TRUE(RunInstruction("0x01F070E0"));  // rsbs r15, r0, r1
}

TEST_F(ExecuteTest, RSBS_I32) {
  registers_.current.user.gprs.r0 = 6u;
  EXPECT_FALSE(RunInstruction("0x030070E2"));  // rsbs r0, r0, #3
  EXPECT_EQ(-3, registers_.current.user.gprs.r0_s);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);

  // Modifies PC
  EXPECT_TRUE(RunInstruction("0x03F070E2"));  // rsbs r15, r0, #3
}

TEST_F(ExecuteTest, RSC) {
  registers_.current.user.gprs.r0 = 1u;
  registers_.current.user.gprs.r1 = 4u;
  EXPECT_FALSE(RunInstruction("0x0100E0E0"));  // rsc r0, r0, r1
  EXPECT_EQ(2u, registers_.current.user.gprs.r0);

  // Modifies PC
  EXPECT_TRUE(RunInstruction("0x01F0E0E0"));  // rsc r15, r0, r1
}

TEST_F(ExecuteTest, RSC_I32) {
  registers_.current.user.gprs.r0 = 1u;
  EXPECT_FALSE(RunInstruction("0x0400E0E2"));  // rsc r0, r0, #4
  EXPECT_EQ(2u, registers_.current.user.gprs.r0);

  // Modifies PC
  EXPECT_TRUE(RunInstruction("0x03F0E0E2"));  // rsc r15, r0, #3
}

TEST_F(ExecuteTest, RSCS) {
  registers_.current.user.gprs.r0 = 6u;
  registers_.current.user.gprs.r1 = 3u;
  EXPECT_FALSE(RunInstruction("0x0100F0E0"));  // rscs r0, r0, r1
  EXPECT_EQ(-4, registers_.current.user.gprs.r0_s);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);

  // Modifies PC
  EXPECT_TRUE(RunInstruction("0x01F0F0E0"));  // rscs r15, r0, r1
}

TEST_F(ExecuteTest, RSCS_I32) {
  registers_.current.user.gprs.r0 = 6u;
  EXPECT_FALSE(RunInstruction("0x0300F0E2"));  // rscs r0, r0, #3
  EXPECT_EQ(-4, registers_.current.user.gprs.r0_s);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);

  // Modifies PC
  EXPECT_TRUE(RunInstruction("0x03F0F0E2"));  // rscs r15, r0, #3
}

TEST_F(ExecuteTest, SBC) {
  registers_.current.user.gprs.r0 = 4u;
  registers_.current.user.gprs.r1 = 1u;
  EXPECT_FALSE(RunInstruction("0x0100C0E0"));  // sbc r0, r0, r1
  EXPECT_EQ(2u, registers_.current.user.gprs.r0);

  // Modifies PC
  EXPECT_TRUE(RunInstruction("0x01F0C0E0"));  // sbc r15, r0, r1
}

TEST_F(ExecuteTest, SBC_I32) {
  registers_.current.user.gprs.r0 = 4u;
  EXPECT_FALSE(RunInstruction("0x0100C0E2"));  // sbc r0, r0, #1
  EXPECT_EQ(2u, registers_.current.user.gprs.r0);

  // Modifies PC
  EXPECT_TRUE(RunInstruction("0x01F0C0E2"));  // sbc r15, r0, #1
}

TEST_F(ExecuteTest, SBCS) {
  registers_.current.user.gprs.r0 = 3u;
  registers_.current.user.gprs.r1 = 6u;
  EXPECT_FALSE(RunInstruction("0x0100D0E0"));  // sbcs r0, r0, r1
  EXPECT_EQ(-4, registers_.current.user.gprs.r0_s);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);

  // Modifies PC
  EXPECT_TRUE(RunInstruction("0x01F0D0E0"));  // sbcs r15, r0, r1
}

TEST_F(ExecuteTest, SBCS_I32) {
  registers_.current.user.gprs.r0 = 3u;
  EXPECT_FALSE(RunInstruction("0x0600D0E2"));  // sbcs r0, r0, #6
  EXPECT_EQ(-4, registers_.current.user.gprs.r0_s);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);

  // Modifies PC
  EXPECT_TRUE(RunInstruction("0x06F0D0E2"));  // sbcs r15, r0, #6
}

TEST_F(ExecuteTest, SMLAL) {
  registers_.current.user.gprs.r0_s = -2147483648;
  registers_.current.user.gprs.r1_s = -1;
  registers_.current.user.gprs.r2_s = 2147483647;
  EXPECT_FALSE(RunInstruction("0x9002E1E0"));  // smlal r0, r1, r0, r2
  EXPECT_EQ(0, registers_.current.user.gprs.r0_s);
  EXPECT_EQ(-1073741824, registers_.current.user.gprs.r1_s);

  // This is technically unpredictable behavior and ARM assemblers should not
  // allow r15 to be specified as the destination operand for this instruction
  uint32_t pc = registers_.current.user.gprs.pc;
  EXPECT_TRUE(RunInstruction("0x90F0E1E0"));  // smlal r15, r1, r0, r0

  registers_.current.user.gprs.pc = pc + 4u;
  EXPECT_TRUE(RunInstruction("0x9000EFE0"));  // smlal r0, r15, r0, r0
}

TEST_F(ExecuteTest, SMLALS) {
  registers_.current.user.gprs.r0_s = -2147483648;
  registers_.current.user.gprs.r1_s = -1;
  registers_.current.user.gprs.r2_s = 2147483647;
  EXPECT_FALSE(RunInstruction("0x9002F1E0"));  // smlals r0, r1, r0, r2
  EXPECT_EQ(0, registers_.current.user.gprs.r0_s);
  EXPECT_EQ(-1073741824, registers_.current.user.gprs.r1_s);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);

  // This is technically unpredictable behavior and ARM assemblers should not
  // allow r15 to be specified as the destination operand for this instruction
  uint32_t pc = registers_.current.user.gprs.pc;
  EXPECT_TRUE(RunInstruction("0x90F2F1E0"));  // smlals r15, r1, r0, r2

  registers_.current.user.gprs.pc = pc + 4u;
  EXPECT_TRUE(RunInstruction("0x9002FFE0"));  // smlals r0, r15, r0, r2
}

TEST_F(ExecuteTest, SMULL) {
  registers_.current.user.gprs.r0_s = -2147483648;
  registers_.current.user.gprs.r1_s = 2147483647;
  EXPECT_FALSE(RunInstruction("0x9001C1E0"));  // smull r0, r1, r0, r1
  EXPECT_EQ(-2147483648, registers_.current.user.gprs.r0_s);
  EXPECT_EQ(-1073741824, registers_.current.user.gprs.r1_s);

  // This is technically unpredictable behavior and ARM assemblers should not
  // allow r15 to be specified as the destination operand for this instruction
  uint32_t pc = registers_.current.user.gprs.pc;
  EXPECT_TRUE(RunInstruction("0x90F0C1E0"));  // smull r15, r1, r0, r0

  registers_.current.user.gprs.pc = pc + 4u;
  EXPECT_TRUE(RunInstruction("0x9000CFE0"));  // smull r0, r15, r0, r0
}

TEST_F(ExecuteTest, SMULLS) {
  registers_.current.user.gprs.r0_s = -2147483648;
  registers_.current.user.gprs.r1_s = 2147483647;
  EXPECT_FALSE(RunInstruction("0x9001D1E0"));  // smulls r0, r1, r0, r1
  EXPECT_EQ(-2147483648, registers_.current.user.gprs.r0_s);
  EXPECT_EQ(-1073741824, registers_.current.user.gprs.r1_s);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);

  // This is technically unpredictable behavior and ARM assemblers should not
  // allow r15 to be specified as the destination operand for this instruction
  uint32_t pc = registers_.current.user.gprs.pc;
  EXPECT_TRUE(RunInstruction("0x90F0D1E0"));  // smulls r15, r1, r0, r0

  registers_.current.user.gprs.pc = pc + 4u;
  EXPECT_TRUE(RunInstruction("0x9000DFE0"));  // smulls r0, r15, r0, r0
}

TEST_F(ExecuteTest, STC) {
  // NOP
}

TEST_F(ExecuteTest, STMDA) {}

TEST_F(ExecuteTest, STMDA_W) {}

TEST_F(ExecuteTest, STMDB) {}

TEST_F(ExecuteTest, STMDB_W) {}

TEST_F(ExecuteTest, STMIA) {}

TEST_F(ExecuteTest, STMIA_W) {}

TEST_F(ExecuteTest, STMIB) {}

TEST_F(ExecuteTest, STMIB_W) {}

TEST_F(ExecuteTest, STMSDA) {}

TEST_F(ExecuteTest, STMSDA_W) {}

TEST_F(ExecuteTest, STMSDB) {}

TEST_F(ExecuteTest, STMSDB_W) {}

TEST_F(ExecuteTest, STMSIA) {}

TEST_F(ExecuteTest, STMSIA_W) {}

TEST_F(ExecuteTest, STMSIB) {}

TEST_F(ExecuteTest, STMSIB_W) {}

TEST_F(ExecuteTest, STR_DAW) {}

TEST_F(ExecuteTest, STR_DAW_I12) {}

TEST_F(ExecuteTest, STR_DB) {}

TEST_F(ExecuteTest, STR_DB_I12) {}

TEST_F(ExecuteTest, STR_DBW) {}

TEST_F(ExecuteTest, STR_DBW_I12) {}

TEST_F(ExecuteTest, STR_IAW) {}

TEST_F(ExecuteTest, STR_IAW_I12) {}

TEST_F(ExecuteTest, STR_IB) {}

TEST_F(ExecuteTest, STR_IB_I12) {}

TEST_F(ExecuteTest, STR_IBW) {}

TEST_F(ExecuteTest, STR_IBW_I12) {}

TEST_F(ExecuteTest, STRB_DAW) {}

TEST_F(ExecuteTest, STRB_DAW_I12) {}

TEST_F(ExecuteTest, STRB_DB) {}

TEST_F(ExecuteTest, STRB_DB_I12) {}

TEST_F(ExecuteTest, STRB_DBW) {}

TEST_F(ExecuteTest, STRB_DBW_I12) {}

TEST_F(ExecuteTest, STRB_IAW) {}

TEST_F(ExecuteTest, STRB_IAW_I12) {}

TEST_F(ExecuteTest, STRB_IB) {}

TEST_F(ExecuteTest, STRB_IB_I12) {}

TEST_F(ExecuteTest, STRB_IBW) {}

TEST_F(ExecuteTest, STRB_IBW_I12) {}

TEST_F(ExecuteTest, STRBT_DAW) {}

TEST_F(ExecuteTest, STRBT_DAW_I12) {}

TEST_F(ExecuteTest, STRBT_IAW) {}

TEST_F(ExecuteTest, STRBT_IAW_I12) {}

TEST_F(ExecuteTest, STRH_DAW) {}

TEST_F(ExecuteTest, STRH_DAW_I8) {}

TEST_F(ExecuteTest, STRH_DB) {}

TEST_F(ExecuteTest, STRH_DB_I8) {}

TEST_F(ExecuteTest, STRH_DBW) {}

TEST_F(ExecuteTest, STRH_DBW_I8) {}

TEST_F(ExecuteTest, STRH_IAW) {}

TEST_F(ExecuteTest, STRH_IAW_I8) {}

TEST_F(ExecuteTest, STRH_IB) {}

TEST_F(ExecuteTest, STRH_IB_I8) {}

TEST_F(ExecuteTest, STRH_IBW) {}

TEST_F(ExecuteTest, STRH_IBW_I8) {}

TEST_F(ExecuteTest, STRT_DAW) {}

TEST_F(ExecuteTest, STRT_DAW_I12) {}

TEST_F(ExecuteTest, STRT_IAW) {}

TEST_F(ExecuteTest, STRT_IAW_I12) {}

TEST_F(ExecuteTest, SUB) {
  registers_.current.user.gprs.r0 = 4u;
  registers_.current.user.gprs.r1 = 1u;
  EXPECT_FALSE(RunInstruction("0x010040E0"));  // sub r0, r0, r1
  EXPECT_EQ(3u, registers_.current.user.gprs.r0);

  // Modifies PC
  EXPECT_TRUE(RunInstruction("0x01F040E0"));  // sub r15, r0, r1
}

TEST_F(ExecuteTest, SUB_I32) {
  registers_.current.user.gprs.r0 = 4u;
  EXPECT_FALSE(RunInstruction("0x010040E2"));  // sub r0, r0, #1
  EXPECT_EQ(3u, registers_.current.user.gprs.r0);

  // Modifies PC
  EXPECT_TRUE(RunInstruction("0x01F040E2"));  // sub r15, r0, #1
}

TEST_F(ExecuteTest, SUBS) {
  registers_.current.user.gprs.r0 = 3u;
  registers_.current.user.gprs.r1 = 6u;
  EXPECT_FALSE(RunInstruction("0x010050E0"));  // subs r0, r0, r1
  EXPECT_EQ(-3, registers_.current.user.gprs.r0_s);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);

  // Modifies PC
  EXPECT_TRUE(RunInstruction("0x01F050E0"));  // subs r15, r0, r1
}

TEST_F(ExecuteTest, SUBS_I32) {
  registers_.current.user.gprs.r0 = 3u;
  EXPECT_FALSE(RunInstruction("0x060050E2"));  // subs r0, r0, #6
  EXPECT_EQ(-3, registers_.current.user.gprs.r0_s);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);

  // Modifies PC
  EXPECT_TRUE(RunInstruction("0x06F050E2"));  // subs r15, r0, #6
}

TEST_F(ExecuteTest, SWI) {
  EXPECT_TRUE(RunInstruction("0x640000EF"));  // swi #100
  EXPECT_EQ(MODE_SVC, registers_.current.user.cpsr.mode);
}

TEST_F(ExecuteTest, SWP) {
  ASSERT_TRUE(Store32LE(nullptr, 0x50u, 0xDEADBEEFu));
  registers_.current.user.gprs.r0 = 0xCAFEBABEu;
  registers_.current.user.gprs.r1 = 0x50u;
  EXPECT_FALSE(RunInstruction("0x900001E1"));  // swp r0, r0, [r1]
  EXPECT_EQ(0xDEADBEEFu, registers_.current.user.gprs.r0);
  uint32_t memory_value;
  ASSERT_TRUE(Load32LE(nullptr, 0x50u, &memory_value));
  EXPECT_EQ(0xCAFEBABEu, memory_value);

  // This is technically unpredictable behavior and ARM assemblers should not
  // allow r15 to be specified as the destination operand for this instruction
  EXPECT_TRUE(RunInstruction("0x90F001E1"));  // subs swp r15, r0, [r1]
}

TEST_F(ExecuteTest, SWPB) {
  ASSERT_TRUE(Store8(nullptr, 0x50u, 0xCCu));
  registers_.current.user.gprs.r0 = 0xBBu;
  registers_.current.user.gprs.r1 = 0x50u;
  EXPECT_FALSE(RunInstruction("0x900041E1"));  // swpb r0, r0, [r1]
  EXPECT_EQ(0xCCu, registers_.current.user.gprs.r0);
  uint8_t memory_value;
  ASSERT_TRUE(Load8(nullptr, 0x50u, &memory_value));
  EXPECT_EQ(0xBBu, memory_value);

  // This is technically unpredictable behavior and ARM assemblers should not
  // allow r15 to be specified as the destination operand for this instruction
  EXPECT_TRUE(RunInstruction("0x90F041E1"));  // subs swpb r15, r0, [r1]
}

TEST_F(ExecuteTest, TEQ) {
  registers_.current.user.gprs.r0 = 0xFFFFFFFEu;
  registers_.current.user.gprs.r1 = 0xFFFFFFFFu;
  EXPECT_FALSE(RunInstruction("0x810030E1"));  // teq r0, r1, lsl #1
  EXPECT_TRUE(registers_.current.user.cpsr.carry);
  EXPECT_TRUE(registers_.current.user.cpsr.zero);
}

TEST_F(ExecuteTest, TEQ_I32) {
  registers_.current.user.gprs.r0 = 0xF000000Fu;
  EXPECT_FALSE(RunInstruction("0xFF0230E3"));  // teq r0, #0xF000000F
  EXPECT_TRUE(registers_.current.user.cpsr.carry);
  EXPECT_TRUE(registers_.current.user.cpsr.zero);
}

TEST_F(ExecuteTest, TST) {
  registers_.current.user.gprs.r0 = 0xFFFFFFFEu;
  registers_.current.user.gprs.r1 = 0xFFFFFFFFu;
  EXPECT_FALSE(RunInstruction("0x810010E1"));  // tst r0, r1, lsl #1
  EXPECT_TRUE(registers_.current.user.cpsr.negative);
}

TEST_F(ExecuteTest, TST_I32) {
  registers_.current.user.gprs.r0 = 0xF000000Fu;
  EXPECT_FALSE(RunInstruction("0xFF0210E3"));  // tst r0, #0xF000000F
  EXPECT_TRUE(registers_.current.user.cpsr.negative);
}

TEST_F(ExecuteTest, UMLAL) {
  registers_.current.user.gprs.r0 = 0xFFFFFFFFu;
  EXPECT_FALSE(RunInstruction("0x9000A1E0"));  // umlal r0, r1, r0, r0
  EXPECT_EQ(0u, registers_.current.user.gprs.r0);
  EXPECT_EQ(0xFFFFFFFFu, registers_.current.user.gprs.r1);

  // This is technically unpredictable behavior and ARM assemblers should not
  // allow r15 to be specified as the destination operand for this instruction
  uint32_t pc = registers_.current.user.gprs.pc;
  EXPECT_TRUE(RunInstruction("0x90F0A1E0"));  // umlal r15, r1, r0, r0

  registers_.current.user.gprs.pc = pc + 4u;
  EXPECT_TRUE(RunInstruction("0x9000AFE0"));  // umlal r0, r15, r0, r0
}

TEST_F(ExecuteTest, UMLALS) {
  registers_.current.user.gprs.r0 = 0xFFFFFFFFu;
  EXPECT_FALSE(RunInstruction("0x9000B1E0"));  // umlals r0, r1, r0, r0
  EXPECT_EQ(0u, registers_.current.user.gprs.r0);
  EXPECT_EQ(0xFFFFFFFFu, registers_.current.user.gprs.r1);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);

  // This is technically unpredictable behavior and ARM assemblers should not
  // allow r15 to be specified as the destination operand for this instruction
  uint32_t pc = registers_.current.user.gprs.pc;
  EXPECT_TRUE(RunInstruction("0x90F0B1E0"));  // umlals r15, r1, r0, r0

  registers_.current.user.gprs.pc = pc + 4u;
  EXPECT_TRUE(RunInstruction("0x9000BFE0"));  // umlals r0, r15, r0, r0
}

TEST_F(ExecuteTest, UMULL) {
  registers_.current.user.gprs.r0 = 0xFFFFFFFFu;
  EXPECT_FALSE(RunInstruction("0x900081E0"));  // umull r0, r1, r0, r0
  EXPECT_EQ(1u, registers_.current.user.gprs.r0);
  EXPECT_EQ(0xFFFFFFFEu, registers_.current.user.gprs.r1);

  // This is technically unpredictable behavior and ARM assemblers should not
  // allow r15 to be specified as the destination operand for this instruction
  uint32_t pc = registers_.current.user.gprs.pc;
  EXPECT_TRUE(RunInstruction("0x90F081E0"));  // umull r15, r1, r0, r0

  registers_.current.user.gprs.pc = pc + 4u;
  EXPECT_TRUE(RunInstruction("0x90008FE0"));  // umull r0, r15, r0, r0
}

TEST_F(ExecuteTest, UMULLS) {
  registers_.current.user.gprs.r0 = 0xFFFFFFFFu;
  EXPECT_FALSE(RunInstruction("0x900091E0"));  // umulls r0, r1, r0, r0
  EXPECT_EQ(1u, registers_.current.user.gprs.r0);
  EXPECT_EQ(0xFFFFFFFEu, registers_.current.user.gprs.r1);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);

  // This is technically unpredictable behavior and ARM assemblers should not
  // allow r15 to be specified as the destination operand for this instruction
  uint32_t pc = registers_.current.user.gprs.pc;
  EXPECT_TRUE(RunInstruction("0x90F091E0"));  // umulls r15, r1, r0, r0

  registers_.current.user.gprs.pc = pc + 4u;
  EXPECT_TRUE(RunInstruction("0x90009FE0"));  // umulls r0, r15, r0, r0
}

TEST_F(ExecuteTest, UNDEF) {
  // This is techically a BKPT instruction which is not present in ARMv4
  EXPECT_TRUE(RunInstruction("0x700020E1"));
  EXPECT_EQ(MODE_UND, registers_.current.user.cpsr.mode);
}