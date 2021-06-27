extern "C" {
#include "emulator/cpu/arm7tdmi/instructions/load_store_register_byte.h"
}

#include <strings.h>

#include <vector>

#include "googletest/include/gtest/gtest.h"

ArmGeneralPurposeRegisters CreateArmGeneralPurposeRegisters() {
  ArmGeneralPurposeRegisters registers;
  memset(&registers, 0, sizeof(ArmGeneralPurposeRegisters));
  return registers;
}

bool ArmGeneralPurposeRegistersAreZero(const ArmGeneralPurposeRegisters &regs) {
  auto zero = CreateArmGeneralPurposeRegisters();
  return !memcmp(&zero, &regs, sizeof(ArmGeneralPurposeRegisters));
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

TEST_F(MemoryTest, ArmLDR) {
  auto registers = CreateArmGeneralPurposeRegisters();

  ASSERT_TRUE(Store32LE(nullptr, 8u, 0xDEADC0DE));
  ArmLDR(&registers, memory_, REGISTER_R0, 8u);
  EXPECT_EQ(0xDEADC0DE, registers.r0);

  uint32_t memory_contents;
  ASSERT_TRUE(Load32LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(0xDEADC0DE, memory_contents);

  registers.r0 = 0u;
  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDR_DecrementPreIndexed) {
  auto registers = CreateArmGeneralPurposeRegisters();

  ASSERT_TRUE(Store32LE(nullptr, 8u, 0xDEADC0DE));
  registers.r1 = 12u;
  ArmLDR_DecrementPreIndexed(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(0xDEADC0DE, registers.r0);
  EXPECT_EQ(8u, registers.r1);

  uint32_t memory_contents;
  ASSERT_TRUE(Load32LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(0xDEADC0DE, memory_contents);

  registers.r0 = 0u;
  registers.r1 = 0u;
  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDR_DecrementPostIndexed) {
  auto registers = CreateArmGeneralPurposeRegisters();

  ASSERT_TRUE(Store32LE(nullptr, 8u, 0xDEADC0DE));
  registers.r1 = 8u;
  ArmLDR_DecrementPostIndexed(&registers, memory_, REGISTER_R0, REGISTER_R1,
                              4u);
  EXPECT_EQ(0xDEADC0DE, registers.r0);
  EXPECT_EQ(4u, registers.r1);

  uint32_t memory_contents;
  ASSERT_TRUE(Load32LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(0xDEADC0DE, memory_contents);

  registers.r0 = 0u;
  registers.r1 = 0u;
  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDR_IncrementPreIndexed) {
  auto registers = CreateArmGeneralPurposeRegisters();

  ASSERT_TRUE(Store32LE(nullptr, 8u, 0xDEADC0DE));
  registers.r1 = 4u;
  ArmLDR_IncrementPreIndexed(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(0xDEADC0DE, registers.r0);
  EXPECT_EQ(8u, registers.r1);

  uint32_t memory_contents;
  ASSERT_TRUE(Load32LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(0xDEADC0DE, memory_contents);

  registers.r0 = 0u;
  registers.r1 = 0u;
  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDR_IncrementPostIndexed) {
  auto registers = CreateArmGeneralPurposeRegisters();

  ASSERT_TRUE(Store32LE(nullptr, 8u, 0xDEADC0DE));
  registers.r1 = 8u;
  ArmLDR_IncrementPostIndexed(&registers, memory_, REGISTER_R0, REGISTER_R1,
                              4u);
  EXPECT_EQ(0xDEADC0DE, registers.r0);
  EXPECT_EQ(12u, registers.r1);

  uint32_t memory_contents;
  ASSERT_TRUE(Load32LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(0xDEADC0DE, memory_contents);

  registers.r0 = 0u;
  registers.r1 = 0u;
  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRB) {
  auto registers = CreateArmGeneralPurposeRegisters();

  ASSERT_TRUE(Store8(nullptr, 8u, 137u));
  registers.r0 = 8u;
  ArmLDRB(&registers, memory_, REGISTER_R0, 8u);
  EXPECT_EQ(137u, registers.r0);

  uint8_t memory_contents;
  ASSERT_TRUE(Load8(nullptr, 8u, &memory_contents));
  EXPECT_EQ(137u, memory_contents);

  registers.r0 = 0u;
  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRB_DecrementPreIndexed) {
  auto registers = CreateArmGeneralPurposeRegisters();

  ASSERT_TRUE(Store8(nullptr, 8u, 137u));
  registers.r1 = 12u;
  ArmLDRB_DecrementPreIndexed(&registers, memory_, REGISTER_R0, REGISTER_R1,
                              4u);
  EXPECT_EQ(137u, registers.r0);
  EXPECT_EQ(8u, registers.r1);

  uint8_t memory_contents;
  ASSERT_TRUE(Load8(nullptr, 8u, &memory_contents));
  EXPECT_EQ(137u, memory_contents);

  registers.r0 = 0u;
  registers.r1 = 0u;
  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRB_DecrementPostIndexed) {
  auto registers = CreateArmGeneralPurposeRegisters();

  ASSERT_TRUE(Store8(nullptr, 8u, 137u));
  registers.r1 = 8u;
  ArmLDRB_DecrementPostIndexed(&registers, memory_, REGISTER_R0, REGISTER_R1,
                               4u);
  EXPECT_EQ(137u, registers.r0);
  EXPECT_EQ(4u, registers.r1);

  uint8_t memory_contents;
  ASSERT_TRUE(Load8(nullptr, 8u, &memory_contents));
  EXPECT_EQ(137u, memory_contents);

  registers.r0 = 0u;
  registers.r1 = 0u;
  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRB_IncrementPreIndexed) {
  auto registers = CreateArmGeneralPurposeRegisters();

  ASSERT_TRUE(Store8(nullptr, 8u, 137u));
  registers.r1 = 4u;
  ArmLDRB_IncrementPreIndexed(&registers, memory_, REGISTER_R0, REGISTER_R1,
                              4u);
  EXPECT_EQ(137u, registers.r0);
  EXPECT_EQ(8u, registers.r1);

  uint8_t memory_contents;
  ASSERT_TRUE(Load8(nullptr, 8u, &memory_contents));
  EXPECT_EQ(137u, memory_contents);

  registers.r0 = 0u;
  registers.r1 = 0u;
  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRB_IncrementPostIndexed) {
  auto registers = CreateArmGeneralPurposeRegisters();

  ASSERT_TRUE(Store8(nullptr, 8u, 137u));
  registers.r1 = 8u;
  ArmLDRB_IncrementPostIndexed(&registers, memory_, REGISTER_R0, REGISTER_R1,
                               4u);
  EXPECT_EQ(137u, registers.r0);
  EXPECT_EQ(12u, registers.r1);

  uint8_t memory_contents;
  ASSERT_TRUE(Load8(nullptr, 8u, &memory_contents));
  EXPECT_EQ(137u, memory_contents);

  registers.r0 = 0u;
  registers.r1 = 0u;
  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmSTR) {
  auto registers = CreateArmGeneralPurposeRegisters();

  registers.r0 = 0xDEADC0DE;
  ArmSTR(&registers, memory_, REGISTER_R0, 8u);
  EXPECT_EQ(0xDEADC0DE, registers.r0);

  uint32_t memory_contents;
  ASSERT_TRUE(Load32LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(0xDEADC0DE, memory_contents);

  ASSERT_TRUE(Store32LE(nullptr, 8u, 0u));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTR_DecrementPreIndexed) {
  auto registers = CreateArmGeneralPurposeRegisters();

  registers.r0 = 0xDEADC0DE;
  registers.r1 = 12u;
  ArmSTR_DecrementPreIndexed(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(0xDEADC0DE, registers.r0);
  EXPECT_EQ(8u, registers.r1);

  uint32_t memory_contents;
  ASSERT_TRUE(Load32LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(0xDEADC0DE, memory_contents);

  ASSERT_TRUE(Store32LE(nullptr, 8u, 0u));
  registers.r0 = 0u;
  registers.r1 = 0u;

  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTR_DecrementPostIndexed) {
  auto registers = CreateArmGeneralPurposeRegisters();

  registers.r0 = 0xDEADC0DE;
  registers.r1 = 8u;
  ArmSTR_DecrementPostIndexed(&registers, memory_, REGISTER_R0, REGISTER_R1,
                              4u);
  EXPECT_EQ(0xDEADC0DE, registers.r0);
  EXPECT_EQ(4u, registers.r1);

  uint32_t memory_contents;
  ASSERT_TRUE(Load32LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(0xDEADC0DE, memory_contents);

  ASSERT_TRUE(Store32LE(nullptr, 8u, 0u));
  registers.r0 = 0u;
  registers.r1 = 0u;

  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTR_IncrementPreIndexed) {
  auto registers = CreateArmGeneralPurposeRegisters();

  registers.r0 = 0xDEADC0DE;
  registers.r1 = 4u;
  ArmSTR_IncrementPreIndexed(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(0xDEADC0DE, registers.r0);
  EXPECT_EQ(8u, registers.r1);

  uint32_t memory_contents;
  ASSERT_TRUE(Load32LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(0xDEADC0DE, memory_contents);

  ASSERT_TRUE(Store32LE(nullptr, 8u, 0u));
  registers.r0 = 0u;
  registers.r1 = 0u;

  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTR_IncrementPostIndexed) {
  auto registers = CreateArmGeneralPurposeRegisters();

  registers.r0 = 0xDEADC0DE;
  registers.r1 = 8u;
  ArmSTR_IncrementPostIndexed(&registers, memory_, REGISTER_R0, REGISTER_R1,
                              4u);
  EXPECT_EQ(0xDEADC0DE, registers.r0);
  EXPECT_EQ(12u, registers.r1);

  uint32_t memory_contents;
  ASSERT_TRUE(Load32LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(0xDEADC0DE, memory_contents);

  ASSERT_TRUE(Store32LE(nullptr, 8u, 0u));
  registers.r0 = 0u;
  registers.r1 = 0u;

  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRB) {
  auto registers = CreateArmGeneralPurposeRegisters();

  registers.r0 = 137u;
  ArmSTRB(&registers, memory_, REGISTER_R0, 8u);
  EXPECT_EQ(137u, registers.r0);

  uint8_t memory_contents;
  ASSERT_TRUE(Load8(nullptr, 8u, &memory_contents));
  EXPECT_EQ(137u, memory_contents);

  ASSERT_TRUE(Store8(nullptr, 8u, 0u));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRB_DecrementPreIndexed) {
  auto registers = CreateArmGeneralPurposeRegisters();

  registers.r0 = 137u;
  registers.r1 = 12u;
  ArmSTRB_DecrementPreIndexed(&registers, memory_, REGISTER_R0, REGISTER_R1,
                              4u);
  EXPECT_EQ(137u, registers.r0);
  EXPECT_EQ(8u, registers.r1);

  uint8_t memory_contents;
  ASSERT_TRUE(Load8(nullptr, 8u, &memory_contents));
  EXPECT_EQ(137u, memory_contents);

  ASSERT_TRUE(Store8(nullptr, 8u, 0u));
  registers.r0 = 0u;
  registers.r1 = 0u;

  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRB_DecrementPostIndexed) {
  auto registers = CreateArmGeneralPurposeRegisters();

  registers.r0 = 137u;
  registers.r1 = 8u;
  ArmSTRB_DecrementPostIndexed(&registers, memory_, REGISTER_R0, REGISTER_R1,
                               4u);
  EXPECT_EQ(137u, registers.r0);
  EXPECT_EQ(4u, registers.r1);

  uint8_t memory_contents;
  ASSERT_TRUE(Load8(nullptr, 8u, &memory_contents));
  EXPECT_EQ(137u, memory_contents);

  ASSERT_TRUE(Store8(nullptr, 8u, 0u));
  registers.r0 = 0u;
  registers.r1 = 0u;

  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRB_IncrementPreIndexed) {
  auto registers = CreateArmGeneralPurposeRegisters();

  registers.r0 = 137u;
  registers.r1 = 4u;
  ArmSTRB_IncrementPreIndexed(&registers, memory_, REGISTER_R0, REGISTER_R1,
                              4u);
  EXPECT_EQ(137u, registers.r0);
  EXPECT_EQ(8u, registers.r1);

  uint8_t memory_contents;
  ASSERT_TRUE(Load8(nullptr, 8u, &memory_contents));
  EXPECT_EQ(137u, memory_contents);

  ASSERT_TRUE(Store8(nullptr, 8u, 0u));
  registers.r0 = 0u;
  registers.r1 = 0u;

  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRB_IncrementPostIndexed) {
  auto registers = CreateArmGeneralPurposeRegisters();

  registers.r0 = 137u;
  registers.r1 = 8u;
  ArmSTRB_IncrementPostIndexed(&registers, memory_, REGISTER_R0, REGISTER_R1,
                               4u);
  EXPECT_EQ(137u, registers.r0);
  EXPECT_EQ(12u, registers.r1);

  uint8_t memory_contents;
  ASSERT_TRUE(Load8(nullptr, 8u, &memory_contents));
  EXPECT_EQ(137u, memory_contents);

  ASSERT_TRUE(Store8(nullptr, 8u, 0u));
  registers.r0 = 0u;
  registers.r1 = 0u;

  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}