extern "C" {
#include "emulator/dma/gba/dma.h"
}

#include "googletest/include/gtest/gtest.h"

#define IE_OFFSET 0x0u
#define IF_OFFSET 0x2u
#define IME_OFFSET 0x8u

#define DMA0SAD_OFFSET 0x00u
#define DMA0DAD_OFFSET 0x04u
#define DMA0CNT_L_OFFSET 0x08u
#define DMA0CNT_H_OFFSET 0x0Au
#define DMA1SAD_OFFSET 0x0Cu
#define DMA1DAD_OFFSET 0x10u
#define DMA1CNT_L_OFFSET 0x14u
#define DMA1CNT_H_OFFSET 0x16u
#define DMA2SAD_OFFSET 0x18u
#define DMA2DAD_OFFSET 0x1Cu
#define DMA2CNT_L_OFFSET 0x20u
#define DMA2CNT_H_OFFSET 0x22u
#define DMA3SAD_OFFSET 0x24u
#define DMA3DAD_OFFSET 0x28u
#define DMA3CNT_L_OFFSET 0x2Cu
#define DMA3CNT_H_OFFSET 0x2Eu

#define GBA_DMA_ADDR_INCREMENT 0u
#define GBA_DMA_ADDR_DECREMENT 1u
#define GBA_DMA_ADDR_FIXED 2u
#define GBA_DMA_ADDR_INCREMENT_RELOAD 3u

#define GBA_DMA_IMMEDIATE 0u
#define GBA_DMA_VBLANK 1u
#define GBA_DMA_HBLANK 2u
#define GBA_DMA_SPECIAL 3u

class DmaUnitTest : public testing::Test {
 public:
  void SetUp() override {
    raised_ = false;
    InterruptLine *irq =
        InterruptLineAllocate(nullptr, InterruptSetLevel, nullptr);
    ASSERT_NE(irq, nullptr);
    ASSERT_TRUE(GbaPlatformAllocate(irq, &plat_, &plat_regs_));
    ASSERT_TRUE(GbaDmaUnitAllocate(plat_, &dma_unit_, &regs_));

    for (char &c : memory_space_) {
      c = 0;
    }

    memory_ =
        MemoryAllocate(nullptr, Load32LEStatic, Load16LEStatic, Load8Static,
                       Store32LEStatic, Store16LEStatic, Store8Static, nullptr);
    ASSERT_NE(nullptr, memory_);

    ASSERT_TRUE(Store16LE(plat_regs_, IE_OFFSET, 0xFFFFu));
    ASSERT_TRUE(Store16LE(plat_regs_, IF_OFFSET, 0xFFFFu));
    ASSERT_TRUE(Store16LE(plat_regs_, IME_OFFSET, 0xFFFFu));
  }

  void TearDown() override {
    GbaPlatformRelease(plat_);
    MemoryFree(plat_regs_);
    GbaDmaUnitRelease(dma_unit_);
    MemoryFree(regs_);
    MemoryFree(memory_);
  }

 protected:
  static bool Load32LEStatic(const void *context, uint32_t address,
                             uint32_t *value) {
    if (address + sizeof(uint32_t) - 1 >= memory_space_.size()) {
      return false;
    }
    char *data = memory_space_.data() + address;
    *value = *reinterpret_cast<uint32_t *>(data);
    return true;
  }

  static bool Load16LEStatic(const void *context, uint32_t address,
                             uint16_t *value) {
    if (address + sizeof(uint16_t) - 1 >= memory_space_.size()) {
      return false;
    }
    char *data = memory_space_.data() + address;
    *value = *reinterpret_cast<uint16_t *>(data);
    return true;
  }

  static bool Load8Static(const void *context, uint32_t address,
                          uint8_t *value) {
    if (address > memory_space_.size()) {
      return false;
    }
    *value = memory_space_[address];
    return true;
  }

  static bool Store32LEStatic(void *context, uint32_t address, uint32_t value) {
    if (address + sizeof(uint32_t) - 1 >= memory_space_.size()) {
      return false;
    }
    char *data = memory_space_.data() + address;
    *reinterpret_cast<uint32_t *>(data) = value;
    return true;
  }

  static bool Store16LEStatic(void *context, uint32_t address, uint16_t value) {
    if (address + sizeof(uint16_t) - 1 >= memory_space_.size()) {
      return false;
    }
    char *data = memory_space_.data() + address;
    *reinterpret_cast<uint16_t *>(data) = value;
    return true;
  }

  static bool Store8Static(void *context, uint32_t address, uint8_t value) {
    if (address >= memory_space_.size()) {
      return false;
    }
    memory_space_[address] = value;
    return true;
  }

  void EnableDma(uint_fast8_t index, uint32_t source, uint32_t dest,
                 uint16_t transfer_count, bool transfer_words,
                 uint_fast8_t src_addr_mode, uint_fast8_t dest_addr_mode,
                 uint_fast8_t trigger, bool repeat, bool irq) {
    uint32_t offset = index * 12u;
    EXPECT_TRUE(Store32LE(regs_, DMA0SAD_OFFSET + offset, source));
    EXPECT_TRUE(Store32LE(regs_, DMA0DAD_OFFSET + offset, dest));
    EXPECT_TRUE(Store16LE(regs_, DMA0CNT_L_OFFSET + offset, transfer_count));

    uint16_t control = 0x8000;
    control |= dest_addr_mode << 5u;
    control |= src_addr_mode << 7u;
    control |= repeat << 9u;
    control |= transfer_words << 10u;
    control |= trigger << 12u;
    control |= irq << 14u;

    EXPECT_TRUE(Store16LE(regs_, DMA0CNT_H_OFFSET + offset, control));
  }

  void DmaDoSteps(uint32_t steps_to_do) {
    while (steps_to_do != 0) {
      GbaDmaUnitStep(dma_unit_, memory_);
      steps_to_do -= 1;
    }
  }

  void CheckDmaIsDisabled(uint_fast8_t index) {
    uint32_t offset = index * 12u;
    uint16_t control;
    EXPECT_TRUE(Load16LE(regs_, DMA0CNT_H_OFFSET + offset, &control));
    EXPECT_FALSE(control >> 15u);
  }

  void CheckDmaIsEnabled(uint_fast8_t index) {
    uint32_t offset = index * 12u;
    uint16_t control;
    EXPECT_TRUE(Load16LE(regs_, DMA0CNT_H_OFFSET + offset, &control));
    EXPECT_TRUE(control >> 15u);
  }

  static void InterruptSetLevel(void *context, bool raised) {
    raised_ = raised;
  }

  static bool raised_;

  static std::vector<char> memory_space_;
  GbaPlatform *plat_;
  Memory *plat_regs_;
  GbaDmaUnit *dma_unit_;
  Memory *regs_;
  Memory *memory_;
};

std::vector<char> DmaUnitTest::memory_space_(1024u, 0);
bool DmaUnitTest::raised_ = false;

