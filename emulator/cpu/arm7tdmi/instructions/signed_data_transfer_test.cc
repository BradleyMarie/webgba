extern "C" {
#include "emulator/cpu/arm7tdmi/instructions/signed_data_transfer.h"
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

TEST_F(MemoryTest, ArmLDRH_IB) {
  auto registers = CreateArmGeneralPurposeRegisters();

  ASSERT_TRUE(Store16LE(nullptr, 8u, 137u));
  registers.r0 = 8u;
  ArmLDRH_IB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(137u, registers.r0);

  uint16_t memory_contents;
  ASSERT_TRUE(Load16LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(137u, memory_contents);

  registers.r0 = 0u;
  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRH_DB) {
  auto registers = CreateArmGeneralPurposeRegisters();

  ASSERT_TRUE(Store16LE(nullptr, 8u, 137u));
  registers.r0 = 8u;
  registers.r1 = 16u;
  ArmLDRH_DB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(137u, registers.r0);
  EXPECT_EQ(16u, registers.r1);

  uint16_t memory_contents;
  ASSERT_TRUE(Load16LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(137u, memory_contents);

  registers.r0 = 0u;
  registers.r1 = 0u;
  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRH_DBW) {
  auto registers = CreateArmGeneralPurposeRegisters();

  ASSERT_TRUE(Store16LE(nullptr, 8u, 137u));
  registers.r1 = 10u;
  ArmLDRH_DBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 2u);
  EXPECT_EQ(137u, registers.r0);
  EXPECT_EQ(8u, registers.r1);

  uint16_t memory_contents;
  ASSERT_TRUE(Load16LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(137u, memory_contents);

  registers.r0 = 0u;
  registers.r1 = 0u;
  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRH_DAW) {
  auto registers = CreateArmGeneralPurposeRegisters();

  ASSERT_TRUE(Store16LE(nullptr, 8u, 137u));
  registers.r1 = 8u;
  ArmLDRH_DAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 2u);
  EXPECT_EQ(137u, registers.r0);
  EXPECT_EQ(6u, registers.r1);

  uint16_t memory_contents;
  ASSERT_TRUE(Load16LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(137u, memory_contents);

  registers.r0 = 0u;
  registers.r1 = 0u;
  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRH_IBW) {
  auto registers = CreateArmGeneralPurposeRegisters();

  ASSERT_TRUE(Store16LE(nullptr, 8u, 137u));
  registers.r1 = 6u;
  ArmLDRH_IBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 2u);
  EXPECT_EQ(137u, registers.r0);
  EXPECT_EQ(8u, registers.r1);

  uint16_t memory_contents;
  ASSERT_TRUE(Load16LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(137u, memory_contents);

  registers.r0 = 0u;
  registers.r1 = 0u;
  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRH_IAW) {
  auto registers = CreateArmGeneralPurposeRegisters();

  ASSERT_TRUE(Store16LE(nullptr, 8u, 137u));
  registers.r1 = 8u;
  ArmLDRH_IAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 2u);
  EXPECT_EQ(137u, registers.r0);
  EXPECT_EQ(10u, registers.r1);

  uint16_t memory_contents;
  ASSERT_TRUE(Load16LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(137u, memory_contents);

  registers.r0 = 0u;
  registers.r1 = 0u;
  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRSB_IB) {
  auto registers = CreateArmGeneralPurposeRegisters();

  ASSERT_TRUE(Store8S(memory_, 8u, -65));
  registers.r0_s = 8u;
  ArmLDRSB_IB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(-65, registers.r0_s);

  int8_t memory_contents;
  ASSERT_TRUE(Load8S(memory_, 8u, &memory_contents));
  EXPECT_EQ(-65, memory_contents);

  registers.r0_s = 0u;
  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRSB_DB) {
  auto registers = CreateArmGeneralPurposeRegisters();

  ASSERT_TRUE(Store8S(memory_, 8u, -65));
  registers.r0_s = 8u;
  registers.r1 = 16u;
  ArmLDRSB_DB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(-65, registers.r0_s);
  EXPECT_EQ(16u, registers.r1);

  int8_t memory_contents;
  ASSERT_TRUE(Load8S(memory_, 8u, &memory_contents));
  EXPECT_EQ(-65, memory_contents);

  registers.r0_s = 0u;
  registers.r1 = 0u;
  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRSB_DBW) {
  auto registers = CreateArmGeneralPurposeRegisters();

  ASSERT_TRUE(Store8S(memory_, 8u, -65));
  registers.r1 = 10u;
  ArmLDRSB_DBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 2u);
  EXPECT_EQ(-65, registers.r0_s);
  EXPECT_EQ(8u, registers.r1);

  int8_t memory_contents;
  ASSERT_TRUE(Load8S(memory_, 8u, &memory_contents));
  EXPECT_EQ(-65, memory_contents);

  registers.r0_s = 0u;
  registers.r1 = 0u;
  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRSB_DAW) {
  auto registers = CreateArmGeneralPurposeRegisters();

  ASSERT_TRUE(Store8S(memory_, 8u, -65));
  registers.r1 = 8u;
  ArmLDRSB_DAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 2u);
  EXPECT_EQ(-65, registers.r0_s);
  EXPECT_EQ(6u, registers.r1);

  int8_t memory_contents;
  ASSERT_TRUE(Load8S(memory_, 8u, &memory_contents));
  EXPECT_EQ(-65, memory_contents);

  registers.r0_s = 0u;
  registers.r1 = 0u;
  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRSB_IBW) {
  auto registers = CreateArmGeneralPurposeRegisters();

  ASSERT_TRUE(Store8S(memory_, 8u, -65));
  registers.r1 = 6u;
  ArmLDRSB_IBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 2u);
  EXPECT_EQ(-65, registers.r0_s);
  EXPECT_EQ(8u, registers.r1);

  int8_t memory_contents;
  ASSERT_TRUE(Load8S(memory_, 8u, &memory_contents));
  EXPECT_EQ(-65, memory_contents);

  registers.r0_s = 0u;
  registers.r1 = 0u;
  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRSB_IAW) {
  auto registers = CreateArmGeneralPurposeRegisters();

  ASSERT_TRUE(Store8S(memory_, 8u, -65));
  registers.r1 = 8u;
  ArmLDRSB_IAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 2u);
  EXPECT_EQ(-65, registers.r0_s);
  EXPECT_EQ(10u, registers.r1);

  int8_t memory_contents;
  ASSERT_TRUE(Load8S(memory_, 8u, &memory_contents));
  EXPECT_EQ(-65, memory_contents);

  registers.r0_s = 0u;
  registers.r1 = 0u;
  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRSH_IB) {
  auto registers = CreateArmGeneralPurposeRegisters();

  ASSERT_TRUE(Store16SLE(memory_, 8u, -65));
  registers.r0_s = 8u;
  ArmLDRSH_IB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(-65, registers.r0_s);

  int16_t memory_contents;
  ASSERT_TRUE(Load16SLE(memory_, 8u, &memory_contents));
  EXPECT_EQ(-65, memory_contents);

  registers.r0_s = 0u;
  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRSH_DB) {
  auto registers = CreateArmGeneralPurposeRegisters();

  ASSERT_TRUE(Store16SLE(memory_, 8u, -65));
  registers.r0_s = 8u;
  registers.r1 = 16u;
  ArmLDRSH_DB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(-65, registers.r0_s);
  EXPECT_EQ(16u, registers.r1);

  int16_t memory_contents;
  ASSERT_TRUE(Load16SLE(memory_, 8u, &memory_contents));
  EXPECT_EQ(-65, memory_contents);

  registers.r0_s = 0u;
  registers.r1 = 0u;
  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRSH_DBW) {
  auto registers = CreateArmGeneralPurposeRegisters();

  ASSERT_TRUE(Store16SLE(memory_, 8u, -65));
  registers.r1 = 10u;
  ArmLDRSH_DBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 2u);
  EXPECT_EQ(-65, registers.r0_s);
  EXPECT_EQ(8u, registers.r1);

  int16_t memory_contents;
  ASSERT_TRUE(Load16SLE(memory_, 8u, &memory_contents));
  EXPECT_EQ(-65, memory_contents);

  registers.r0_s = 0u;
  registers.r1 = 0u;
  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRSH_DAW) {
  auto registers = CreateArmGeneralPurposeRegisters();

  ASSERT_TRUE(Store16SLE(memory_, 8u, -65));
  registers.r1 = 8u;
  ArmLDRSH_DAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 2u);
  EXPECT_EQ(-65, registers.r0_s);
  EXPECT_EQ(6u, registers.r1);

  int16_t memory_contents;
  ASSERT_TRUE(Load16SLE(memory_, 8u, &memory_contents));
  EXPECT_EQ(-65, memory_contents);

  registers.r0_s = 0u;
  registers.r1 = 0u;
  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRSH_IBW) {
  auto registers = CreateArmGeneralPurposeRegisters();

  ASSERT_TRUE(Store16SLE(memory_, 8u, -65));
  registers.r1 = 6u;
  ArmLDRSH_IBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 2u);
  EXPECT_EQ(-65, registers.r0_s);
  EXPECT_EQ(8u, registers.r1);

  int16_t memory_contents;
  ASSERT_TRUE(Load16SLE(memory_, 8u, &memory_contents));
  EXPECT_EQ(-65, memory_contents);

  registers.r0_s = 0u;
  registers.r1 = 0u;
  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmLDRSH_IAW) {
  auto registers = CreateArmGeneralPurposeRegisters();

  ASSERT_TRUE(Store16SLE(memory_, 8u, -65));
  registers.r1 = 8u;
  ArmLDRSH_IAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 2u);
  EXPECT_EQ(-65, registers.r0_s);
  EXPECT_EQ(10u, registers.r1);

  int16_t memory_contents;
  ASSERT_TRUE(Load16SLE(memory_, 8u, &memory_contents));
  EXPECT_EQ(-65, memory_contents);

  registers.r0_s = 0u;
  registers.r1 = 0u;
  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
}

