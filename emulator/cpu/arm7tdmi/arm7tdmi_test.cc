extern "C" {
#include "emulator/cpu/arm7tdmi/arm7tdmi.h"
}

#include "googletest/include/gtest/gtest.h"

class ExecuteTest : public testing::Test {
 public:
  void SetUp() override {
    for (char &c : memory_space_) {
      c = 0;
    }
    instruction_end_ = 0x100u;

    memory_ = MemoryAllocate(nullptr, Load32LE, Load16LE, Load8, Store32LE,
                             Store16LE, Store8, nullptr);
    ASSERT_NE(nullptr, memory_);

    ASSERT_TRUE(Arm7TdmiAllocate(&cpu_, &rst_, &fiq_, &irq_));

    // Initialize CPU state
    AddInstruction(0x0u, "0x02DCA0E3");  // mov sp, #0x200
    AddInstruction(0x4u, "0x00D08DE5");  // str sp, [sp]
    AddInstruction(0x8u, "0x01FCA0E3");  // mov pc, #0x100
    Run(3u);

    uint32_t value;
    ASSERT_TRUE(Load32LE(memory_, 0x200u, &value));
    ASSERT_EQ(0x200u, value);
  }

  void TearDown() override {
    MemoryFree(memory_);
    Arm7TdmiFree(cpu_);
    InterruptLineFree(rst_);
    InterruptLineFree(fiq_);
    InterruptLineFree(irq_);
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
  void AddInstruction(std::string instruction_hex) {
    if (instruction_hex[0u] == '0' && instruction_hex[1u] == 'x') {
      instruction_hex.erase(0u, 2u);
    }

    assert(instruction_hex.size() == 4u || instruction_hex.size() == 8u);
    for (size_t i = 0u; i < instruction_hex.size(); i += 2u) {
      std::string hex_byte;
      hex_byte += instruction_hex[i];
      hex_byte += instruction_hex[i + 1u];
      memory_space_[instruction_end_++] =
          (char)std::stoul(hex_byte, nullptr, 16u);
    }
  }

  void AddInstruction(uint32_t address, std::string instruction_hex) {
    if (instruction_hex[0u] == '0' && instruction_hex[1u] == 'x') {
      instruction_hex.erase(0u, 2u);
    }

    assert(instruction_hex.size() == 4u || instruction_hex.size() == 8u);
    for (size_t i = 0u; i < instruction_hex.size(); i += 2u) {
      std::string hex_byte;
      hex_byte += instruction_hex[i];
      hex_byte += instruction_hex[i + 1u];
      memory_space_[address++] = (char)std::stoul(hex_byte, nullptr, 16u);
    }
  }

  void Run(uint32_t num_steps) {
    for (uint32_t i = 0; i < num_steps; i++) {
      Arm7TdmiStep(cpu_, memory_);
    }
  }

  static std::vector<char> memory_space_;
  size_t instruction_end_;
  Arm7Tdmi *cpu_;
  InterruptLine *rst_;
  InterruptLine *fiq_;
  InterruptLine *irq_;
  Memory *memory_;
};

std::vector<char> ExecuteTest::memory_space_(1024u, 0);

TEST_F(ExecuteTest, ArmGCD) {
  AddInstruction("0x0F00A0E3");  // mov r0, #15
  AddInstruction("0x0A10A0E3");  // mov r1, #10
  AddInstruction("0x010050E1");  // cmp r0, r1
  AddInstruction("0x010040C0");  // subgt r0, r0, r1
  AddInstruction("0x001041B0");  // sublt r1, r1, r0
  AddInstruction("0xFBFFFF1A");  // bne #-12
  AddInstruction("0x00008DE5");  // str r0, [sp]
  Run(15u);

  uint32_t value;
  EXPECT_TRUE(Load32LE(memory_, 0x200u, &value));
  EXPECT_EQ(5u, value);
}

TEST_F(ExecuteTest, ThumbGCD) {
  // ARM Instructions
  AddInstruction("0x01E08FE2");  // add lr, pc, #1
  AddInstruction("0x1EFF2FE1");  // bx lr

  // Thumb Instructions
  AddInstruction("0x0F20");  // movs r0, #15
  AddInstruction("0x0A21");  // movs r1, #10
  AddInstruction("0x8842");  // cmp r0, r1
  AddInstruction("0x04D0");  // beq #12
  AddInstruction("0x01DB");  // blt #6
  AddInstruction("0x401A");  // subs r0, r0, r1
  AddInstruction("0xFAE7");  // b #-8
  AddInstruction("0x091A");  // subs r1, r1, r0
  AddInstruction("0xF8E7");  // b #-12
  AddInstruction("0x0090");  // str r0, [sp]
  Run(17u);

  uint32_t value;
  EXPECT_TRUE(Load32LE(memory_, 0x200u, &value));
  EXPECT_EQ(5u, value);
}

TEST_F(ExecuteTest, ModeSwitches) {
  // ARM Instructions
  AddInstruction("0x01E08FE2");  // add lr, pc, #1
  AddInstruction("0x1EFF2FE1");  // bx lr

  // Thumb Instructions
  AddInstruction("0x00A0");  // add r0, pc, #0
  AddInstruction("0x0047");  // bx r0

  // Arm Instructions
  AddInstruction("0xFF70A0E3");  // mov r7, #255
  AddInstruction("0x00708DE5");  // str r7, [sp]
  Run(6u);

  uint32_t value;
  EXPECT_TRUE(Load32LE(memory_, 0x200u, &value));
  EXPECT_EQ(255u, value);
}

TEST_F(ExecuteTest, ArmPrefetchABT) {
  AddInstruction("0xFFE4A0E3");         // mov lr, #0xFF000000
  AddInstruction("0x1EFF2FE1");         // bx lr
  AddInstruction(0x0Cu, "0x02DCA0E3");  // mov sp, #0x200
  AddInstruction(0x10u, "0x00E08DE5");  // str lr, [sp]
  Run(5u);

  uint32_t value;
  EXPECT_TRUE(Load32LE(memory_, 0x200u, &value));
  EXPECT_EQ(0xFF000004, value);
}

TEST_F(ExecuteTest, ThumbPrefetchABT) {
  AddInstruction("0x1FE2A0E3");         // mov lr, #0xF0000001
  AddInstruction("0x1EFF2FE1");         // bx lr
  AddInstruction(0x0Cu, "0x02DCA0E3");  // mov sp, #0x200
  AddInstruction(0x10u, "0x00E08DE5");  // str lr, [sp]
  Run(5u);

  uint32_t value;
  EXPECT_TRUE(Load32LE(memory_, 0x200u, &value));
  EXPECT_EQ(0xF0000004, value);
}

TEST_F(ExecuteTest, DoFIQ) {
  InterruptLineSetLevel(fiq_, true);

  AddInstruction(0x1Cu, "0x03DCA0E3");  // mov sp, #0x300
  AddInstruction(0x20u, "0x00D08DE5");  // str sp, [sp]
  AddInstruction(0x24u, "0x04E08DE5");  // str lr, [sp,#4]
  Run(4u);

  uint32_t value;
  EXPECT_TRUE(Load32LE(memory_, 0x300u, &value));
  EXPECT_EQ(0x300u, value);
  EXPECT_TRUE(Load32LE(memory_, 0x304u, &value));
  EXPECT_EQ(0x104u, value);
}

TEST_F(ExecuteTest, DoIRQ) {
  InterruptLineSetLevel(irq_, true);

  AddInstruction(0x18u, "0x03DCA0E3");  // mov sp, #0x300
  AddInstruction(0x1Cu, "0x00D08DE5");  // str sp, [sp]
  AddInstruction(0x20u, "0x04E08DE5");  // str lr, [sp,#4]
  Run(4u);

  uint32_t value;
  EXPECT_TRUE(Load32LE(memory_, 0x300u, &value));
  EXPECT_EQ(0x300u, value);
  EXPECT_TRUE(Load32LE(memory_, 0x304u, &value));
  EXPECT_EQ(0x104u, value);
}

TEST_F(ExecuteTest, FIQPreemptsIRQ) {
  InterruptLineSetLevel(fiq_, true);
  InterruptLineSetLevel(irq_, true);

  AddInstruction(0x1Cu, "0x03DCA0E3");  // mov sp, #0x300
  AddInstruction(0x20u, "0x00D08DE5");  // str sp, [sp]
  Run(3u);

  uint32_t value;
  EXPECT_TRUE(Load32LE(memory_, 0x300u, &value));
  EXPECT_EQ(0x300u, value);
}

TEST_F(ExecuteTest, RSTPreemptsAll) {
  InterruptLineSetLevel(rst_, true);
  InterruptLineSetLevel(fiq_, true);
  InterruptLineSetLevel(irq_, true);
  Run(1u);

  InterruptLineSetLevel(rst_, false);
  AddInstruction(0x0u, "0x03DCA0E3");  // mov sp, #0x300
  AddInstruction(0x4u, "0x00D08DE5");  // str sp, [sp]
  Run(3u);

  uint32_t value;
  EXPECT_TRUE(Load32LE(memory_, 0x300u, &value));
  EXPECT_EQ(0x300u, value);
}