TEST_F(DmaUnitTest, GbaDmaUnitRegistersLoad16LE) {
  uint16_t contents;
  EXPECT_TRUE(Store16LE(regs_, DMA0SAD_OFFSET, 0x1122u));
  EXPECT_FALSE(Load16LE(regs_, DMA0SAD_OFFSET, &contents));
  EXPECT_TRUE(Store16LE(regs_, DMA0SAD_OFFSET + 2u, 0x1122u));
  EXPECT_FALSE(Load16LE(regs_, DMA0SAD_OFFSET + 2u, &contents));
  EXPECT_TRUE(Store16LE(regs_, DMA0DAD_OFFSET, 0x1122u));
  EXPECT_FALSE(Load16LE(regs_, DMA0DAD_OFFSET, &contents));
  EXPECT_TRUE(Store16LE(regs_, DMA0DAD_OFFSET + 2u, 0x1122u));
  EXPECT_FALSE(Load16LE(regs_, DMA0DAD_OFFSET + 2u, &contents));
  EXPECT_TRUE(Store16LE(regs_, DMA0CNT_L_OFFSET, 0x1122u));
  EXPECT_FALSE(Load16LE(regs_, DMA0CNT_L_OFFSET, &contents));
  EXPECT_TRUE(Store16LE(regs_, DMA0CNT_H_OFFSET, 0xAABBu));
  EXPECT_TRUE(Load16LE(regs_, DMA0CNT_H_OFFSET, &contents));
  EXPECT_EQ(0xAABBu, contents);
  EXPECT_TRUE(Store16LE(regs_, DMA1SAD_OFFSET, 0x1122u));
  EXPECT_FALSE(Load16LE(regs_, DMA1SAD_OFFSET, &contents));
  EXPECT_TRUE(Store16LE(regs_, DMA1SAD_OFFSET + 2u, 0x1122u));
  EXPECT_FALSE(Load16LE(regs_, DMA1SAD_OFFSET + 2u, &contents));
  EXPECT_TRUE(Store16LE(regs_, DMA1DAD_OFFSET, 0x1122u));
  EXPECT_FALSE(Load16LE(regs_, DMA1DAD_OFFSET, &contents));
  EXPECT_TRUE(Store16LE(regs_, DMA1DAD_OFFSET + 2u, 0x1122u));
  EXPECT_FALSE(Load16LE(regs_, DMA1DAD_OFFSET + 2u, &contents));
  EXPECT_TRUE(Store16LE(regs_, DMA1CNT_L_OFFSET, 0x1122u));
  EXPECT_FALSE(Load16LE(regs_, DMA1CNT_L_OFFSET, &contents));
  EXPECT_TRUE(Store16LE(regs_, DMA1CNT_H_OFFSET, 0xAABBu));
  EXPECT_TRUE(Load16LE(regs_, DMA1CNT_H_OFFSET, &contents));
  EXPECT_EQ(0xAABBu, contents);
  EXPECT_TRUE(Store16LE(regs_, DMA2SAD_OFFSET, 0x1122u));
  EXPECT_FALSE(Load16LE(regs_, DMA2SAD_OFFSET, &contents));
  EXPECT_TRUE(Store16LE(regs_, DMA2SAD_OFFSET + 2u, 0x1122u));
  EXPECT_FALSE(Load16LE(regs_, DMA2SAD_OFFSET + 2u, &contents));
  EXPECT_TRUE(Store16LE(regs_, DMA2DAD_OFFSET, 0x1122u));
  EXPECT_FALSE(Load16LE(regs_, DMA2DAD_OFFSET, &contents));
  EXPECT_TRUE(Store16LE(regs_, DMA2DAD_OFFSET + 2u, 0x1122u));
  EXPECT_FALSE(Load16LE(regs_, DMA2DAD_OFFSET + 2u, &contents));
  EXPECT_TRUE(Store16LE(regs_, DMA2CNT_L_OFFSET, 0x1122u));
  EXPECT_FALSE(Load16LE(regs_, DMA2CNT_L_OFFSET, &contents));
  EXPECT_TRUE(Store16LE(regs_, DMA2CNT_H_OFFSET, 0xAABBu));
  EXPECT_TRUE(Load16LE(regs_, DMA2CNT_H_OFFSET, &contents));
  EXPECT_EQ(0xAABBu, contents);
  EXPECT_TRUE(Store16LE(regs_, DMA3SAD_OFFSET, 0x1122u));
  EXPECT_FALSE(Load16LE(regs_, DMA3SAD_OFFSET, &contents));
  EXPECT_TRUE(Store16LE(regs_, DMA3SAD_OFFSET + 2u, 0x1122u));
  EXPECT_FALSE(Load16LE(regs_, DMA3SAD_OFFSET + 2u, &contents));
  EXPECT_TRUE(Store16LE(regs_, DMA3DAD_OFFSET, 0x1122u));
  EXPECT_FALSE(Load16LE(regs_, DMA3DAD_OFFSET, &contents));
  EXPECT_TRUE(Store16LE(regs_, DMA3DAD_OFFSET + 2u, 0x1122u));
  EXPECT_FALSE(Load16LE(regs_, DMA3DAD_OFFSET + 2u, &contents));
  EXPECT_TRUE(Store16LE(regs_, DMA3CNT_L_OFFSET, 0x1122u));
  EXPECT_FALSE(Load16LE(regs_, DMA3CNT_L_OFFSET, &contents));
  EXPECT_TRUE(Store16LE(regs_, DMA3CNT_H_OFFSET, 0xAABBu));
  EXPECT_TRUE(Load16LE(regs_, DMA3CNT_H_OFFSET, &contents));
  EXPECT_EQ(0xAABBu, contents);

  // Out of Bounds
  EXPECT_TRUE(Store16LE(regs_, DMA3CNT_H_OFFSET + 2u, 0xAABBu));
  EXPECT_FALSE(Load16LE(regs_, DMA3CNT_H_OFFSET + 2u, &contents));
}

