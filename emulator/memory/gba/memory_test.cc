extern "C" {
#include "emulator/memory/gba/memory.h"

#include "emulator/memory/gba/bios/bios_data.h"
}

#include "googletest/include/gtest/gtest.h"

class GbaMemoryTest : public testing::Test {
 public:
  void SetUp() override {
    ppu_registers_ =
        MemoryAllocate(&ppu_registers_, Load32LEFunc, Load16LEFunc, Load8Func,
                       Store32LEFunc, Store16LEFunc, Store8Func, nullptr);
    ASSERT_NE(nullptr, ppu_registers_);
    sound_registers_ =
        MemoryAllocate(&sound_registers_, Load32LEFunc, Load16LEFunc, Load8Func,
                       Store32LEFunc, Store16LEFunc, Store8Func, nullptr);
    ASSERT_NE(nullptr, sound_registers_);
    dma_registers_ =
        MemoryAllocate(&dma_registers_, Load32LEFunc, Load16LEFunc, Load8Func,
                       Store32LEFunc, Store16LEFunc, Store8Func, nullptr);
    ASSERT_NE(nullptr, dma_registers_);
    timer_registers_ =
        MemoryAllocate(&timer_registers_, Load32LEFunc, Load16LEFunc, Load8Func,
                       Store32LEFunc, Store16LEFunc, Store8Func, nullptr);
    ASSERT_NE(nullptr, timer_registers_);
    peripheral_registers_ = MemoryAllocate(
        &peripheral_registers_, Load32LEFunc, Load16LEFunc, Load8Func,
        Store32LEFunc, Store16LEFunc, Store8Func, nullptr);
    ASSERT_NE(nullptr, peripheral_registers_);
    platform_registers_ = MemoryAllocate(&platform_registers_, Load32LEFunc,
                                         Load16LEFunc, Load8Func, Store32LEFunc,
                                         Store16LEFunc, Store8Func, nullptr);
    ASSERT_NE(nullptr, platform_registers_);
    palette_ =
        MemoryAllocate(&palette_, Load32LEFunc, Load16LEFunc, Load8Func,
                       Store32LEFunc, Store16LEFunc, Store8Func, nullptr);
    ASSERT_NE(nullptr, palette_);
    vram_ = MemoryAllocate(&vram_, Load32LEFunc, Load16LEFunc, Load8Func,
                           Store32LEFunc, Store16LEFunc, Store8Func, nullptr);
    ASSERT_NE(nullptr, vram_);
    oam_ = MemoryAllocate(&oam_, Load32LEFunc, Load16LEFunc, Load8Func,
                          Store32LEFunc, Store16LEFunc, Store8Func, nullptr);
    ASSERT_NE(nullptr, oam_);
    game_ = MemoryAllocate(&game_, Load32LEFunc, Load16LEFunc, Load8Func,
                           Store32LEFunc, Store16LEFunc, Store8Func, nullptr);
    ASSERT_NE(nullptr, game_);
    sram_ = MemoryAllocate(&sram_, Load32LEFunc, Load16LEFunc, Load8Func,
                           Store32LEFunc, Store16LEFunc, Store8Func, nullptr);
    ASSERT_NE(nullptr, sram_);
    memory_ = GbaMemoryAllocate(ppu_registers_, sound_registers_,
                                dma_registers_, timer_registers_,
                                peripheral_registers_, platform_registers_,
                                palette_, vram_, oam_, game_, sram_);
    ASSERT_NE(nullptr, memory_);
  }

  void TearDown() override {
    // banks_ are owned by memory_ and do not need to be freed
    MemoryFree(memory_);
  }

  static bool Load32LEFunc(const void* context, uint32_t address,
                           uint32_t* value) {
    EXPECT_EQ(expected_bank_, context);
    EXPECT_EQ(expected_address_, address);
    *value = expected32_;
    return expected_response_;
  }

  static bool Load16LEFunc(const void* context, uint32_t address,
                           uint16_t* value) {
    EXPECT_EQ(expected_bank_, context);
    EXPECT_EQ(expected_address_, address);
    *value = expected16_;
    return expected_response_;
  }

  static bool Load8Func(const void* context, uint32_t address, uint8_t* value) {
    EXPECT_EQ(expected_bank_, context);
    EXPECT_EQ(expected_address_, address);
    *value = expected8_;
    return expected_response_;
  }

