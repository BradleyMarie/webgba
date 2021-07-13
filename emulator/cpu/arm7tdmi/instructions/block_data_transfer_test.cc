extern "C" {
#include "emulator/cpu/arm7tdmi/instructions/block_data_transfer.h"
}

#include <cstring>
#include <limits>
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

class LdmTest : public testing::TestWithParam<uint16_t> {
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
      const ArmAllRegisters &registers, ArmRegisterIndex address_register,
      uint32_t address_register_initial_value, uint_fast16_t register_list) {
    uint32_t expected_value = 16u;
    for (uint32_t i = 0u; i < 16u; i++) {
      uint32_t index = 15u - i;
      if (register_list & (1u << index)) {
        EXPECT_EQ(expected_value--, registers.current.user.gprs.gprs[index]);
      } else {
        if (index == address_register) {
          EXPECT_EQ(address_register_initial_value,
                    registers.current.user.gprs.gprs[index]);
        } else {
          EXPECT_EQ(0u, registers.current.user.gprs.gprs[index]);
        }
      }
    }
  }

  void ValidateGeneralPurposeRegisterContentsAscending(
      const ArmAllRegisters &registers, ArmRegisterIndex address_register,
      uint32_t address_register_initial_value, uint_fast16_t register_list) {
    uint32_t expected_value = 1u;
    for (uint32_t i = 0u; i < 16u; i++) {
      if (register_list & (1u << i)) {
        EXPECT_EQ(expected_value++, registers.current.user.gprs.gprs[i]);
      } else {
        if (i == address_register) {
          EXPECT_EQ(address_register_initial_value,
                    registers.current.user.gprs.gprs[i]);
        } else {
          EXPECT_EQ(0u, registers.current.user.gprs.gprs[i]);
        }
      }
    }
  }

  static std::vector<char> memory_space_;
  Memory *memory_;
};

std::vector<char> LdmTest::memory_space_(384, 0);

TEST_P(LdmTest, ArmLDMDA) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 0x13Cu;
  EXPECT_TRUE(ArmLDMDA(&registers, memory_, REGISTER_R0, GetParam()));

  ValidateGeneralPurposeRegisterContentsDescending(registers, REGISTER_R0,
                                                   0x13Cu, GetParam());
}

TEST_P(LdmTest, ArmLDMDB) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 0x140u;
  EXPECT_TRUE(ArmLDMDB(&registers, memory_, REGISTER_R0, GetParam()));

  ValidateGeneralPurposeRegisterContentsDescending(registers, REGISTER_R0,
                                                   0x140u, GetParam());
}

TEST_P(LdmTest, ArmLDMDAW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 0x13Cu;
  EXPECT_TRUE(ArmLDMDAW(&registers, memory_, REGISTER_R0, GetParam()));
  uint32_t end_address = 0x13Cu - __builtin_popcount(GetParam()) * 4u;

  ValidateGeneralPurposeRegisterContentsDescending(registers, REGISTER_R0,
                                                   end_address, GetParam());
}

TEST_P(LdmTest, ArmLDMDBW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 0x140u;
  EXPECT_TRUE(ArmLDMDBW(&registers, memory_, REGISTER_R0, GetParam()));
  uint32_t end_address = 0x140u - __builtin_popcount(GetParam()) * 4u;

  ValidateGeneralPurposeRegisterContentsDescending(registers, REGISTER_R0,
                                                   end_address, GetParam());
}

TEST_P(LdmTest, ArmLDMIA) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 0x100u;
  EXPECT_TRUE(ArmLDMIA(&registers, memory_, REGISTER_R0, GetParam()));

  ValidateGeneralPurposeRegisterContentsAscending(registers, REGISTER_R0,
                                                  0x100u, GetParam());
}

TEST_P(LdmTest, ArmLDMIB) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 0xFCu;
  EXPECT_TRUE(ArmLDMIB(&registers, memory_, REGISTER_R0, GetParam()));

  ValidateGeneralPurposeRegisterContentsAscending(registers, REGISTER_R0, 0xFCu,
                                                  GetParam());
}

TEST_P(LdmTest, ArmLDMIAW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 0x100u;
  EXPECT_TRUE(ArmLDMIAW(&registers, memory_, REGISTER_R0, GetParam()));
  uint32_t end_address = 0x100u + __builtin_popcount(GetParam()) * 4u;

  ValidateGeneralPurposeRegisterContentsAscending(registers, REGISTER_R0,
                                                  end_address, GetParam());
}

TEST_P(LdmTest, ArmLDMIBW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 0xFCu;
  EXPECT_TRUE(ArmLDMIBW(&registers, memory_, REGISTER_R0, GetParam()));
  uint32_t end_address = 0xFCu + __builtin_popcount(GetParam()) * 4u;

  ValidateGeneralPurposeRegisterContentsAscending(registers, REGISTER_R0,
                                                  end_address, GetParam());
}

TEST_P(LdmTest, ArmLDMSDA) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_USR;

  registers.current.user.gprs.r0 = 0x13Cu;
  EXPECT_TRUE(ArmLDMSDA(&registers, memory_, REGISTER_R0, GetParam()));

  ValidateGeneralPurposeRegisterContentsDescending(registers, REGISTER_R0,
                                                   0x13Cu, GetParam());
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

TEST_P(LdmTest, ArmLDMSDB) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_USR;

  registers.current.user.gprs.r0 = 0x140u;
  EXPECT_TRUE(ArmLDMSDB(&registers, memory_, REGISTER_R0, GetParam()));

  ValidateGeneralPurposeRegisterContentsDescending(registers, REGISTER_R0,
                                                   0x140u, GetParam());
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