TEST_F(DmaUnitTest, GbaDmaUnitRegistersLoad8) {
  uint8_t contents;
  EXPECT_TRUE(Store8(regs_, DMA0SAD_OFFSET, 0x11u));
  EXPECT_FALSE(Load8(regs_, DMA0SAD_OFFSET, &contents));
  EXPECT_TRUE(Store8(regs_, DMA0SAD_OFFSET + 1u, 0x22u));
  EXPECT_FALSE(Load8(regs_, DMA0SAD_OFFSET + 1u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA0SAD_OFFSET + 2u, 0x11u));
  EXPECT_FALSE(Load8(regs_, DMA0SAD_OFFSET + 2u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA0SAD_OFFSET + 3u, 0x22u));
  EXPECT_FALSE(Load8(regs_, DMA0SAD_OFFSET + 3u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA0DAD_OFFSET, 0x11u));
  EXPECT_FALSE(Load8(regs_, DMA0DAD_OFFSET, &contents));
  EXPECT_TRUE(Store8(regs_, DMA0DAD_OFFSET + 1u, 0x22u));
  EXPECT_FALSE(Load8(regs_, DMA0DAD_OFFSET + 1u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA0DAD_OFFSET + 2u, 0x11u));
  EXPECT_FALSE(Load8(regs_, DMA0DAD_OFFSET + 2u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA0DAD_OFFSET + 3u, 0x22u));
  EXPECT_FALSE(Load8(regs_, DMA0DAD_OFFSET + 3u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA0CNT_L_OFFSET, 0x11u));
  EXPECT_FALSE(Load8(regs_, DMA0CNT_L_OFFSET, &contents));
  EXPECT_TRUE(Store8(regs_, DMA0CNT_L_OFFSET + 1u, 0x22u));
  EXPECT_FALSE(Load8(regs_, DMA0CNT_L_OFFSET + 1u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA0CNT_H_OFFSET, 0xAAu));
  EXPECT_TRUE(Load8(regs_, DMA0CNT_H_OFFSET, &contents));
  EXPECT_EQ(0xAAu, contents);
  EXPECT_TRUE(Store8(regs_, DMA0CNT_H_OFFSET + 1u, 0xBBu));
  EXPECT_TRUE(Load8(regs_, DMA0CNT_H_OFFSET + 1u, &contents));
  EXPECT_EQ(0xBBu, contents);
  EXPECT_TRUE(Store8(regs_, DMA1SAD_OFFSET, 0x11u));
  EXPECT_FALSE(Load8(regs_, DMA1SAD_OFFSET, &contents));
  EXPECT_TRUE(Store8(regs_, DMA1SAD_OFFSET + 1u, 0x22u));
  EXPECT_FALSE(Load8(regs_, DMA1SAD_OFFSET + 1u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA1SAD_OFFSET + 2u, 0x11u));
  EXPECT_FALSE(Load8(regs_, DMA1SAD_OFFSET + 2u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA1SAD_OFFSET + 3u, 0x22u));
  EXPECT_FALSE(Load8(regs_, DMA1SAD_OFFSET + 3u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA1DAD_OFFSET, 0x11u));
  EXPECT_FALSE(Load8(regs_, DMA1DAD_OFFSET, &contents));
  EXPECT_TRUE(Store8(regs_, DMA1DAD_OFFSET + 1u, 0x22u));
  EXPECT_FALSE(Load8(regs_, DMA1DAD_OFFSET + 1u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA1DAD_OFFSET + 2u, 0x11u));
  EXPECT_FALSE(Load8(regs_, DMA1DAD_OFFSET + 2u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA1DAD_OFFSET + 3u, 0x22u));
  EXPECT_FALSE(Load8(regs_, DMA1DAD_OFFSET + 3u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA1CNT_L_OFFSET, 0x11u));
  EXPECT_FALSE(Load8(regs_, DMA1CNT_L_OFFSET, &contents));
  EXPECT_TRUE(Store8(regs_, DMA1CNT_L_OFFSET + 1u, 0x22u));
  EXPECT_FALSE(Load8(regs_, DMA1CNT_L_OFFSET + 1u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA1CNT_H_OFFSET, 0xAAu));
  EXPECT_TRUE(Load8(regs_, DMA1CNT_H_OFFSET, &contents));
  EXPECT_EQ(0xAAu, contents);
  EXPECT_TRUE(Store8(regs_, DMA1CNT_H_OFFSET + 1u, 0xBBu));
  EXPECT_TRUE(Load8(regs_, DMA1CNT_H_OFFSET + 1u, &contents));
  EXPECT_EQ(0xBBu, contents);
  EXPECT_TRUE(Store8(regs_, DMA2SAD_OFFSET, 0x11u));
  EXPECT_FALSE(Load8(regs_, DMA2SAD_OFFSET, &contents));
  EXPECT_TRUE(Store8(regs_, DMA2SAD_OFFSET + 1u, 0x22u));
  EXPECT_FALSE(Load8(regs_, DMA2SAD_OFFSET + 1u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA2SAD_OFFSET + 2u, 0x11u));
  EXPECT_FALSE(Load8(regs_, DMA2SAD_OFFSET + 2u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA2SAD_OFFSET + 3u, 0x22u));
  EXPECT_FALSE(Load8(regs_, DMA2SAD_OFFSET + 3u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA2DAD_OFFSET, 0x11u));
  EXPECT_FALSE(Load8(regs_, DMA2DAD_OFFSET, &contents));
  EXPECT_TRUE(Store8(regs_, DMA2DAD_OFFSET + 1u, 0x22u));
  EXPECT_FALSE(Load8(regs_, DMA2DAD_OFFSET + 1u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA2DAD_OFFSET + 2u, 0x11u));
  EXPECT_FALSE(Load8(regs_, DMA2DAD_OFFSET + 2u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA2DAD_OFFSET + 3u, 0x22u));
  EXPECT_FALSE(Load8(regs_, DMA2DAD_OFFSET + 3u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA2CNT_L_OFFSET, 0x11u));
  EXPECT_FALSE(Load8(regs_, DMA2CNT_L_OFFSET, &contents));
  EXPECT_TRUE(Store8(regs_, DMA2CNT_L_OFFSET + 1u, 0x22u));
  EXPECT_FALSE(Load8(regs_, DMA2CNT_L_OFFSET + 1u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA2CNT_H_OFFSET, 0xAAu));
  EXPECT_TRUE(Load8(regs_, DMA2CNT_H_OFFSET, &contents));
  EXPECT_EQ(0xAAu, contents);
  EXPECT_TRUE(Store8(regs_, DMA2CNT_H_OFFSET + 1u, 0xBBu));
  EXPECT_TRUE(Load8(regs_, DMA2CNT_H_OFFSET + 1u, &contents));
  EXPECT_EQ(0xBBu, contents);
  EXPECT_TRUE(Store8(regs_, DMA3SAD_OFFSET, 0x11u));
  EXPECT_FALSE(Load8(regs_, DMA3SAD_OFFSET, &contents));
  EXPECT_TRUE(Store8(regs_, DMA3SAD_OFFSET + 1u, 0x22u));
  EXPECT_FALSE(Load8(regs_, DMA3SAD_OFFSET + 1u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA3SAD_OFFSET + 2u, 0x11u));
  EXPECT_FALSE(Load8(regs_, DMA3SAD_OFFSET + 2u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA3SAD_OFFSET + 3u, 0x22u));
  EXPECT_FALSE(Load8(regs_, DMA3SAD_OFFSET + 3u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA3DAD_OFFSET, 0x11u));
  EXPECT_FALSE(Load8(regs_, DMA3DAD_OFFSET, &contents));
  EXPECT_TRUE(Store8(regs_, DMA3DAD_OFFSET + 1u, 0x22u));
  EXPECT_FALSE(Load8(regs_, DMA3DAD_OFFSET + 1u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA3DAD_OFFSET + 2u, 0x11u));
  EXPECT_FALSE(Load8(regs_, DMA3DAD_OFFSET + 2u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA3DAD_OFFSET + 3u, 0x22u));
  EXPECT_FALSE(Load8(regs_, DMA3DAD_OFFSET + 3u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA3CNT_L_OFFSET, 0x11u));
  EXPECT_FALSE(Load8(regs_, DMA3CNT_L_OFFSET, &contents));
  EXPECT_TRUE(Store8(regs_, DMA3CNT_L_OFFSET + 1u, 0x22u));
  EXPECT_FALSE(Load8(regs_, DMA3CNT_L_OFFSET + 1u, &contents));
  EXPECT_TRUE(Store8(regs_, DMA3CNT_H_OFFSET, 0xAAu));
  EXPECT_TRUE(Load8(regs_, DMA3CNT_H_OFFSET, &contents));
  EXPECT_EQ(0xAAu, contents);
  EXPECT_TRUE(Store8(regs_, DMA3CNT_H_OFFSET + 1u, 0xBBu));
  EXPECT_TRUE(Load8(regs_, DMA3CNT_H_OFFSET + 1u, &contents));
  EXPECT_EQ(0xBBu, contents);

  // Out of Bounds
  EXPECT_TRUE(Store8(regs_, DMA3CNT_H_OFFSET + 2u, 0xAAu));
  EXPECT_FALSE(Load8(regs_, DMA3CNT_H_OFFSET + 2u, &contents));
}

