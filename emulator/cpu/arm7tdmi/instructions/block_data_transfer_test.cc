extern "C" {
#include "emulator/cpu/arm7tdmi/instructions/block_data_transfer.h"
}

#include <strings.h>

#include <limits>
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

ArmAllRegisters CreateArmAllRegisters() {
  ArmAllRegisters registers;
  memset(&registers, 0, sizeof(ArmAllRegisters));
  return registers;
}

bool ArmAllRegistersAreZero(const ArmAllRegisters &regs) {
  auto zero = CreateArmAllRegisters();
  return !memcmp(&zero, &regs, sizeof(ArmAllRegisters));
}

class MemoryTest : public testing::TestWithParam<uint16_t> {
 public:
  void SetUp() override {
    for (char &c : memory_space_) {
      c = 0;
    }
    memory_ = MemoryAllocate(nullptr, Load32LE, Load16LE, Load8, Store32LE,
                             Store16LE, Store8, nullptr);
    ASSERT_NE(nullptr, memory_);

    ASSERT_TRUE(Store32LE(memory_, 0x100u, 1u));
    ASSERT_TRUE(Store32LE(memory_, 0x104, 2u));
    ASSERT_TRUE(Store32LE(memory_, 0x108, 3u));
    ASSERT_TRUE(Store32LE(memory_, 0x10C, 4u));
    ASSERT_TRUE(Store32LE(memory_, 0x110, 5u));
    ASSERT_TRUE(Store32LE(memory_, 0x114, 6u));
    ASSERT_TRUE(Store32LE(memory_, 0x118, 7u));
    ASSERT_TRUE(Store32LE(memory_, 0x11C, 8u));
    ASSERT_TRUE(Store32LE(memory_, 0x120, 9u));
    ASSERT_TRUE(Store32LE(memory_, 0x124, 10u));
    ASSERT_TRUE(Store32LE(memory_, 0x128, 11u));
    ASSERT_TRUE(Store32LE(memory_, 0x12C, 12u));
    ASSERT_TRUE(Store32LE(memory_, 0x130, 13u));
    ASSERT_TRUE(Store32LE(memory_, 0x134, 14u));
    ASSERT_TRUE(Store32LE(memory_, 0x138, 15u));
    ASSERT_TRUE(Store32LE(memory_, 0x13C, 16u));
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

  void ValidateGeneralPurposeRegisterContentsDescending(
      const ArmGeneralPurposeRegisters &registers,
      ArmRegisterIndex address_register,
      uint32_t address_register_initial_value, uint_fast16_t register_list) {
    uint32_t expected_value = 16u;
    for (uint32_t i = 0u; i < 16u; i++) {
      uint32_t index = 15u - i;
      if (register_list & (1u << index)) {
        EXPECT_EQ(expected_value--, registers.gprs[index]);
      } else {
        if (index == address_register) {
          EXPECT_EQ(address_register_initial_value, registers.gprs[index]);
        } else {
          EXPECT_EQ(0u, registers.gprs[index]);
        }
      }
    }
  }

  void ValidateGeneralPurposeRegisterContentsAscending(
      const ArmGeneralPurposeRegisters &registers,
      ArmRegisterIndex address_register,
      uint32_t address_register_initial_value, uint_fast16_t register_list) {
    uint32_t expected_value = 1u;
    for (uint32_t i = 0u; i < 16u; i++) {
      if (register_list & (1u << i)) {
        EXPECT_EQ(expected_value++, registers.gprs[i]);
      } else {
        if (i == address_register) {
          EXPECT_EQ(address_register_initial_value, registers.gprs[i]);
        } else {
          EXPECT_EQ(0u, registers.gprs[i]);
        }
      }
    }
  }

  static std::vector<char> memory_space_;
  Memory *memory_;
};

std::vector<char> MemoryTest::memory_space_(384, 0);

TEST_P(MemoryTest, ArmLDMDA) {
  auto registers = CreateArmGeneralPurposeRegisters();

  registers.r0 = 0x13Cu;
  ArmLDMDA(&registers, memory_, REGISTER_R0, GetParam());

  ValidateGeneralPurposeRegisterContentsDescending(registers, REGISTER_R0,
                                                   0x13Cu, GetParam());
}

TEST_P(MemoryTest, ArmLDMDB) {
  auto registers = CreateArmGeneralPurposeRegisters();

  registers.r0 = 0x140u;
  ArmLDMDB(&registers, memory_, REGISTER_R0, GetParam());

  ValidateGeneralPurposeRegisterContentsDescending(registers, REGISTER_R0,
                                                   0x140u, GetParam());
}

TEST_P(MemoryTest, ArmLDMDAW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  auto registers = CreateArmGeneralPurposeRegisters();

  registers.r0 = 0x13Cu;
  ArmLDMDAW(&registers, memory_, REGISTER_R0, GetParam());
  uint32_t end_address = 0x13Cu - __builtin_popcount(GetParam()) * 4u;

  ValidateGeneralPurposeRegisterContentsDescending(registers, REGISTER_R0,
                                                   end_address, GetParam());
}

TEST_P(MemoryTest, ArmLDMDBW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  auto registers = CreateArmGeneralPurposeRegisters();

  registers.r0 = 0x140u;
  ArmLDMDBW(&registers, memory_, REGISTER_R0, GetParam());
  uint32_t end_address = 0x140u - __builtin_popcount(GetParam()) * 4u;

  ValidateGeneralPurposeRegisterContentsDescending(registers, REGISTER_R0,
                                                   end_address, GetParam());
}

TEST_P(MemoryTest, ArmLDMIA) {
  auto registers = CreateArmGeneralPurposeRegisters();

  registers.r0 = 0x100u;
  ArmLDMIA(&registers, memory_, REGISTER_R0, GetParam());

  ValidateGeneralPurposeRegisterContentsAscending(registers, REGISTER_R0,
                                                  0x100u, GetParam());
}

TEST_P(MemoryTest, ArmLDMIB) {
  auto registers = CreateArmGeneralPurposeRegisters();

  registers.r0 = 0xFCu;
  ArmLDMIB(&registers, memory_, REGISTER_R0, GetParam());

  ValidateGeneralPurposeRegisterContentsAscending(registers, REGISTER_R0, 0xFCu,
                                                  GetParam());
}

TEST_P(MemoryTest, ArmLDMIAW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  auto registers = CreateArmGeneralPurposeRegisters();

  registers.r0 = 0x100u;
  ArmLDMIAW(&registers, memory_, REGISTER_R0, GetParam());
  uint32_t end_address = 0x100u + __builtin_popcount(GetParam()) * 4u;

  ValidateGeneralPurposeRegisterContentsAscending(registers, REGISTER_R0,
                                                  end_address, GetParam());
}

TEST_P(MemoryTest, ArmLDMIBW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  auto registers = CreateArmGeneralPurposeRegisters();

  registers.r0 = 0xFCu;
  ArmLDMIBW(&registers, memory_, REGISTER_R0, GetParam());
  uint32_t end_address = 0xFCu + __builtin_popcount(GetParam()) * 4u;

  ValidateGeneralPurposeRegisterContentsAscending(registers, REGISTER_R0,
                                                  end_address, GetParam());
}

TEST_P(MemoryTest, ArmLDMSDA) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_USR;

