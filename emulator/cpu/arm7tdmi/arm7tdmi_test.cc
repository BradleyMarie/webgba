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

    auto rst_line = InterruptLineAllocate(nullptr, Rst, nullptr);
    ASSERT_NE(nullptr, rst_line);

    auto fiq_line = InterruptLineAllocate(nullptr, Fiq, nullptr);
    ASSERT_NE(nullptr, fiq_line);

    auto irq_line = InterruptLineAllocate(nullptr, Irq, nullptr);
    ASSERT_NE(nullptr, irq_line);

    cpu_ = Arm7TdmiAllocate(rst_line, fiq_line, irq_line);
    ASSERT_NE(nullptr, cpu_);
    ASSERT_EQ(0x8u, cpu_->registers.current.user.gprs.pc);

    cpu_->registers.current.user.gprs.pc = 0x108u;
    cpu_->registers.current.user.gprs.sp = 0x200u;
    cpu_->registers.current.spsr.mode = MODE_USR;

    rst = false;
    fiq = false;
    irq = false;
  }

  void TearDown() override {
    MemoryFree(memory_);
    Arm7TdmiFree(cpu_);
  }

 private:
  static bool Rst(const void *context) { return rst; };
  static bool Fiq(const void *context) { return fiq; };
  static bool Irq(const void *context) { return irq; };

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

  void Run(uint32_t num_steps) {
    for (uint32_t i = 0; i < num_steps; i++) {
      Arm7TdmiStep(cpu_, memory_);
    }
  }

  static std::vector<char> memory_space_;
  size_t instruction_end_;
  Arm7Tdmi *cpu_;
  Memory *memory_;

  static bool rst;
  static bool fiq;
  static bool irq;
};

std::vector<char> ExecuteTest::memory_space_(1024u, 0);
bool ExecuteTest::rst = false;
bool ExecuteTest::fiq = false;
bool ExecuteTest::irq = false;

TEST_F(ExecuteTest, ArmGCD) {
  AddInstruction("0x0F00A0E3");  // mov r0, #15
  AddInstruction("0x0A10A0E3");  // mov r1, #10
  AddInstruction("0x010050E1");  // cmp r0, r1
  AddInstruction("0x010040C0");  // subgt r0, r0, r1
  AddInstruction("0x001041B0");  // sublt r1, r1, r0
  AddInstruction("0xFBFFFF1A");  // bne #-12
  Run(20u);
  EXPECT_EQ(5u, cpu_->registers.current.user.gprs.r0);
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
  AddInstruction("0x02DB");  // blt #8
  AddInstruction("0x401A");  // subs r0, r0, r1
  AddInstruction("0xFAE7");  // b #-8
  AddInstruction("0x091A");  // subs r1, r1, r0
  AddInstruction("0xF8E7");  // b #-12
  Run(40u);
  EXPECT_EQ(5u, cpu_->registers.current.user.gprs.r0);
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
  Run(5u);
  EXPECT_EQ(255u, cpu_->registers.current.user.gprs.r7);
}

TEST_F(ExecuteTest, ArmPrefetchABT) {
  AddInstruction("0xFFE4A0E3");  // mov lr, #0xFF000000
  AddInstruction("0x1EFF2FE1");  // bx lr
  Run(3u);

  EXPECT_EQ(MODE_ABT, cpu_->registers.current.user.cpsr.mode);
  EXPECT_EQ(0x14u, cpu_->registers.current.user.gprs.pc);
  EXPECT_FALSE(cpu_->registers.current.spsr.thumb);
}

TEST_F(ExecuteTest, ThumbPrefetchABT) {
  AddInstruction("0x1FE2A0E3");  // mov lr, #0xF0000001
  AddInstruction("0x1EFF2FE1");  // bx lr
  Run(3u);

  EXPECT_EQ(MODE_ABT, cpu_->registers.current.user.cpsr.mode);
  EXPECT_EQ(0x14u, cpu_->registers.current.user.gprs.pc);
  EXPECT_TRUE(cpu_->registers.current.spsr.thumb);
}

TEST_F(ExecuteTest, DoFIQ) {
  fiq = true;
  Run(1u);

  EXPECT_EQ(MODE_FIQ, cpu_->registers.current.user.cpsr.mode);
  EXPECT_EQ(0x24u, cpu_->registers.current.user.gprs.pc);
}

TEST_F(ExecuteTest, DoIRQ) {
  irq = true;
  Run(1u);

  EXPECT_EQ(MODE_IRQ, cpu_->registers.current.user.cpsr.mode);
  EXPECT_EQ(0x20u, cpu_->registers.current.user.gprs.pc);
}

TEST_F(ExecuteTest, FIQPreemptsIRQ) {
  fiq = true;
  irq = true;
  Run(1u);

  EXPECT_EQ(MODE_FIQ, cpu_->registers.current.user.cpsr.mode);
  EXPECT_EQ(0x24u, cpu_->registers.current.user.gprs.pc);
}

TEST_F(ExecuteTest, MaskedFIQ) {
  cpu_->registers.current.user.cpsr.fiq_disable = true;
  fiq = true;
  Run(1u);

  EXPECT_EQ(MODE_SVC, cpu_->registers.current.user.cpsr.mode);
  EXPECT_EQ(0x10Cu, cpu_->registers.current.user.gprs.pc);
}

TEST_F(ExecuteTest, MaskedIRQ) {
  cpu_->registers.current.user.cpsr.irq_disable = true;
  irq = true;
  Run(1u);

  EXPECT_EQ(MODE_SVC, cpu_->registers.current.user.cpsr.mode);
  EXPECT_EQ(0x10Cu, cpu_->registers.current.user.gprs.pc);
}

TEST_F(ExecuteTest, RSTPreemptsAll) {
  fiq = true;
  irq = true;
  rst = true;
  Run(1u);

  EXPECT_EQ(MODE_SVC, cpu_->registers.current.user.cpsr.mode);
  EXPECT_EQ(0x8u, cpu_->registers.current.user.gprs.pc);
}