TEST_F(DmaUnitTest, GbaDmaUnitRegistersLoad32LE) {
  uint32_t contents;
  EXPECT_TRUE(Store32LE(regs_, DMA0SAD_OFFSET, 0x11223344u));
  EXPECT_FALSE(Load32LE(regs_, DMA0SAD_OFFSET, &contents));
  EXPECT_TRUE(Store32LE(regs_, DMA0DAD_OFFSET, 0x11233442u));
  EXPECT_FALSE(Load32LE(regs_, DMA0DAD_OFFSET, &contents));
  EXPECT_TRUE(Store32LE(regs_, DMA0CNT_L_OFFSET, 0x11223344u));
  EXPECT_TRUE(Load32LE(regs_, DMA0CNT_L_OFFSET, &contents));
  EXPECT_EQ(0x11220000u, contents);
  EXPECT_TRUE(Store32LE(regs_, DMA1SAD_OFFSET, 0x11223344u));
  EXPECT_FALSE(Load32LE(regs_, DMA1SAD_OFFSET, &contents));
  EXPECT_TRUE(Store32LE(regs_, DMA1DAD_OFFSET, 0x11233442u));
  EXPECT_FALSE(Load32LE(regs_, DMA1DAD_OFFSET, &contents));
  EXPECT_TRUE(Store32LE(regs_, DMA1CNT_L_OFFSET, 0x11223344u));
  EXPECT_TRUE(Load32LE(regs_, DMA1CNT_L_OFFSET, &contents));
  EXPECT_EQ(0x11220000u, contents);
  EXPECT_TRUE(Store32LE(regs_, DMA2SAD_OFFSET, 0x11223344u));
  EXPECT_FALSE(Load32LE(regs_, DMA2SAD_OFFSET, &contents));
  EXPECT_TRUE(Store32LE(regs_, DMA2DAD_OFFSET, 0x11233442u));
  EXPECT_FALSE(Load32LE(regs_, DMA2DAD_OFFSET, &contents));
  EXPECT_TRUE(Store32LE(regs_, DMA2CNT_L_OFFSET, 0x11223344u));
  EXPECT_TRUE(Load32LE(regs_, DMA2CNT_L_OFFSET, &contents));
  EXPECT_EQ(0x11220000u, contents);
  EXPECT_TRUE(Store32LE(regs_, DMA3SAD_OFFSET, 0x11223344u));
  EXPECT_FALSE(Load32LE(regs_, DMA3SAD_OFFSET, &contents));
  EXPECT_TRUE(Store32LE(regs_, DMA3DAD_OFFSET, 0x11233442u));
  EXPECT_FALSE(Load32LE(regs_, DMA3DAD_OFFSET, &contents));
  EXPECT_TRUE(Store32LE(regs_, DMA3CNT_L_OFFSET, 0x11223344u));
  EXPECT_TRUE(Load32LE(regs_, DMA3CNT_L_OFFSET, &contents));
  EXPECT_EQ(0x11220000u, contents);

  // Out of Bounds
  EXPECT_TRUE(Store32LE(regs_, DMA3CNT_L_OFFSET + 4u, 0xAABBCCDDu));
  EXPECT_FALSE(Load32LE(regs_, DMA3CNT_L_OFFSET + 4u, &contents));
}

TEST_F(DmaUnitTest, DefaultUnitIsInactive) {
  EXPECT_FALSE(GbaDmaUnitIsActive(dma_unit_));
}

TEST_F(DmaUnitTest, TestDmaIrq0) {
  EXPECT_TRUE(Store32LEStatic(nullptr, 0u, 0x12345678u));
  EnableDma(/*index=*/0u, /*source=*/0u, /*dest=*/16u, /*transfer_count=*/1u,
            /*transfer_words=*/true, /*src_addr_mode=*/GBA_DMA_ADDR_INCREMENT,
            /*dest_addr_mode=*/GBA_DMA_ADDR_INCREMENT,
            /*trigger=*/GBA_DMA_IMMEDIATE, /*repeat=*/false, /*irq=*/true);
  EXPECT_TRUE(GbaDmaUnitIsActive(dma_unit_));
  EXPECT_FALSE(raised_);

  DmaDoSteps(1u);

  EXPECT_FALSE(GbaDmaUnitIsActive(dma_unit_));
  EXPECT_TRUE(raised_);

  uint16_t interrupt_flags;
  EXPECT_TRUE(Load16LE(plat_regs_, IF_OFFSET, &interrupt_flags));
  EXPECT_EQ(1u << 8u, interrupt_flags);

  uint32_t value;
  EXPECT_TRUE(Load32LEStatic(nullptr, 16u, &value));
  EXPECT_EQ(0x12345678u, value);

  CheckDmaIsDisabled(0u);
}

TEST_F(DmaUnitTest, TestDmaIrq1) {
  EXPECT_TRUE(Store32LEStatic(nullptr, 0u, 0x12345678u));
  EnableDma(/*index=*/1u, /*source=*/0u, /*dest=*/16u, /*transfer_count=*/1u,
            /*transfer_words=*/true, /*src_addr_mode=*/GBA_DMA_ADDR_INCREMENT,
            /*dest_addr_mode=*/GBA_DMA_ADDR_INCREMENT,
            /*trigger=*/GBA_DMA_IMMEDIATE, /*repeat=*/false, /*irq=*/true);
  EXPECT_TRUE(GbaDmaUnitIsActive(dma_unit_));
  EXPECT_FALSE(raised_);

  DmaDoSteps(1u);

  EXPECT_FALSE(GbaDmaUnitIsActive(dma_unit_));
  EXPECT_TRUE(raised_);

  uint16_t interrupt_flags;
  EXPECT_TRUE(Load16LE(plat_regs_, IF_OFFSET, &interrupt_flags));
  EXPECT_EQ(1u << 9u, interrupt_flags);

  uint32_t value;
  EXPECT_TRUE(Load32LEStatic(nullptr, 16u, &value));
  EXPECT_EQ(0x12345678u, value);

  CheckDmaIsDisabled(1u);
}

TEST_F(DmaUnitTest, TestDmaIrq2) {
  EXPECT_TRUE(Store32LEStatic(nullptr, 0u, 0x12345678u));
  EnableDma(/*index=*/2u, /*source=*/0u, /*dest=*/16u, /*transfer_count=*/1u,
            /*transfer_words=*/true, /*src_addr_mode=*/GBA_DMA_ADDR_INCREMENT,
            /*dest_addr_mode=*/GBA_DMA_ADDR_INCREMENT,
            /*trigger=*/GBA_DMA_IMMEDIATE, /*repeat=*/false, /*irq=*/true);
  EXPECT_TRUE(GbaDmaUnitIsActive(dma_unit_));
  EXPECT_FALSE(raised_);

  DmaDoSteps(1u);

  EXPECT_FALSE(GbaDmaUnitIsActive(dma_unit_));
  EXPECT_TRUE(raised_);

  uint16_t interrupt_flags;
  EXPECT_TRUE(Load16LE(plat_regs_, IF_OFFSET, &interrupt_flags));
  EXPECT_EQ(1u << 10u, interrupt_flags);

  uint32_t value;
  EXPECT_TRUE(Load32LEStatic(nullptr, 16u, &value));
  EXPECT_EQ(0x12345678u, value);

  CheckDmaIsDisabled(2u);
}