  registers.current.user.gprs.r0 = 0x13Cu;
  ArmLDMSDA(&registers, memory_, REGISTER_R0, GetParam());

  ValidateGeneralPurposeRegisterContentsDescending(
      registers.current.user.gprs, REGISTER_R0, 0x13Cu, GetParam());
  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r2 = 0u;
  registers.current.user.gprs.r3 = 0u;
  registers.current.user.gprs.r4 = 0u;
  registers.current.user.gprs.r5 = 0u;
  registers.current.user.gprs.r6 = 0u;
  registers.current.user.gprs.r7 = 0u;
  registers.current.user.gprs.r8 = 0u;
  registers.current.user.gprs.r9 = 0u;
  registers.current.user.gprs.r10 = 0u;
  registers.current.user.gprs.r11 = 0u;
  registers.current.user.gprs.r12 = 0u;
  registers.current.user.gprs.r13 = 0u;
  registers.current.user.gprs.r14 = 0u;
  registers.current.user.gprs.r15 = 0u;
  ArmAllRegistersAreZero(registers);
}

TEST_P(MemoryTest, ArmLDMSDB) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_USR;

  registers.current.user.gprs.r0 = 0x140u;
  ArmLDMSDB(&registers, memory_, REGISTER_R0, GetParam());

  ValidateGeneralPurposeRegisterContentsDescending(
      registers.current.user.gprs, REGISTER_R0, 0x140u, GetParam());
  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r2 = 0u;
  registers.current.user.gprs.r3 = 0u;
  registers.current.user.gprs.r4 = 0u;
  registers.current.user.gprs.r5 = 0u;
  registers.current.user.gprs.r6 = 0u;
  registers.current.user.gprs.r7 = 0u;
  registers.current.user.gprs.r8 = 0u;
  registers.current.user.gprs.r9 = 0u;
  registers.current.user.gprs.r10 = 0u;
  registers.current.user.gprs.r11 = 0u;
  registers.current.user.gprs.r12 = 0u;
  registers.current.user.gprs.r13 = 0u;
  registers.current.user.gprs.r14 = 0u;
  registers.current.user.gprs.r15 = 0u;
  ArmAllRegistersAreZero(registers);
}

