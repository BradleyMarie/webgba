extern "C" {
#include "emulator/cpu/arm7tdmi/decoders/thumb/execute.h"
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
    uint16_t instruction = ToInstruction(instruction_hex);
    bool result = ThumbInstructionExecute(instruction, &registers_, memory_);
    return result;
  }

  static std::vector<char> memory_space_;
  ArmAllRegisters registers_;
  Memory *memory_;

 private:
  uint16_t ToInstruction(std::string instruction_hex) {
    if (instruction_hex[0u] == '0' && instruction_hex[1u] == 'x') {
      instruction_hex.erase(0u, 2u);
    }

    assert(instruction_hex.size() == 4u);
    uint16_t instruction;
    for (size_t i = 0u; i < instruction_hex.size(); i += 2u) {
      std::string hex_byte;
      hex_byte += instruction_hex[i];
      hex_byte += instruction_hex[i + 1u];
      unsigned long byte = std::stoul(hex_byte, nullptr, 16u);
      instruction += byte << (i * 4u);
    }

    return instruction;
  }
};

std::vector<char> ExecuteTest::memory_space_(1024u, 0);

TEST_F(ExecuteTest, THUMB_OPCODE_ADCS) {
  registers_.current.user.gprs.r0 = 1u;
  registers_.current.user.cpsr.carry = true;
  EXPECT_FALSE(RunInstruction("0x4041"));  // adcs r0, r0, r0
  EXPECT_EQ(3u, registers_.current.user.gprs.r0);
}

TEST_F(ExecuteTest, THUMB_OPCODE_ADD_ANY) {
  registers_.current.user.gprs.r9 = 1u;
  registers_.current.user.gprs.r10 = 1u;
  EXPECT_FALSE(RunInstruction("0xD144"));  // add r9, r10
  EXPECT_EQ(2u, registers_.current.user.gprs.r9);
}

TEST_F(ExecuteTest, THUMB_OPCODE_ADD_PC) {
  EXPECT_FALSE(RunInstruction("0xFFA7"));  // add r7, pc, #1020
  EXPECT_EQ(1284u, registers_.current.user.gprs.r7);
}

TEST_F(ExecuteTest, THUMB_OPCODE_ADD_SP) {
  EXPECT_FALSE(RunInstruction("0xFFAF"));  // add r7, sp, #1020
  EXPECT_EQ(1532u, registers_.current.user.gprs.r7);
}

TEST_F(ExecuteTest, THUMB_OPCODE_ADD_SP_I7) {
  EXPECT_FALSE(RunInstruction("0x7FB0"));  // add sp, #508
  EXPECT_EQ(1020u, registers_.current.user.gprs.sp);
}

TEST_F(ExecuteTest, THUMB_OPCODE_ADDS) {
  registers_.current.user.gprs.r5 = 0xFFFFFFFFu;
  registers_.current.user.gprs.r6 = 0x3u;
  EXPECT_FALSE(RunInstruction("0x7719"));  // adds r7, r6, r5
  EXPECT_EQ(2u, registers_.current.user.gprs.r7);
  EXPECT_TRUE(registers_.current.user.cpsr.carry);
}

TEST_F(ExecuteTest, THUMB_OPCODE_ADDS_I3) {
  registers_.current.user.gprs.r6 = 0xFFFFFFFFu;
  EXPECT_FALSE(RunInstruction("0xF71C"));  // adds r7, r6, #3
  EXPECT_EQ(2u, registers_.current.user.gprs.r7);
  EXPECT_TRUE(registers_.current.user.cpsr.carry);
}

TEST_F(ExecuteTest, THUMB_OPCODE_ADDS_I8) {
  registers_.current.user.gprs.r7 = 0xFFFFFFFFu;
  EXPECT_FALSE(RunInstruction("0xFF37"));  // adds r7, #255
  EXPECT_EQ(254u, registers_.current.user.gprs.r7);
  EXPECT_TRUE(registers_.current.user.cpsr.carry);
}

TEST_F(ExecuteTest, THUMB_OPCODE_ANDS) {}

TEST_F(ExecuteTest, THUMB_OPCODE_ASRS) {}

TEST_F(ExecuteTest, THUMB_OPCODE_ASRS_I5) {}

TEST_F(ExecuteTest, THUMB_OPCODE_B_FWD) {}

TEST_F(ExecuteTest, THUMB_OPCODE_B_FWD_COND) {}