TEST_F(DmaUnitTest, TestDmaIrq3) {
  EXPECT_TRUE(Store32LEStatic(nullptr, 0u, 0x12345678u));
  EnableDma(/*index=*/3u, /*source=*/0u, /*dest=*/16u, /*transfer_count=*/1u,
            /*transfer_words=*/true, /*src_addr_mode=*/GBA_DMA_ADDR_INCREMENT,
            /*dest_addr_mode=*/GBA_DMA_ADDR_INCREMENT,
            /*trigger=*/GBA_DMA_IMMEDIATE, /*repeat=*/false, /*irq=*/true);
  EXPECT_TRUE(GbaDmaUnitIsActive(dma_unit_));
  EXPECT_FALSE(raised_);

  DmaDoSteps(1u);

  EXPECT_FALSE(GbaDmaUnitIsActive(dma_unit_));
  EXPECT_TRUE(raised_);

  uint16_t interrupt_flags;
  EXPECT_TRUE(Load16LE(plat_regs_, IF_OFFSET, &interrupt_flags));
  EXPECT_EQ(1u << 11u, interrupt_flags);

  uint32_t value;
  EXPECT_TRUE(Load32LEStatic(nullptr, 16u, &value));
  EXPECT_EQ(0x12345678u, value);

  CheckDmaIsDisabled(3u);
}

TEST_F(DmaUnitTest, TestDmaPriority) {
  EXPECT_TRUE(Store32LEStatic(nullptr, 0u, 0x12345678u));
  EXPECT_TRUE(Store32LEStatic(nullptr, 4u, 0xFFFFFFFFu));
  EnableDma(/*index=*/0u, /*source=*/0u, /*dest=*/16u, /*transfer_count=*/1u,
            /*transfer_words=*/true, /*src_addr_mode=*/GBA_DMA_ADDR_INCREMENT,
            /*dest_addr_mode=*/GBA_DMA_ADDR_INCREMENT,
            /*trigger=*/GBA_DMA_IMMEDIATE, /*repeat=*/false, /*irq=*/false);
  EnableDma(/*index=*/1u, /*source=*/4u, /*dest=*/20u, /*transfer_count=*/1u,
            /*transfer_words=*/true, /*src_addr_mode=*/GBA_DMA_ADDR_INCREMENT,
            /*dest_addr_mode=*/GBA_DMA_ADDR_INCREMENT,
            /*trigger=*/GBA_DMA_IMMEDIATE, /*repeat=*/false, /*irq=*/false);
  EXPECT_TRUE(GbaDmaUnitIsActive(dma_unit_));

  DmaDoSteps(1u);

  EXPECT_TRUE(GbaDmaUnitIsActive(dma_unit_));
  CheckDmaIsDisabled(0u);
  CheckDmaIsEnabled(1u);

  uint32_t value;
  EXPECT_TRUE(Load32LEStatic(nullptr, 16u, &value));
  EXPECT_EQ(0x12345678u, value);
  EXPECT_TRUE(Load32LEStatic(nullptr, 20u, &value));
  EXPECT_EQ(0u, value);

  DmaDoSteps(1u);

  EXPECT_FALSE(GbaDmaUnitIsActive(dma_unit_));

  EXPECT_TRUE(Load32LEStatic(nullptr, 16u, &value));
  EXPECT_EQ(0x12345678u, value);
  EXPECT_TRUE(Load32LEStatic(nullptr, 20u, &value));
  EXPECT_EQ(0xFFFFFFFFu, value);

  CheckDmaIsDisabled(0u);
  CheckDmaIsDisabled(1u);
}

TEST_F(DmaUnitTest, TestDmaIncrementSrcIncrementDest) {
  EXPECT_TRUE(Store32LEStatic(nullptr, 0u, 0x12345678u));
  EXPECT_TRUE(Store32LEStatic(nullptr, 4u, 0xFFFFFFFFu));
  EnableDma(/*index=*/0u, /*source=*/0u, /*dest=*/16u, /*transfer_count=*/2u,
            /*transfer_words=*/true, /*src_addr_mode=*/GBA_DMA_ADDR_INCREMENT,
            /*dest_addr_mode=*/GBA_DMA_ADDR_INCREMENT,
            /*trigger=*/GBA_DMA_IMMEDIATE, /*repeat=*/false, /*irq=*/false);
  EXPECT_TRUE(GbaDmaUnitIsActive(dma_unit_));

  DmaDoSteps(1u);
  EXPECT_TRUE(GbaDmaUnitIsActive(dma_unit_));
  CheckDmaIsEnabled(0u);
  DmaDoSteps(1u);
  EXPECT_FALSE(GbaDmaUnitIsActive(dma_unit_));
  CheckDmaIsDisabled(0u);

  uint32_t value;
  EXPECT_TRUE(Load32LEStatic(nullptr, 16u, &value));
  EXPECT_EQ(0x12345678u, value);
  EXPECT_TRUE(Load32LEStatic(nullptr, 20u, &value));
  EXPECT_EQ(0xFFFFFFFFu, value);
}

TEST_F(DmaUnitTest, TestDmaDecrementSrcIncrementDest) {
  EXPECT_TRUE(Store32LEStatic(nullptr, 0u, 0x12345678u));
  EXPECT_TRUE(Store32LEStatic(nullptr, 4u, 0xFFFFFFFFu));
  EnableDma(/*index=*/0u, /*source=*/4u, /*dest=*/16u, /*transfer_count=*/2u,
            /*transfer_words=*/true, /*src_addr_mode=*/GBA_DMA_ADDR_DECREMENT,
            /*dest_addr_mode=*/GBA_DMA_ADDR_INCREMENT,
            /*trigger=*/GBA_DMA_IMMEDIATE, /*repeat=*/false, /*irq=*/false);
  EXPECT_TRUE(GbaDmaUnitIsActive(dma_unit_));

  DmaDoSteps(1u);
  EXPECT_TRUE(GbaDmaUnitIsActive(dma_unit_));
  CheckDmaIsEnabled(0u);
  DmaDoSteps(1u);
  EXPECT_FALSE(GbaDmaUnitIsActive(dma_unit_));
  CheckDmaIsDisabled(0u);

  uint32_t value;
  EXPECT_TRUE(Load32LEStatic(nullptr, 16u, &value));
  EXPECT_EQ(0xFFFFFFFFu, value);
  EXPECT_TRUE(Load32LEStatic(nullptr, 20u, &value));
  EXPECT_EQ(0x12345678u, value);
}

