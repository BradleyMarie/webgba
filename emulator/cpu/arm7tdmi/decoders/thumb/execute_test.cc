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
    memory_fails_ =
        MemoryAllocate(nullptr, Load32LEFails, Load16LEFails, Load8Fails,
                       Store32LEFails, Store16LEFails, Store8Fails, nullptr);
    ASSERT_NE(nullptr, memory_fails_);

    memset(&registers_, 0, sizeof(ArmAllRegisters));
    registers_.current.user.cpsr.mode = MODE_SVC;
    registers_.current.spsr.mode = MODE_USR;
    registers_.current.user.gprs.pc = 0x108;
    registers_.current.user.gprs.sp = 0x200;
  }

  void TearDown() override {
    MemoryFree(memory_);
    MemoryFree(memory_fails_);
  }

 protected:
  static uint32_t Load32(uint32_t address) {
    uint32_t result = 0u;
    EXPECT_TRUE(Load32LE(nullptr, address, &result));
    return result;
  }

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

  static uint16_t Load16(uint32_t address) {
    uint16_t result = 0u;
    EXPECT_TRUE(Load16LE(nullptr, address, &result));
    return result;
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

  static uint8_t Load8(uint32_t address) {
    uint8_t result = 0u;
    EXPECT_TRUE(Load8(nullptr, address, &result));
    return result;
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

  static void Store32(uint32_t address, uint32_t value) {
    EXPECT_TRUE(Store32LE(nullptr, address, value));
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

  static void Store16(uint32_t address, uint16_t value) {
    EXPECT_TRUE(Store16LE(nullptr, address, value));
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

  static void Store8(uint32_t address, uint8_t value) {
    EXPECT_TRUE(Store8(nullptr, address, value));
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
    return regs.current.user.cpsr.mode == MODE_ABT &&
           regs.current.user.gprs.pc == 0x10u;
  }

  // Assumes little-endian hex string
  bool RunInstruction(std::string instruction_hex) {
    uint16_t instruction = ToInstruction(instruction_hex);
    bool result = ThumbInstructionExecute(instruction, &registers_, memory_);
    return result;
  }

  // Assumes little-endian hex string
  bool RunInstructionBadMemory(std::string instruction_hex) {
    uint16_t instruction = ToInstruction(instruction_hex);
    bool result =
        ThumbInstructionExecute(instruction, &registers_, memory_fails_);
    return result;
  }

  static std::vector<char> memory_space_;
  ArmAllRegisters registers_;
  Memory *memory_;
  Memory *memory_fails_;

 private:
  uint16_t ToInstruction(std::string instruction_hex) {
    if (instruction_hex[0u] == '0' && instruction_hex[1u] == 'x') {
      instruction_hex.erase(0u, 2u);
    }

    assert(instruction_hex.size() == 4u);
    uint16_t instruction = 0u;
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

std::vector<char> ExecuteTest::memory_space_(2048u, 0);

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
  EXPECT_TRUE(RunInstruction("0xCF44"));  // add r15, r9
  EXPECT_EQ(0x10Au, registers_.current.user.gprs.r15);
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

TEST_F(ExecuteTest, THUMB_OPCODE_ANDS) {
  registers_.current.user.gprs.r7 = 0xFFFFFFFEu;
  registers_.current.user.gprs.r6 = 0x80000003u;
  EXPECT_FALSE(RunInstruction("0x3740"));  // ands r7, r6
  EXPECT_EQ(0x80000002u, registers_.current.user.gprs.r7);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);
}

TEST_F(ExecuteTest, THUMB_OPCODE_ASRS) {
  registers_.current.user.gprs.r7 = 0xFFFFFFF0u;
  registers_.current.user.gprs.r6 = 1u;
  EXPECT_FALSE(RunInstruction("0x3741"));  // asrs r7, r6
  EXPECT_EQ(0xFFFFFFF8u, registers_.current.user.gprs.r7);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);
}

TEST_F(ExecuteTest, THUMB_OPCODE_ASRS_I5) {
  registers_.current.user.gprs.r7 = 0x80000000u;
  EXPECT_FALSE(RunInstruction("0xFF17"));  // asrs r7, #31
  EXPECT_EQ(0xFFFFFFFFu, registers_.current.user.gprs.r7);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);
}

TEST_F(ExecuteTest, THUMB_OPCODE_B_FWD) {
  EXPECT_TRUE(RunInstruction("0xFDE3"));  // b #2046
  EXPECT_EQ(2306u, registers_.current.user.gprs.pc);
}

TEST_F(ExecuteTest, THUMB_OPCODE_B_FWD_COND_TAKEN) {
  registers_.current.user.cpsr.zero = true;
  EXPECT_TRUE(RunInstruction("0x7DDD"));  // ble #254
  EXPECT_EQ(514u, registers_.current.user.gprs.pc);
}

TEST_F(ExecuteTest, THUMB_OPCODE_B_FWD_COND_NOT_TAKEN) {
  EXPECT_FALSE(RunInstruction("0x7DDD"));  // ble #254
  EXPECT_EQ(0x108u, registers_.current.user.gprs.pc);
}

TEST_F(ExecuteTest, THUMB_OPCODE_B_REV) {
  EXPECT_TRUE(RunInstruction("0xFFE7"));  // b #2
  EXPECT_EQ(0x106u, registers_.current.user.gprs.pc);
}

TEST_F(ExecuteTest, THUMB_OPCODE_B_REV_COND_TAKEN) {
  registers_.current.user.cpsr.zero = true;
  EXPECT_TRUE(RunInstruction("0xFFDD"));  // ble #2
  EXPECT_EQ(0x106u, registers_.current.user.gprs.pc);
}

TEST_F(ExecuteTest, THUMB_OPCODE_B_REV_COND_NOT_TAKEN) {
  EXPECT_FALSE(RunInstruction("0xFFDD"));  // ble #2
  EXPECT_EQ(0x108u, registers_.current.user.gprs.pc);
}

TEST_F(ExecuteTest, THUMB_OPCODE_BICS) {
  registers_.current.user.gprs.r7 = 0xFFFFFFF0u;
  registers_.current.user.gprs.r6 = 0x70000000u;
  EXPECT_FALSE(RunInstruction("0xB743"));  // bics r7, r6
  EXPECT_EQ(0x8FFFFFF0u, registers_.current.user.gprs.r7);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);
}

TEST_F(ExecuteTest, THUMB_OPCODE_BL) {
  // Not Needed
}

TEST_F(ExecuteTest, THUMB_OPCODE_BL_FWD) {
  // bl #0x80000
  registers_.current.user.gprs.r15 = 0x80000004u;
  EXPECT_FALSE(RunInstruction("0x7FF0"));
  registers_.current.user.gprs.r15 += 2u;
  EXPECT_TRUE(RunInstruction("0xFEFF"));
  EXPECT_EQ(0x80080000u, registers_.current.user.gprs.r15);
  EXPECT_EQ(0x80000005u, registers_.current.user.gprs.r14);
}

TEST_F(ExecuteTest, THUMB_OPCODE_BL_REV) {
  // bl #-0x80000
  registers_.current.user.gprs.r15 = 0x80000004u;
  EXPECT_FALSE(RunInstruction("0x7FF7"));
  registers_.current.user.gprs.r15 += 2u;
  EXPECT_TRUE(RunInstruction("0xFEFF"));
  EXPECT_EQ(0x7FF80000u, registers_.current.user.gprs.r15);
  EXPECT_EQ(0x80000005u, registers_.current.user.gprs.r14);
}

TEST_F(ExecuteTest, THUMB_OPCODE_BX) {
  registers_.current.user.gprs.r7 = 0x1000u;
  EXPECT_TRUE(RunInstruction("0x3847"));  // bx r7
  EXPECT_EQ(0x1000u, registers_.current.user.gprs.r15);
}

TEST_F(ExecuteTest, THUMB_OPCODE_CMN) {
  registers_.current.user.gprs.r7 = 0xFFFFFFFFu;
  registers_.current.user.gprs.r6 = 0x00000001u;
  EXPECT_FALSE(RunInstruction("0xFE42"));  // cmn r6, r7
  EXPECT_TRUE(registers_.current.user.cpsr.zero);
}

TEST_F(ExecuteTest, THUMB_OPCODE_CMP) {
  registers_.current.user.gprs.r7 = 0xFFFFFFFFu;
  registers_.current.user.gprs.r6 = 0xFFFFFFFFu;
  EXPECT_FALSE(RunInstruction("0xBE42"));  // cmp r6, r7
  EXPECT_TRUE(registers_.current.user.cpsr.zero);
}

TEST_F(ExecuteTest, THUMB_OPCODE_CMP_I8) {
  registers_.current.user.gprs.r7 = 255u;
  EXPECT_FALSE(RunInstruction("0xFF2F"));  // cmp r7, #255
  EXPECT_TRUE(registers_.current.user.cpsr.zero);
}

TEST_F(ExecuteTest, THUMB_OPCODE_CMP_ANY) {
  registers_.current.user.gprs.r13 = 0xFFFFFFFFu;
  registers_.current.user.gprs.r14 = 0xFFFFFFFFu;
  EXPECT_FALSE(RunInstruction("0xF545"));  // cmp r13, r14
  EXPECT_TRUE(registers_.current.user.cpsr.zero);
}

TEST_F(ExecuteTest, THUMB_OPCODE_EORS) {
  registers_.current.user.gprs.r7 = 0x00010001u;
  registers_.current.user.gprs.r6 = 0x80000001u;
  EXPECT_FALSE(RunInstruction("0x7E40"));  // eors r6, r7
  EXPECT_EQ(0x80010000u, registers_.current.user.gprs.r6);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);
}

