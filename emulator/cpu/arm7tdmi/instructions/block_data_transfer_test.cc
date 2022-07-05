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
      uint32_t address_register_expected_value, uint_fast16_t register_list) {
    if (register_list == 0u) {
      for (uint32_t i = 0u; i <= 14u; i++) {
        if (i == address_register) {
          EXPECT_EQ(address_register_expected_value,
                    registers.current.user.gprs.gprs[i]);
        } else {
          EXPECT_EQ(0u, registers.current.user.gprs.gprs[i]);
        }
      }
      return;
    }

    uint32_t expected_value = 16u;
    for (uint32_t i = 0u; i < 16u; i++) {
      uint32_t index = 15u - i;
      if (register_list & (1u << index)) {
        if (index == REGISTER_PC) {
          EXPECT_EQ((expected_value-- >> 2u) << 2u,
                    ArmCurrentInstruction(&registers));
        } else {
          EXPECT_EQ(expected_value--, registers.current.user.gprs.gprs[index]);
        }
      } else {
        if (index == address_register) {
          if (index == REGISTER_PC) {
            EXPECT_EQ(address_register_expected_value,
                      ArmCurrentInstruction(&registers));
          } else {
            EXPECT_EQ(address_register_expected_value,
                      registers.current.user.gprs.gprs[index]);
          }
        } else {
          if (index == REGISTER_PC) {
            EXPECT_EQ(4u, registers.current.user.gprs.gprs[index]);
          } else {
            EXPECT_EQ(0u, registers.current.user.gprs.gprs[index]);
          }
        }
      }
    }
  }

  void ValidateGeneralPurposeRegisterContentsAscending(
      const ArmAllRegisters &registers, ArmRegisterIndex address_register,
      uint32_t address_register_expected_value, uint_fast16_t register_list) {
    if (register_list == 0u) {
      for (uint32_t i = 0u; i <= 14u; i++) {
        if (i == address_register) {
          EXPECT_EQ(address_register_expected_value,
                    registers.current.user.gprs.gprs[i]);
        } else {
          EXPECT_EQ(0u, registers.current.user.gprs.gprs[i]);
        }
      }
      return;
    }

    uint32_t expected_value = 1u;
    for (uint32_t i = 0u; i < 16u; i++) {
      if (register_list & (1u << i)) {
        if (i == REGISTER_PC) {
          EXPECT_EQ((expected_value++ >> 2u) << 2u,
                    ArmCurrentInstruction(&registers));
        } else {
          EXPECT_EQ(expected_value++, registers.current.user.gprs.gprs[i]);
        }
      } else {
        if (i == address_register) {
          if (i == REGISTER_PC) {
            EXPECT_EQ(address_register_expected_value,
                      ArmCurrentInstruction(&registers));
          } else {
            EXPECT_EQ(address_register_expected_value,
                      registers.current.user.gprs.gprs[i]);
          }
        } else {
          if (i == REGISTER_PC) {
            EXPECT_EQ(4u, registers.current.user.gprs.gprs[i]);
          } else {
            EXPECT_EQ(0u, registers.current.user.gprs.gprs[i]);
          }
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
  ArmLDMDA(&registers, memory_, REGISTER_R0, GetParam());

  ValidateGeneralPurposeRegisterContentsDescending(registers, REGISTER_R0,
                                                   0x13Cu, GetParam());
}

TEST_P(LdmTest, ArmLDMDB) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 0x140u;
  ArmLDMDB(&registers, memory_, REGISTER_R0, GetParam());

  ValidateGeneralPurposeRegisterContentsDescending(registers, REGISTER_R0,
                                                   0x140u, GetParam());
}

TEST_P(LdmTest, ArmLDMDAW) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 0x13Cu;
  ArmLDMDAW(&registers, memory_, REGISTER_R0, GetParam());
  uint32_t end_address = 0x13Cu - __builtin_popcount(GetParam()) * 4u;

  uint32_t address_register_expected_value;
  if (GetParam() & (1u << REGISTER_R0)) {
    address_register_expected_value = 0u;
  } else if (GetParam() == 0u) {
    address_register_expected_value = 0x13Cu - 0x40u;
  } else {
    address_register_expected_value = end_address;
  }

  ValidateGeneralPurposeRegisterContentsDescending(
      registers, REGISTER_R0, address_register_expected_value, GetParam());
}

TEST_P(LdmTest, ArmLDMDBW) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 0x140u;
  ArmLDMDBW(&registers, memory_, REGISTER_R0, GetParam());
  uint32_t end_address = 0x140u - __builtin_popcount(GetParam()) * 4u;

  uint32_t address_register_expected_value;
  if (GetParam() & (1u << REGISTER_R0)) {
    address_register_expected_value = 0u;
  } else if (GetParam() == 0u) {
    address_register_expected_value = 0x140u - 0x40u;
  } else {
    address_register_expected_value = end_address;
  }

  ValidateGeneralPurposeRegisterContentsDescending(
      registers, REGISTER_R0, address_register_expected_value, GetParam());
}

TEST_P(LdmTest, ArmLDMIA) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 0x100u;
  ArmLDMIA(&registers, memory_, REGISTER_R0, GetParam());

  ValidateGeneralPurposeRegisterContentsAscending(registers, REGISTER_R0,
                                                  0x100u, GetParam());
}