TEST_F(DmaUnitTest, TestDmaFixedSrcIncrementDest) {
  EXPECT_TRUE(Store32LEStatic(nullptr, 0u, 0x12345678u));
  EnableDma(/*index=*/0u, /*source=*/0u, /*dest=*/16u, /*transfer_count=*/2u,
            /*transfer_words=*/true, /*src_addr_mode=*/GBA_DMA_ADDR_FIXED,
            /*dest_addr_mode=*/GBA_DMA_ADDR_INCREMENT,
            /*trigger=*/GBA_DMA_IMMEDIATE, /*repeat=*/false, /*irq=*/false);
  EXPECT_TRUE(GbaDmaUnitIsActive(dma_unit_));

  DmaDoSteps(1u);
  EXPECT_TRUE(GbaDmaUnitIsActive(dma_unit_));
  CheckDmaIsEnabled(0u);
  DmaDoSteps(1u);
  EXPECT_FALSE(GbaDmaUnitIsActive(dma_unit_));
  CheckDmaIsDisabled(0u);

  uint32_t value;
  EXPECT_TRUE(Load32LEStatic(nullptr, 16u, &value));
  EXPECT_EQ(0x12345678u, value);
  EXPECT_TRUE(Load32LEStatic(nullptr, 20u, &value));
  EXPECT_EQ(0x12345678u, value);
}

TEST_F(DmaUnitTest, TestDmaIncrementSrcDecrementDest) {
  EXPECT_TRUE(Store32LEStatic(nullptr, 0u, 0x12345678u));
  EXPECT_TRUE(Store32LEStatic(nullptr, 4u, 0xFFFFFFFFu));
  EnableDma(/*index=*/0u, /*source=*/0u, /*dest=*/20u, /*transfer_count=*/2u,
            /*transfer_words=*/true, /*src_addr_mode=*/GBA_DMA_ADDR_INCREMENT,
            /*dest_addr_mode=*/GBA_DMA_ADDR_DECREMENT,
            /*trigger=*/GBA_DMA_IMMEDIATE, /*repeat=*/false, /*irq=*/false);
  EXPECT_TRUE(GbaDmaUnitIsActive(dma_unit_));

  DmaDoSteps(1u);
  EXPECT_TRUE(GbaDmaUnitIsActive(dma_unit_));
  CheckDmaIsEnabled(0u);
  DmaDoSteps(1u);
  EXPECT_FALSE(GbaDmaUnitIsActive(dma_unit_));
  CheckDmaIsDisabled(0u);

  uint32_t value;
  EXPECT_TRUE(Load32LEStatic(nullptr, 16u, &value));
  EXPECT_EQ(0xFFFFFFFFu, value);
  EXPECT_TRUE(Load32LEStatic(nullptr, 20u, &value));
  EXPECT_EQ(0x12345678u, value);
}

TEST_F(DmaUnitTest, TestDmaIncrementSrcFixedDest) {
  EXPECT_TRUE(Store32LEStatic(nullptr, 0u, 0x12345678u));
  EXPECT_TRUE(Store32LEStatic(nullptr, 4u, 0xFFFFFFFFu));
  EnableDma(/*index=*/0u, /*source=*/0u, /*dest=*/16u, /*transfer_count=*/2u,
            /*transfer_words=*/true, /*src_addr_mode=*/GBA_DMA_ADDR_INCREMENT,
            /*dest_addr_mode=*/GBA_DMA_ADDR_FIXED,
            /*trigger=*/GBA_DMA_IMMEDIATE, /*repeat=*/false, /*irq=*/false);
  EXPECT_TRUE(GbaDmaUnitIsActive(dma_unit_));

  DmaDoSteps(1u);
  EXPECT_TRUE(GbaDmaUnitIsActive(dma_unit_));
  CheckDmaIsEnabled(0u);
  DmaDoSteps(1u);
  EXPECT_FALSE(GbaDmaUnitIsActive(dma_unit_));
  CheckDmaIsDisabled(0u);

  uint32_t value;
  EXPECT_TRUE(Load32LEStatic(nullptr, 16u, &value));
  EXPECT_EQ(0xFFFFFFFFu, value);
  EXPECT_TRUE(Load32LEStatic(nullptr, 20u, &value));
  EXPECT_EQ(0u, value);
}

TEST_F(DmaUnitTest, TestDmaIncrementSrcIncrementDestHalfWord) {
  EXPECT_TRUE(Store32LEStatic(nullptr, 0u, 0x1234u));
  EXPECT_TRUE(Store32LEStatic(nullptr, 2u, 0x5678u));
  EXPECT_TRUE(Store32LEStatic(nullptr, 4u, 0xFFFFu));
  EnableDma(/*index=*/0u, /*source=*/0u, /*dest=*/16u, /*transfer_count=*/3u,
            /*transfer_words=*/false, /*src_addr_mode=*/GBA_DMA_ADDR_INCREMENT,
            /*dest_addr_mode=*/GBA_DMA_ADDR_INCREMENT,
            /*trigger=*/GBA_DMA_IMMEDIATE, /*repeat=*/false, /*irq=*/false);
  EXPECT_TRUE(GbaDmaUnitIsActive(dma_unit_));

  DmaDoSteps(1u);
  EXPECT_TRUE(GbaDmaUnitIsActive(dma_unit_));
  CheckDmaIsEnabled(0u);
  DmaDoSteps(1u);
  EXPECT_TRUE(GbaDmaUnitIsActive(dma_unit_));
  CheckDmaIsEnabled(0u);
  DmaDoSteps(1u);
  EXPECT_FALSE(GbaDmaUnitIsActive(dma_unit_));
  CheckDmaIsDisabled(0u);

  uint16_t value;
  EXPECT_TRUE(Load16LEStatic(nullptr, 16u, &value));
  EXPECT_EQ(0x1234u, value);
  EXPECT_TRUE(Load16LEStatic(nullptr, 18u, &value));
  EXPECT_EQ(0x5678u, value);
  EXPECT_TRUE(Load16LEStatic(nullptr, 20u, &value));
  EXPECT_EQ(0xFFFFu, value);
}

TEST_F(DmaUnitTest, TestVBlankDma0) {
  EnableDma(/*index=*/0u, /*source=*/0u, /*dest=*/16u, /*transfer_count=*/3u,
            /*transfer_words=*/false, /*src_addr_mode=*/GBA_DMA_ADDR_INCREMENT,
            /*dest_addr_mode=*/GBA_DMA_ADDR_INCREMENT,
            /*trigger=*/GBA_DMA_VBLANK, /*repeat=*/false, /*irq=*/false);
  EXPECT_FALSE(GbaDmaUnitIsActive(dma_unit_));
  CheckDmaIsEnabled(0u);

  GbaDmaUnitSignalVBlank(dma_unit_);

  EXPECT_TRUE(GbaDmaUnitIsActive(dma_unit_));
  CheckDmaIsEnabled(0u);
}

TEST_F(DmaUnitTest, TestVBlankDma1) {
  EnableDma(/*index=*/1u, /*source=*/0u, /*dest=*/16u, /*transfer_count=*/3u,
            /*transfer_words=*/false, /*src_addr_mode=*/GBA_DMA_ADDR_INCREMENT,
            /*dest_addr_mode=*/GBA_DMA_ADDR_INCREMENT,
            /*trigger=*/GBA_DMA_VBLANK, /*repeat=*/false, /*irq=*/false);
  EXPECT_FALSE(GbaDmaUnitIsActive(dma_unit_));
  CheckDmaIsEnabled(1u);

  GbaDmaUnitSignalVBlank(dma_unit_);

  EXPECT_TRUE(GbaDmaUnitIsActive(dma_unit_));
  CheckDmaIsEnabled(1u);
}