TEST_F(ExecuteTest, THUMB_OPCODE_LDMIA) {
  Store32(0x1DCu, 1u);
  Store32(0x1E0u, 2u);
  Store32(0x1E4u, 3u);
  Store32(0x1E8u, 4u);
  Store32(0x1ECu, 5u);
  Store32(0x1F0u, 6u);
  Store32(0x1F4u, 7u);
  Store32(0x1F8u, 8u);
  registers_.current.user.gprs.r0 = 0x1DCu;
  EXPECT_FALSE(RunInstruction("0xFFC8"));  // ldmia r0, {r0-r7}
  EXPECT_EQ(1u, registers_.current.user.gprs.r0);
  EXPECT_EQ(2u, registers_.current.user.gprs.r1);
  EXPECT_EQ(3u, registers_.current.user.gprs.r2);
  EXPECT_EQ(4u, registers_.current.user.gprs.r3);
  EXPECT_EQ(5u, registers_.current.user.gprs.r4);
  EXPECT_EQ(6u, registers_.current.user.gprs.r5);
  EXPECT_EQ(7u, registers_.current.user.gprs.r6);
  EXPECT_EQ(8u, registers_.current.user.gprs.r7);
}

TEST_F(ExecuteTest, THUMB_OPCODE_LDMIAW) {
  Store32(0x1DCu, 1u);
  Store32(0x1E0u, 2u);
  Store32(0x1E4u, 3u);
  Store32(0x1E8u, 4u);
  Store32(0x1ECu, 5u);
  Store32(0x1F0u, 6u);
  Store32(0x1F4u, 7u);
  registers_.current.user.gprs.r7 = 0x1DCu;
  EXPECT_FALSE(RunInstruction("0x7FCF"));  // ldmia r7!, {r0-r6}
  EXPECT_EQ(1u, registers_.current.user.gprs.r0);
  EXPECT_EQ(2u, registers_.current.user.gprs.r1);
  EXPECT_EQ(3u, registers_.current.user.gprs.r2);
  EXPECT_EQ(4u, registers_.current.user.gprs.r3);
  EXPECT_EQ(5u, registers_.current.user.gprs.r4);
  EXPECT_EQ(6u, registers_.current.user.gprs.r5);
  EXPECT_EQ(7u, registers_.current.user.gprs.r6);
  EXPECT_EQ(0x1F8u, registers_.current.user.gprs.r7);
}