TEST_P(LdmTest, ArmLDMIB) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 0xFCu;
  ArmLDMIB(&registers, memory_, REGISTER_R0, GetParam());

  ValidateGeneralPurposeRegisterContentsAscending(registers, REGISTER_R0, 0xFCu,
                                                  GetParam());
}

TEST_P(LdmTest, ArmLDMIAW) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 0x100u;
  ArmLDMIAW(&registers, memory_, REGISTER_R0, GetParam());
  uint32_t end_address = 0x100u + __builtin_popcount(GetParam()) * 4u;

  uint32_t address_register_expected_value;
  if (GetParam() & (1u << REGISTER_R0)) {
    address_register_expected_value = 0u;
  } else if (GetParam() == 0u) {
    address_register_expected_value = 0x100u + 0x40u;
  } else {
    address_register_expected_value = end_address;
  }

  ValidateGeneralPurposeRegisterContentsAscending(
      registers, REGISTER_R0, address_register_expected_value, GetParam());
}

TEST_P(LdmTest, ArmLDMIBW) {
  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r0 = 0xFCu;
  ArmLDMIBW(&registers, memory_, REGISTER_R0, GetParam());
  uint32_t end_address = 0xFCu + __builtin_popcount(GetParam()) * 4u;

  uint32_t address_register_expected_value;
  if (GetParam() & (1u << REGISTER_R0)) {
    address_register_expected_value = 0u;
  } else if (GetParam() == 0u) {
    address_register_expected_value = 0xFCu + 0x40u;
  } else {
    address_register_expected_value = end_address;
  }

  ValidateGeneralPurposeRegisterContentsAscending(
      registers, REGISTER_R0, address_register_expected_value, GetParam());
}

TEST_P(LdmTest, ArmLDMSDA) {
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_USR;

  registers.current.user.gprs.r0 = 0x13Cu;
  ArmLDMSDA(&registers, memory_, REGISTER_R0, GetParam());

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
  ArmLDMSDB(&registers, memory_, REGISTER_R0, GetParam());

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
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_USR;

  registers.current.user.gprs.r0 = 0x13Cu;
  ArmLDMSDAW(&registers, memory_, REGISTER_R0, GetParam());
  uint32_t end_address = 0x13Cu - __builtin_popcount(GetParam()) * 4u;

  uint32_t address_register_expected_value;
  if (GetParam() & (1u << REGISTER_R0)) {
    address_register_expected_value = 0u;
  } else if (GetParam() == 0u) {
    address_register_expected_value = 0x13Cu - 0x40u;
  } else {
    address_register_expected_value = end_address;
  }

  ValidateGeneralPurposeRegisterContentsDescending(
      registers, REGISTER_R0, address_register_expected_value, GetParam());
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
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_USR;

  registers.current.user.gprs.r0 = 0x140u;
  ArmLDMSDBW(&registers, memory_, REGISTER_R0, GetParam());
  uint32_t end_address = 0x140u - __builtin_popcount(GetParam()) * 4u;

  uint32_t address_register_expected_value;
  if (GetParam() & (1u << REGISTER_R0)) {
    address_register_expected_value = 0u;
  } else if (GetParam() == 0u) {
    address_register_expected_value = 0x140u - 0x40u;
  } else {
    address_register_expected_value = end_address;
  }

  ValidateGeneralPurposeRegisterContentsDescending(
      registers, REGISTER_R0, address_register_expected_value, GetParam());
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
  ArmLDMSIA(&registers, memory_, REGISTER_R0, GetParam());

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
  ArmLDMSIB(&registers, memory_, REGISTER_R0, GetParam());

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
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_USR;

  registers.current.user.gprs.r0 = 0x100u;
  ArmLDMSIAW(&registers, memory_, REGISTER_R0, GetParam());
  uint32_t end_address = 0x100u + __builtin_popcount(GetParam()) * 4u;

  uint32_t address_register_expected_value;
  if (GetParam() & (1u << REGISTER_R0)) {
    address_register_expected_value = 0u;
  } else if (GetParam() == 0u) {
    address_register_expected_value = 0x100u + 0x40u;
  } else {
    address_register_expected_value = end_address;
  }

  ValidateGeneralPurposeRegisterContentsAscending(
      registers, REGISTER_R0, address_register_expected_value, GetParam());
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
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_USR;

  registers.current.user.gprs.r0 = 0xFCu;
  ArmLDMSIBW(&registers, memory_, REGISTER_R0, GetParam());
  uint32_t end_address = 0xFCu + __builtin_popcount(GetParam()) * 4u;

  uint32_t address_register_expected_value;
  if (GetParam() & (1u << REGISTER_R0)) {
    address_register_expected_value = 0u;
  } else if (GetParam() == 0u) {
    address_register_expected_value = 0xFCu + 0x40u;
  } else {
    address_register_expected_value = end_address;
  }

  ValidateGeneralPurposeRegisterContentsAscending(
      registers, REGISTER_R0, address_register_expected_value, GetParam());
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
  ArmLDMSDA(&registers, memory_, REGISTER_R0, GetParam());

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
  ArmLDMSDB(&registers, memory_, REGISTER_R0, GetParam());

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
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SYS;

  registers.current.user.gprs.r0 = 0x13Cu;
  ArmLDMSDAW(&registers, memory_, REGISTER_R0, GetParam());
  uint32_t end_address = 0x13Cu - __builtin_popcount(GetParam()) * 4u;

  uint32_t address_register_expected_value;
  if (GetParam() & (1u << REGISTER_R0)) {
    address_register_expected_value = 0u;
  } else if (GetParam() == 0u) {
    address_register_expected_value = 0x13Cu - 0x40u;
  } else {
    address_register_expected_value = end_address;
  }

  ValidateGeneralPurposeRegisterContentsDescending(
      registers, REGISTER_R0, address_register_expected_value, GetParam());
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
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SYS;

  registers.current.user.gprs.r0 = 0x140u;
  ArmLDMSDBW(&registers, memory_, REGISTER_R0, GetParam());
  uint32_t end_address = 0x140u - __builtin_popcount(GetParam()) * 4u;

  uint32_t address_register_expected_value;
  if (GetParam() & (1u << REGISTER_R0)) {
    address_register_expected_value = 0u;
  } else if (GetParam() == 0u) {
    address_register_expected_value = 0x140u - 0x40u;
  } else {
    address_register_expected_value = end_address;
  }

  ValidateGeneralPurposeRegisterContentsDescending(
      registers, REGISTER_R0, address_register_expected_value, GetParam());
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
  ArmLDMSIA(&registers, memory_, REGISTER_R0, GetParam());

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
  ArmLDMSIB(&registers, memory_, REGISTER_R0, GetParam());

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
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SYS;

  registers.current.user.gprs.r0 = 0x100u;
  ArmLDMSIAW(&registers, memory_, REGISTER_R0, GetParam());
  uint32_t end_address = 0x100u + __builtin_popcount(GetParam()) * 4u;

  uint32_t address_register_expected_value;
  if (GetParam() & (1u << REGISTER_R0)) {
    address_register_expected_value = 0u;
  } else if (GetParam() == 0u) {
    address_register_expected_value = 0x100u + 0x40u;
  } else {
    address_register_expected_value = end_address;
  }

  ValidateGeneralPurposeRegisterContentsAscending(
      registers, REGISTER_R0, address_register_expected_value, GetParam());
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
  auto registers = CreateArmAllRegisters();
  registers.current.user.cpsr.mode = MODE_SYS;

  registers.current.user.gprs.r0 = 0xFCu;
  ArmLDMSIBW(&registers, memory_, REGISTER_R0, GetParam());
  uint32_t end_address = 0xFCu + __builtin_popcount(GetParam()) * 4u;

  uint32_t address_register_expected_value;
  if (GetParam() & (1u << REGISTER_R0)) {
    address_register_expected_value = 0u;
  } else if (GetParam() == 0u) {
    address_register_expected_value = 0xFCu + 0x40u;
  } else {
    address_register_expected_value = end_address;
  }

  ValidateGeneralPurposeRegisterContentsAscending(
      registers, REGISTER_R0, address_register_expected_value, GetParam());
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

  ValidateGeneralPurposeRegisterContentsDescending(registers, REGISTER_R0,
                                                   0x13Cu, GetParam());
}