TEST_P(LdmTest, ArmLDMSDAW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_USR;

  registers.current.user.gprs.r0 = 0x13Cu;
  EXPECT_TRUE(ArmLDMSDAW(&registers, memory_, REGISTER_R0, GetParam()));
  uint32_t end_address = 0x13Cu - __builtin_popcount(GetParam()) * 4u;

  ValidateGeneralPurposeRegisterContentsDescending(registers, REGISTER_R0,
                                                   end_address, GetParam());
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

TEST_P(LdmTest, ArmLDMSDBW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_USR;

  registers.current.user.gprs.r0 = 0x140u;
  EXPECT_TRUE(ArmLDMSDBW(&registers, memory_, REGISTER_R0, GetParam()));
  uint32_t end_address = 0x140u - __builtin_popcount(GetParam()) * 4u;

  ValidateGeneralPurposeRegisterContentsDescending(registers, REGISTER_R0,
                                                   end_address, GetParam());
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

TEST_P(LdmTest, ArmLDMSIA) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_USR;

  registers.current.user.gprs.r0 = 0x100u;
  EXPECT_TRUE(ArmLDMSIA(&registers, memory_, REGISTER_R0, GetParam()));

  ValidateGeneralPurposeRegisterContentsAscending(registers, REGISTER_R0,
                                                  0x100u, GetParam());
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

TEST_P(LdmTest, ArmLDMSIB) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_USR;

  registers.current.user.gprs.r0 = 0xFCu;
  EXPECT_TRUE(ArmLDMSIB(&registers, memory_, REGISTER_R0, GetParam()));

  ValidateGeneralPurposeRegisterContentsAscending(registers, REGISTER_R0, 0xFCu,
                                                  GetParam());
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

TEST_P(LdmTest, ArmLDMSIAW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_USR;

  registers.current.user.gprs.r0 = 0x100u;
  EXPECT_TRUE(ArmLDMSIAW(&registers, memory_, REGISTER_R0, GetParam()));
  uint32_t end_address = 0x100u + __builtin_popcount(GetParam()) * 4u;

  ValidateGeneralPurposeRegisterContentsAscending(registers, REGISTER_R0,
                                                  end_address, GetParam());
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

TEST_P(LdmTest, ArmLDMSIBW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_USR;

  registers.current.user.gprs.r0 = 0xFCu;
  EXPECT_TRUE(ArmLDMSIBW(&registers, memory_, REGISTER_R0, GetParam()));
  uint32_t end_address = 0xFCu + __builtin_popcount(GetParam()) * 4u;

  ValidateGeneralPurposeRegisterContentsAscending(registers, REGISTER_R0,
                                                  end_address, GetParam());
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

TEST_P(LdmTest, SysArmLDMSDA) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SYS;

  registers.current.user.gprs.r0 = 0x13Cu;
  EXPECT_TRUE(ArmLDMSDA(&registers, memory_, REGISTER_R0, GetParam()));

  ValidateGeneralPurposeRegisterContentsDescending(registers, REGISTER_R0,
                                                   0x13Cu, GetParam());
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

TEST_P(LdmTest, SysArmLDMSDB) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SYS;

  registers.current.user.gprs.r0 = 0x140u;
  EXPECT_TRUE(ArmLDMSDB(&registers, memory_, REGISTER_R0, GetParam()));

  ValidateGeneralPurposeRegisterContentsDescending(registers, REGISTER_R0,
                                                   0x140u, GetParam());
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

TEST_P(LdmTest, SysArmLDMSDAW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SYS;

  registers.current.user.gprs.r0 = 0x13Cu;
  EXPECT_TRUE(ArmLDMSDAW(&registers, memory_, REGISTER_R0, GetParam()));
  uint32_t end_address = 0x13Cu - __builtin_popcount(GetParam()) * 4u;

  ValidateGeneralPurposeRegisterContentsDescending(registers, REGISTER_R0,
                                                   end_address, GetParam());
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

TEST_P(LdmTest, SysArmLDMSDBW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SYS;

  registers.current.user.gprs.r0 = 0x140u;
  EXPECT_TRUE(ArmLDMSDBW(&registers, memory_, REGISTER_R0, GetParam()));
  uint32_t end_address = 0x140u - __builtin_popcount(GetParam()) * 4u;

  ValidateGeneralPurposeRegisterContentsDescending(registers, REGISTER_R0,
                                                   end_address, GetParam());
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

TEST_P(LdmTest, SysArmLDMSIA) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SYS;

  registers.current.user.gprs.r0 = 0x100u;
  EXPECT_TRUE(ArmLDMSIA(&registers, memory_, REGISTER_R0, GetParam()));

  ValidateGeneralPurposeRegisterContentsAscending(registers, REGISTER_R0,
                                                  0x100u, GetParam());
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

TEST_P(LdmTest, SysArmLDMSIB) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SYS;

  registers.current.user.gprs.r0 = 0xFCu;
  EXPECT_TRUE(ArmLDMSIB(&registers, memory_, REGISTER_R0, GetParam()));

  ValidateGeneralPurposeRegisterContentsAscending(registers, REGISTER_R0, 0xFCu,
                                                  GetParam());
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

TEST_P(LdmTest, SysArmLDMSIAW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SYS;

  registers.current.user.gprs.r0 = 0x100u;
  EXPECT_TRUE(ArmLDMSIAW(&registers, memory_, REGISTER_R0, GetParam()));
  uint32_t end_address = 0x100u + __builtin_popcount(GetParam()) * 4u;

  ValidateGeneralPurposeRegisterContentsAscending(registers, REGISTER_R0,
                                                  end_address, GetParam());
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

TEST_P(LdmTest, SysArmLDMSIBW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SYS;

  registers.current.user.gprs.r0 = 0xFCu;
  EXPECT_TRUE(ArmLDMSIBW(&registers, memory_, REGISTER_R0, GetParam()));
  uint32_t end_address = 0xFCu + __builtin_popcount(GetParam()) * 4u;

  ValidateGeneralPurposeRegisterContentsAscending(registers, REGISTER_R0,
                                                  end_address, GetParam());
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

TEST_P(LdmTest, SvcArmLDMSDA) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;
  registers.current.spsr.mode = MODE_USR;

  registers.current.user.gprs.r0 = 0x13Cu;
  EXPECT_TRUE(ArmLDMSDA(&registers, memory_, REGISTER_R0, GetParam()));
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

  ValidateGeneralPurposeRegisterContentsDescending(registers, REGISTER_R0,
                                                   0x13Cu, GetParam());
}

TEST_P(LdmTest, SvcArmLDMSDB) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;
  registers.current.spsr.mode = MODE_USR;

  registers.current.user.gprs.r0 = 0x140u;
  EXPECT_TRUE(ArmLDMSDB(&registers, memory_, REGISTER_R0, GetParam()));
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

  ValidateGeneralPurposeRegisterContentsDescending(registers, REGISTER_R0,
                                                   0x140u, GetParam());
}

TEST_P(LdmTest, SvcArmLDMSDAW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;
  registers.current.spsr.mode = MODE_USR;

  registers.current.user.gprs.r0 = 0x13Cu;
  EXPECT_TRUE(ArmLDMSDAW(&registers, memory_, REGISTER_R0, GetParam()));
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
  ValidateGeneralPurposeRegisterContentsDescending(registers, REGISTER_R0,
                                                   end_address, GetParam());
}

TEST_P(LdmTest, SvcArmLDMSDBW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;
  registers.current.spsr.mode = MODE_USR;

  registers.current.user.gprs.r0 = 0x140u;
  EXPECT_TRUE(ArmLDMSDBW(&registers, memory_, REGISTER_R0, GetParam()));
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
  ValidateGeneralPurposeRegisterContentsDescending(registers, REGISTER_R0,
                                                   end_address, GetParam());
}

TEST_P(LdmTest, SvcArmLDMSIA) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;
  registers.current.spsr.mode = MODE_USR;

  registers.current.user.gprs.r0 = 0x100u;
  EXPECT_TRUE(ArmLDMSIA(&registers, memory_, REGISTER_R0, GetParam()));
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

  ValidateGeneralPurposeRegisterContentsAscending(registers, REGISTER_R0,
                                                  0x100u, GetParam());
}

TEST_P(LdmTest, SvcArmLDMSIB) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;
  registers.current.spsr.mode = MODE_USR;

  registers.current.user.gprs.r0 = 0xFCu;
  EXPECT_TRUE(ArmLDMSIB(&registers, memory_, REGISTER_R0, GetParam()));
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

  ValidateGeneralPurposeRegisterContentsAscending(registers, REGISTER_R0, 0xFCu,
                                                  GetParam());
}

TEST_P(LdmTest, SvcArmLDMSIAW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;
  registers.current.spsr.mode = MODE_USR;

  registers.current.user.gprs.r0 = 0x100u;
  EXPECT_TRUE(ArmLDMSIAW(&registers, memory_, REGISTER_R0, GetParam()));
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
  ValidateGeneralPurposeRegisterContentsAscending(registers, REGISTER_R0,
                                                  end_address, GetParam());
}

TEST_P(LdmTest, SvcArmLDMSIBW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SVC;
  registers.current.spsr.mode = MODE_USR;

  registers.current.user.gprs.r0 = 0xFCu;
  EXPECT_TRUE(ArmLDMSIBW(&registers, memory_, REGISTER_R0, GetParam()));
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
  ValidateGeneralPurposeRegisterContentsAscending(registers, REGISTER_R0,
                                                  end_address, GetParam());
}

INSTANTIATE_TEST_SUITE_P(ArmLdmTestModule, LdmTest,
                         testing::Range(std::numeric_limits<uint16_t>::min(),
                                        std::numeric_limits<uint16_t>::max(),
                                        51u));

class StmTest : public testing::TestWithParam<uint16_t> {
 public:
  void SetUp() override {
    for (char &c : memory_space_) {
      c = 0;
    }
    memory_ = MemoryAllocate(nullptr, Load32LE, Load16LE, Load8, Store32LE,
                             Store16LE, Store8, nullptr);
    ASSERT_NE(nullptr, memory_);

    registers_.current.user.gprs.r0 = 1u;
    registers_.current.user.gprs.r1 = 2u;
    registers_.current.user.gprs.r2 = 3u;
    registers_.current.user.gprs.r3 = 4u;
    registers_.current.user.gprs.r4 = 5u;
    registers_.current.user.gprs.r5 = 6u;
    registers_.current.user.gprs.r6 = 7u;
    registers_.current.user.gprs.r7 = 8u;
    registers_.current.user.gprs.r8 = 9u;
    registers_.current.user.gprs.r9 = 10u;
    registers_.current.user.gprs.r10 = 11u;
    registers_.current.user.gprs.r11 = 12u;
    registers_.current.user.gprs.r12 = 13u;
    registers_.current.user.gprs.r13 = 14u;
    registers_.current.user.gprs.r14 = 15u;
    registers_.current.user.gprs.r15 = 16u;
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

  void ValidateMemoryContentsDescending(uint32_t address,
                                        ArmRegisterIndex address_register,
                                        uint32_t address_register_initial_value,
                                        uint_fast16_t register_list) {
    uint32_t initial_address = address;
    for (uint32_t i = 0u; i < 16u; i++) {
      uint32_t index = 15u - i;
      uint32_t value;
      EXPECT_TRUE(Load32LE(memory_, address, &value));
      if (register_list & (1u << index)) {
        address -= 4;
        if (index == address_register) {
          EXPECT_EQ(address_register_initial_value, value);
        } else {
          EXPECT_EQ(index + 1, value);
        }
      }
    }
    while (address != initial_address + 4) {
      uint8_t value;
      EXPECT_TRUE(Load8(memory_, address, &value));
      EXPECT_EQ(0u, value);
      address -= 1;
      if (address == 0) {
        address = memory_space_.size() - 1;
      }
    }
  }

  void ValidateMemoryContentsAscending(uint32_t address,
                                       ArmRegisterIndex address_register,
                                       uint32_t address_register_initial_value,
                                       uint_fast16_t register_list) {
    uint32_t initial_address = address;
    for (uint32_t i = 0u; i < 16u; i++) {
      uint32_t value;
      EXPECT_TRUE(Load32LE(memory_, address, &value));
      if (register_list & (1u << i)) {
        address += 4;
        if (i == address_register) {
          EXPECT_EQ(address_register_initial_value, value);
        } else {
          EXPECT_EQ(i + 1, value);
        }
      }
    }
    while (address != initial_address) {
      uint8_t value;
      EXPECT_TRUE(Load8(memory_, address, &value));
      EXPECT_EQ(0u, value);
      address += 1;
      if (address == memory_space_.size()) {
        address = 0;
      }
    }
  }

  void ValidateRegisters(ArmRegisterIndex address_register, uint32_t address) {
    for (uint_fast8_t i = 0u; i < 16u; i++) {
      if (i == address_register) {
        EXPECT_EQ(address, registers_.current.user.gprs.gprs[i]);
      } else {
        EXPECT_EQ(i + 1u, registers_.current.user.gprs.gprs[i]);
      }
    }
  }

  static std::vector<char> memory_space_;
  ArmAllRegisters registers_;
  Memory *memory_;
};

std::vector<char> StmTest::memory_space_(384, 0);

TEST_P(StmTest, ArmSTMDA) {
  registers_.current.user.gprs.r0 = 0x13Cu;
  EXPECT_TRUE(ArmSTMDA(&registers_, memory_, REGISTER_R0, GetParam()));

  ValidateMemoryContentsDescending(0x13Cu, REGISTER_R0, 0x13Cu, GetParam());
  ValidateRegisters(REGISTER_R0, 0x13Cu);
}

TEST_P(StmTest, ArmSTMDB) {
  registers_.current.user.gprs.r0 = 0x140u;
  EXPECT_TRUE(ArmSTMDB(&registers_, memory_, REGISTER_R0, GetParam()));

  ValidateMemoryContentsDescending(0x13Cu, REGISTER_R0, 0x140u, GetParam());
  ValidateRegisters(REGISTER_R0, 0x140u);
}

TEST_P(StmTest, ArmSTMDAW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  registers_.current.user.gprs.r0 = 0x13Cu;
  EXPECT_TRUE(ArmSTMDAW(&registers_, memory_, REGISTER_R0, GetParam()));
  uint32_t end_address = 0x13Cu - __builtin_popcount(GetParam()) * 4u;

  ValidateMemoryContentsDescending(0x13Cu, REGISTER_R0, end_address,
                                   GetParam());
  ValidateRegisters(REGISTER_R0, end_address);
}

TEST_P(StmTest, ArmSTMDBW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  registers_.current.user.gprs.r0 = 0x140u;
  EXPECT_TRUE(ArmSTMDBW(&registers_, memory_, REGISTER_R0, GetParam()));
  uint32_t end_address = 0x140u - __builtin_popcount(GetParam()) * 4u;

  ValidateMemoryContentsDescending(0x13Cu, REGISTER_R0, end_address,
                                   GetParam());
  ValidateRegisters(REGISTER_R0, end_address);
}

TEST_P(StmTest, ArmSTMIA) {
  registers_.current.user.gprs.r0 = 0x100u;
  EXPECT_TRUE(ArmSTMIA(&registers_, memory_, REGISTER_R0, GetParam()));

  ValidateMemoryContentsAscending(0x100u, REGISTER_R0, 0x100u, GetParam());
  ValidateRegisters(REGISTER_R0, 0x100u);
}

TEST_P(StmTest, ArmSTMIB) {
  registers_.current.user.gprs.r0 = 0xFCu;
  EXPECT_TRUE(ArmSTMIB(&registers_, memory_, REGISTER_R0, GetParam()));

  ValidateMemoryContentsAscending(0x100u, REGISTER_R0, 0xFCu, GetParam());
  ValidateRegisters(REGISTER_R0, 0xFCu);
}

TEST_P(StmTest, ArmSTMIAW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  registers_.current.user.gprs.r0 = 0x100u;
  EXPECT_TRUE(ArmSTMIAW(&registers_, memory_, REGISTER_R0, GetParam()));
  uint32_t end_address = 0x100u + __builtin_popcount(GetParam()) * 4u;

  ValidateMemoryContentsAscending(0x100u, REGISTER_R0, end_address, GetParam());
  ValidateRegisters(REGISTER_R0, end_address);
}

TEST_P(StmTest, ArmSTMIBW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  registers_.current.user.gprs.r0 = 0xFCu;
  EXPECT_TRUE(ArmSTMIBW(&registers_, memory_, REGISTER_R0, GetParam()));
  uint32_t end_address = 0xFCu + __builtin_popcount(GetParam()) * 4u;

  ValidateMemoryContentsAscending(0x100u, REGISTER_R0, end_address, GetParam());
  ValidateRegisters(REGISTER_R0, end_address);
}

TEST_P(StmTest, ArmSTMSDA) {
  registers_.current.user.gprs.r0 = 0x13Cu;
  registers_.current.user.cpsr.mode = MODE_USR;
  EXPECT_TRUE(ArmSTMSDA(&registers_, memory_, REGISTER_R0, GetParam()));

  ValidateMemoryContentsDescending(0x13Cu, REGISTER_R0, 0x13Cu, GetParam());
  ValidateRegisters(REGISTER_R0, 0x13Cu);
}

TEST_P(StmTest, ArmSTMSDB) {
  registers_.current.user.gprs.r0 = 0x140u;
  registers_.current.user.cpsr.mode = MODE_USR;
  EXPECT_TRUE(ArmSTMSDB(&registers_, memory_, REGISTER_R0, GetParam()));

  ValidateMemoryContentsDescending(0x13Cu, REGISTER_R0, 0x140u, GetParam());
  ValidateRegisters(REGISTER_R0, 0x140u);
}

TEST_P(StmTest, ArmSTMSDAW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  registers_.current.user.gprs.r0 = 0x13Cu;
  registers_.current.user.cpsr.mode = MODE_USR;
  EXPECT_TRUE(ArmSTMSDAW(&registers_, memory_, REGISTER_R0, GetParam()));
  uint32_t end_address = 0x13Cu - __builtin_popcount(GetParam()) * 4u;

  ValidateMemoryContentsDescending(0x13Cu, REGISTER_R0, end_address,
                                   GetParam());
  ValidateRegisters(REGISTER_R0, end_address);
}

TEST_P(StmTest, ArmSTMSDBW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  registers_.current.user.gprs.r0 = 0x140u;
  registers_.current.user.cpsr.mode = MODE_USR;
  EXPECT_TRUE(ArmSTMSDBW(&registers_, memory_, REGISTER_R0, GetParam()));
  uint32_t end_address = 0x140u - __builtin_popcount(GetParam()) * 4u;

  ValidateMemoryContentsDescending(0x13Cu, REGISTER_R0, end_address,
                                   GetParam());
  ValidateRegisters(REGISTER_R0, end_address);
}

TEST_P(StmTest, ArmSTMSIA) {
  registers_.current.user.gprs.r0 = 0x100u;
  registers_.current.user.cpsr.mode = MODE_USR;
  EXPECT_TRUE(ArmSTMSIA(&registers_, memory_, REGISTER_R0, GetParam()));

  ValidateMemoryContentsAscending(0x100u, REGISTER_R0, 0x100u, GetParam());
  ValidateRegisters(REGISTER_R0, 0x100u);
}

TEST_P(StmTest, ArmSTMSIB) {
  registers_.current.user.gprs.r0 = 0xFCu;
  registers_.current.user.cpsr.mode = MODE_USR;
  EXPECT_TRUE(ArmSTMSIB(&registers_, memory_, REGISTER_R0, GetParam()));

  ValidateMemoryContentsAscending(0x100u, REGISTER_R0, 0xFCu, GetParam());
  ValidateRegisters(REGISTER_R0, 0xFCu);
}

TEST_P(StmTest, ArmSTMSIAW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  registers_.current.user.gprs.r0 = 0x100u;
  registers_.current.user.cpsr.mode = MODE_USR;
  EXPECT_TRUE(ArmSTMSIAW(&registers_, memory_, REGISTER_R0, GetParam()));
  uint32_t end_address = 0x100u + __builtin_popcount(GetParam()) * 4u;

  ValidateMemoryContentsAscending(0x100u, REGISTER_R0, end_address, GetParam());
  ValidateRegisters(REGISTER_R0, end_address);
}

TEST_P(StmTest, ArmSTMSIBW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  registers_.current.user.gprs.r0 = 0xFCu;
  registers_.current.user.cpsr.mode = MODE_USR;
  EXPECT_TRUE(ArmSTMSIBW(&registers_, memory_, REGISTER_R0, GetParam()));
  uint32_t end_address = 0xFCu + __builtin_popcount(GetParam()) * 4u;

  ValidateMemoryContentsAscending(0x100u, REGISTER_R0, end_address, GetParam());
  ValidateRegisters(REGISTER_R0, end_address);
}

TEST_P(StmTest, SysArmSTMSDA) {
  registers_.current.user.gprs.r0 = 0x13Cu;
  registers_.current.user.cpsr.mode = MODE_SYS;
  EXPECT_TRUE(ArmSTMSDA(&registers_, memory_, REGISTER_R0, GetParam()));

  ValidateMemoryContentsDescending(0x13Cu, REGISTER_R0, 0x13Cu, GetParam());
  ValidateRegisters(REGISTER_R0, 0x13Cu);
}

TEST_P(StmTest, SysArmSTMSDB) {
  registers_.current.user.gprs.r0 = 0x140u;
  registers_.current.user.cpsr.mode = MODE_SYS;
  EXPECT_TRUE(ArmSTMSDB(&registers_, memory_, REGISTER_R0, GetParam()));

  ValidateMemoryContentsDescending(0x13Cu, REGISTER_R0, 0x140u, GetParam());
  ValidateRegisters(REGISTER_R0, 0x140u);
}

TEST_P(StmTest, SysArmSTMSDAW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  registers_.current.user.gprs.r0 = 0x13Cu;
  registers_.current.user.cpsr.mode = MODE_SYS;
  EXPECT_TRUE(ArmSTMSDAW(&registers_, memory_, REGISTER_R0, GetParam()));
  uint32_t end_address = 0x13Cu - __builtin_popcount(GetParam()) * 4u;

  ValidateMemoryContentsDescending(0x13Cu, REGISTER_R0, end_address,
                                   GetParam());
  ValidateRegisters(REGISTER_R0, end_address);
}

TEST_P(StmTest, SysArmSTMSDBW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  registers_.current.user.gprs.r0 = 0x140u;
  registers_.current.user.cpsr.mode = MODE_SYS;
  EXPECT_TRUE(ArmSTMSDBW(&registers_, memory_, REGISTER_R0, GetParam()));
  uint32_t end_address = 0x140u - __builtin_popcount(GetParam()) * 4u;

  ValidateMemoryContentsDescending(0x13Cu, REGISTER_R0, end_address,
                                   GetParam());
  ValidateRegisters(REGISTER_R0, end_address);
}

TEST_P(StmTest, SysArmSTMSIA) {
  registers_.current.user.gprs.r0 = 0x100u;
  registers_.current.user.cpsr.mode = MODE_SYS;
  EXPECT_TRUE(ArmSTMSIA(&registers_, memory_, REGISTER_R0, GetParam()));

  ValidateMemoryContentsAscending(0x100u, REGISTER_R0, 0x100u, GetParam());
  ValidateRegisters(REGISTER_R0, 0x100u);
}

TEST_P(StmTest, SysArmSTMSIB) {
  registers_.current.user.gprs.r0 = 0xFCu;
  registers_.current.user.cpsr.mode = MODE_SYS;
  EXPECT_TRUE(ArmSTMSIB(&registers_, memory_, REGISTER_R0, GetParam()));

  ValidateMemoryContentsAscending(0x100u, REGISTER_R0, 0xFCu, GetParam());
  ValidateRegisters(REGISTER_R0, 0xFCu);
}

TEST_P(StmTest, SysArmSTMSIAW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  registers_.current.user.gprs.r0 = 0x100u;
  registers_.current.user.cpsr.mode = MODE_SYS;
  EXPECT_TRUE(ArmSTMSIAW(&registers_, memory_, REGISTER_R0, GetParam()));
  uint32_t end_address = 0x100u + __builtin_popcount(GetParam()) * 4u;

  ValidateMemoryContentsAscending(0x100u, REGISTER_R0, end_address, GetParam());
  ValidateRegisters(REGISTER_R0, end_address);
}

TEST_P(StmTest, SysArmSTMSIBW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  registers_.current.user.gprs.r0 = 0xFCu;
  registers_.current.user.cpsr.mode = MODE_SYS;
  EXPECT_TRUE(ArmSTMSIBW(&registers_, memory_, REGISTER_R0, GetParam()));
  uint32_t end_address = 0xFCu + __builtin_popcount(GetParam()) * 4u;

  ValidateMemoryContentsAscending(0x100u, REGISTER_R0, end_address, GetParam());
  ValidateRegisters(REGISTER_R0, end_address);
}

TEST_P(StmTest, SvcArmSTMSDA) {
  registers_.current.user.gprs.r0 = 0x13Cu;
  registers_.current.user.cpsr.mode = MODE_USR;
  ArmProgramStatusRegister new_status = registers_.current.user.cpsr;
  new_status.mode = MODE_SVC;
  ArmLoadCPSR(&registers_, new_status);

  EXPECT_TRUE(ArmSTMSDA(&registers_, memory_, REGISTER_R0, GetParam()));
  new_status.mode = MODE_USR;
  ArmLoadCPSR(&registers_, new_status);

  ValidateMemoryContentsDescending(0x13Cu, REGISTER_R0, 0x13Cu, GetParam());
  ValidateRegisters(REGISTER_R0, 0x13Cu);
}

TEST_P(StmTest, SvcArmSTMSDB) {
  registers_.current.user.gprs.r0 = 0x140u;
  registers_.current.user.cpsr.mode = MODE_SVC;
  ArmProgramStatusRegister new_status = registers_.current.user.cpsr;
  new_status.mode = MODE_SVC;
  ArmLoadCPSR(&registers_, new_status);

  EXPECT_TRUE(ArmSTMSDB(&registers_, memory_, REGISTER_R0, GetParam()));
  new_status.mode = MODE_USR;
  ArmLoadCPSR(&registers_, new_status);

  ValidateMemoryContentsDescending(0x13Cu, REGISTER_R0, 0x140u, GetParam());
  ValidateRegisters(REGISTER_R0, 0x140u);
}

TEST_P(StmTest, SvcArmSTMSDAW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  registers_.current.user.gprs.r0 = 0x13Cu;
  registers_.current.user.cpsr.mode = MODE_SVC;
  ArmProgramStatusRegister new_status = registers_.current.user.cpsr;
  new_status.mode = MODE_SVC;
  ArmLoadCPSR(&registers_, new_status);

  EXPECT_TRUE(ArmSTMSDAW(&registers_, memory_, REGISTER_R0, GetParam()));
  new_status.mode = MODE_USR;
  ArmLoadCPSR(&registers_, new_status);

  uint32_t end_address = 0x13Cu - __builtin_popcount(GetParam()) * 4u;
  ValidateMemoryContentsDescending(0x13Cu, REGISTER_R0, end_address,
                                   GetParam());
  ValidateRegisters(REGISTER_R0, end_address);
}

TEST_P(StmTest, SvcArmSTMSDBW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  registers_.current.user.gprs.r0 = 0x140u;
  registers_.current.user.cpsr.mode = MODE_SVC;
  ArmProgramStatusRegister new_status = registers_.current.user.cpsr;
  new_status.mode = MODE_SVC;
  ArmLoadCPSR(&registers_, new_status);

  EXPECT_TRUE(ArmSTMSDBW(&registers_, memory_, REGISTER_R0, GetParam()));
  new_status.mode = MODE_USR;
  ArmLoadCPSR(&registers_, new_status);

  uint32_t end_address = 0x140u - __builtin_popcount(GetParam()) * 4u;
  ValidateMemoryContentsDescending(0x13Cu, REGISTER_R0, end_address,
                                   GetParam());
  ValidateRegisters(REGISTER_R0, end_address);
}

TEST_P(StmTest, SvcArmSTMSIA) {
  registers_.current.user.gprs.r0 = 0x100u;
  registers_.current.user.cpsr.mode = MODE_SVC;
  ArmProgramStatusRegister new_status = registers_.current.user.cpsr;
  new_status.mode = MODE_SVC;
  ArmLoadCPSR(&registers_, new_status);

  EXPECT_TRUE(ArmSTMSIA(&registers_, memory_, REGISTER_R0, GetParam()));
  new_status.mode = MODE_USR;
  ArmLoadCPSR(&registers_, new_status);

  ValidateMemoryContentsAscending(0x100u, REGISTER_R0, 0x100u, GetParam());
  ValidateRegisters(REGISTER_R0, 0x100u);
}

TEST_P(StmTest, SvcArmSTMSIB) {
  registers_.current.user.gprs.r0 = 0xFCu;
  registers_.current.user.cpsr.mode = MODE_SVC;
  ArmProgramStatusRegister new_status = registers_.current.user.cpsr;
  new_status.mode = MODE_SVC;
  ArmLoadCPSR(&registers_, new_status);

  EXPECT_TRUE(ArmSTMSIB(&registers_, memory_, REGISTER_R0, GetParam()));
  new_status.mode = MODE_USR;
  ArmLoadCPSR(&registers_, new_status);

  ValidateMemoryContentsAscending(0x100u, REGISTER_R0, 0xFCu, GetParam());
  ValidateRegisters(REGISTER_R0, 0xFCu);
}

TEST_P(StmTest, SvcArmSTMSIAW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  registers_.current.user.gprs.r0 = 0x100u;
  registers_.current.user.cpsr.mode = MODE_SVC;
  ArmProgramStatusRegister new_status = registers_.current.user.cpsr;
  new_status.mode = MODE_SVC;
  ArmLoadCPSR(&registers_, new_status);

  EXPECT_TRUE(ArmSTMSIAW(&registers_, memory_, REGISTER_R0, GetParam()));
  new_status.mode = MODE_USR;
  ArmLoadCPSR(&registers_, new_status);

  uint32_t end_address = 0x100u + __builtin_popcount(GetParam()) * 4u;
  ValidateMemoryContentsAscending(0x100u, REGISTER_R0, end_address, GetParam());
  ValidateRegisters(REGISTER_R0, end_address);
}

TEST_P(StmTest, SvcArmSTMSIBW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  registers_.current.user.gprs.r0 = 0xFCu;
  registers_.current.user.cpsr.mode = MODE_SVC;
  ArmProgramStatusRegister new_status = registers_.current.user.cpsr;
  new_status.mode = MODE_SVC;
  ArmLoadCPSR(&registers_, new_status);

  EXPECT_TRUE(ArmSTMSIBW(&registers_, memory_, REGISTER_R0, GetParam()));
  new_status.mode = MODE_USR;
  ArmLoadCPSR(&registers_, new_status);

  uint32_t end_address = 0xFCu + __builtin_popcount(GetParam()) * 4u;
  ValidateMemoryContentsAscending(0x100u, REGISTER_R0, end_address, GetParam());
  ValidateRegisters(REGISTER_R0, end_address);
}

INSTANTIATE_TEST_SUITE_P(ArmStmTestModule, StmTest,
                         testing::Range(std::numeric_limits<uint16_t>::min(),
                                        std::numeric_limits<uint16_t>::max(),
                                        51u));

class MemoryFailsTest : public testing::TestWithParam<uint16_t> {
 public:
  void SetUp() override {
    memory_ = MemoryAllocate(nullptr, Load32LE, Load16LE, Load8, Store32LE,
                             Store16LE, Store8, nullptr);
    ASSERT_NE(nullptr, memory_);

    registers_.current.user.cpsr.mode = MODE_USR;
    registers_.current.user.gprs.r0 = 1u;
    registers_.current.user.gprs.r1 = 2u;
    registers_.current.user.gprs.r2 = 3u;
    registers_.current.user.gprs.r3 = 4u;
    registers_.current.user.gprs.r4 = 5u;
    registers_.current.user.gprs.r5 = 6u;
    registers_.current.user.gprs.r6 = 7u;
    registers_.current.user.gprs.r7 = 8u;
    registers_.current.user.gprs.r8 = 9u;
    registers_.current.user.gprs.r9 = 10u;
    registers_.current.user.gprs.r10 = 11u;
    registers_.current.user.gprs.r11 = 12u;
    registers_.current.user.gprs.r12 = 13u;
    registers_.current.user.gprs.r13 = 14u;
    registers_.current.user.gprs.r14 = 15u;
    registers_.current.user.gprs.r15 = 16u;
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
    return regs.current.user.cpsr.mode == MODE_ABT &&
           regs.current.user.gprs.pc == 0x10u;
  }

  Memory *memory_;
  ArmAllRegisters registers_;
};

TEST_P(MemoryFailsTest, ArmLDMDA) {
  if (GetParam() == 0u) {
    return;
  }

  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0 = 0x13Cu;
  EXPECT_FALSE(ArmLDMDA(&registers, memory_, REGISTER_R0, GetParam()));
  EXPECT_EQ(0x13Cu, registers.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_P(MemoryFailsTest, ArmLDMDB) {
  if (GetParam() == 0u) {
    return;
  }

  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0 = 0x140u;
  EXPECT_FALSE(ArmLDMDB(&registers, memory_, REGISTER_R0, GetParam()));
  EXPECT_EQ(0x140u, registers.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_P(MemoryFailsTest, ArmLDMDAW) {
  if (GetParam() == 0u || GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0 = 0x13Cu;
  EXPECT_FALSE(ArmLDMDAW(&registers, memory_, REGISTER_R0, GetParam()));
  uint32_t end_address = 0x13Cu - __builtin_popcount(GetParam()) * 4u;
  EXPECT_EQ(end_address, registers.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_P(MemoryFailsTest, ArmLDMDBW) {
  if (GetParam() == 0u || GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0 = 0x140u;
  EXPECT_FALSE(ArmLDMDBW(&registers, memory_, REGISTER_R0, GetParam()));
  uint32_t end_address = 0x140u - __builtin_popcount(GetParam()) * 4u;
  EXPECT_EQ(end_address, registers.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_P(MemoryFailsTest, ArmLDMIA) {
  if (GetParam() == 0u) {
    return;
  }

  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0 = 0x100u;
  EXPECT_FALSE(ArmLDMIA(&registers, memory_, REGISTER_R0, GetParam()));
  EXPECT_EQ(0x100u, registers.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_P(MemoryFailsTest, ArmLDMIB) {
  if (GetParam() == 0u) {
    return;
  }

  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0 = 0xFCu;
  EXPECT_FALSE(ArmLDMIB(&registers, memory_, REGISTER_R0, GetParam()));
  EXPECT_EQ(0xFCu, registers.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_P(MemoryFailsTest, ArmLDMIAW) {
  if (GetParam() == 0u || GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0 = 0x100u;
  EXPECT_FALSE(ArmLDMIAW(&registers, memory_, REGISTER_R0, GetParam()));
  uint32_t end_address = 0x100u + __builtin_popcount(GetParam()) * 4u;
  EXPECT_EQ(end_address, registers.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_P(MemoryFailsTest, ArmLDMIBW) {
  if (GetParam() == 0u || GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0 = 0xFCu;
  EXPECT_FALSE(ArmLDMIBW(&registers, memory_, REGISTER_R0, GetParam()));
  uint32_t end_address = 0xFCu + __builtin_popcount(GetParam()) * 4u;
  EXPECT_EQ(end_address, registers.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_P(MemoryFailsTest, ArmLDMSDA) {
  if (GetParam() == 0u) {
    return;
  }

  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0 = 0x13Cu;
  EXPECT_FALSE(ArmLDMSDA(&registers, memory_, REGISTER_R0, GetParam()));
  EXPECT_EQ(0x13Cu, registers.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_P(MemoryFailsTest, ArmLDMSDB) {
  if (GetParam() == 0u) {
    return;
  }

  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0 = 0x140u;
  EXPECT_FALSE(ArmLDMSDB(&registers, memory_, REGISTER_R0, GetParam()));
  EXPECT_EQ(0x140u, registers.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_P(MemoryFailsTest, ArmLDMSDAW) {
  if (GetParam() == 0u || GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0 = 0x13Cu;
  EXPECT_FALSE(ArmLDMSDAW(&registers, memory_, REGISTER_R0, GetParam()));
  uint32_t end_address = 0x13Cu - __builtin_popcount(GetParam()) * 4u;
  EXPECT_EQ(end_address, registers.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_P(MemoryFailsTest, ArmLDMSDBW) {
  if (GetParam() == 0u || GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0 = 0x140u;
  EXPECT_FALSE(ArmLDMSDBW(&registers, memory_, REGISTER_R0, GetParam()));
  uint32_t end_address = 0x140u - __builtin_popcount(GetParam()) * 4u;
  EXPECT_EQ(end_address, registers.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_P(MemoryFailsTest, ArmLDMSIA) {
  if (GetParam() == 0u) {
    return;
  }

  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0 = 0x100u;
  EXPECT_FALSE(ArmLDMSIA(&registers, memory_, REGISTER_R0, GetParam()));
  EXPECT_EQ(0x100u, registers.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_P(MemoryFailsTest, ArmLDMSIB) {
  if (GetParam() == 0u) {
    return;
  }

  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0 = 0xFCu;
  EXPECT_FALSE(ArmLDMSIB(&registers, memory_, REGISTER_R0, GetParam()));
  EXPECT_EQ(0xFCu, registers.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_P(MemoryFailsTest, ArmLDMSIAW) {
  if (GetParam() == 0u || GetParam() == 0u ||
      GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0 = 0x100u;
  EXPECT_FALSE(ArmLDMSIAW(&registers, memory_, REGISTER_R0, GetParam()));
  uint32_t end_address = 0x100u + __builtin_popcount(GetParam()) * 4u;
  EXPECT_EQ(end_address, registers.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_P(MemoryFailsTest, ArmLDMSIBW) {
  if (GetParam() == 0u || GetParam() == 0u ||
      GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0 = 0xFCu;
  EXPECT_FALSE(ArmLDMSIBW(&registers, memory_, REGISTER_R0, GetParam()));
  uint32_t end_address = 0xFCu + __builtin_popcount(GetParam()) * 4u;
  EXPECT_EQ(end_address, registers.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_P(MemoryFailsTest, ArmSTMDA) {
  if (GetParam() == 0u) {
    return;
  }

  registers_.current.user.gprs.r0 = 0x13Cu;
  EXPECT_FALSE(ArmSTMDA(&registers_, memory_, REGISTER_R0, GetParam()));
  EXPECT_EQ(0x13Cu, registers_.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_P(MemoryFailsTest, ArmSTMDB) {
  if (GetParam() == 0u) {
    return;
  }

  registers_.current.user.gprs.r0 = 0x140u;
  EXPECT_FALSE(ArmSTMDB(&registers_, memory_, REGISTER_R0, GetParam()));
  EXPECT_EQ(0x140u, registers_.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_P(MemoryFailsTest, ArmSTMDAW) {
  if (GetParam() == 0u || GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  registers_.current.user.gprs.r0 = 0x13Cu;
  EXPECT_FALSE(ArmSTMDAW(&registers_, memory_, REGISTER_R0, GetParam()));
  uint32_t end_address = 0x13Cu - __builtin_popcount(GetParam()) * 4u;
  EXPECT_EQ(end_address, registers_.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_P(MemoryFailsTest, ArmSTMDBW) {
  if (GetParam() == 0u || GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  registers_.current.user.gprs.r0 = 0x140u;
  EXPECT_FALSE(ArmSTMDBW(&registers_, memory_, REGISTER_R0, GetParam()));
  uint32_t end_address = 0x140u - __builtin_popcount(GetParam()) * 4u;
  EXPECT_EQ(end_address, registers_.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_P(MemoryFailsTest, ArmSTMIA) {
  if (GetParam() == 0u) {
    return;
  }

  registers_.current.user.gprs.r0 = 0x100u;
  EXPECT_FALSE(ArmSTMIA(&registers_, memory_, REGISTER_R0, GetParam()));
  EXPECT_EQ(0x100u, registers_.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_P(MemoryFailsTest, ArmSTMIB) {
  if (GetParam() == 0u) {
    return;
  }

  registers_.current.user.gprs.r0 = 0xFCu;
  EXPECT_FALSE(ArmSTMIB(&registers_, memory_, REGISTER_R0, GetParam()));
  EXPECT_EQ(0xFCu, registers_.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_P(MemoryFailsTest, ArmSTMIAW) {
  if (GetParam() == 0u || GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  registers_.current.user.gprs.r0 = 0x100u;
  EXPECT_FALSE(ArmSTMIAW(&registers_, memory_, REGISTER_R0, GetParam()));
  uint32_t end_address = 0x100u + __builtin_popcount(GetParam()) * 4u;
  EXPECT_EQ(end_address, registers_.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_P(MemoryFailsTest, ArmSTMIBW) {
  if (GetParam() == 0u || GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  registers_.current.user.gprs.r0 = 0xFCu;
  EXPECT_FALSE(ArmSTMIBW(&registers_, memory_, REGISTER_R0, GetParam()));
  uint32_t end_address = 0xFCu + __builtin_popcount(GetParam()) * 4u;
  EXPECT_EQ(end_address, registers_.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_P(MemoryFailsTest, ArmSTMSDA) {
  if (GetParam() == 0u) {
    return;
  }

  registers_.current.user.gprs.r0 = 0x13Cu;
  registers_.current.user.cpsr.mode = MODE_USR;
  EXPECT_FALSE(ArmSTMSDA(&registers_, memory_, REGISTER_R0, GetParam()));
  EXPECT_EQ(0x13Cu, registers_.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_P(MemoryFailsTest, ArmSTMSDB) {
  if (GetParam() == 0u) {
    return;
  }

  registers_.current.user.gprs.r0 = 0x140u;
  registers_.current.user.cpsr.mode = MODE_USR;
  EXPECT_FALSE(ArmSTMSDB(&registers_, memory_, REGISTER_R0, GetParam()));
  EXPECT_EQ(0x140u, registers_.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_P(MemoryFailsTest, ArmSTMSDAW) {
  if (GetParam() == 0u || GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  registers_.current.user.gprs.r0 = 0x13Cu;
  registers_.current.user.cpsr.mode = MODE_USR;
  EXPECT_FALSE(ArmSTMSDAW(&registers_, memory_, REGISTER_R0, GetParam()));
  uint32_t end_address = 0x13Cu - __builtin_popcount(GetParam()) * 4u;
  EXPECT_EQ(end_address, registers_.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_P(MemoryFailsTest, ArmSTMSDBW) {
  if (GetParam() == 0u || GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  registers_.current.user.gprs.r0 = 0x140u;
  registers_.current.user.cpsr.mode = MODE_USR;
  EXPECT_FALSE(ArmSTMSDBW(&registers_, memory_, REGISTER_R0, GetParam()));
  uint32_t end_address = 0x140u - __builtin_popcount(GetParam()) * 4u;
  EXPECT_EQ(end_address, registers_.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_P(MemoryFailsTest, ArmSTMSIA) {
  if (GetParam() == 0u) {
    return;
  }

  registers_.current.user.gprs.r0 = 0x100u;
  registers_.current.user.cpsr.mode = MODE_USR;
  EXPECT_FALSE(ArmSTMSIA(&registers_, memory_, REGISTER_R0, GetParam()));
  EXPECT_EQ(0x100u, registers_.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_P(MemoryFailsTest, ArmSTMSIB) {
  if (GetParam() == 0u) {
    return;
  }

  registers_.current.user.gprs.r0 = 0xFCu;
  registers_.current.user.cpsr.mode = MODE_USR;
  EXPECT_FALSE(ArmSTMSIB(&registers_, memory_, REGISTER_R0, GetParam()));
  EXPECT_EQ(0xFCu, registers_.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_P(MemoryFailsTest, ArmSTMSIAW) {
  if (GetParam() == 0u || GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  registers_.current.user.gprs.r0 = 0x100u;
  registers_.current.user.cpsr.mode = MODE_USR;
  EXPECT_FALSE(ArmSTMSIAW(&registers_, memory_, REGISTER_R0, GetParam()));
  uint32_t end_address = 0x100u + __builtin_popcount(GetParam()) * 4u;
  EXPECT_EQ(end_address, registers_.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_P(MemoryFailsTest, ArmSTMSIBW) {
  if (GetParam() == 0u || GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  registers_.current.user.gprs.r0 = 0xFCu;
  registers_.current.user.cpsr.mode = MODE_USR;
  EXPECT_FALSE(ArmSTMSIBW(&registers_, memory_, REGISTER_R0, GetParam()));
  uint32_t end_address = 0xFCu + __builtin_popcount(GetParam()) * 4u;
  EXPECT_EQ(end_address, registers_.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

INSTANTIATE_TEST_SUITE_P(MemoryFailsTestModule, MemoryFailsTest,
                         testing::Range(std::numeric_limits<uint16_t>::min(),
                                        std::numeric_limits<uint16_t>::max(),
                                        51u));