TEST_F(MemoryTest, ArmSTRH_IB) {
  auto registers = CreateArmGeneralPurposeRegisters();

  registers.r0 = 137u;
  ArmSTRH_IB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(137u, registers.r0);

  uint16_t memory_contents;
  ASSERT_TRUE(Load16LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(137u, memory_contents);

  ASSERT_TRUE(Store16LE(nullptr, 8u, 0u));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRH_DB) {
  auto registers = CreateArmGeneralPurposeRegisters();

  registers.r0 = 137u;
  registers.r1 = 16u;
  ArmSTRH_DB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(137u, registers.r0);
  EXPECT_EQ(16u, registers.r1);

  uint16_t memory_contents;
  ASSERT_TRUE(Load16LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(137u, memory_contents);

  ASSERT_TRUE(Store16LE(nullptr, 8u, 0u));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRH_DBW) {
  auto registers = CreateArmGeneralPurposeRegisters();

  registers.r0 = 137u;
  registers.r1 = 12u;
  ArmSTRH_DBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(137u, registers.r0);
  EXPECT_EQ(8u, registers.r1);

  uint16_t memory_contents;
  ASSERT_TRUE(Load16LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(137u, memory_contents);

  ASSERT_TRUE(Store16LE(nullptr, 8u, 0u));
  registers.r0 = 0u;
  registers.r1 = 0u;

  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRH_DAW) {
  auto registers = CreateArmGeneralPurposeRegisters();

  registers.r0 = 137u;
  registers.r1 = 8u;
  ArmSTRH_DAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(137u, registers.r0);
  EXPECT_EQ(4u, registers.r1);

  uint16_t memory_contents;
  ASSERT_TRUE(Load16LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(137u, memory_contents);

  ASSERT_TRUE(Store16LE(nullptr, 8u, 0u));
  registers.r0 = 0u;
  registers.r1 = 0u;

  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRH_IBW) {
  auto registers = CreateArmGeneralPurposeRegisters();

  registers.r0 = 137u;
  registers.r1 = 4u;
  ArmSTRH_IBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(137u, registers.r0);
  EXPECT_EQ(8u, registers.r1);

  uint16_t memory_contents;
  ASSERT_TRUE(Load16LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(137u, memory_contents);

  ASSERT_TRUE(Store16LE(nullptr, 8u, 0u));
  registers.r0 = 0u;
  registers.r1 = 0u;

  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRH_IAW) {
  auto registers = CreateArmGeneralPurposeRegisters();

  registers.r0 = 137u;
  registers.r1 = 8u;
  ArmSTRH_IAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(137u, registers.r0);
  EXPECT_EQ(12u, registers.r1);

  uint16_t memory_contents;
  ASSERT_TRUE(Load16LE(nullptr, 8u, &memory_contents));
  EXPECT_EQ(137u, memory_contents);

  ASSERT_TRUE(Store16LE(nullptr, 8u, 0u));
  registers.r0 = 0u;
  registers.r1 = 0u;

  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRSB_IB) {
  auto registers = CreateArmGeneralPurposeRegisters();

  registers.r0_s = -65;
  ArmSTRSB_IB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(-65, registers.r0_s);

  int8_t memory_contents;
  ASSERT_TRUE(Load8S(memory_, 8u, &memory_contents));
  EXPECT_EQ(-65, memory_contents);

  ASSERT_TRUE(Store8S(memory_, 8u, 0u));
  registers.r0_s = 0u;

  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRSB_DB) {
  auto registers = CreateArmGeneralPurposeRegisters();

  registers.r0_s = -65;
  registers.r1 = 16u;
  ArmSTRSB_DB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(-65, registers.r0_s);
  EXPECT_EQ(16u, registers.r1);

  int8_t memory_contents;
  ASSERT_TRUE(Load8S(memory_, 8u, &memory_contents));
  EXPECT_EQ(-65, memory_contents);

  ASSERT_TRUE(Store8S(memory_, 8u, 0u));
  registers.r0_s = 0u;
  registers.r1 = 0u;

  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRSB_DBW) {
  auto registers = CreateArmGeneralPurposeRegisters();

  registers.r0_s = -65;
  registers.r1 = 12u;
  ArmSTRSB_DBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(-65, registers.r0_s);
  EXPECT_EQ(8u, registers.r1);

  int8_t memory_contents;
  ASSERT_TRUE(Load8S(memory_, 8u, &memory_contents));
  EXPECT_EQ(-65, memory_contents);

  ASSERT_TRUE(Store8S(memory_, 8u, 0u));
  registers.r0_s = 0u;
  registers.r1 = 0u;

  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRSB_DAW) {
  auto registers = CreateArmGeneralPurposeRegisters();

  registers.r0_s = -65;
  registers.r1 = 8u;
  ArmSTRSB_DAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(-65, registers.r0_s);
  EXPECT_EQ(4u, registers.r1);

  int8_t memory_contents;
  ASSERT_TRUE(Load8S(memory_, 8u, &memory_contents));
  EXPECT_EQ(-65, memory_contents);

  ASSERT_TRUE(Store8S(memory_, 8u, 0u));
  registers.r0_s = 0u;
  registers.r1 = 0u;

  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRSB_IBW) {
  auto registers = CreateArmGeneralPurposeRegisters();

  registers.r0_s = -65;
  registers.r1 = 4u;
  ArmSTRSB_IBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(-65, registers.r0_s);
  EXPECT_EQ(8u, registers.r1);

  int8_t memory_contents;
  ASSERT_TRUE(Load8S(memory_, 8u, &memory_contents));
  EXPECT_EQ(-65, memory_contents);

  ASSERT_TRUE(Store8S(memory_, 8u, 0u));
  registers.r0_s = 0u;
  registers.r1 = 0u;

  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRSB_IAW) {
  auto registers = CreateArmGeneralPurposeRegisters();

  registers.r0_s = -65;
  registers.r1 = 8u;
  ArmSTRSB_IAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(-65, registers.r0_s);
  EXPECT_EQ(12u, registers.r1);

  int8_t memory_contents;
  ASSERT_TRUE(Load8S(memory_, 8u, &memory_contents));
  EXPECT_EQ(-65, memory_contents);

  ASSERT_TRUE(Store8S(memory_, 8u, 0u));
  registers.r0_s = 0u;
  registers.r1 = 0u;

  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRSH_IB) {
  auto registers = CreateArmGeneralPurposeRegisters();

  registers.r0_s = -65;
  ArmSTRSH_IB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(-65, registers.r0_s);

  int16_t memory_contents;
  ASSERT_TRUE(Load16SLE(memory_, 8u, &memory_contents));
  EXPECT_EQ(-65, memory_contents);

  ASSERT_TRUE(Store16SLE(memory_, 8u, 0u));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRSH_DB) {
  auto registers = CreateArmGeneralPurposeRegisters();

  registers.r0_s = -65;
  registers.r1 = 16u;
  ArmSTRSH_DB(&registers, memory_, REGISTER_R0, REGISTER_R1, 8u);
  EXPECT_EQ(-65, registers.r0_s);
  EXPECT_EQ(16u, registers.r1);

  int16_t memory_contents;
  ASSERT_TRUE(Load16SLE(memory_, 8u, &memory_contents));
  EXPECT_EQ(-65, memory_contents);

  ASSERT_TRUE(Store16SLE(memory_, 8u, 0u));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRSH_DBW) {
  auto registers = CreateArmGeneralPurposeRegisters();

  registers.r0_s = -65;
  registers.r1 = 12u;
  ArmSTRSH_DBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(-65, registers.r0_s);
  EXPECT_EQ(8u, registers.r1);

  int16_t memory_contents;
  ASSERT_TRUE(Load16SLE(memory_, 8u, &memory_contents));
  EXPECT_EQ(-65, memory_contents);

  ASSERT_TRUE(Store16SLE(memory_, 8u, 0u));
  registers.r0_s = 0u;
  registers.r1 = 0u;

  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRSH_DAW) {
  auto registers = CreateArmGeneralPurposeRegisters();

  registers.r0_s = -65;
  registers.r1 = 8u;
  ArmSTRSH_DAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(-65, registers.r0_s);
  EXPECT_EQ(4u, registers.r1);

  int16_t memory_contents;
  ASSERT_TRUE(Load16SLE(memory_, 8u, &memory_contents));
  EXPECT_EQ(-65, memory_contents);

  ASSERT_TRUE(Store16SLE(memory_, 8u, 0u));
  registers.r0_s = 0u;
  registers.r1 = 0u;

  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRSH_IBW) {
  auto registers = CreateArmGeneralPurposeRegisters();

  registers.r0_s = -65;
  registers.r1 = 4u;
  ArmSTRSH_IBW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(-65, registers.r0_s);
  EXPECT_EQ(8u, registers.r1);

  int16_t memory_contents;
  ASSERT_TRUE(Load16SLE(memory_, 8u, &memory_contents));
  EXPECT_EQ(-65, memory_contents);

  ASSERT_TRUE(Store16SLE(memory_, 8u, 0u));
  registers.r0_s = 0u;
  registers.r1 = 0u;

  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}

TEST_F(MemoryTest, ArmSTRSH_IAW) {
  auto registers = CreateArmGeneralPurposeRegisters();

  registers.r0_s = -65;
  registers.r1 = 8u;
  ArmSTRSH_IAW(&registers, memory_, REGISTER_R0, REGISTER_R1, 4u);
  EXPECT_EQ(-65, registers.r0_s);
  EXPECT_EQ(12u, registers.r1);

  int16_t memory_contents;
  ASSERT_TRUE(Load16SLE(memory_, 8u, &memory_contents));
  EXPECT_EQ(-65, memory_contents);

  ASSERT_TRUE(Store16SLE(memory_, 8u, 0u));
  registers.r0_s = 0u;
  registers.r1 = 0u;

  EXPECT_TRUE(ArmGeneralPurposeRegistersAreZero(registers));
  EXPECT_TRUE(MemoryIsZero());
}