TEST_P(LdmTest, SvcArmLDMSDB) {
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

  ValidateGeneralPurposeRegisterContentsDescending(registers, REGISTER_R0,
                                                   0x140u, GetParam());
}

TEST_P(LdmTest, SvcArmLDMSDAW) {
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

  uint32_t address_register_expected_value;
  if (GetParam() & (1u << REGISTER_R0)) {
    address_register_expected_value = 0u;
  } else if (GetParam() == 0u) {
    address_register_expected_value = 0x13Cu - 0x40u;
  } else {
    address_register_expected_value = end_address;
  }

  ValidateGeneralPurposeRegisterContentsDescending(
      registers, REGISTER_R0, address_register_expected_value, GetParam());
}

TEST_P(LdmTest, SvcArmLDMSDBW) {
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

  uint32_t address_register_expected_value;
  if (GetParam() & (1u << REGISTER_R0)) {
    address_register_expected_value = 0u;
  } else if (GetParam() == 0u) {
    address_register_expected_value = 0x140u - 0x40u;
  } else {
    address_register_expected_value = end_address;
  }

  ValidateGeneralPurposeRegisterContentsDescending(
      registers, REGISTER_R0, address_register_expected_value, GetParam());
}

TEST_P(LdmTest, SvcArmLDMSIA) {
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

  ValidateGeneralPurposeRegisterContentsAscending(registers, REGISTER_R0,
                                                  0x100u, GetParam());
}

TEST_P(LdmTest, SvcArmLDMSIB) {
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

  uint32_t end_address;
  if (GetParam() == 0u) {
    end_address = 0x100u + 0x40u;
  } else {
    end_address = 0x100u + __builtin_popcount(GetParam()) * 4u;
  }

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

  uint32_t end_address;
  if (GetParam() == 0u) {
    end_address = 0xFCu + 0x40u;
  } else {
    end_address = 0xFCu + __builtin_popcount(GetParam()) * 4u;
  }

  ValidateGeneralPurposeRegisterContentsAscending(registers, REGISTER_R0,
                                                  end_address, GetParam());
}