TEST_P(MemoryTest, ArmLDMSDAW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_USR;

  registers.current.user.gprs.r0 = 0x13Cu;
  ArmLDMSDAW(&registers, memory_, REGISTER_R0, GetParam());
  uint32_t end_address = 0x13Cu - __builtin_popcount(GetParam()) * 4u;

  ValidateGeneralPurposeRegisterContentsDescending(
      registers.current.user.gprs, REGISTER_R0, end_address, GetParam());
  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r2 = 0u;
  registers.current.user.gprs.r3 = 0u;
  registers.current.user.gprs.r4 = 0u;
  registers.current.user.gprs.r5 = 0u;
  registers.current.user.gprs.r6 = 0u;
  registers.current.user.gprs.r7 = 0u;
  registers.current.user.gprs.r8 = 0u;
  registers.current.user.gprs.r9 = 0u;
  registers.current.user.gprs.r10 = 0u;
  registers.current.user.gprs.r11 = 0u;
  registers.current.user.gprs.r12 = 0u;
  registers.current.user.gprs.r13 = 0u;
  registers.current.user.gprs.r14 = 0u;
  registers.current.user.gprs.r15 = 0u;
  ArmAllRegistersAreZero(registers);
}

TEST_P(MemoryTest, ArmLDMSDBW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_USR;

  registers.current.user.gprs.r0 = 0x140u;
  ArmLDMSDBW(&registers, memory_, REGISTER_R0, GetParam());
  uint32_t end_address = 0x140u - __builtin_popcount(GetParam()) * 4u;

  ValidateGeneralPurposeRegisterContentsDescending(
      registers.current.user.gprs, REGISTER_R0, end_address, GetParam());
  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r2 = 0u;
  registers.current.user.gprs.r3 = 0u;
  registers.current.user.gprs.r4 = 0u;
  registers.current.user.gprs.r5 = 0u;
  registers.current.user.gprs.r6 = 0u;
  registers.current.user.gprs.r7 = 0u;
  registers.current.user.gprs.r8 = 0u;
  registers.current.user.gprs.r9 = 0u;
  registers.current.user.gprs.r10 = 0u;
  registers.current.user.gprs.r11 = 0u;
  registers.current.user.gprs.r12 = 0u;
  registers.current.user.gprs.r13 = 0u;
  registers.current.user.gprs.r14 = 0u;
  registers.current.user.gprs.r15 = 0u;
  ArmAllRegistersAreZero(registers);
}

TEST_P(MemoryTest, ArmLDMSIA) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_USR;

  registers.current.user.gprs.r0 = 0x100u;
  ArmLDMSIA(&registers, memory_, REGISTER_R0, GetParam());

  ValidateGeneralPurposeRegisterContentsAscending(
      registers.current.user.gprs, REGISTER_R0, 0x100u, GetParam());
  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r2 = 0u;
  registers.current.user.gprs.r3 = 0u;
  registers.current.user.gprs.r4 = 0u;
  registers.current.user.gprs.r5 = 0u;
  registers.current.user.gprs.r6 = 0u;
  registers.current.user.gprs.r7 = 0u;
  registers.current.user.gprs.r8 = 0u;
  registers.current.user.gprs.r9 = 0u;
  registers.current.user.gprs.r10 = 0u;
  registers.current.user.gprs.r11 = 0u;
  registers.current.user.gprs.r12 = 0u;
  registers.current.user.gprs.r13 = 0u;
  registers.current.user.gprs.r14 = 0u;
  registers.current.user.gprs.r15 = 0u;
  ArmAllRegistersAreZero(registers);
}