TEST_F(ExecuteTest, THUMB_OPCODE_LDR) {
  Store32(0x200u, 1u);
  registers_.current.user.gprs.r7 = 0x100u;
  EXPECT_FALSE(RunInstruction("0xFE59"));  // ldr r6, [r7, r7]
  EXPECT_EQ(1u, registers_.current.user.gprs.r6);
}

TEST_F(ExecuteTest, THUMB_OPCODE_LDR_I5) {
  Store32(0x200u, 1u);
  registers_.current.user.gprs.r7 = 0x184u;
  EXPECT_FALSE(RunInstruction("0xFE6F"));  // ldr r6, [r7, #124]
  EXPECT_EQ(1u, registers_.current.user.gprs.r6);
}

TEST_F(ExecuteTest, THUMB_OPCODE_LDR_PC_OFFSET_I8) {
  Store32(0x504u, 1u);
  EXPECT_FALSE(RunInstruction("0xFF4F"));  // ldr r7, [pc, #1020]
  EXPECT_EQ(1u, registers_.current.user.gprs.r7);
}

TEST_F(ExecuteTest, THUMB_OPCODE_LDR_SP_OFFSET_I8) {
  Store32(0x5FCu, 1u);
  EXPECT_FALSE(RunInstruction("0xFF9F"));  // ldr r7, [sp, #1020]
  EXPECT_EQ(1u, registers_.current.user.gprs.r7);
}