  static bool Store32LEFunc(void* context, uint32_t address, uint32_t value) {
    EXPECT_EQ(expected_bank_, context);
    EXPECT_EQ(expected_address_, address);
    EXPECT_EQ(expected32_, value);
    return expected_response_;
  }

  static bool Store16LEFunc(void* context, uint32_t address, uint16_t value) {
    EXPECT_EQ(expected_bank_, context);
    EXPECT_EQ(expected_address_, address);
    EXPECT_EQ(expected16_, value);
    return expected_response_;
  }

  static bool Store8Func(void* context, uint32_t address, uint8_t value) {
    EXPECT_EQ(expected_bank_, context);
    EXPECT_EQ(expected_address_, address);
    EXPECT_EQ(expected8_, value);
    return expected_response_;
  }

  static void TestIoRegisterAddress(Memory** bank, uint32_t start,
                                    uint32_t end) {
    for (uint32_t addr = start; addr < end; addr++) {
      expected_bank_ = bank;
      expected_address_ = addr - start;

      if (addr % 4u == 0u) {
        expected32_ = addr;
        expected_response_ = true;

        uint32_t value;
        EXPECT_TRUE(Store32LE(memory_, addr, expected32_));
        EXPECT_TRUE(Load32LE(memory_, addr, &value));
        EXPECT_EQ(expected32_, value);

        expected_response_ = false;
        EXPECT_TRUE(Store32LE(memory_, addr, expected32_));
        EXPECT_TRUE(Load32LE(memory_, addr, &value));
        EXPECT_EQ(0u, value);
      }

      if (addr % 2u == 0) {
        expected16_ = (uint16_t)addr;
        expected_response_ = true;

        uint16_t value;
        EXPECT_TRUE(Store16LE(memory_, addr, expected16_));
        EXPECT_TRUE(Load16LE(memory_, addr, &value));
        EXPECT_EQ(expected16_, value);

        expected_response_ = false;
        EXPECT_TRUE(Store16LE(memory_, addr, expected16_));
        EXPECT_TRUE(Load16LE(memory_, addr, &value));
        EXPECT_EQ(0u, value);
      }

      expected8_ = (uint8_t)addr;
      expected_response_ = true;

      uint8_t value;
      EXPECT_TRUE(Store8(memory_, addr, expected8_));
      EXPECT_TRUE(Load8(memory_, addr, &value));
      EXPECT_EQ(expected8_, value);

      expected_response_ = false;
      EXPECT_TRUE(Store8(memory_, addr, expected8_));
      EXPECT_TRUE(Load8(memory_, addr, &value));
      EXPECT_EQ(0u, value);
    }
  }

 protected:
  static Memory* ppu_registers_;
  static Memory* sound_registers_;
  static Memory* dma_registers_;
  static Memory* timer_registers_;
  static Memory* peripheral_registers_;
  static Memory* platform_registers_;
  static Memory* palette_;
  static Memory* vram_;
  static Memory* oam_;
  static Memory* game_;
  static Memory* sram_;
  static Memory* memory_;

  static Memory** expected_bank_;
  static uint32_t expected_address_;
  static uint32_t expected32_;
  static uint16_t expected16_;
  static uint8_t expected8_;
  static bool expected_response_;
};

Memory* GbaMemoryTest::ppu_registers_;
Memory* GbaMemoryTest::sound_registers_;
Memory* GbaMemoryTest::dma_registers_;
Memory* GbaMemoryTest::timer_registers_;
Memory* GbaMemoryTest::peripheral_registers_;
Memory* GbaMemoryTest::platform_registers_;
Memory* GbaMemoryTest::palette_;
Memory* GbaMemoryTest::vram_;
Memory* GbaMemoryTest::oam_;
Memory* GbaMemoryTest::game_;
Memory* GbaMemoryTest::sram_;
Memory* GbaMemoryTest::memory_;
Memory** GbaMemoryTest::expected_bank_;
uint32_t GbaMemoryTest::expected_address_;
uint32_t GbaMemoryTest::expected32_;
uint16_t GbaMemoryTest::expected16_;
uint8_t GbaMemoryTest::expected8_;
bool GbaMemoryTest::expected_response_;