TEST_P(MemoryTest, ArmLDMSIB) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_USR;

  registers.current.user.gprs.r0 = 0xFCu;
  ArmLDMSIB(&registers, memory_, REGISTER_R0, GetParam());

  ValidateGeneralPurposeRegisterContentsAscending(
      registers.current.user.gprs, REGISTER_R0, 0xFCu, GetParam());
  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r2 = 0u;
  registers.current.user.gprs.r3 = 0u;
  registers.current.user.gprs.r4 = 0u;
  registers.current.user.gprs.r5 = 0u;
  registers.current.user.gprs.r6 = 0u;
  registers.current.user.gprs.r7 = 0u;
  registers.current.user.gprs.r8 = 0u;
  registers.current.user.gprs.r9 = 0u;
  registers.current.user.gprs.r10 = 0u;
  registers.current.user.gprs.r11 = 0u;
  registers.current.user.gprs.r12 = 0u;
  registers.current.user.gprs.r13 = 0u;
  registers.current.user.gprs.r14 = 0u;
  registers.current.user.gprs.r15 = 0u;
  ArmAllRegistersAreZero(registers);
}

TEST_P(MemoryTest, ArmLDMSIAW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_USR;

  registers.current.user.gprs.r0 = 0x100u;
  ArmLDMSIAW(&registers, memory_, REGISTER_R0, GetParam());
  uint32_t end_address = 0x100u + __builtin_popcount(GetParam()) * 4u;

  ValidateGeneralPurposeRegisterContentsAscending(
      registers.current.user.gprs, REGISTER_R0, end_address, GetParam());
  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r2 = 0u;
  registers.current.user.gprs.r3 = 0u;
  registers.current.user.gprs.r4 = 0u;
  registers.current.user.gprs.r5 = 0u;
  registers.current.user.gprs.r6 = 0u;
  registers.current.user.gprs.r7 = 0u;
  registers.current.user.gprs.r8 = 0u;
  registers.current.user.gprs.r9 = 0u;
  registers.current.user.gprs.r10 = 0u;
  registers.current.user.gprs.r11 = 0u;
  registers.current.user.gprs.r12 = 0u;
  registers.current.user.gprs.r13 = 0u;
  registers.current.user.gprs.r14 = 0u;
  registers.current.user.gprs.r15 = 0u;
  ArmAllRegistersAreZero(registers);
}

TEST_P(MemoryTest, ArmLDMSIBW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_USR;

  registers.current.user.gprs.r0 = 0xFCu;
  ArmLDMSIBW(&registers, memory_, REGISTER_R0, GetParam());
  uint32_t end_address = 0xFCu + __builtin_popcount(GetParam()) * 4u;

  ValidateGeneralPurposeRegisterContentsAscending(
      registers.current.user.gprs, REGISTER_R0, end_address, GetParam());
  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r2 = 0u;
  registers.current.user.gprs.r3 = 0u;
  registers.current.user.gprs.r4 = 0u;
  registers.current.user.gprs.r5 = 0u;
  registers.current.user.gprs.r6 = 0u;
  registers.current.user.gprs.r7 = 0u;
  registers.current.user.gprs.r8 = 0u;
  registers.current.user.gprs.r9 = 0u;
  registers.current.user.gprs.r10 = 0u;
  registers.current.user.gprs.r11 = 0u;
  registers.current.user.gprs.r12 = 0u;
  registers.current.user.gprs.r13 = 0u;
  registers.current.user.gprs.r14 = 0u;
  registers.current.user.gprs.r15 = 0u;
  ArmAllRegistersAreZero(registers);
}

TEST_P(MemoryTest, SysArmLDMSDA) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SYS;

  registers.current.user.gprs.r0 = 0x13Cu;
  ArmLDMSDA(&registers, memory_, REGISTER_R0, GetParam());

  ValidateGeneralPurposeRegisterContentsDescending(
      registers.current.user.gprs, REGISTER_R0, 0x13Cu, GetParam());
  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r2 = 0u;
  registers.current.user.gprs.r3 = 0u;
  registers.current.user.gprs.r4 = 0u;
  registers.current.user.gprs.r5 = 0u;
  registers.current.user.gprs.r6 = 0u;
  registers.current.user.gprs.r7 = 0u;
  registers.current.user.gprs.r8 = 0u;
  registers.current.user.gprs.r9 = 0u;
  registers.current.user.gprs.r10 = 0u;
  registers.current.user.gprs.r11 = 0u;
  registers.current.user.gprs.r12 = 0u;
  registers.current.user.gprs.r13 = 0u;
  registers.current.user.gprs.r14 = 0u;
  registers.current.user.gprs.r15 = 0u;
  ArmAllRegistersAreZero(registers);
}