TEST_F(ExecuteTest, THUMB_OPCODE_LDRB) {
  Store8(0x200u, 1u);
  registers_.current.user.gprs.r7 = 0x100u;
  EXPECT_FALSE(RunInstruction("0xFE5D"));  // ldrb r6, [r7, r7]
  EXPECT_EQ(1u, registers_.current.user.gprs.r6);
}

TEST_F(ExecuteTest, THUMB_OPCODE_LDRB_I5) {
  Store8(0x200u, 1u);
  registers_.current.user.gprs.r7 = 0x1E1u;
  EXPECT_FALSE(RunInstruction("0xFE7F"));  // ldrb r6, [r7, #31]
  EXPECT_EQ(1u, registers_.current.user.gprs.r6);
}

TEST_F(ExecuteTest, THUMB_OPCODE_LDRH) {
  Store16(0x200u, 1u);
  registers_.current.user.gprs.r7 = 0x100u;
  EXPECT_FALSE(RunInstruction("0xFE5B"));  // ldrh r6, [r7, r7]
  EXPECT_EQ(1u, registers_.current.user.gprs.r6);
}

TEST_F(ExecuteTest, THUMB_OPCODE_LDRH_I5) {
  Store16(0x200u, 1u);
  registers_.current.user.gprs.r7 = 0x1C2u;
  EXPECT_FALSE(RunInstruction("0xFE8F"));  // ldrh r6, [r7, #62]
  EXPECT_EQ(1u, registers_.current.user.gprs.r6);
}

TEST_F(ExecuteTest, THUMB_OPCODE_LDRSB) {
  Store8(0x200u, 0xFFu);
  registers_.current.user.gprs.r7 = 0x100u;
  EXPECT_FALSE(RunInstruction("0xFE57"));  // ldrsb r6, [r7, r7]
  EXPECT_EQ(0xFFFFFFFFu, registers_.current.user.gprs.r6);
}

TEST_F(ExecuteTest, THUMB_OPCODE_LDRSH) {
  Store16(0x200u, 0xFFFFu);
  registers_.current.user.gprs.r7 = 0x100u;
  EXPECT_FALSE(RunInstruction("0xFE5F"));  // ldrsh r6, [r7, r7]
  EXPECT_EQ(0xFFFFFFFFu, registers_.current.user.gprs.r6);
}

TEST_F(ExecuteTest, THUMB_OPCODE_LSLS) {
  registers_.current.user.gprs.r7 = 0xF000000Fu;
  registers_.current.user.gprs.r6 = 1u;
  EXPECT_FALSE(RunInstruction("0xB740"));  // lsls r7, r6
  EXPECT_EQ(0xE000001Eu, registers_.current.user.gprs.r7);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);
  EXPECT_TRUE(registers_.current.user.cpsr.carry);
}

TEST_F(ExecuteTest, THUMB_OPCODE_LSLS_I5) {
  registers_.current.user.gprs.r7 = 0xF000000Fu;
  EXPECT_FALSE(RunInstruction("0x7F00"));  // lsls r7, r7, #1
  EXPECT_EQ(0xE000001Eu, registers_.current.user.gprs.r7);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);
  EXPECT_TRUE(registers_.current.user.cpsr.carry);
}

TEST_F(ExecuteTest, THUMB_OPCODE_LSRS) {
  registers_.current.user.gprs.r7 = 0xF000000Fu;
  registers_.current.user.gprs.r6 = 1u;
  EXPECT_FALSE(RunInstruction("0xF740"));  // lsrs r7, r6
  EXPECT_EQ(0x78000007u, registers_.current.user.gprs.r7);
  EXPECT_TRUE(registers_.current.user.cpsr.carry);
}

TEST_F(ExecuteTest, THUMB_OPCODE_LSRS_I5) {
  registers_.current.user.gprs.r7 = 0xF000000Fu;
  EXPECT_FALSE(RunInstruction("0x7F08"));  // lsrs r7, r7, #1
  EXPECT_EQ(0x78000007u, registers_.current.user.gprs.r7);
  EXPECT_TRUE(registers_.current.user.cpsr.carry);
}

