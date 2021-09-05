extern "C" {
#include "emulator/ppu/gba/io/io.h"
}

#include "googletest/include/gtest/gtest.h"

#define REGISTERS_SIZE 88u

#define DISPCNT_OFFSET 0x00u
#define GREENSWP_OFFSET 0x02u
#define DISPSTAT_OFFSET 0x04u
#define VCOUNT_OFFSET 0x06u
#define BG0CNT_OFFSET 0x08u
#define BG1CNT_OFFSET 0x0Au
#define BG2CNT_OFFSET 0x0Cu
#define BG3CNT_OFFSET 0x0Eu
#define BG0HOFS_OFFSET 0x10u
#define BG0VOFS_OFFSET 0x12u
#define BG1HOFS_OFFSET 0x14u
#define BG1VOFS_OFFSET 0x16u
#define BG2HOFS_OFFSET 0x18u
#define BG2VOFS_OFFSET 0x1Au
#define BG3HOFS_OFFSET 0x1Cu
#define BG3VOFS_OFFSET 0x1Eu
#define BG2PA_OFFSET 0x20u
#define BG2PB_OFFSET 0x22u
#define BG2PC_OFFSET 0x24u
#define BG2PD_OFFSET 0x26u
#define BG2X_OFFSET 0x28u
#define BG2X_OFFSET_HI 0x2Au
#define BG2Y_OFFSET 0x2Cu
#define BG2Y_OFFSET_HI 0x2Eu
#define BG3PA_OFFSET 0x30u
#define BG3PB_OFFSET 0x32u
#define BG3PC_OFFSET 0x34u
#define BG3PD_OFFSET 0x36u
#define BG3X_OFFSET 0x38u
#define BG3X_OFFSET_HI 0x3Au
#define BG3Y_OFFSET 0x3Cu
#define BG3Y_OFFSET_HI 0x3Eu
#define WIN0H_OFFSET 0x40u
#define WIN1H_OFFSET 0x42u
#define WIN0V_OFFSET 0x44u
#define WIN1V_OFFSET 0x46u
#define WININ_OFFSET 0x48u
#define WINOUT_OFFSET 0x4Au
#define MOSAIC_OFFSET 0x4Cu
#define BLDCNT_OFFSET 0x50u
#define BLDALPHA_OFFSET 0x52u
#define BLDY_OFFSET 0x54u

class IoTest : public testing::Test {
 public:
  void SetUp() override {
    memset(&registers_, 0, sizeof(GbaPpuRegisters));
    memset(&internal_registers_, 0, sizeof(GbaPpuInternalRegisters));
    memory_ = GbaPpuIoAllocate(&registers_, &internal_registers_, FreeRoutine,
                               nullptr);
    ASSERT_NE(nullptr, memory_);
  }

  void TearDown() override { MemoryFree(memory_); }

  static void FreeRoutine(void* context) { ASSERT_EQ(nullptr, context); }

 protected:
  GbaPpuRegisters registers_;
  GbaPpuInternalRegisters internal_registers_;
  Memory* memory_;
};

TEST_F(IoTest, GbaPpuRegistersLoad16LE) {
  for (uint32_t address = 0; address < REGISTERS_SIZE; address += 2) {
    uint16_t contents = 1u;
    if (address == DISPCNT_OFFSET || address == GREENSWP_OFFSET ||
        address == DISPSTAT_OFFSET || address == VCOUNT_OFFSET ||
        address == BG0CNT_OFFSET || address == BG1CNT_OFFSET ||
        address == BG2CNT_OFFSET || address == BG3CNT_OFFSET ||
        address == WININ_OFFSET || address == WINOUT_OFFSET ||
        address == BLDCNT_OFFSET) {
      EXPECT_TRUE(Load16LE(memory_, address, &contents));
      EXPECT_EQ(0u, contents);
    } else {
      EXPECT_FALSE(Load16LE(memory_, address, &contents));
    }
  }
}

TEST_F(IoTest, GbaPpuRegistersLoad32LEUnusedLow) {
  uint32_t contents;
  EXPECT_FALSE(Load32LE(memory_, 0x58u, &contents));
}