TEST_P(MemoryTest, SysArmLDMSDB) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SYS;

  registers.current.user.gprs.r0 = 0x140u;
  ArmLDMSDB(&registers, memory_, REGISTER_R0, GetParam());

  ValidateGeneralPurposeRegisterContentsDescending(
      registers.current.user.gprs, REGISTER_R0, 0x140u, GetParam());
  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r2 = 0u;
  registers.current.user.gprs.r3 = 0u;
  registers.current.user.gprs.r4 = 0u;
  registers.current.user.gprs.r5 = 0u;
  registers.current.user.gprs.r6 = 0u;
  registers.current.user.gprs.r7 = 0u;
  registers.current.user.gprs.r8 = 0u;
  registers.current.user.gprs.r9 = 0u;
  registers.current.user.gprs.r10 = 0u;
  registers.current.user.gprs.r11 = 0u;
  registers.current.user.gprs.r12 = 0u;
  registers.current.user.gprs.r13 = 0u;
  registers.current.user.gprs.r14 = 0u;
  registers.current.user.gprs.r15 = 0u;
  ArmAllRegistersAreZero(registers);
}

TEST_P(MemoryTest, SysArmLDMSDAW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SYS;

  registers.current.user.gprs.r0 = 0x13Cu;
  ArmLDMSDAW(&registers, memory_, REGISTER_R0, GetParam());
  uint32_t end_address = 0x13Cu - __builtin_popcount(GetParam()) * 4u;

  ValidateGeneralPurposeRegisterContentsDescending(
      registers.current.user.gprs, REGISTER_R0, end_address, GetParam());
  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r2 = 0u;
  registers.current.user.gprs.r3 = 0u;
  registers.current.user.gprs.r4 = 0u;
  registers.current.user.gprs.r5 = 0u;
  registers.current.user.gprs.r6 = 0u;
  registers.current.user.gprs.r7 = 0u;
  registers.current.user.gprs.r8 = 0u;
  registers.current.user.gprs.r9 = 0u;
  registers.current.user.gprs.r10 = 0u;
  registers.current.user.gprs.r11 = 0u;
  registers.current.user.gprs.r12 = 0u;
  registers.current.user.gprs.r13 = 0u;
  registers.current.user.gprs.r14 = 0u;
  registers.current.user.gprs.r15 = 0u;
  ArmAllRegistersAreZero(registers);
}

TEST_P(MemoryTest, SysArmLDMSDBW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SYS;

  registers.current.user.gprs.r0 = 0x140u;
  ArmLDMSDBW(&registers, memory_, REGISTER_R0, GetParam());
  uint32_t end_address = 0x140u - __builtin_popcount(GetParam()) * 4u;

  ValidateGeneralPurposeRegisterContentsDescending(
      registers.current.user.gprs, REGISTER_R0, end_address, GetParam());
  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r2 = 0u;
  registers.current.user.gprs.r3 = 0u;
  registers.current.user.gprs.r4 = 0u;
  registers.current.user.gprs.r5 = 0u;
  registers.current.user.gprs.r6 = 0u;
  registers.current.user.gprs.r7 = 0u;
  registers.current.user.gprs.r8 = 0u;
  registers.current.user.gprs.r9 = 0u;
  registers.current.user.gprs.r10 = 0u;
  registers.current.user.gprs.r11 = 0u;
  registers.current.user.gprs.r12 = 0u;
  registers.current.user.gprs.r13 = 0u;
  registers.current.user.gprs.r14 = 0u;
  registers.current.user.gprs.r15 = 0u;
  ArmAllRegistersAreZero(registers);
}

TEST_P(MemoryTest, SysArmLDMSIA) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SYS;

  registers.current.user.gprs.r0 = 0x100u;
  ArmLDMSIA(&registers, memory_, REGISTER_R0, GetParam());

  ValidateGeneralPurposeRegisterContentsAscending(
      registers.current.user.gprs, REGISTER_R0, 0x100u, GetParam());
  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r2 = 0u;
  registers.current.user.gprs.r3 = 0u;
  registers.current.user.gprs.r4 = 0u;
  registers.current.user.gprs.r5 = 0u;
  registers.current.user.gprs.r6 = 0u;
  registers.current.user.gprs.r7 = 0u;
  registers.current.user.gprs.r8 = 0u;
  registers.current.user.gprs.r9 = 0u;
  registers.current.user.gprs.r10 = 0u;
  registers.current.user.gprs.r11 = 0u;
  registers.current.user.gprs.r12 = 0u;
  registers.current.user.gprs.r13 = 0u;
  registers.current.user.gprs.r14 = 0u;
  registers.current.user.gprs.r15 = 0u;
  ArmAllRegistersAreZero(registers);
}