TEST_F(ExecuteTest, THUMB_OPCODE_MOV_ANY) {
  registers_.current.user.gprs.r13 = 0xF000000Fu;
  EXPECT_FALSE(RunInstruction("0xEE46"));  // mov r14, r13
  EXPECT_EQ(0xF000000Fu, registers_.current.user.gprs.r14);
  EXPECT_TRUE(RunInstruction("0xF746"));  // mov r15, r14
  EXPECT_EQ(0xF000000Fu, registers_.current.user.gprs.r15);
}

TEST_F(ExecuteTest, THUMB_OPCODE_MOVS_I8) {
  EXPECT_FALSE(RunInstruction("0xFF27"));  // movs r7, #255
  EXPECT_EQ(255u, registers_.current.user.gprs.r7);
  EXPECT_FALSE(RunInstruction("0x0027"));  // movs r7, #0
  EXPECT_EQ(0u, registers_.current.user.gprs.r7);
  EXPECT_TRUE(registers_.current.user.cpsr.zero);
}

TEST_F(ExecuteTest, THUMB_OPCODE_MULS) {
  registers_.current.user.gprs.r7 = 0xFFFFu;
  EXPECT_FALSE(RunInstruction("0x7F43"));  // muls r7, r7
  EXPECT_EQ(0xFFFE0001, registers_.current.user.gprs.r7);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);
}

TEST_F(ExecuteTest, THUMB_OPCODE_MVNS) {
  registers_.current.user.gprs.r0 = 0x70FF00FFu;
  EXPECT_FALSE(RunInstruction("0xC143"));  // mvns r1, r0
  EXPECT_EQ(0x8F00FF00, registers_.current.user.gprs.r1);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);
}

TEST_F(ExecuteTest, THUMB_OPCODE_NEGS) {
  registers_.current.user.gprs.r6_s = 256;
  EXPECT_FALSE(RunInstruction("0x7742"));  // negs r7, r6
  EXPECT_EQ(-256, registers_.current.user.gprs.r7_s);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);
}

TEST_F(ExecuteTest, THUMB_OPCODE_ORRS) {
  registers_.current.user.gprs.r7 = 0x00010001u;
  registers_.current.user.gprs.r6 = 0x80000001u;
  EXPECT_FALSE(RunInstruction("0x3E43"));  // orrs r6, r7
  EXPECT_EQ(0x80010001u, registers_.current.user.gprs.r6);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);
}

TEST_F(ExecuteTest, THUMB_OPCODE_POP) {
  Store32(0x1DCu, 1u);
  Store32(0x1E0u, 2u);
  Store32(0x1E4u, 3u);
  Store32(0x1E8u, 4u);
  Store32(0x1ECu, 5u);
  Store32(0x1F0u, 6u);
  Store32(0x1F4u, 7u);
  Store32(0x1F8u, 8u);
  registers_.current.user.gprs.sp = 0x1DCu;
  EXPECT_FALSE(RunInstruction("0xFFBC"));  // pop {r0-r7}
  EXPECT_EQ(0x1FCu, registers_.current.user.gprs.sp);
  EXPECT_EQ(1u, registers_.current.user.gprs.r0);
  EXPECT_EQ(2u, registers_.current.user.gprs.r1);
  EXPECT_EQ(3u, registers_.current.user.gprs.r2);
  EXPECT_EQ(4u, registers_.current.user.gprs.r3);
  EXPECT_EQ(5u, registers_.current.user.gprs.r4);
  EXPECT_EQ(6u, registers_.current.user.gprs.r5);
  EXPECT_EQ(7u, registers_.current.user.gprs.r6);
  EXPECT_EQ(8u, registers_.current.user.gprs.r7);
}

TEST_F(ExecuteTest, THUMB_OPCODE_POP_PC) {
  Store32(0x1DCu, 1u);
  Store32(0x1E0u, 2u);
  Store32(0x1E4u, 3u);
  Store32(0x1E8u, 4u);
  Store32(0x1ECu, 5u);
  Store32(0x1F0u, 6u);
  Store32(0x1F4u, 7u);
  Store32(0x1F8u, 8u);
  Store32(0x1FCu, 9u);
  registers_.current.user.gprs.sp = 0x1DCu;
  EXPECT_TRUE(RunInstruction("0xFFBD"));  // pop {r0-r7, pc}
  EXPECT_EQ(0x200u, registers_.current.user.gprs.sp);
  EXPECT_EQ(1u, registers_.current.user.gprs.r0);
  EXPECT_EQ(2u, registers_.current.user.gprs.r1);
  EXPECT_EQ(3u, registers_.current.user.gprs.r2);
  EXPECT_EQ(4u, registers_.current.user.gprs.r3);
  EXPECT_EQ(5u, registers_.current.user.gprs.r4);
  EXPECT_EQ(6u, registers_.current.user.gprs.r5);
  EXPECT_EQ(7u, registers_.current.user.gprs.r6);
  EXPECT_EQ(8u, registers_.current.user.gprs.r7);
  EXPECT_EQ(9u, registers_.current.user.gprs.r15);
}