TEST_F(DmaUnitTest, TestVBlankDma2) {
  EnableDma(/*index=*/2u, /*source=*/0u, /*dest=*/16u, /*transfer_count=*/3u,
            /*transfer_words=*/false, /*src_addr_mode=*/GBA_DMA_ADDR_INCREMENT,
            /*dest_addr_mode=*/GBA_DMA_ADDR_INCREMENT,
            /*trigger=*/GBA_DMA_VBLANK, /*repeat=*/false, /*irq=*/false);
  EXPECT_FALSE(GbaDmaUnitIsActive(dma_unit_));
  CheckDmaIsEnabled(2u);

  GbaDmaUnitSignalVBlank(dma_unit_);

  EXPECT_TRUE(GbaDmaUnitIsActive(dma_unit_));
  CheckDmaIsEnabled(2u);
}

TEST_F(DmaUnitTest, TestVBlankDma3) {
  EnableDma(/*index=*/3u, /*source=*/0u, /*dest=*/16u, /*transfer_count=*/3u,
            /*transfer_words=*/false, /*src_addr_mode=*/GBA_DMA_ADDR_INCREMENT,
            /*dest_addr_mode=*/GBA_DMA_ADDR_INCREMENT,
            /*trigger=*/GBA_DMA_VBLANK, /*repeat=*/false, /*irq=*/false);
  EXPECT_FALSE(GbaDmaUnitIsActive(dma_unit_));
  CheckDmaIsEnabled(3u);

  GbaDmaUnitSignalVBlank(dma_unit_);

  EXPECT_TRUE(GbaDmaUnitIsActive(dma_unit_));
  CheckDmaIsEnabled(3u);
}

TEST_F(DmaUnitTest, TestHBlankDma0) {
  EnableDma(/*index=*/0u, /*source=*/0u, /*dest=*/16u, /*transfer_count=*/3u,
            /*transfer_words=*/false, /*src_addr_mode=*/GBA_DMA_ADDR_INCREMENT,
            /*dest_addr_mode=*/GBA_DMA_ADDR_INCREMENT,
            /*trigger=*/GBA_DMA_HBLANK, /*repeat=*/false, /*irq=*/false);
  EXPECT_FALSE(GbaDmaUnitIsActive(dma_unit_));
  CheckDmaIsEnabled(0u);

  GbaDmaUnitSignalHBlank(dma_unit_, 0u);

  EXPECT_TRUE(GbaDmaUnitIsActive(dma_unit_));
  CheckDmaIsEnabled(0u);
}

TEST_F(DmaUnitTest, TestHBlankDma1) {
  EnableDma(/*index=*/1u, /*source=*/0u, /*dest=*/16u, /*transfer_count=*/3u,
            /*transfer_words=*/false, /*src_addr_mode=*/GBA_DMA_ADDR_INCREMENT,
            /*dest_addr_mode=*/GBA_DMA_ADDR_INCREMENT,
            /*trigger=*/GBA_DMA_HBLANK, /*repeat=*/false, /*irq=*/false);
  EXPECT_FALSE(GbaDmaUnitIsActive(dma_unit_));
  CheckDmaIsEnabled(1u);

  GbaDmaUnitSignalHBlank(dma_unit_, 0u);

  EXPECT_TRUE(GbaDmaUnitIsActive(dma_unit_));
  CheckDmaIsEnabled(1u);
}

TEST_F(DmaUnitTest, TestHBlankDma2) {
  EnableDma(/*index=*/2u, /*source=*/0u, /*dest=*/16u, /*transfer_count=*/3u,
            /*transfer_words=*/false, /*src_addr_mode=*/GBA_DMA_ADDR_INCREMENT,
            /*dest_addr_mode=*/GBA_DMA_ADDR_INCREMENT,
            /*trigger=*/GBA_DMA_HBLANK, /*repeat=*/false, /*irq=*/false);
  EXPECT_FALSE(GbaDmaUnitIsActive(dma_unit_));
  CheckDmaIsEnabled(2u);

  GbaDmaUnitSignalHBlank(dma_unit_, 0u);

  EXPECT_TRUE(GbaDmaUnitIsActive(dma_unit_));
  CheckDmaIsEnabled(2u);
}

TEST_F(DmaUnitTest, TestHBlankDma3) {
  EnableDma(/*index=*/3u, /*source=*/0u, /*dest=*/16u, /*transfer_count=*/3u,
            /*transfer_words=*/false, /*src_addr_mode=*/GBA_DMA_ADDR_INCREMENT,
            /*dest_addr_mode=*/GBA_DMA_ADDR_INCREMENT,
            /*trigger=*/GBA_DMA_HBLANK, /*repeat=*/false, /*irq=*/false);
  EXPECT_FALSE(GbaDmaUnitIsActive(dma_unit_));
  CheckDmaIsEnabled(3u);

  GbaDmaUnitSignalHBlank(dma_unit_, 0u);

  EXPECT_TRUE(GbaDmaUnitIsActive(dma_unit_));
  CheckDmaIsEnabled(3u);
}

TEST_F(DmaUnitTest, TestSpecialDma1) {
  EnableDma(/*index=*/1u, /*source=*/0u, /*dest=*/16u, /*transfer_count=*/3u,
            /*transfer_words=*/false, /*src_addr_mode=*/GBA_DMA_ADDR_INCREMENT,
            /*dest_addr_mode=*/GBA_DMA_ADDR_INCREMENT,
            /*trigger=*/GBA_DMA_SPECIAL, /*repeat=*/false, /*irq=*/false);
  EXPECT_FALSE(GbaDmaUnitIsActive(dma_unit_));
  CheckDmaIsEnabled(1u);

  GbaDmaUnitSignalFifoRefresh(dma_unit_, 0u);
  EXPECT_FALSE(GbaDmaUnitIsActive(dma_unit_));
  CheckDmaIsEnabled(1u);

  GbaDmaUnitSignalFifoRefresh(dma_unit_, 16u);
  EXPECT_TRUE(GbaDmaUnitIsActive(dma_unit_));
  CheckDmaIsEnabled(1u);

  EXPECT_TRUE(Store32LEStatic(nullptr, 0u, 0x12345678u));
  DmaDoSteps(1u);

  uint32_t value;
  EXPECT_TRUE(Load32LEStatic(nullptr, 16u, &value));
  EXPECT_EQ(0x12345678u, value);

  EXPECT_FALSE(GbaDmaUnitIsActive(dma_unit_));
}