TEST_P(MemoryTest, SysArmLDMSIB) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SYS;

  registers.current.user.gprs.r0 = 0xFCu;
  ArmLDMSIB(&registers, memory_, REGISTER_R0, GetParam());

  ValidateGeneralPurposeRegisterContentsAscending(
      registers.current.user.gprs, REGISTER_R0, 0xFCu, GetParam());
  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r2 = 0u;
  registers.current.user.gprs.r3 = 0u;
  registers.current.user.gprs.r4 = 0u;
  registers.current.user.gprs.r5 = 0u;
  registers.current.user.gprs.r6 = 0u;
  registers.current.user.gprs.r7 = 0u;
  registers.current.user.gprs.r8 = 0u;
  registers.current.user.gprs.r9 = 0u;
  registers.current.user.gprs.r10 = 0u;
  registers.current.user.gprs.r11 = 0u;
  registers.current.user.gprs.r12 = 0u;
  registers.current.user.gprs.r13 = 0u;
  registers.current.user.gprs.r14 = 0u;
  registers.current.user.gprs.r15 = 0u;
  ArmAllRegistersAreZero(registers);
}

TEST_P(MemoryTest, SysArmLDMSIAW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SYS;

  registers.current.user.gprs.r0 = 0x100u;
  ArmLDMSIAW(&registers, memory_, REGISTER_R0, GetParam());
  uint32_t end_address = 0x100u + __builtin_popcount(GetParam()) * 4u;

  ValidateGeneralPurposeRegisterContentsAscending(
      registers.current.user.gprs, REGISTER_R0, end_address, GetParam());
  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r2 = 0u;
  registers.current.user.gprs.r3 = 0u;
  registers.current.user.gprs.r4 = 0u;
  registers.current.user.gprs.r5 = 0u;
  registers.current.user.gprs.r6 = 0u;
  registers.current.user.gprs.r7 = 0u;
  registers.current.user.gprs.r8 = 0u;
  registers.current.user.gprs.r9 = 0u;
  registers.current.user.gprs.r10 = 0u;
  registers.current.user.gprs.r11 = 0u;
  registers.current.user.gprs.r12 = 0u;
  registers.current.user.gprs.r13 = 0u;
  registers.current.user.gprs.r14 = 0u;
  registers.current.user.gprs.r15 = 0u;
  ArmAllRegistersAreZero(registers);
}

TEST_P(MemoryTest, SysArmLDMSIBW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SYS;

  registers.current.user.gprs.r0 = 0xFCu;
  ArmLDMSIBW(&registers, memory_, REGISTER_R0, GetParam());
  uint32_t end_address = 0xFCu + __builtin_popcount(GetParam()) * 4u;

  ValidateGeneralPurposeRegisterContentsAscending(
      registers.current.user.gprs, REGISTER_R0, end_address, GetParam());
  registers.current.user.gprs.r0 = 0u;
  registers.current.user.gprs.r1 = 0u;
  registers.current.user.gprs.r2 = 0u;
  registers.current.user.gprs.r3 = 0u;
  registers.current.user.gprs.r4 = 0u;
  registers.current.user.gprs.r5 = 0u;
  registers.current.user.gprs.r6 = 0u;
  registers.current.user.gprs.r7 = 0u;
  registers.current.user.gprs.r8 = 0u;
  registers.current.user.gprs.r9 = 0u;
  registers.current.user.gprs.r10 = 0u;
  registers.current.user.gprs.r11 = 0u;
  registers.current.user.gprs.r12 = 0u;
  registers.current.user.gprs.r13 = 0u;
  registers.current.user.gprs.r14 = 0u;
  registers.current.user.gprs.r15 = 0u;
  ArmAllRegistersAreZero(registers);
}

TEST_P(MemoryTest, SvcArmLDMSDA) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;
  registers.current.spsr.mode = MODE_USR;

  registers.current.user.gprs.r0 = 0x13Cu;
  ArmLDMSDA(&registers, memory_, REGISTER_R0, GetParam());
  if (GetParam() & (1u << REGISTER_R15)) {
    EXPECT_EQ(MODE_USR, registers.current.user.cpsr.mode);
    ArmProgramStatusRegister new_status = registers.current.user.cpsr;
    new_status.mode = MODE_SVC;
    ArmLoadCPSR(&registers, new_status);
  } else {
    EXPECT_EQ(MODE_SVC, registers.current.user.cpsr.mode);
    EXPECT_EQ(0u, registers.current.user.gprs.r13);
    EXPECT_EQ(0u, registers.current.user.gprs.r14);
    ArmProgramStatusRegister new_status = registers.current.user.cpsr;
    new_status.mode = MODE_USR;
    ArmLoadCPSR(&registers, new_status);
  }

  ValidateGeneralPurposeRegisterContentsDescending(
      registers.current.user.gprs, REGISTER_R0, 0x13Cu, GetParam());
}