TEST_F(ExecuteTest, THUMB_OPCODE_PUSH) {
  registers_.current.user.gprs.r0 = 1u;
  registers_.current.user.gprs.r1 = 2u;
  registers_.current.user.gprs.r2 = 3u;
  registers_.current.user.gprs.r3 = 4u;
  registers_.current.user.gprs.r4 = 5u;
  registers_.current.user.gprs.r5 = 6u;
  registers_.current.user.gprs.r6 = 7u;
  registers_.current.user.gprs.r7 = 8u;
  registers_.current.user.gprs.lr = 9u;
  EXPECT_FALSE(RunInstruction("0xFFB5"));  // push {r0-r7, lr}
  EXPECT_EQ(registers_.current.user.gprs.sp, 0x1DCu);
  EXPECT_EQ(1u, Load32(0x1DCu));
  EXPECT_EQ(2u, Load32(0x1E0u));
  EXPECT_EQ(3u, Load32(0x1E4u));
  EXPECT_EQ(4u, Load32(0x1E8u));
  EXPECT_EQ(5u, Load32(0x1ECu));
  EXPECT_EQ(6u, Load32(0x1F0u));
  EXPECT_EQ(7u, Load32(0x1F4u));
  EXPECT_EQ(8u, Load32(0x1F8u));
  EXPECT_EQ(9u, Load32(0x1FCu));
}

TEST_F(ExecuteTest, THUMB_OPCODE_RORS) {
  registers_.current.user.gprs.r7 = 0xF000000Fu;
  registers_.current.user.gprs.r6 = 1u;
  EXPECT_FALSE(RunInstruction("0xF741"));  // rors r7, r6
  EXPECT_EQ(0xF8000007u, registers_.current.user.gprs.r7);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);
  EXPECT_TRUE(registers_.current.user.cpsr.carry);
}

TEST_F(ExecuteTest, THUMB_OPCODE_SBCS) {
  registers_.current.user.gprs.r0 = 3u;
  registers_.current.user.gprs.r1 = 6u;
  EXPECT_FALSE(RunInstruction("0x8841"));  // sbcs r0, r0, r1
  EXPECT_EQ(-4, registers_.current.user.gprs.r0_s);
  EXPECT_TRUE(registers_.current.user.cpsr.negative);
}

TEST_F(ExecuteTest, THUMB_OPCODE_STMIA) {
  registers_.current.user.gprs.r1 = 2u;
  registers_.current.user.gprs.r2 = 3u;
  registers_.current.user.gprs.r3 = 4u;
  registers_.current.user.gprs.r4 = 5u;
  registers_.current.user.gprs.r5 = 6u;
  registers_.current.user.gprs.r6 = 7u;
  registers_.current.user.gprs.r7 = 8u;
  registers_.current.user.gprs.r0 = 0x1DCu;
  EXPECT_FALSE(RunInstruction("0xFFC0"));  // stmia r0!, {r0-r7}
  EXPECT_EQ(registers_.current.user.gprs.r0, 0x1FCu);
  EXPECT_EQ(0x1DCu, Load32(0x1DCu));
  EXPECT_EQ(2u, Load32(0x1E0u));
  EXPECT_EQ(3u, Load32(0x1E4u));
  EXPECT_EQ(4u, Load32(0x1E8u));
  EXPECT_EQ(5u, Load32(0x1ECu));
  EXPECT_EQ(6u, Load32(0x1F0u));
  EXPECT_EQ(7u, Load32(0x1F4u));
  EXPECT_EQ(8u, Load32(0x1F8u));
}

TEST_F(ExecuteTest, THUMB_OPCODE_STR) {
  registers_.current.user.gprs.r7 = 0x100u;
  registers_.current.user.gprs.r6 = 1u;
  EXPECT_FALSE(RunInstruction("0xFE51"));  // str r6, [r7, r7]
  EXPECT_EQ(1u, Load32(0x200u));
}