TEST_F(DmaUnitTest, TestSpecialDma2) {
  EnableDma(/*index=*/2u, /*source=*/0u, /*dest=*/16u, /*transfer_count=*/3u,
            /*transfer_words=*/false, /*src_addr_mode=*/GBA_DMA_ADDR_INCREMENT,
            /*dest_addr_mode=*/GBA_DMA_ADDR_INCREMENT,
            /*trigger=*/GBA_DMA_SPECIAL, /*repeat=*/false, /*irq=*/false);
  EXPECT_FALSE(GbaDmaUnitIsActive(dma_unit_));
  CheckDmaIsEnabled(2u);

  GbaDmaUnitSignalFifoRefresh(dma_unit_, 0u);
  EXPECT_FALSE(GbaDmaUnitIsActive(dma_unit_));
  CheckDmaIsEnabled(2u);

  GbaDmaUnitSignalFifoRefresh(dma_unit_, 16u);
  EXPECT_TRUE(GbaDmaUnitIsActive(dma_unit_));
  CheckDmaIsEnabled(2u);

  EXPECT_TRUE(Store32LEStatic(nullptr, 0u, 0x12345678u));
  DmaDoSteps(1u);

  uint32_t value;
  EXPECT_TRUE(Load32LEStatic(nullptr, 16u, &value));
  EXPECT_EQ(0x12345678u, value);

  EXPECT_FALSE(GbaDmaUnitIsActive(dma_unit_));
}

TEST_F(DmaUnitTest, TestSpecialDma3) {
  EnableDma(/*index=*/3u, /*source=*/0u, /*dest=*/16u, /*transfer_count=*/1u,
            /*transfer_words=*/false, /*src_addr_mode=*/GBA_DMA_ADDR_FIXED,
            /*dest_addr_mode=*/GBA_DMA_ADDR_FIXED,
            /*trigger=*/GBA_DMA_SPECIAL, /*repeat=*/false, /*irq=*/false);
  EXPECT_FALSE(GbaDmaUnitIsActive(dma_unit_));
  CheckDmaIsEnabled(3u);

  GbaDmaUnitSignalHBlank(dma_unit_, 0u);
  EXPECT_FALSE(GbaDmaUnitIsActive(dma_unit_));
  CheckDmaIsEnabled(3u);

  GbaDmaUnitSignalHBlank(dma_unit_, 1u);
  EXPECT_FALSE(GbaDmaUnitIsActive(dma_unit_));
  CheckDmaIsEnabled(3u);

  for (uint_fast8_t i = 2; i < 162; i++) {
    GbaDmaUnitSignalHBlank(dma_unit_, i);
    EXPECT_TRUE(GbaDmaUnitIsActive(dma_unit_));
    CheckDmaIsEnabled(3u);

    DmaDoSteps(1u);

    EXPECT_FALSE(GbaDmaUnitIsActive(dma_unit_));
    CheckDmaIsDisabled(3u);

    EnableDma(/*index=*/3u, /*source=*/0u, /*dest=*/16u, /*transfer_count=*/1u,
              /*transfer_words=*/false, /*src_addr_mode=*/GBA_DMA_ADDR_FIXED,
              /*dest_addr_mode=*/GBA_DMA_ADDR_FIXED,
              /*trigger=*/GBA_DMA_SPECIAL, /*repeat=*/false, /*irq=*/false);
  }

  GbaDmaUnitSignalHBlank(dma_unit_, 162u);
  EXPECT_FALSE(GbaDmaUnitIsActive(dma_unit_));
  CheckDmaIsEnabled(3u);
}

TEST_F(DmaUnitTest, TestRepeat) {
  EXPECT_TRUE(Store32LEStatic(nullptr, 0u, 0x1234u));
  EXPECT_TRUE(Store32LEStatic(nullptr, 2u, 0x5678u));
  EXPECT_TRUE(Store32LEStatic(nullptr, 4u, 0xFFFFu));
  EnableDma(/*index=*/0u, /*source=*/0u, /*dest=*/16u, /*transfer_count=*/1u,
            /*transfer_words=*/false, /*src_addr_mode=*/GBA_DMA_ADDR_INCREMENT,
            /*dest_addr_mode=*/GBA_DMA_ADDR_INCREMENT,
            /*trigger=*/GBA_DMA_HBLANK, /*repeat=*/true, /*irq=*/false);
  GbaDmaUnitSignalHBlank(dma_unit_, 0u);
  EXPECT_TRUE(GbaDmaUnitIsActive(dma_unit_));

  DmaDoSteps(1u);
  EXPECT_FALSE(GbaDmaUnitIsActive(dma_unit_));
  CheckDmaIsEnabled(0u);

  GbaDmaUnitSignalHBlank(dma_unit_, 0u);
  EXPECT_TRUE(GbaDmaUnitIsActive(dma_unit_));

  DmaDoSteps(1u);
  EXPECT_FALSE(GbaDmaUnitIsActive(dma_unit_));
  CheckDmaIsEnabled(0u);

  GbaDmaUnitSignalHBlank(dma_unit_, 0u);
  EXPECT_TRUE(GbaDmaUnitIsActive(dma_unit_));

  DmaDoSteps(1u);
  EXPECT_FALSE(GbaDmaUnitIsActive(dma_unit_));
  CheckDmaIsEnabled(0u);

  uint16_t value;
  EXPECT_TRUE(Load16LEStatic(nullptr, 16u, &value));
  EXPECT_EQ(0x1234u, value);
  EXPECT_TRUE(Load16LEStatic(nullptr, 18u, &value));
  EXPECT_EQ(0x5678u, value);
  EXPECT_TRUE(Load16LEStatic(nullptr, 20u, &value));
  EXPECT_EQ(0xFFFFu, value);
}

TEST_F(DmaUnitTest, TestRepeatReload) {
  EXPECT_TRUE(Store32LEStatic(nullptr, 0u, 0x1234u));
  EXPECT_TRUE(Store32LEStatic(nullptr, 2u, 0x5678u));
  EXPECT_TRUE(Store32LEStatic(nullptr, 4u, 0xFFFFu));
  EnableDma(/*index=*/0u, /*source=*/0u, /*dest=*/16u, /*transfer_count=*/1u,
            /*transfer_words=*/false, /*src_addr_mode=*/GBA_DMA_ADDR_INCREMENT,
            /*dest_addr_mode=*/GBA_DMA_ADDR_INCREMENT_RELOAD,
            /*trigger=*/GBA_DMA_HBLANK, /*repeat=*/true, /*irq=*/false);
  GbaDmaUnitSignalHBlank(dma_unit_, 0u);
  EXPECT_TRUE(GbaDmaUnitIsActive(dma_unit_));

  DmaDoSteps(1u);
  EXPECT_FALSE(GbaDmaUnitIsActive(dma_unit_));
  CheckDmaIsEnabled(0u);

  GbaDmaUnitSignalHBlank(dma_unit_, 0u);
  EXPECT_TRUE(GbaDmaUnitIsActive(dma_unit_));

  DmaDoSteps(1u);
  EXPECT_FALSE(GbaDmaUnitIsActive(dma_unit_));
  CheckDmaIsEnabled(0u);

  GbaDmaUnitSignalHBlank(dma_unit_, 0u);
  EXPECT_TRUE(GbaDmaUnitIsActive(dma_unit_));

  DmaDoSteps(1u);
  EXPECT_FALSE(GbaDmaUnitIsActive(dma_unit_));
  CheckDmaIsEnabled(0u);

  uint16_t value;
  EXPECT_TRUE(Load16LEStatic(nullptr, 16u, &value));
  EXPECT_EQ(0xFFFFu, value);
  EXPECT_TRUE(Load16LEStatic(nullptr, 18u, &value));
  EXPECT_EQ(0x0000, value);
  EXPECT_TRUE(Load16LEStatic(nullptr, 20u, &value));
  EXPECT_EQ(0x0000, value);
}