TEST_P(MemoryTest, SvcArmLDMSDB) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;
  registers.current.spsr.mode = MODE_USR;

  registers.current.user.gprs.r0 = 0x140u;
  ArmLDMSDB(&registers, memory_, REGISTER_R0, GetParam());
  if (GetParam() & (1u << REGISTER_R15)) {
    EXPECT_EQ(MODE_USR, registers.current.user.cpsr.mode);
    ArmProgramStatusRegister new_status = registers.current.user.cpsr;
    new_status.mode = MODE_SVC;
    ArmLoadCPSR(&registers, new_status);
  } else {
    EXPECT_EQ(MODE_SVC, registers.current.user.cpsr.mode);
    EXPECT_EQ(0u, registers.current.user.gprs.r13);
    EXPECT_EQ(0u, registers.current.user.gprs.r14);
    ArmProgramStatusRegister new_status = registers.current.user.cpsr;
    new_status.mode = MODE_USR;
    ArmLoadCPSR(&registers, new_status);
  }

  ValidateGeneralPurposeRegisterContentsDescending(
      registers.current.user.gprs, REGISTER_R0, 0x140u, GetParam());
}

TEST_P(MemoryTest, SvcArmLDMSDAW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;
  registers.current.spsr.mode = MODE_USR;

  registers.current.user.gprs.r0 = 0x13Cu;
  ArmLDMSDAW(&registers, memory_, REGISTER_R0, GetParam());
  if (GetParam() & (1u << REGISTER_R15)) {
    EXPECT_EQ(MODE_USR, registers.current.user.cpsr.mode);
    ArmProgramStatusRegister new_status = registers.current.user.cpsr;
    new_status.mode = MODE_SVC;
    ArmLoadCPSR(&registers, new_status);
  } else {
    EXPECT_EQ(MODE_SVC, registers.current.user.cpsr.mode);
    EXPECT_EQ(0u, registers.current.user.gprs.r13);
    EXPECT_EQ(0u, registers.current.user.gprs.r14);
    ArmProgramStatusRegister new_status = registers.current.user.cpsr;
    new_status.mode = MODE_USR;
    ArmLoadCPSR(&registers, new_status);
  }

  uint32_t end_address = 0x13Cu - __builtin_popcount(GetParam()) * 4u;
  ValidateGeneralPurposeRegisterContentsDescending(
      registers.current.user.gprs, REGISTER_R0, end_address, GetParam());
}

TEST_P(MemoryTest, SvcArmLDMSDBW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;
  registers.current.spsr.mode = MODE_USR;

  registers.current.user.gprs.r0 = 0x140u;
  ArmLDMSDBW(&registers, memory_, REGISTER_R0, GetParam());
  if (GetParam() & (1u << REGISTER_R15)) {
    EXPECT_EQ(MODE_USR, registers.current.user.cpsr.mode);
    ArmProgramStatusRegister new_status = registers.current.user.cpsr;
    new_status.mode = MODE_SVC;
    ArmLoadCPSR(&registers, new_status);
  } else {
    EXPECT_EQ(MODE_SVC, registers.current.user.cpsr.mode);
    EXPECT_EQ(0u, registers.current.user.gprs.r13);
    EXPECT_EQ(0u, registers.current.user.gprs.r14);
    ArmProgramStatusRegister new_status = registers.current.user.cpsr;
    new_status.mode = MODE_USR;
    ArmLoadCPSR(&registers, new_status);
  }

  uint32_t end_address = 0x140u - __builtin_popcount(GetParam()) * 4u;
  ValidateGeneralPurposeRegisterContentsDescending(
      registers.current.user.gprs, REGISTER_R0, end_address, GetParam());
}