TEST_F(ExecuteTest, THUMB_OPCODE_STR_I5) {
  registers_.current.user.gprs.r7 = 0x184u;
  registers_.current.user.gprs.r6 = 1u;
  EXPECT_FALSE(RunInstruction("0xFE67"));  // str r6, [r7, #124]
  EXPECT_EQ(1u, Load32(0x200u));
}

TEST_F(ExecuteTest, THUMB_OPCODE_STR_SP_OFFSET_I8) {
  registers_.current.user.gprs.r7 = 1u;
  EXPECT_FALSE(RunInstruction("0xFF97"));  // str r7, [sp, #1020]
  EXPECT_EQ(1u, Load32(0x5FCu));
}

TEST_F(ExecuteTest, THUMB_OPCODE_STRB) {
  registers_.current.user.gprs.r6 = 1u;
  registers_.current.user.gprs.r7 = 0x100u;
  EXPECT_FALSE(RunInstruction("0xFE55"));  // strb r6, [r7, r7]
  EXPECT_EQ(1u, Load8(0x200u));
}

TEST_F(ExecuteTest, THUMB_OPCODE_STRB_I5) {
  registers_.current.user.gprs.r6 = 1u;
  registers_.current.user.gprs.r7 = 0x1E1u;
  EXPECT_FALSE(RunInstruction("0xFE77"));  // strb r6, [r7, #31]
  EXPECT_EQ(1u, Load8(0x200u));
}

TEST_F(ExecuteTest, THUMB_OPCODE_STRH) {
  registers_.current.user.gprs.r6 = 1u;
  registers_.current.user.gprs.r7 = 0x100u;
  EXPECT_FALSE(RunInstruction("0xFE53"));  // strh r6, [r7, r7]
  EXPECT_EQ(1u, Load16(0x200u));
}

TEST_F(ExecuteTest, THUMB_OPCODE_STRH_I5) {
  registers_.current.user.gprs.r6 = 1u;
  registers_.current.user.gprs.r7 = 0x1C2u;
  EXPECT_FALSE(RunInstruction("0xFE87"));  // strh r6, [r7, #62]
  EXPECT_EQ(1u, Load16(0x200u));
}

TEST_F(ExecuteTest, THUMB_OPCODE_SUB_SP_I7) {
  registers_.current.user.gprs.r13 = 510u;
  EXPECT_FALSE(RunInstruction("0xFFB0"));  // sub sp, #508
  EXPECT_EQ(2u, registers_.current.user.gprs.sp);
}

TEST_F(ExecuteTest, THUMB_OPCODE_SUBS) {
  registers_.current.user.gprs.r6 = 256u;
  registers_.current.user.gprs.r5 = 255u;
  EXPECT_FALSE(RunInstruction("0x771B"));  // subs r7, r6, r5
  EXPECT_EQ(1u, registers_.current.user.gprs.r7);
  EXPECT_TRUE(registers_.current.user.cpsr.carry);
}

TEST_F(ExecuteTest, THUMB_OPCODE_SUBS_I3) {
  registers_.current.user.gprs.r6 = 4u;
  EXPECT_FALSE(RunInstruction("0xF71E"));  // subs r7, r6, #3
  EXPECT_EQ(1u, registers_.current.user.gprs.r7);
  EXPECT_TRUE(registers_.current.user.cpsr.carry);
}

TEST_F(ExecuteTest, THUMB_OPCODE_SUBS_I8) {
  registers_.current.user.gprs.r7 = 256u;
  EXPECT_FALSE(RunInstruction("0xFF3F"));  // subs r7, #255
  EXPECT_EQ(1u, registers_.current.user.gprs.r7);
  EXPECT_TRUE(registers_.current.user.cpsr.carry);
}

TEST_F(ExecuteTest, THUMB_OPCODE_SWI) {
  EXPECT_TRUE(RunInstruction("0xFFDF"));  // swi #255
  EXPECT_EQ(MODE_SVC, registers_.current.user.cpsr.mode);
}

TEST_F(ExecuteTest, THUMB_OPCODE_TST) {
  registers_.current.user.gprs.r7 = 256u;
  registers_.current.user.gprs.r6 = 255u;
  EXPECT_FALSE(RunInstruction("0x3742"));  // tst r7, r6
  EXPECT_TRUE(registers_.current.user.cpsr.zero);
}

TEST_F(ExecuteTest, THUMB_OPCODE_UNDEF) {
  // This is techically a BKPT instruction which is not present in ARMv4
  EXPECT_TRUE(RunInstruction("0x00BE"));
  EXPECT_EQ(MODE_UND, registers_.current.user.cpsr.mode);
}