TEST_P(LdmTest, ThumbPOP) {
  if (GetParam() & (1u << REGISTER_R13)) {
    return;
  }

  auto registers = CreateArmAllRegisters();

  registers.current.user.gprs.r13 = 0x100u;
  ThumbPOP(&registers, memory_, GetParam());
  uint32_t end_address = 0x100u + __builtin_popcount(GetParam()) * 4u;

  uint32_t address_register_expected_value;
  if (GetParam() & (1u << REGISTER_R13)) {
    address_register_expected_value = 0u;
  } else if (GetParam() == 0u) {
    address_register_expected_value = 0x100u + 0x40u;
  } else {
    address_register_expected_value = end_address;
  }

  ValidateGeneralPurposeRegisterContentsAscending(
      registers, REGISTER_R13, address_register_expected_value, GetParam());
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
    registers_.current.user.cpsr.value = 0u;
    registers_.current.user.cpsr.mode = MODE_USR;
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
    if (register_list == 0u) {
      uint32_t value;
      EXPECT_TRUE(Load32LE(memory_, address - 60u, &value));
      EXPECT_EQ(20u, value);
      return;
    }

    uint32_t initial_address = address;
    for (uint32_t i = 0u; i < 16u; i++) {
      uint32_t index = 15u - i;
      uint32_t value;
      EXPECT_TRUE(Load32LE(memory_, address, &value));
      if (register_list & (1u << index)) {
        address -= 4;
        if (index == address_register) {
          EXPECT_EQ(address_register_initial_value, value);
        } else if (index == REGISTER_R15) {
          EXPECT_EQ(index + 5, value);
        } else {
          EXPECT_EQ(index + 1, value);
        }
      }
    }
    while (address != initial_address + 4) {
      uint8_t value = 1u;
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
    if (register_list == 0u) {
      uint32_t value = 1u;
      EXPECT_TRUE(Load32LE(memory_, address, &value));
      EXPECT_EQ(20u, value);
      return;
    }

    uint32_t initial_address = address;
    for (uint32_t i = 0u; i < 16u; i++) {
      uint32_t value;
      EXPECT_TRUE(Load32LE(memory_, address, &value));
      if (register_list & (1u << i)) {
        address += 4;
        if (i == address_register) {
          EXPECT_EQ(address_register_initial_value, value);
        } else if (i == REGISTER_R15) {
          EXPECT_EQ(i + 5, value);
        } else {
          EXPECT_EQ(i + 1, value);
        }
      }
    }
    while (address != initial_address) {
      uint8_t value = 1u;
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
      } else if (i == REGISTER_PC) {
        EXPECT_EQ(i + 5u, registers_.current.user.gprs.gprs[i]);
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
  ArmSTMDA(&registers_, memory_, REGISTER_R0, GetParam());

  ValidateMemoryContentsDescending(0x13Cu, REGISTER_R0, 0x13Cu, GetParam());
  ValidateRegisters(REGISTER_R0, 0x13Cu);
}

TEST_P(StmTest, ArmSTMDB) {
  registers_.current.user.gprs.r0 = 0x140u;
  ArmSTMDB(&registers_, memory_, REGISTER_R0, GetParam());

  ValidateMemoryContentsDescending(0x13Cu, REGISTER_R0, 0x140u, GetParam());
  ValidateRegisters(REGISTER_R0, 0x140u);
}

TEST_P(StmTest, ArmSTMDAW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  registers_.current.user.gprs.r0 = 0x13Cu;
  ArmSTMDAW(&registers_, memory_, REGISTER_R0, GetParam());

  uint32_t end_address;
  if (GetParam() == 0u) {
    end_address = 0x13Cu - 0x40u;
  } else {
    end_address = 0x13Cu - __builtin_popcount(GetParam()) * 4u;
  }

  ValidateMemoryContentsDescending(0x13Cu, REGISTER_R0, end_address,
                                   GetParam());
  ValidateRegisters(REGISTER_R0, end_address);
}

TEST_P(StmTest, ArmSTMDBW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  registers_.current.user.gprs.r0 = 0x140u;
  ArmSTMDBW(&registers_, memory_, REGISTER_R0, GetParam());

  uint32_t end_address;
  if (GetParam() == 0u) {
    end_address = 0x140u - 0x40u;
  } else {
    end_address = 0x140u - __builtin_popcount(GetParam()) * 4u;
  }

  ValidateMemoryContentsDescending(0x13Cu, REGISTER_R0, end_address,
                                   GetParam());
  ValidateRegisters(REGISTER_R0, end_address);
}

TEST_P(StmTest, ArmSTMIA) {
  registers_.current.user.gprs.r0 = 0x100u;
  ArmSTMIA(&registers_, memory_, REGISTER_R0, GetParam());

  ValidateMemoryContentsAscending(0x100u, REGISTER_R0, 0x100u, GetParam());
  ValidateRegisters(REGISTER_R0, 0x100u);
}

TEST_P(StmTest, ArmSTMIB) {
  registers_.current.user.gprs.r0 = 0xFCu;
  ArmSTMIB(&registers_, memory_, REGISTER_R0, GetParam());

  ValidateMemoryContentsAscending(0x100u, REGISTER_R0, 0xFCu, GetParam());
  ValidateRegisters(REGISTER_R0, 0xFCu);
}

TEST_P(StmTest, ArmSTMIAW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  registers_.current.user.gprs.r0 = 0x100u;
  ArmSTMIAW(&registers_, memory_, REGISTER_R0, GetParam());

  uint32_t end_address;
  if (GetParam() == 0u) {
    end_address = 0x100u + 0x40u;
  } else {
    end_address = 0x100u + __builtin_popcount(GetParam()) * 4u;
  }

  ValidateMemoryContentsAscending(0x100u, REGISTER_R0, end_address, GetParam());
  ValidateRegisters(REGISTER_R0, end_address);
}

TEST_P(StmTest, ArmSTMIBW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  registers_.current.user.gprs.r0 = 0xFCu;
  ArmSTMIBW(&registers_, memory_, REGISTER_R0, GetParam());

  uint32_t end_address;
  if (GetParam() == 0u) {
    end_address = 0xFCu + 0x40u;
  } else {
    end_address = 0xFCu + __builtin_popcount(GetParam()) * 4u;
  }

  ValidateMemoryContentsAscending(0x100u, REGISTER_R0, end_address, GetParam());
  ValidateRegisters(REGISTER_R0, end_address);
}

TEST_P(StmTest, ArmSTMSDA) {
  registers_.current.user.gprs.r0 = 0x13Cu;
  registers_.current.user.cpsr.mode = MODE_USR;
  ArmSTMSDA(&registers_, memory_, REGISTER_R0, GetParam());

  ValidateMemoryContentsDescending(0x13Cu, REGISTER_R0, 0x13Cu, GetParam());
  ValidateRegisters(REGISTER_R0, 0x13Cu);
}

TEST_P(StmTest, ArmSTMSDB) {
  registers_.current.user.gprs.r0 = 0x140u;
  registers_.current.user.cpsr.mode = MODE_USR;
  ArmSTMSDB(&registers_, memory_, REGISTER_R0, GetParam());

  ValidateMemoryContentsDescending(0x13Cu, REGISTER_R0, 0x140u, GetParam());
  ValidateRegisters(REGISTER_R0, 0x140u);
}

TEST_P(StmTest, ArmSTMSDAW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  registers_.current.user.gprs.r0 = 0x13Cu;
  registers_.current.user.cpsr.mode = MODE_USR;
  ArmSTMSDAW(&registers_, memory_, REGISTER_R0, GetParam());

  uint32_t end_address;
  if (GetParam() == 0u) {
    end_address = 0x13Cu - 0x40u;
  } else {
    end_address = 0x13Cu - __builtin_popcount(GetParam()) * 4u;
  }

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
  ArmSTMSDBW(&registers_, memory_, REGISTER_R0, GetParam());

  uint32_t end_address;
  if (GetParam() == 0u) {
    end_address = 0x140u - 0x40u;
  } else {
    end_address = 0x140u - __builtin_popcount(GetParam()) * 4u;
  }

  ValidateMemoryContentsDescending(0x13Cu, REGISTER_R0, end_address,
                                   GetParam());
  ValidateRegisters(REGISTER_R0, end_address);
}

TEST_P(StmTest, ArmSTMSIA) {
  registers_.current.user.gprs.r0 = 0x100u;
  registers_.current.user.cpsr.mode = MODE_USR;
  ArmSTMSIA(&registers_, memory_, REGISTER_R0, GetParam());

  ValidateMemoryContentsAscending(0x100u, REGISTER_R0, 0x100u, GetParam());
  ValidateRegisters(REGISTER_R0, 0x100u);
}

TEST_P(StmTest, ArmSTMSIB) {
  registers_.current.user.gprs.r0 = 0xFCu;
  registers_.current.user.cpsr.mode = MODE_USR;
  ArmSTMSIB(&registers_, memory_, REGISTER_R0, GetParam());

  ValidateMemoryContentsAscending(0x100u, REGISTER_R0, 0xFCu, GetParam());
  ValidateRegisters(REGISTER_R0, 0xFCu);
}

TEST_P(StmTest, ArmSTMSIAW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  registers_.current.user.gprs.r0 = 0x100u;
  registers_.current.user.cpsr.mode = MODE_USR;
  ArmSTMSIAW(&registers_, memory_, REGISTER_R0, GetParam());

  uint32_t end_address;
  if (GetParam() == 0u) {
    end_address = 0x100u + 0x40u;
  } else {
    end_address = 0x100u + __builtin_popcount(GetParam()) * 4u;
  }

  ValidateMemoryContentsAscending(0x100u, REGISTER_R0, end_address, GetParam());
  ValidateRegisters(REGISTER_R0, end_address);
}

TEST_P(StmTest, ArmSTMSIBW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  registers_.current.user.gprs.r0 = 0xFCu;
  registers_.current.user.cpsr.mode = MODE_USR;
  ArmSTMSIBW(&registers_, memory_, REGISTER_R0, GetParam());

  uint32_t end_address;
  if (GetParam() == 0u) {
    end_address = 0xFCu + 0x40u;
  } else {
    end_address = 0xFCu + __builtin_popcount(GetParam()) * 4u;
  }

  ValidateMemoryContentsAscending(0x100u, REGISTER_R0, end_address, GetParam());
  ValidateRegisters(REGISTER_R0, end_address);
}

TEST_P(StmTest, SysArmSTMSDA) {
  registers_.current.user.gprs.r0 = 0x13Cu;
  registers_.current.user.cpsr.mode = MODE_SYS;
  ArmSTMSDA(&registers_, memory_, REGISTER_R0, GetParam());

  ValidateMemoryContentsDescending(0x13Cu, REGISTER_R0, 0x13Cu, GetParam());
  ValidateRegisters(REGISTER_R0, 0x13Cu);
}

TEST_P(StmTest, SysArmSTMSDB) {
  registers_.current.user.gprs.r0 = 0x140u;
  registers_.current.user.cpsr.mode = MODE_SYS;
  ArmSTMSDB(&registers_, memory_, REGISTER_R0, GetParam());

  ValidateMemoryContentsDescending(0x13Cu, REGISTER_R0, 0x140u, GetParam());
  ValidateRegisters(REGISTER_R0, 0x140u);
}

TEST_P(StmTest, SysArmSTMSDAW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  registers_.current.user.gprs.r0 = 0x13Cu;
  registers_.current.user.cpsr.mode = MODE_SYS;
  ArmSTMSDAW(&registers_, memory_, REGISTER_R0, GetParam());

  uint32_t end_address;
  if (GetParam() == 0u) {
    end_address = 0x13Cu - 0x40u;
  } else {
    end_address = 0x13Cu - __builtin_popcount(GetParam()) * 4u;
  }

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
  ArmSTMSDBW(&registers_, memory_, REGISTER_R0, GetParam());

  uint32_t end_address;
  if (GetParam() == 0u) {
    end_address = 0x140u - 0x40u;
  } else {
    end_address = 0x140u - __builtin_popcount(GetParam()) * 4u;
  }

  ValidateMemoryContentsDescending(0x13Cu, REGISTER_R0, end_address,
                                   GetParam());
  ValidateRegisters(REGISTER_R0, end_address);
}

TEST_P(StmTest, SysArmSTMSIA) {
  registers_.current.user.gprs.r0 = 0x100u;
  registers_.current.user.cpsr.mode = MODE_SYS;
  ArmSTMSIA(&registers_, memory_, REGISTER_R0, GetParam());

  ValidateMemoryContentsAscending(0x100u, REGISTER_R0, 0x100u, GetParam());
  ValidateRegisters(REGISTER_R0, 0x100u);
}

TEST_P(StmTest, SysArmSTMSIB) {
  registers_.current.user.gprs.r0 = 0xFCu;
  registers_.current.user.cpsr.mode = MODE_SYS;
  ArmSTMSIB(&registers_, memory_, REGISTER_R0, GetParam());

  ValidateMemoryContentsAscending(0x100u, REGISTER_R0, 0xFCu, GetParam());
  ValidateRegisters(REGISTER_R0, 0xFCu);
}

TEST_P(StmTest, SysArmSTMSIAW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  registers_.current.user.gprs.r0 = 0x100u;
  registers_.current.user.cpsr.mode = MODE_SYS;
  ArmSTMSIAW(&registers_, memory_, REGISTER_R0, GetParam());

  uint32_t end_address;
  if (GetParam() == 0u) {
    end_address = 0x100u + 0x40u;
  } else {
    end_address = 0x100u + __builtin_popcount(GetParam()) * 4u;
  }

  ValidateMemoryContentsAscending(0x100u, REGISTER_R0, end_address, GetParam());
  ValidateRegisters(REGISTER_R0, end_address);
}

TEST_P(StmTest, SysArmSTMSIBW) {
  if (GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  registers_.current.user.gprs.r0 = 0xFCu;
  registers_.current.user.cpsr.mode = MODE_SYS;
  ArmSTMSIBW(&registers_, memory_, REGISTER_R0, GetParam());

  uint32_t end_address;
  if (GetParam() == 0u) {
    end_address = 0xFCu + 0x40u;
  } else {
    end_address = 0xFCu + __builtin_popcount(GetParam()) * 4u;
  }

  ValidateMemoryContentsAscending(0x100u, REGISTER_R0, end_address, GetParam());
  ValidateRegisters(REGISTER_R0, end_address);
}

TEST_P(StmTest, SvcArmSTMSDA) {
  registers_.current.user.gprs.r0 = 0x13Cu;
  registers_.current.user.cpsr.mode = MODE_USR;
  ArmProgramStatusRegister new_status = registers_.current.user.cpsr;
  new_status.mode = MODE_SVC;
  ArmLoadCPSR(&registers_, new_status);

  ArmSTMSDA(&registers_, memory_, REGISTER_R0, GetParam());
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

  ArmSTMSDB(&registers_, memory_, REGISTER_R0, GetParam());
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

  ArmSTMSDAW(&registers_, memory_, REGISTER_R0, GetParam());
  new_status.mode = MODE_USR;
  ArmLoadCPSR(&registers_, new_status);

  uint32_t end_address;
  if (GetParam() == 0u) {
    end_address = 0x13Cu - 0x40u;
  } else {
    end_address = 0x13Cu - __builtin_popcount(GetParam()) * 4u;
  }

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

  ArmSTMSDBW(&registers_, memory_, REGISTER_R0, GetParam());
  new_status.mode = MODE_USR;
  ArmLoadCPSR(&registers_, new_status);

  uint32_t end_address;
  if (GetParam() == 0u) {
    end_address = 0x140u - 0x40u;
  } else {
    end_address = 0x140u - __builtin_popcount(GetParam()) * 4u;
  }

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

  ArmSTMSIA(&registers_, memory_, REGISTER_R0, GetParam());
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

  ArmSTMSIB(&registers_, memory_, REGISTER_R0, GetParam());
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

  ArmSTMSIAW(&registers_, memory_, REGISTER_R0, GetParam());
  new_status.mode = MODE_USR;
  ArmLoadCPSR(&registers_, new_status);

  uint32_t end_address;
  if (GetParam() == 0u) {
    end_address = 0x100u + 0x40u;
  } else {
    end_address = 0x100u + __builtin_popcount(GetParam()) * 4u;
  }

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

  ArmSTMSIBW(&registers_, memory_, REGISTER_R0, GetParam());
  new_status.mode = MODE_USR;
  ArmLoadCPSR(&registers_, new_status);

  uint32_t end_address;
  if (GetParam() == 0u) {
    end_address = 0xFCu + 0x40u;
  } else {
    end_address = 0xFCu + __builtin_popcount(GetParam()) * 4u;
  }

  ValidateMemoryContentsAscending(0x100u, REGISTER_R0, end_address, GetParam());
  ValidateRegisters(REGISTER_R0, end_address);
}

TEST_P(StmTest, ThumbPUSH) {
  if (GetParam() & (1u << REGISTER_R13) || GetParam() & (1u << REGISTER_PC)) {
    return;
  }

  registers_.current.user.gprs.r13 = 0x140u;
  ThumbPUSH(&registers_, memory_, GetParam());

  uint32_t end_address;
  if (GetParam() == 0u) {
    end_address = 0x140u - 0x40u;
  } else {
    end_address = 0x140u - __builtin_popcount(GetParam()) * 4u;
  }

  ValidateMemoryContentsDescending(0x13Cu, REGISTER_R13, end_address,
                                   GetParam());
  ValidateRegisters(REGISTER_R13, end_address);
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
    registers_.current.user.cpsr.value = 0u;
    registers_.current.user.cpsr.mode = MODE_USR;
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
  ArmAllRegisters registers_;
};

TEST_P(MemoryFailsTest, ArmLDMDA) {
  if (GetParam() == 0u) {
    return;
  }

  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0 = 0x13Cu;
  ArmLDMDA(&registers, memory_, REGISTER_R0, GetParam());
  EXPECT_EQ(0x13Cu, registers.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_P(MemoryFailsTest, ArmLDMDB) {
  if (GetParam() == 0u) {
    return;
  }

  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0 = 0x140u;
  ArmLDMDB(&registers, memory_, REGISTER_R0, GetParam());
  EXPECT_EQ(0x140u, registers.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_P(MemoryFailsTest, ArmLDMDAW) {
  if (GetParam() == 0u || GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0 = 0x13Cu;
  ArmLDMDAW(&registers, memory_, REGISTER_R0, GetParam());
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
  ArmLDMDBW(&registers, memory_, REGISTER_R0, GetParam());
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
  ArmLDMIA(&registers, memory_, REGISTER_R0, GetParam());
  EXPECT_EQ(0x100u, registers.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_P(MemoryFailsTest, ArmLDMIB) {
  if (GetParam() == 0u) {
    return;
  }

  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0 = 0xFCu;
  ArmLDMIB(&registers, memory_, REGISTER_R0, GetParam());
  EXPECT_EQ(0xFCu, registers.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_P(MemoryFailsTest, ArmLDMIAW) {
  if (GetParam() == 0u || GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0 = 0x100u;
  ArmLDMIAW(&registers, memory_, REGISTER_R0, GetParam());
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
  ArmLDMIBW(&registers, memory_, REGISTER_R0, GetParam());
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
  ArmLDMSDA(&registers, memory_, REGISTER_R0, GetParam());
  EXPECT_EQ(0x13Cu, registers.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_P(MemoryFailsTest, ArmLDMSDB) {
  if (GetParam() == 0u) {
    return;
  }

  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0 = 0x140u;
  ArmLDMSDB(&registers, memory_, REGISTER_R0, GetParam());
  EXPECT_EQ(0x140u, registers.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_P(MemoryFailsTest, ArmLDMSDAW) {
  if (GetParam() == 0u || GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0 = 0x13Cu;
  ArmLDMSDAW(&registers, memory_, REGISTER_R0, GetParam());
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
  ArmLDMSDBW(&registers, memory_, REGISTER_R0, GetParam());
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
  ArmLDMSIA(&registers, memory_, REGISTER_R0, GetParam());
  EXPECT_EQ(0x100u, registers.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_P(MemoryFailsTest, ArmLDMSIB) {
  if (GetParam() == 0u) {
    return;
  }

  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r0 = 0xFCu;
  ArmLDMSIB(&registers, memory_, REGISTER_R0, GetParam());
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
  ArmLDMSIAW(&registers, memory_, REGISTER_R0, GetParam());
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
  ArmLDMSIBW(&registers, memory_, REGISTER_R0, GetParam());
  uint32_t end_address = 0xFCu + __builtin_popcount(GetParam()) * 4u;
  EXPECT_EQ(end_address, registers.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_P(MemoryFailsTest, ArmSTMDA) {
  if (GetParam() == 0u) {
    return;
  }

  registers_.current.user.gprs.r0 = 0x13Cu;
  ArmSTMDA(&registers_, memory_, REGISTER_R0, GetParam());
  EXPECT_EQ(0x13Cu, registers_.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_P(MemoryFailsTest, ArmSTMDB) {
  if (GetParam() == 0u) {
    return;
  }

  registers_.current.user.gprs.r0 = 0x140u;
  ArmSTMDB(&registers_, memory_, REGISTER_R0, GetParam());
  EXPECT_EQ(0x140u, registers_.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_P(MemoryFailsTest, ArmSTMDAW) {
  if (GetParam() == 0u || GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  registers_.current.user.gprs.r0 = 0x13Cu;
  ArmSTMDAW(&registers_, memory_, REGISTER_R0, GetParam());
  uint32_t end_address = 0x13Cu - __builtin_popcount(GetParam()) * 4u;
  EXPECT_EQ(end_address, registers_.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_P(MemoryFailsTest, ArmSTMDBW) {
  if (GetParam() == 0u || GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  registers_.current.user.gprs.r0 = 0x140u;
  ArmSTMDBW(&registers_, memory_, REGISTER_R0, GetParam());
  uint32_t end_address = 0x140u - __builtin_popcount(GetParam()) * 4u;
  EXPECT_EQ(end_address, registers_.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_P(MemoryFailsTest, ArmSTMIA) {
  if (GetParam() == 0u) {
    return;
  }

  registers_.current.user.gprs.r0 = 0x100u;
  ArmSTMIA(&registers_, memory_, REGISTER_R0, GetParam());
  EXPECT_EQ(0x100u, registers_.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_P(MemoryFailsTest, ArmSTMIB) {
  if (GetParam() == 0u) {
    return;
  }

  registers_.current.user.gprs.r0 = 0xFCu;
  ArmSTMIB(&registers_, memory_, REGISTER_R0, GetParam());
  EXPECT_EQ(0xFCu, registers_.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_P(MemoryFailsTest, ArmSTMIAW) {
  if (GetParam() == 0u || GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  registers_.current.user.gprs.r0 = 0x100u;
  ArmSTMIAW(&registers_, memory_, REGISTER_R0, GetParam());
  uint32_t end_address = 0x100u + __builtin_popcount(GetParam()) * 4u;
  EXPECT_EQ(end_address, registers_.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_P(MemoryFailsTest, ArmSTMIBW) {
  if (GetParam() == 0u || GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  registers_.current.user.gprs.r0 = 0xFCu;
  ArmSTMIBW(&registers_, memory_, REGISTER_R0, GetParam());
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
  ArmSTMSDA(&registers_, memory_, REGISTER_R0, GetParam());
  EXPECT_EQ(0x13Cu, registers_.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_P(MemoryFailsTest, ArmSTMSDB) {
  if (GetParam() == 0u) {
    return;
  }

  registers_.current.user.gprs.r0 = 0x140u;
  registers_.current.user.cpsr.mode = MODE_USR;
  ArmSTMSDB(&registers_, memory_, REGISTER_R0, GetParam());
  EXPECT_EQ(0x140u, registers_.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_P(MemoryFailsTest, ArmSTMSDAW) {
  if (GetParam() == 0u || GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  registers_.current.user.gprs.r0 = 0x13Cu;
  registers_.current.user.cpsr.mode = MODE_USR;
  ArmSTMSDAW(&registers_, memory_, REGISTER_R0, GetParam());
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
  ArmSTMSDBW(&registers_, memory_, REGISTER_R0, GetParam());
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
  ArmSTMSIA(&registers_, memory_, REGISTER_R0, GetParam());
  EXPECT_EQ(0x100u, registers_.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_P(MemoryFailsTest, ArmSTMSIB) {
  if (GetParam() == 0u) {
    return;
  }

  registers_.current.user.gprs.r0 = 0xFCu;
  registers_.current.user.cpsr.mode = MODE_USR;
  ArmSTMSIB(&registers_, memory_, REGISTER_R0, GetParam());
  EXPECT_EQ(0xFCu, registers_.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_P(MemoryFailsTest, ArmSTMSIAW) {
  if (GetParam() == 0u || GetParam() & (1u << REGISTER_R0)) {
    return;
  }

  registers_.current.user.gprs.r0 = 0x100u;
  registers_.current.user.cpsr.mode = MODE_USR;
  ArmSTMSIAW(&registers_, memory_, REGISTER_R0, GetParam());
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
  ArmSTMSIBW(&registers_, memory_, REGISTER_R0, GetParam());
  uint32_t end_address = 0xFCu + __builtin_popcount(GetParam()) * 4u;
  EXPECT_EQ(end_address, registers_.current.user.gprs.r0);

  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

TEST_P(MemoryFailsTest, ThumbPOP) {
  if (GetParam() == 0u || GetParam() & (1u << REGISTER_R13)) {
    return;
  }

  auto registers = CreateArmAllRegistersInMode();

  registers.current.user.gprs.r13 = 0x100u;
  ThumbPOP(&registers, memory_, GetParam());
  // Writeback is overwritten by exception

  EXPECT_TRUE(ArmIsDataAbort(registers));
}

TEST_P(MemoryFailsTest, ThumbPUSH) {
  if (GetParam() == 0u || GetParam() & (1u << REGISTER_R13) ||
      GetParam() & (1u << REGISTER_PC)) {
    return;
  }

  registers_.current.user.gprs.r13 = 0x140u;
  ThumbPUSH(&registers_, memory_, GetParam());
  // Writeback is overwritten by exception

  EXPECT_TRUE(ArmIsDataAbort(registers_));
}

INSTANTIATE_TEST_SUITE_P(MemoryFailsTestModule, MemoryFailsTest,
                         testing::Range(std::numeric_limits<uint16_t>::min(),
                                        std::numeric_limits<uint16_t>::max(),
                                        51u));