TEST_F(IoTest, GbaPpuRegistersLoad32LEUnusedHigh) {
  uint32_t contents = 0xFFFFFFFFu;
  EXPECT_TRUE(Store16LE(memory_, BLDCNT_OFFSET, 16u));
  EXPECT_TRUE(Load32LE(memory_, BLDCNT_OFFSET, &contents));
  EXPECT_EQ(0u, contents);
}

TEST_F(IoTest, GbaPpuRegistersLoad32LE) {
  uint32_t contents;
  EXPECT_TRUE(Store32LE(memory_, WININ_OFFSET, 0x11223344u));
  EXPECT_TRUE(Load32LE(memory_, WININ_OFFSET, &contents));
  EXPECT_EQ(0x11223344u, contents);
}

TEST_F(IoTest, GbaPpuRegistersLoad8Aligned) {
  uint8_t contents;
  EXPECT_TRUE(Store16LE(memory_, WININ_OFFSET, 0x1122u));
  EXPECT_TRUE(Load8(memory_, WININ_OFFSET, &contents));
  EXPECT_EQ(0x22, contents);
}

TEST_F(IoTest, GbaPpuRegistersLoad8Unaligned) {
  uint8_t contents;
  EXPECT_TRUE(Store16LE(memory_, WININ_OFFSET, 0x1122u));
  EXPECT_TRUE(Load8(memory_, WININ_OFFSET + 1u, &contents));
  EXPECT_EQ(0x11, contents);
}

TEST_F(IoTest, GbaPpuRegistersLoad8Fails) {
  uint8_t contents;
  EXPECT_FALSE(Load8(memory_, 0x4Eu, &contents));
}

TEST_F(IoTest, GbaPpuRegistersStore16LEVCount) {
  uint16_t contents;
  registers_.vcount = 0u;
  EXPECT_TRUE(Store16LE(memory_, VCOUNT_OFFSET, 0xFFFFu));
  EXPECT_TRUE(Load16LE(memory_, VCOUNT_OFFSET, &contents));
  EXPECT_EQ(0u, contents);

  registers_.vcount = 1u;
  EXPECT_TRUE(Store16LE(memory_, VCOUNT_OFFSET, 0xFFFFu));
  EXPECT_TRUE(Load16LE(memory_, VCOUNT_OFFSET, &contents));
  EXPECT_EQ(1u, contents);
}

TEST_F(IoTest, GbaPpuRegistersStore16LEDispstat) {
  uint16_t contents;
  EXPECT_TRUE(Store16LE(memory_, DISPSTAT_OFFSET, 0xFFFFu));
  EXPECT_TRUE(Load16LE(memory_, DISPSTAT_OFFSET, &contents));
  EXPECT_EQ(0xFFF8u, contents);
}

TEST_F(IoTest, GbaPpuRegistersStore16LE) {
  uint16_t contents;
  EXPECT_TRUE(Store16LE(memory_, WININ_OFFSET, 0xFFFFu));
  EXPECT_TRUE(Load16LE(memory_, WININ_OFFSET, &contents));
  EXPECT_EQ(0xFFFFu, contents);
}

TEST_F(IoTest, GbaPpuRegistersStore8Aligned) {
  uint16_t contents;
  EXPECT_TRUE(Store16LE(memory_, WININ_OFFSET, 0x1122u));
  EXPECT_TRUE(Store8(memory_, WININ_OFFSET, 0x33u));
  EXPECT_TRUE(Load16LE(memory_, WININ_OFFSET, &contents));
  EXPECT_EQ(0x1133u, contents);
}

TEST_F(IoTest, GbaPpuRegistersStore8Unaligned) {
  uint16_t contents;
  EXPECT_TRUE(Store16LE(memory_, WININ_OFFSET, 0x1122u));
  EXPECT_TRUE(Store8(memory_, WININ_OFFSET + 1u, 0x33u));
  EXPECT_TRUE(Load16LE(memory_, WININ_OFFSET, &contents));
  EXPECT_EQ(0x3322u, contents);
}