TEST_P(MemoryTest, SvcArmLDMSIA) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;
  registers.current.spsr.mode = MODE_USR;

  registers.current.user.gprs.r0 = 0x100u;
  ArmLDMSIA(&registers, memory_, REGISTER_R0, GetParam());
  if (GetParam() & (1u << REGISTER_R15)) {
    EXPECT_EQ(MODE_USR, registers.current.user.cpsr.mode);
    ArmProgramStatusRegister new_status = registers.current.user.cpsr;
    new_status.mode = MODE_SVC;
    ArmLoadCPSR(&registers, new_status);
  } else {
    EXPECT_EQ(MODE_SVC, registers.current.user.cpsr.mode);
    EXPECT_EQ(0u, registers.current.user.gprs.r13);
    EXPECT_EQ(0u, registers.current.user.gprs.r14);
    ArmProgramStatusRegister new_status = registers.current.user.cpsr;
    new_status.mode = MODE_USR;
    ArmLoadCPSR(&registers, new_status);
  }

  ValidateGeneralPurposeRegisterContentsAscending(
      registers.current.user.gprs, REGISTER_R0, 0x100u, GetParam());
}

TEST_P(MemoryTest, SvcArmLDMSIB) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;
  registers.current.spsr.mode = MODE_USR;

  registers.current.user.gprs.r0 = 0xFCu;
  ArmLDMSIB(&registers, memory_, REGISTER_R0, GetParam());
  if (GetParam() & (1u << REGISTER_R15)) {
    EXPECT_EQ(MODE_USR, registers.current.user.cpsr.mode);
    ArmProgramStatusRegister new_status = registers.current.user.cpsr;
    new_status.mode = MODE_SVC;
    ArmLoadCPSR(&registers, new_status);
  } else {
    EXPECT_EQ(MODE_SVC, registers.current.user.cpsr.mode);
    EXPECT_EQ(0u, registers.current.user.gprs.r13);
    EXPECT_EQ(0u, registers.current.user.gprs.r14);
    ArmProgramStatusRegister new_status = registers.current.user.cpsr;
    new_status.mode = MODE_USR;
    ArmLoadCPSR(&registers, new_status);
  }

  ValidateGeneralPurposeRegisterContentsAscending(
      registers.current.user.gprs, REGISTER_R0, 0xFCu, GetParam());
}

TEST_P(MemoryTest, SvcArmLDMSIAW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;
  registers.current.spsr.mode = MODE_USR;

  registers.current.user.gprs.r0 = 0x100u;
  ArmLDMSIAW(&registers, memory_, REGISTER_R0, GetParam());
  if (GetParam() & (1u << REGISTER_R15)) {
    EXPECT_EQ(MODE_USR, registers.current.user.cpsr.mode);
    ArmProgramStatusRegister new_status = registers.current.user.cpsr;
    new_status.mode = MODE_SVC;
    ArmLoadCPSR(&registers, new_status);
  } else {
    EXPECT_EQ(MODE_SVC, registers.current.user.cpsr.mode);
    EXPECT_EQ(0u, registers.current.user.gprs.r13);
    EXPECT_EQ(0u, registers.current.user.gprs.r14);
    ArmProgramStatusRegister new_status = registers.current.user.cpsr;
    new_status.mode = MODE_USR;
    ArmLoadCPSR(&registers, new_status);
  }

  uint32_t end_address = 0x100u + __builtin_popcount(GetParam()) * 4u;
  ValidateGeneralPurposeRegisterContentsAscending(
      registers.current.user.gprs, REGISTER_R0, end_address, GetParam());
}

TEST_P(MemoryTest, SvcArmLDMSIBW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;
  registers.current.spsr.mode = MODE_USR;

  registers.current.user.gprs.r0 = 0xFCu;
  ArmLDMSIBW(&registers, memory_, REGISTER_R0, GetParam());
  if (GetParam() & (1u << REGISTER_R15)) {
    EXPECT_EQ(MODE_USR, registers.current.user.cpsr.mode);
    ArmProgramStatusRegister new_status = registers.current.user.cpsr;
    new_status.mode = MODE_SVC;
    ArmLoadCPSR(&registers, new_status);
  } else {
    EXPECT_EQ(MODE_SVC, registers.current.user.cpsr.mode);
    EXPECT_EQ(0u, registers.current.user.gprs.r13);
    EXPECT_EQ(0u, registers.current.user.gprs.r14);
    ArmProgramStatusRegister new_status = registers.current.user.cpsr;
    new_status.mode = MODE_USR;
    ArmLoadCPSR(&registers, new_status);
  }

  uint32_t end_address = 0xFCu + __builtin_popcount(GetParam()) * 4u;
  ValidateGeneralPurposeRegisterContentsAscending(
      registers.current.user.gprs, REGISTER_R0, end_address, GetParam());
}

INSTANTIATE_TEST_SUITE_P(ArmBlockDataTransferTestModule, MemoryTest,
                         testing::Range(std::numeric_limits<uint16_t>::min(),
                                        std::numeric_limits<uint16_t>::max()));
//(uint16_t)33040u));