TEST_F(GbaMemoryTest, BiosBank) {
  for (uint32_t addr = 0x00000000u; addr < 0x02000000u; addr++) {
    if (addr % 4u == 0u) {
      uint32_t value;
      EXPECT_TRUE(Store32LE(memory_, addr, 0xFFFFFFFFu));
      EXPECT_TRUE(Load32LE(memory_, addr, &value));

      if (addr <= bios_size - 4u) {
        uint32_t* ptr = (uint32_t*)(void*)&bios_data[addr];
        EXPECT_EQ(*ptr, value);
      } else {
        EXPECT_EQ(0u, value);
      }
    }

    if (addr % 2u == 0) {
      uint16_t value;
      EXPECT_TRUE(Store16LE(memory_, addr, 0xFFFFu));
      EXPECT_TRUE(Load16LE(memory_, addr, &value));

      if (addr <= bios_size - 2u) {
        uint16_t* ptr = (uint16_t*)(void*)&bios_data[addr];
        EXPECT_EQ(*ptr, value);
      } else {
        EXPECT_EQ(0u, value);
      }
    }

    uint8_t value;
    EXPECT_TRUE(Store8(memory_, addr, 0xFFu));
    EXPECT_TRUE(Load8(memory_, addr, &value));

    if (addr < bios_size) {
      EXPECT_EQ(bios_data[addr], value);
    } else {
      EXPECT_EQ(0u, value);
    }
  }
}

TEST_F(GbaMemoryTest, WRamBank) {
  for (uint32_t addr = 0x2040000u; addr < 0x03000000u; addr++) {
    if (addr % 4u == 0u) {
      uint32_t value;
      EXPECT_TRUE(Store32LE(memory_, addr, addr));
      EXPECT_TRUE(Load32LE(memory_, addr, &value));
      EXPECT_EQ(addr, value);
      EXPECT_TRUE(Load32LE(memory_, addr & 0x203FFFFu, &value));
      EXPECT_EQ(addr, value);
    }

    if (addr % 2u == 0) {
      uint16_t value;
      EXPECT_TRUE(Store16LE(memory_, addr, (uint16_t)addr));
      EXPECT_TRUE(Load16LE(memory_, addr, &value));
      EXPECT_EQ((uint16_t)addr, value);
      EXPECT_TRUE(Load16LE(memory_, addr & 0x203FFFFu, &value));
      EXPECT_EQ((uint16_t)addr, value);
    }

    uint8_t value;
    EXPECT_TRUE(Store8(memory_, addr, (uint8_t)addr));
    EXPECT_TRUE(Load8(memory_, addr, &value));
    EXPECT_EQ((uint8_t)addr, value);
    EXPECT_TRUE(Load8(memory_, addr & 0x203FFFFu, &value));
    EXPECT_EQ((uint8_t)addr, value);
  }
}

TEST_F(GbaMemoryTest, IRamBank) {
  for (uint32_t addr = 0x03000000u; addr < 0x04000000u; addr++) {
    if (addr % 4u == 0u) {
      uint32_t value;
      EXPECT_TRUE(Store32LE(memory_, addr, addr));
      EXPECT_TRUE(Load32LE(memory_, addr, &value));
      EXPECT_EQ(addr, value);
      EXPECT_TRUE(Load32LE(memory_, addr & 0x03007FFFu, &value));
      EXPECT_EQ(addr, value);
    }

    if (addr % 2u == 0) {
      uint16_t value;
      EXPECT_TRUE(Store16LE(memory_, addr, (uint16_t)addr));
      EXPECT_TRUE(Load16LE(memory_, addr, &value));
      EXPECT_EQ((uint16_t)addr, value);
      EXPECT_TRUE(Load16LE(memory_, addr & 0x03007FFFu, &value));
      EXPECT_EQ((uint16_t)addr, value);
    }

    uint8_t value;
    EXPECT_TRUE(Store8(memory_, addr, (uint8_t)addr));
    EXPECT_TRUE(Load8(memory_, addr, &value));
    EXPECT_EQ((uint8_t)addr, value);
    EXPECT_TRUE(Load8(memory_, addr & 0x03007FFFu, &value));
    EXPECT_EQ((uint8_t)addr, value);
  }
}

TEST_F(GbaMemoryTest, IoBank) {
  TestIoRegisterAddress(&ppu_registers_, 0x4000000u, 0x4000060u);
  TestIoRegisterAddress(&sound_registers_, 0x40000060u, 0x40000B0u);
  TestIoRegisterAddress(&dma_registers_, 0x40000B0u, 0x4000100u);
  TestIoRegisterAddress(&timer_registers_, 0x4000100u, 0x4000120u);
  TestIoRegisterAddress(&peripheral_registers_, 0x4000120u, 0x4000200u);
  TestIoRegisterAddress(&platform_registers_, 0x4000200u, 0x5000000u);
}