TEST_F(ExecuteTest, THUMB_OPCODE_B_REV) {}

TEST_F(ExecuteTest, THUMB_OPCODE_B_REV_COND) {}

TEST_F(ExecuteTest, THUMB_OPCODE_BICS) {}

TEST_F(ExecuteTest, THUMB_OPCODE_BL) {}

TEST_F(ExecuteTest, THUMB_OPCODE_BL_FWD) {}

TEST_F(ExecuteTest, THUMB_OPCODE_BL_REV) {}

TEST_F(ExecuteTest, THUMB_OPCODE_BX) {}

TEST_F(ExecuteTest, THUMB_OPCODE_CMN) {}

TEST_F(ExecuteTest, THUMB_OPCODE_CMP) {}

TEST_F(ExecuteTest, THUMB_OPCODE_CMP_I8) {}

TEST_F(ExecuteTest, THUMB_OPCODE_CMP_ANY) {}

TEST_F(ExecuteTest, THUMB_OPCODE_EORS) {}

TEST_F(ExecuteTest, THUMB_OPCODE_LDMIA) {}

TEST_F(ExecuteTest, THUMB_OPCODE_LDR) {}

TEST_F(ExecuteTest, THUMB_OPCODE_LDR_I5) {}

TEST_F(ExecuteTest, THUMB_OPCODE_LDR_PC_OFFSET_I8) {}

TEST_F(ExecuteTest, THUMB_OPCODE_LDR_SP_OFFSET_I8) {}

TEST_F(ExecuteTest, THUMB_OPCODE_LDRB) {}

TEST_F(ExecuteTest, THUMB_OPCODE_LDRB_I5) {}

TEST_F(ExecuteTest, THUMB_OPCODE_LDRH) {}

TEST_F(ExecuteTest, THUMB_OPCODE_LDRH_I5) {}

TEST_F(ExecuteTest, THUMB_OPCODE_LDRSB) {}

TEST_F(ExecuteTest, THUMB_OPCODE_LDRSH) {}

TEST_F(ExecuteTest, THUMB_OPCODE_LSLS) {}

TEST_F(ExecuteTest, THUMB_OPCODE_LSLS_I5) {}

TEST_F(ExecuteTest, THUMB_OPCODE_LSRS) {}

TEST_F(ExecuteTest, THUMB_OPCODE_LSRS_I5) {}

TEST_F(ExecuteTest, THUMB_OPCODE_MOV_ANY) {}

TEST_F(ExecuteTest, THUMB_OPCODE_MOVS_I8) {}

TEST_F(ExecuteTest, THUMB_OPCODE_MULS) {}

TEST_F(ExecuteTest, THUMB_OPCODE_MVNS) {}

TEST_F(ExecuteTest, THUMB_OPCODE_NEGS) {}

TEST_F(ExecuteTest, THUMB_OPCODE_ORRS) {}

TEST_F(ExecuteTest, THUMB_OPCODE_POP) {}

TEST_F(ExecuteTest, THUMB_OPCODE_PUSH) {}

TEST_F(ExecuteTest, THUMB_OPCODE_RORS) {}

TEST_F(ExecuteTest, THUMB_OPCODE_SBCS) {}

TEST_F(ExecuteTest, THUMB_OPCODE_STMIA) {}

TEST_F(ExecuteTest, THUMB_OPCODE_STR) {}

TEST_F(ExecuteTest, THUMB_OPCODE_STR_I5) {}

TEST_F(ExecuteTest, THUMB_OPCODE_STR_SP_OFFSET_I8) {}

TEST_F(ExecuteTest, THUMB_OPCODE_STRB) {}

TEST_F(ExecuteTest, THUMB_OPCODE_STRB_I5) {}

TEST_F(ExecuteTest, THUMB_OPCODE_STRH) {}

TEST_F(ExecuteTest, THUMB_OPCODE_STRH_I5) {}

TEST_F(ExecuteTest, THUMB_OPCODE_SUB_SP_I7) {}

TEST_F(ExecuteTest, THUMB_OPCODE_SUBS) {}

TEST_F(ExecuteTest, THUMB_OPCODE_SUBS_I3) {}

TEST_F(ExecuteTest, THUMB_OPCODE_SUBS_I8) {}

TEST_F(ExecuteTest, THUMB_OPCODE_SWI) {}

TEST_F(ExecuteTest, THUMB_OPCODE_TST) {}

TEST_F(ExecuteTest, THUMB_OPCODE_UNDEF) {}