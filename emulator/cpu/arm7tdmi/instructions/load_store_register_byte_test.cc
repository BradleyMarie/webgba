extern "C" {
#include "emulator/cpu/arm7tdmi/instructions/load_store_register_byte.h"
}

#include <cstring>
#include <vector>

#include "googletest/include/gtest/gtest.h"

ArmAllRegisters CreateArmAllRegisters() {
  ArmAllRegisters registers;
  memset(&registers, 0, sizeof(ArmAllRegisters));
  return registers;
}

bool ArmAllRegistersAreZero(const ArmAllRegisters &regs) {
  auto zero = CreateArmAllRegisters();
  return !memcmp(&zero, &regs, sizeof(ArmAllRegisters));
}

class MemoryTest : public testing::Test {
 public:
  void SetUp() override {
    for (char &c : memory_space_) {
      c = 0;
    }
    memory_ = MemoryAllocate(nullptr, Load32LE, Load16LE, Load8, Store32LE,
                             Store16LE, Store8, nullptr);
    ASSERT_NE(nullptr, memory_);
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

  bool MemoryIsZero() {
    for (char c : memory_space_) {
      if (c != 0) {
        return false;
      }
    }
    return true;
  }

  static std::vector<char> memory_space_;
  Memory *memory_;
};

std::vector<char> MemoryTest::memory_space_(1024, 0);

TEST_F(MemoryTest, ArmLDR_IB) {
  auto registers = CreateArmAllRegisters();

  ASSERT_TRUE(Store32LE(nullptr, 8u, 0xDEADC0DE));
  ArmLDR_IB(&registers, memory_, REGISTER_R0, REGISTER_R0, 8u);
  EXPECT_EQ(0xDEADC0DE, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint32_t memory_contents;
  ASSERT_TRUE(Load32LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(0xDEADC0DE, memory_contents);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDR_DB) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.gprs.r0 = 16u;

  ASSERT_TRUE(Store32LE(nullptr, 8u, 0xDEADC0DE));
  ArmLDR_DB(&registers, memory_, REGISTER_R0, REGISTER_R0, 8u);
  EXPECT_EQ(0xDEADC0DE, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint32_t memory_contents;
  ASSERT_TRUE(Load32LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(0xDEADC0DE, memory_contents);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDR_DBW) {
  auto registers = CreateArmAllRegisters();

  ASSERT_TRUE(Store32LE(nullptr, 8u, 0xDEADC0DE));
  registers.current.user.gprs.r1 = 12u;
  ArmLDR_DBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(0xDEADC0DE, registers.current.user.gprs.r0);
  EXPECT_EQ(8u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint32_t memory_contents;
  ASSERT_TRUE(Load32LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(0xDEADC0DE, memory_contents);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDR_DAW) {
  auto registers = CreateArmAllRegisters();

  ASSERT_TRUE(Store32LE(nullptr, 8u, 0xDEADC0DE));
  registers.current.user.gprs.r1 = 8u;
  ArmLDR_DAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(0xDEADC0DE, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint32_t memory_contents;
  ASSERT_TRUE(Load32LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(0xDEADC0DE, memory_contents);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDR_IBW) {
  auto registers = CreateArmAllRegisters();

  ASSERT_TRUE(Store32LE(nullptr, 8u, 0xDEADC0DE));
  registers.current.user.gprs.r1 = 4u;
  ArmLDR_IBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(0xDEADC0DE, registers.current.user.gprs.r0);
  EXPECT_EQ(8u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint32_t memory_contents;
  ASSERT_TRUE(Load32LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(0xDEADC0DE, memory_contents);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDR_IAW) {
  auto registers = CreateArmAllRegisters();

  ASSERT_TRUE(Store32LE(nullptr, 8u, 0xDEADC0DE));
  registers.current.user.gprs.r1 = 8u;
  ArmLDR_IAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(0xDEADC0DE, registers.current.user.gprs.r0);
  EXPECT_EQ(12u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint32_t memory_contents;
  ASSERT_TRUE(Load32LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(0xDEADC0DE, memory_contents);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRT_IB) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;

  ASSERT_TRUE(Store32LE(nullptr, 8u, 0xDEADC0DE));
  ArmLDRT_IB(&registers, memory_, REGISTER_R0, REGISTER_R0, 8u);
  EXPECT_EQ(0xDEADC0DE, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint32_t memory_contents;
  ASSERT_TRUE(Load32LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(0xDEADC0DE, memory_contents);

  registers.current.user.cpsr.mode = 0u;
  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRT_DB) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.gprs.r0 = 16u;
  registers.current.user.cpsr.mode = MODE_SVC;

  ASSERT_TRUE(Store32LE(nullptr, 8u, 0xDEADC0DE));
  ArmLDRT_DB(&registers, memory_, REGISTER_R0, REGISTER_R0, 8u);
  EXPECT_EQ(0xDEADC0DE, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint32_t memory_contents;
  ASSERT_TRUE(Load32LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(0xDEADC0DE, memory_contents);

  registers.current.user.cpsr.mode = 0u;
  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRT_DAW) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;

  ASSERT_TRUE(Store32LE(nullptr, 8u, 0xDEADC0DE));
  registers.current.user.gprs.r1 = 8u;
  ArmLDRT_DAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(0xDEADC0DE, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint32_t memory_contents;
  ASSERT_TRUE(Load32LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(0xDEADC0DE, memory_contents);

  registers.current.user.cpsr.mode = 0u;
  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRT_IAW) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;

  ASSERT_TRUE(Store32LE(nullptr, 8u, 0xDEADC0DE));
  registers.current.user.gprs.r1 = 8u;
  ArmLDRT_IAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(0xDEADC0DE, registers.current.user.gprs.r0);
  EXPECT_EQ(12u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint32_t memory_contents;
  ASSERT_TRUE(Load32LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(0xDEADC0DE, memory_contents);

  registers.current.user.cpsr.mode = 0u;
  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRB_IB) {
  auto registers = CreateArmAllRegisters();

  ASSERT_TRUE(Store8(nullptr, 8u, 137u));
  registers.current.user.gprs.r0 = 8u;
  ArmLDRB_IB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(137u, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint8_t memory_contents;
  ASSERT_TRUE(Load8(nullptr, 8u, &memory_contents));
  EXPECT_EQ(137u, memory_contents);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRB_DB) {
  auto registers = CreateArmAllRegisters();

  ASSERT_TRUE(Store8(nullptr, 8u, 137u));
  registers.current.user.gprs.r0 = 8u;
  registers.current.user.gprs.r1 = 16u;
  ArmLDRB_DB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(137u, registers.current.user.gprs.r0);
  EXPECT_EQ(16u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint8_t memory_contents;
  ASSERT_TRUE(Load8(nullptr, 8u, &memory_contents));
  EXPECT_EQ(137u, memory_contents);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRB_DBW) {
  auto registers = CreateArmAllRegisters();

  ASSERT_TRUE(Store8(nullptr, 8u, 137u));
  registers.current.user.gprs.r1 = 12u;
  ArmLDRB_DBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(137u, registers.current.user.gprs.r0);
  EXPECT_EQ(8u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint8_t memory_contents;
  ASSERT_TRUE(Load8(nullptr, 8u, &memory_contents));
  EXPECT_EQ(137u, memory_contents);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRB_DAW) {
  auto registers = CreateArmAllRegisters();

  ASSERT_TRUE(Store8(nullptr, 8u, 137u));
  registers.current.user.gprs.r1 = 8u;
  ArmLDRB_DAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(137u, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint8_t memory_contents;
  ASSERT_TRUE(Load8(nullptr, 8u, &memory_contents));
  EXPECT_EQ(137u, memory_contents);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRB_IBW) {
  auto registers = CreateArmAllRegisters();

  ASSERT_TRUE(Store8(nullptr, 8u, 137u));
  registers.current.user.gprs.r1 = 4u;
  ArmLDRB_IBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(137u, registers.current.user.gprs.r0);
  EXPECT_EQ(8u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint8_t memory_contents;
  ASSERT_TRUE(Load8(nullptr, 8u, &memory_contents));
  EXPECT_EQ(137u, memory_contents);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRB_IAW) {
  auto registers = CreateArmAllRegisters();

  ASSERT_TRUE(Store8(nullptr, 8u, 137u));
  registers.current.user.gprs.r1 = 8u;
  ArmLDRB_IAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(137u, registers.current.user.gprs.r0);
  EXPECT_EQ(12u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint8_t memory_contents;
  ASSERT_TRUE(Load8(nullptr, 8u, &memory_contents));
  EXPECT_EQ(137u, memory_contents);

  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRBT_IB) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;

  ASSERT_TRUE(Store8(nullptr, 8u, 137u));
  registers.current.user.gprs.r0 = 8u;
  ArmLDRBT_IB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(137u, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint8_t memory_contents;
  ASSERT_TRUE(Load8(nullptr, 8u, &memory_contents));
  EXPECT_EQ(137u, memory_contents);

  registers.current.user.cpsr.mode = 0u;
  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRBT_DB) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;

  ASSERT_TRUE(Store8(nullptr, 8u, 137u));
  registers.current.user.gprs.r0 = 8u;
  registers.current.user.gprs.r1 = 16u;
  ArmLDRBT_DB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(137u, registers.current.user.gprs.r0);
  EXPECT_EQ(16u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint8_t memory_contents;
  ASSERT_TRUE(Load8(nullptr, 8u, &memory_contents));
  EXPECT_EQ(137u, memory_contents);

  registers.current.user.cpsr.mode = 0u;
  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRBT_DAW) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;

  ASSERT_TRUE(Store8(nullptr, 8u, 137u));
  registers.current.user.gprs.r1 = 8u;
  ArmLDRBT_DAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(137u, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint8_t memory_contents;
  ASSERT_TRUE(Load8(nullptr, 8u, &memory_contents));
  EXPECT_EQ(137u, memory_contents);

  registers.current.user.cpsr.mode = 0u;
  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRBT_IAW) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;

  ASSERT_TRUE(Store8(nullptr, 8u, 137u));
  registers.current.user.gprs.r1 = 8u;
  ArmLDRBT_IAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(137u, registers.current.user.gprs.r0);
  EXPECT_EQ(12u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint8_t memory_contents;
  ASSERT_TRUE(Load8(nullptr, 8u, &memory_contents));
  EXPECT_EQ(137u, memory_contents);

  registers.current.user.cpsr.mode = 0u;
  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;
  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmSTR_IB) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 0xDEADC0DE;
  ArmSTR_IB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(0xDEADC0DE, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint32_t memory_contents;
  ASSERT_TRUE(Load32LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(0xDEADC0DE, memory_contents);

  ASSERT_TRUE(Store32LE(nullptr, 8u, 0u));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTR_DB) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 0xDEADC0DE;
  registers.current.user.gprs.r1 = 16u;
  ArmSTR_DB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(0xDEADC0DE, registers.current.user.gprs.r0);
  EXPECT_EQ(16u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint32_t memory_contents;
  ASSERT_TRUE(Load32LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(0xDEADC0DE, memory_contents);

  ASSERT_TRUE(Store32LE(nullptr, 8u, 0u));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTR_DBW) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 0xDEADC0DE;
  registers.current.user.gprs.r1 = 12u;
  ArmSTR_DBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(0xDEADC0DE, registers.current.user.gprs.r0);
  EXPECT_EQ(8u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint32_t memory_contents;
  ASSERT_TRUE(Load32LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(0xDEADC0DE, memory_contents);

  ASSERT_TRUE(Store32LE(nullptr, 8u, 0u));
  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;

  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTR_DAW) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 0xDEADC0DE;
  registers.current.user.gprs.r1 = 8u;
  ArmSTR_DAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(0xDEADC0DE, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint32_t memory_contents;
  ASSERT_TRUE(Load32LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(0xDEADC0DE, memory_contents);

  ASSERT_TRUE(Store32LE(nullptr, 8u, 0u));
  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;

  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTR_IBW) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 0xDEADC0DE;
  registers.current.user.gprs.r1 = 4u;
  ArmSTR_IBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(0xDEADC0DE, registers.current.user.gprs.r0);
  EXPECT_EQ(8u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint32_t memory_contents;
  ASSERT_TRUE(Load32LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(0xDEADC0DE, memory_contents);

  ASSERT_TRUE(Store32LE(nullptr, 8u, 0u));
  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;

  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTR_IAW) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 0xDEADC0DE;
  registers.current.user.gprs.r1 = 8u;
  ArmSTR_IAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(0xDEADC0DE, registers.current.user.gprs.r0);
  EXPECT_EQ(12u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint32_t memory_contents;
  ASSERT_TRUE(Load32LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(0xDEADC0DE, memory_contents);

  ASSERT_TRUE(Store32LE(nullptr, 8u, 0u));
  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;

  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRT_IB) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;

  registers.current.user.gprs.r0 = 0xDEADC0DE;
  ArmSTRT_IB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(0xDEADC0DE, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint32_t memory_contents;
  ASSERT_TRUE(Load32LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(0xDEADC0DE, memory_contents);

  ASSERT_TRUE(Store32LE(nullptr, 8u, 0u));
  registers.current.user.cpsr.mode = 0u;
  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;

  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRT_DB) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;

  registers.current.user.gprs.r0 = 0xDEADC0DE;
  registers.current.user.gprs.r1 = 16u;
  ArmSTRT_DB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(0xDEADC0DE, registers.current.user.gprs.r0);
  EXPECT_EQ(16u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint32_t memory_contents;
  ASSERT_TRUE(Load32LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(0xDEADC0DE, memory_contents);

  ASSERT_TRUE(Store32LE(nullptr, 8u, 0u));
  registers.current.user.cpsr.mode = 0u;
  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;

  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRT_DAW) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;

  registers.current.user.gprs.r0 = 0xDEADC0DE;
  registers.current.user.gprs.r1 = 8u;
  ArmSTRT_DAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(0xDEADC0DE, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint32_t memory_contents;
  ASSERT_TRUE(Load32LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(0xDEADC0DE, memory_contents);

  ASSERT_TRUE(Store32LE(nullptr, 8u, 0u));
  registers.current.user.cpsr.mode = 0u;
  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;

  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRT_IAW) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;

  registers.current.user.gprs.r0 = 0xDEADC0DE;
  registers.current.user.gprs.r1 = 8u;
  ArmSTRT_IAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(0xDEADC0DE, registers.current.user.gprs.r0);
  EXPECT_EQ(12u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint32_t memory_contents;
  ASSERT_TRUE(Load32LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(0xDEADC0DE, memory_contents);

  ASSERT_TRUE(Store32LE(nullptr, 8u, 0u));
  registers.current.user.cpsr.mode = 0u;
  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;

  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRB_IB) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 137u;
  ArmSTRB_IB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(137u, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint8_t memory_contents;
  ASSERT_TRUE(Load8(nullptr, 8u, &memory_contents));
  EXPECT_EQ(137u, memory_contents);

  ASSERT_TRUE(Store8(nullptr, 8u, 0u));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRB_DB) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 137u;
  registers.current.user.gprs.r1 = 16u;
  ArmSTRB_DB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(137u, registers.current.user.gprs.r0);
  EXPECT_EQ(16u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint8_t memory_contents;
  ASSERT_TRUE(Load8(nullptr, 8u, &memory_contents));
  EXPECT_EQ(137u, memory_contents);

  ASSERT_TRUE(Store8(nullptr, 8u, 0u));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRB_DBW) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 137u;
  registers.current.user.gprs.r1 = 12u;
  ArmSTRB_DBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(137u, registers.current.user.gprs.r0);
  EXPECT_EQ(8u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint8_t memory_contents;
  ASSERT_TRUE(Load8(nullptr, 8u, &memory_contents));
  EXPECT_EQ(137u, memory_contents);

  ASSERT_TRUE(Store8(nullptr, 8u, 0u));
  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;

  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRB_DAW) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 137u;
  registers.current.user.gprs.r1 = 8u;
  ArmSTRB_DAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(137u, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint8_t memory_contents;
  ASSERT_TRUE(Load8(nullptr, 8u, &memory_contents));
  EXPECT_EQ(137u, memory_contents);

  ASSERT_TRUE(Store8(nullptr, 8u, 0u));
  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;

  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRB_IBW) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 137u;
  registers.current.user.gprs.r1 = 4u;
  ArmSTRB_IBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(137u, registers.current.user.gprs.r0);
  EXPECT_EQ(8u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint8_t memory_contents;
  ASSERT_TRUE(Load8(nullptr, 8u, &memory_contents));
  EXPECT_EQ(137u, memory_contents);

  ASSERT_TRUE(Store8(nullptr, 8u, 0u));
  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;

  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRB_IAW) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 137u;
  registers.current.user.gprs.r1 = 8u;
  ArmSTRB_IAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(137u, registers.current.user.gprs.r0);
  EXPECT_EQ(12u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint8_t memory_contents;
  ASSERT_TRUE(Load8(nullptr, 8u, &memory_contents));
  EXPECT_EQ(137u, memory_contents);

  ASSERT_TRUE(Store8(nullptr, 8u, 0u));
  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;

  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRBT_IB) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;

  registers.current.user.gprs.r0 = 137u;
  ArmSTRBT_IB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(137u, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint8_t memory_contents;
  ASSERT_TRUE(Load8(nullptr, 8u, &memory_contents));
  EXPECT_EQ(137u, memory_contents);

  ASSERT_TRUE(Store8(nullptr, 8u, 0u));
  registers.current.user.cpsr.mode = 0u;
  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;

  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRBT_DB) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;

  registers.current.user.gprs.r0 = 137u;
  registers.current.user.gprs.r1 = 16u;
  ArmSTRBT_DB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(137u, registers.current.user.gprs.r0);
  EXPECT_EQ(16u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint8_t memory_contents;
  ASSERT_TRUE(Load8(nullptr, 8u, &memory_contents));
  EXPECT_EQ(137u, memory_contents);

  ASSERT_TRUE(Store8(nullptr, 8u, 0u));
  registers.current.user.cpsr.mode = 0u;
  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;

  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRBT_DAW) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;

  registers.current.user.gprs.r0 = 137u;
  registers.current.user.gprs.r1 = 8u;
  ArmSTRBT_DAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(137u, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint8_t memory_contents;
  ASSERT_TRUE(Load8(nullptr, 8u, &memory_contents));
  EXPECT_EQ(137u, memory_contents);

  ASSERT_TRUE(Store8(nullptr, 8u, 0u));
  registers.current.user.cpsr.mode = 0u;
  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;

  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRBT_IAW) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;

  registers.current.user.gprs.r0 = 137u;
  registers.current.user.gprs.r1 = 8u;
  ArmSTRBT_IAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(137u, registers.current.user.gprs.r0);
  EXPECT_EQ(12u, registers.current.user.gprs.r1);
  EXPECT_EQ(4u, registers.current.user.gprs.pc);

  uint8_t memory_contents;
  ASSERT_TRUE(Load8(nullptr, 8u, &memory_contents));
  EXPECT_EQ(137u, memory_contents);

  ASSERT_TRUE(Store8(nullptr, 8u, 0u));
  registers.current.user.cpsr.mode = 0u;
  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.pc = 0u;

  EXPECT_TRUE(ArmAllRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

class MemoryFailsTest : public testing::Test {
 public:
  void SetUp() override {
    memory_ = MemoryAllocate(nullptr, Load32LE, Load16LE, Load8, Store32LE,
                             Store16LE, Store8, nullptr);
    ASSERT_NE(nullptr, memory_);
  }

  void TearDown() override { MemoryFree(memory_); }

 protected:
  static bool Load32LE(const void *context, uint32_t address, uint32_t *value) {
    return false;
  }

  static bool Load16LE(const void *context, uint32_t address, uint16_t *value) {
    return false;
  }

  static bool Load8(const void *context, uint32_t address, uint8_t *value) {
    return false;
  }

  static bool Store32LE(void *context, uint32_t address, uint32_t value) {
    return false;
  }

  static bool Store16LE(void *context, uint32_t address, uint16_t value) {
    return false;
  }

  static bool Store8(void *context, uint32_t address, uint8_t value) {
    return false;
  }

  ArmAllRegisters CreateArmAllRegistersInMode() {
    ArmAllRegisters registers;
    memset(&registers, 0, sizeof(ArmAllRegisters));
    registers.current.user.cpsr.mode = MODE_USR;
    return registers;
  }

  bool ArmIsDataAbort(const ArmAllRegisters &regs) {
    return regs.current.user.cpsr.mode == MODE_ABT;
  }

  Memory *memory_;
};

TEST_F(MemoryFailsTest, ArmLDR_IB) {
  auto registers = CreateArmAllRegistersInMode();

  ArmLDR_IB(&registers, memory_, REGISTER_R0, REGISTER_R0, 8u);
  EXPECT_EQ(0u, registers.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmLDR_DB) {
  auto registers = CreateArmAllRegistersInMode();
  registers.current.user.gprs.r0 = 16u;

  ArmLDR_DB(&registers, memory_, REGISTER_R0, REGISTER_R0, 8u);
  EXPECT_EQ(16u, registers.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmLDR_DBW) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r1 = 12u;
  ArmLDR_DBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(0u, registers.current.user.gprs.r0);
  EXPECT_EQ(8u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmLDR_DAW) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r1 = 8u;
  ArmLDR_DAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(0u, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmLDR_IBW) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r1 = 4u;
  ArmLDR_IBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(0u, registers.current.user.gprs.r0);
  EXPECT_EQ(8u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmLDR_IAW) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r1 = 8u;
  ArmLDR_IAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(0u, registers.current.user.gprs.r0);
  EXPECT_EQ(12u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmLDRT_IB) {
  auto registers = CreateArmAllRegistersInMode();
  registers.current.user.cpsr.mode = MODE_SVC;

  ArmLDRT_IB(&registers, memory_, REGISTER_R0, REGISTER_R0, 8u);
  EXPECT_EQ(0u, registers.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmLDRT_DB) {
  auto registers = CreateArmAllRegistersInMode();
  registers.current.user.gprs.r0 = 16u;
  registers.current.user.cpsr.mode = MODE_SVC;

  ArmLDRT_DB(&registers, memory_, REGISTER_R0, REGISTER_R0, 8u);
  EXPECT_EQ(16u, registers.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmLDRT_DAW) {
  auto registers = CreateArmAllRegistersInMode();
  registers.current.user.cpsr.mode = MODE_SVC;

  registers.current.user.gprs.r1 = 8u;
  ArmLDRT_DAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(0u, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmLDRT_IAW) {
  auto registers = CreateArmAllRegistersInMode();
  registers.current.user.cpsr.mode = MODE_SVC;

  registers.current.user.gprs.r1 = 8u;
  ArmLDRT_IAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(0u, registers.current.user.gprs.r0);
  EXPECT_EQ(12u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmLDRB_IB) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0 = 8u;
  ArmLDRB_IB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(8u, registers.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmLDRB_DB) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0 = 8u;
  registers.current.user.gprs.r1 = 16u;
  ArmLDRB_DB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(8u, registers.current.user.gprs.r0);
  EXPECT_EQ(16u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmLDRB_DBW) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r1 = 12u;
  ArmLDRB_DBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(0u, registers.current.user.gprs.r0);
  EXPECT_EQ(8u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmLDRB_DAW) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r1 = 8u;
  ArmLDRB_DAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(0u, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmLDRB_IBW) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r1 = 4u;
  ArmLDRB_IBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(0u, registers.current.user.gprs.r0);
  EXPECT_EQ(8u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmLDRB_IAW) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r1 = 8u;
  ArmLDRB_IAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(0u, registers.current.user.gprs.r0);
  EXPECT_EQ(12u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmLDRBT_IB) {
  auto registers = CreateArmAllRegistersInMode();
  registers.current.user.cpsr.mode = MODE_SVC;

  registers.current.user.gprs.r0 = 8u;
  ArmLDRBT_IB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(8u, registers.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmLDRBT_DB) {
  auto registers = CreateArmAllRegistersInMode();
  registers.current.user.cpsr.mode = MODE_SVC;

  registers.current.user.gprs.r0 = 8u;
  registers.current.user.gprs.r1 = 16u;
  ArmLDRBT_DB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(8u, registers.current.user.gprs.r0);
  EXPECT_EQ(16u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmLDRBT_DAW) {
  auto registers = CreateArmAllRegistersInMode();
  registers.current.user.cpsr.mode = MODE_SVC;

  registers.current.user.gprs.r1 = 8u;
  ArmLDRBT_DAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(0u, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmLDRBT_IAW) {
  auto registers = CreateArmAllRegistersInMode();
  registers.current.user.cpsr.mode = MODE_SVC;

  registers.current.user.gprs.r1 = 8u;
  ArmLDRBT_IAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(0u, registers.current.user.gprs.r0);
  EXPECT_EQ(12u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmSTR_IB) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0 = 0xDEADC0DE;
  ArmSTR_IB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(0xDEADC0DE, registers.current.user.gprs.r0);
  EXPECT_EQ(0u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmSTR_DB) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0 = 0xDEADC0DE;
  registers.current.user.gprs.r1 = 16u;
  ArmSTR_DB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(0xDEADC0DE, registers.current.user.gprs.r0);
  EXPECT_EQ(16u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmSTR_DBW) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0 = 0xDEADC0DE;
  registers.current.user.gprs.r1 = 12u;
  ArmSTR_DBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(0xDEADC0DE, registers.current.user.gprs.r0);
  EXPECT_EQ(8u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmSTR_DAW) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0 = 0xDEADC0DE;
  registers.current.user.gprs.r1 = 8u;
  ArmSTR_DAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(0xDEADC0DE, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmSTR_IBW) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0 = 0xDEADC0DE;
  registers.current.user.gprs.r1 = 4u;
  ArmSTR_IBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(0xDEADC0DE, registers.current.user.gprs.r0);
  EXPECT_EQ(8u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmSTR_IAW) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0 = 0xDEADC0DE;
  registers.current.user.gprs.r1 = 8u;
  ArmSTR_IAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(0xDEADC0DE, registers.current.user.gprs.r0);
  EXPECT_EQ(12u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmSTRT_IB) {
  auto registers = CreateArmAllRegistersInMode();
  registers.current.user.cpsr.mode = MODE_SVC;

  registers.current.user.gprs.r0 = 0xDEADC0DE;
  ArmSTRT_IB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(0xDEADC0DE, registers.current.user.gprs.r0);
  EXPECT_EQ(0u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmSTRT_DB) {
  auto registers = CreateArmAllRegistersInMode();
  registers.current.user.cpsr.mode = MODE_SVC;

  registers.current.user.gprs.r0 = 0xDEADC0DE;
  registers.current.user.gprs.r1 = 16u;
  ArmSTRT_DB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(0xDEADC0DE, registers.current.user.gprs.r0);
  EXPECT_EQ(16u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmSTRT_DAW) {
  auto registers = CreateArmAllRegistersInMode();
  registers.current.user.cpsr.mode = MODE_SVC;

  registers.current.user.gprs.r0 = 0xDEADC0DE;
  registers.current.user.gprs.r1 = 8u;
  ArmSTRT_DAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(0xDEADC0DE, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmSTRT_IAW) {
  auto registers = CreateArmAllRegistersInMode();
  registers.current.user.cpsr.mode = MODE_SVC;

  registers.current.user.gprs.r0 = 0xDEADC0DE;
  registers.current.user.gprs.r1 = 8u;
  ArmSTRT_IAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(0xDEADC0DE, registers.current.user.gprs.r0);
  EXPECT_EQ(12u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmSTRB_IB) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0 = 137u;
  ArmSTRB_IB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(137u, registers.current.user.gprs.r0);
  EXPECT_EQ(0u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmSTRB_DB) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0 = 137u;
  registers.current.user.gprs.r1 = 16u;
  ArmSTRB_DB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(137u, registers.current.user.gprs.r0);
  EXPECT_EQ(16u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmSTRB_DBW) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0 = 137u;
  registers.current.user.gprs.r1 = 12u;
  ArmSTRB_DBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(137u, registers.current.user.gprs.r0);
  EXPECT_EQ(8u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmSTRB_DAW) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0 = 137u;
  registers.current.user.gprs.r1 = 8u;
  ArmSTRB_DAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(137u, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmSTRB_IBW) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0 = 137u;
  registers.current.user.gprs.r1 = 4u;
  ArmSTRB_IBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(137u, registers.current.user.gprs.r0);
  EXPECT_EQ(8u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmSTRB_IAW) {
  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0 = 137u;
  registers.current.user.gprs.r1 = 8u;
  ArmSTRB_IAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(137u, registers.current.user.gprs.r0);
  EXPECT_EQ(12u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmSTRBT_IB) {
  auto registers = CreateArmAllRegistersInMode();
  registers.current.user.cpsr.mode = MODE_SVC;

  registers.current.user.gprs.r0 = 137u;
  ArmSTRBT_IB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(137u, registers.current.user.gprs.r0);
  EXPECT_EQ(0u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmSTRBT_DB) {
  auto registers = CreateArmAllRegistersInMode();
  registers.current.user.cpsr.mode = MODE_SVC;

  registers.current.user.gprs.r0 = 137u;
  registers.current.user.gprs.r1 = 16u;
  ArmSTRBT_DB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(137u, registers.current.user.gprs.r0);
  EXPECT_EQ(16u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmSTRBT_DAW) {
  auto registers = CreateArmAllRegistersInMode();
  registers.current.user.cpsr.mode = MODE_SVC;

  registers.current.user.gprs.r0 = 137u;
  registers.current.user.gprs.r1 = 8u;
  ArmSTRBT_DAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(137u, registers.current.user.gprs.r0);
  EXPECT_EQ(4u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_F(MemoryFailsTest, ArmSTRBT_IAW) {
  auto registers = CreateArmAllRegistersInMode();
  registers.current.user.cpsr.mode = MODE_SVC;

  registers.current.user.gprs.r0 = 137u;
  registers.current.user.gprs.r1 = 8u;
  ArmSTRBT_IAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(137u, registers.current.user.gprs.r0);
  EXPECT_EQ(12u, registers.current.user.gprs.r1);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}