//
// Data Abort Exceptions
//

TEST_F(ExecuteTest, THUMB_OPCODE_FAILS_LDMIA) {
  EXPECT_TRUE(RunInstructionBadMemory("0xFFC8"));  // ldmia r0, {r0-r7}
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, THUMB_OPCODE_FAILS_LDMIAW) {
  EXPECT_TRUE(RunInstructionBadMemory("0x7FCF"));  // ldmia r7!, {r0-r6}
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, THUMB_OPCODE_FAILS_LDR) {
  EXPECT_TRUE(RunInstructionBadMemory("0xFE59"));  // ldr r6, [r7, r7]
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, THUMB_OPCODE_FAILS_LDR_I5) {
  EXPECT_TRUE(RunInstructionBadMemory("0xFE6F"));  // ldr r6, [r7, #124]
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, THUMB_OPCODE_FAILS_LDR_PC_OFFSET_I8) {
  EXPECT_TRUE(RunInstructionBadMemory("0xFF4F"));  // ldr r7, [pc, #1020]
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, THUMB_OPCODE_FAILS_LDR_SP_OFFSET_I8) {
  EXPECT_TRUE(RunInstructionBadMemory("0xFF9F"));  // ldr r7, [sp, #1020]
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, THUMB_OPCODE_FAILS_LDRB) {
  EXPECT_TRUE(RunInstructionBadMemory("0xFE5D"));  // ldrb r6, [r7, r7]
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, THUMB_OPCODE_FAILS_LDRB_I5) {
  EXPECT_TRUE(RunInstructionBadMemory("0xFE7F"));  // ldrb r6, [r7, #31]
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, THUMB_OPCODE_FAILS_LDRH) {
  EXPECT_TRUE(RunInstructionBadMemory("0xFE5B"));  // ldrh r6, [r7, r7]
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, THUMB_OPCODE_FAILS_LDRH_I5) {
  EXPECT_TRUE(RunInstructionBadMemory("0xFE8F"));  // ldrh r6, [r7, #62]
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, THUMB_OPCODE_FAILS_LDRSB) {
  EXPECT_TRUE(RunInstructionBadMemory("0xFE57"));  // ldrsb r6, [r7, r7]
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, THUMB_OPCODE_FAILS_LDRSH) {
  EXPECT_TRUE(RunInstructionBadMemory("0xFE5F"));  // ldrsh r6, [r7, r7]
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, THUMB_OPCODE_FAILS_POP) {
  EXPECT_TRUE(RunInstructionBadMemory("0xFFBC"));  // pop {r0-r7}
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, THUMB_OPCODE_FAILS_POP_PC) {
  EXPECT_TRUE(RunInstructionBadMemory("0xFFBD"));  // pop {r0-r7, pc}
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, THUMB_OPCODE_FAILS_PUSH) {
  EXPECT_TRUE(RunInstructionBadMemory("0xFFB5"));  // push {r0-r7, lr}
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, THUMB_OPCODE_FAILS_STMIA) {
  EXPECT_TRUE(RunInstructionBadMemory("0xFFC0"));  // stmia r0!, {r0-r7}
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, THUMB_OPCODE_FAILS_STR) {
  EXPECT_TRUE(RunInstructionBadMemory("0xFE51"));  // str r6, [r7, r7]
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, THUMB_OPCODE_FAILS_STR_I5) {
  EXPECT_TRUE(RunInstructionBadMemory("0xFE67"));  // str r6, [r7, #124]
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, THUMB_OPCODE_FAILS_STR_SP_OFFSET_I8) {
  EXPECT_TRUE(RunInstructionBadMemory("0xFF97"));  // str r7, [sp, #1020]
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, THUMB_OPCODE_FAILS_STRB) {
  EXPECT_TRUE(RunInstructionBadMemory("0xFE55"));  // strb r6, [r7, r7]
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, THUMB_OPCODE_FAILS_STRB_I5) {
  EXPECT_TRUE(RunInstructionBadMemory("0xFE77"));  // strb r6, [r7, #31]
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, THUMB_OPCODE_FAILS_STRH) {
  EXPECT_TRUE(RunInstructionBadMemory("0xFE53"));  // strh r6, [r7, r7]
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_F(ExecuteTest, THUMB_OPCODE_FAILS_STRH_I5) {
  EXPECT_TRUE(RunInstructionBadMemory("0xFE87"));  // strh r6, [r7, #62]
  EXPECT_TRUE(ArmIsDataAbort(registers_));
}