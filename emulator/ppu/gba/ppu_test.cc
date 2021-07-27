extern "C" {
#include "emulator/ppu/gba/ppu.h"
}

#include "googletest/include/gtest/gtest.h"

#define PRAM_SIZE 1024u
#define VRAM_SIZE (96u * 1024u)
#define OAM_SIZE 1024u
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

class PpuTest : public testing::Test {
 public:
  void SetUp() override {
    ASSERT_TRUE(
        GbaInterruptControllerAllocate(&ic_, &ic_regs_, &rst_, &fiq_, &irq_));
    ASSERT_TRUE(GbaPpuAllocate(ic_, &ppu_, &pram_, &vram_, &oam_, &regs_));
  }

  void TearDown() override {
    GbaInterruptControllerRelease(ic_);
    InterruptLineFree(rst_);
    InterruptLineFree(fiq_);
    InterruptLineFree(irq_);
    GbaPpuFree(ppu_);
    MemoryFree(pram_);
    MemoryFree(vram_);
    MemoryFree(oam_);
    MemoryFree(regs_);
  }

 protected:
  GbaInterruptController *ic_;
  Memory *ic_regs_;
  InterruptLine *rst_;
  InterruptLine *fiq_;
  InterruptLine *irq_;
  GbaPpu *ppu_;
  Memory *pram_;
  Memory *vram_;
  Memory *oam_;
  Memory *regs_;
};

TEST_F(PpuTest, PRamStore8Fails) {
  EXPECT_FALSE(Store8(pram_, PRAM_SIZE - 1u, 0x20u));
}

TEST_F(PpuTest, PRamLoad8Succeeds) {
  EXPECT_TRUE(Store16LE(pram_, PRAM_SIZE - 2u, 0x2030u));
  uint8_t value;
  EXPECT_TRUE(Load8(pram_, PRAM_SIZE - 1u, &value));
  EXPECT_EQ(0x20u, value);
}

TEST_F(PpuTest, PRamLoadStore8Fails) {
  EXPECT_FALSE(Store8(pram_, PRAM_SIZE, 0x20u));
  uint8_t value;
  EXPECT_FALSE(Load8(pram_, PRAM_SIZE, &value));
}

TEST_F(PpuTest, PRamLoadStore16Succeeds) {
  EXPECT_TRUE(Store16LE(pram_, PRAM_SIZE - 2u, 0x2030u));
  uint16_t value;
  EXPECT_TRUE(Load16LE(pram_, PRAM_SIZE - 2u, &value));
  EXPECT_EQ(0x2030u, value);
}

TEST_F(PpuTest, PRamLoadStore16Fails) {
  EXPECT_FALSE(Store16LE(pram_, PRAM_SIZE - 1u, 0x2030u));
  uint16_t value;
  EXPECT_FALSE(Load16LE(pram_, PRAM_SIZE - 1u, &value));
}

TEST_F(PpuTest, PRamLoadStore32Succeeds) {
  EXPECT_TRUE(Store32LE(pram_, PRAM_SIZE - 4u, 0x20304050u));
  uint32_t value;
  EXPECT_TRUE(Load32LE(pram_, PRAM_SIZE - 4u, &value));
  EXPECT_EQ(0x20304050u, value);
}

TEST_F(PpuTest, PRamLoadStore32Fails) {
  EXPECT_FALSE(Store32LE(pram_, PRAM_SIZE - 3u, 0x20304050u));
  uint32_t value;
  EXPECT_FALSE(Load32LE(pram_, PRAM_SIZE - 3u, &value));
}

TEST_F(PpuTest, VRamStore8Fails) {
  EXPECT_FALSE(Store8(vram_, VRAM_SIZE - 1u, 0x20u));
}

TEST_F(PpuTest, VRamLoad8Succeeds) {
  EXPECT_TRUE(Store16LE(vram_, VRAM_SIZE - 2u, 0x2030u));
  uint8_t value;
  EXPECT_TRUE(Load8(vram_, VRAM_SIZE - 1u, &value));
  EXPECT_EQ(0x20u, value);
}

TEST_F(PpuTest, VRamLoadStore8Fails) {
  EXPECT_FALSE(Store8(vram_, VRAM_SIZE, 0x20u));
  uint8_t value;
  EXPECT_FALSE(Load8(vram_, VRAM_SIZE, &value));
}

TEST_F(PpuTest, VRamLoadStore16Succeeds) {
  EXPECT_TRUE(Store16LE(vram_, VRAM_SIZE - 2u, 0x2030u));
  uint16_t value;
  EXPECT_TRUE(Load16LE(vram_, VRAM_SIZE - 2u, &value));
  EXPECT_EQ(0x2030u, value);
}

TEST_F(PpuTest, VRamLoadStore16Fails) {
  EXPECT_FALSE(Store16LE(vram_, VRAM_SIZE - 1u, 0x2030u));
  uint16_t value;
  EXPECT_FALSE(Load16LE(vram_, VRAM_SIZE - 1u, &value));
}

TEST_F(PpuTest, VRamLoadStore32Succeeds) {
  EXPECT_TRUE(Store32LE(vram_, VRAM_SIZE - 4u, 0x20304050u));
  uint32_t value;
  EXPECT_TRUE(Load32LE(vram_, VRAM_SIZE - 4u, &value));
  EXPECT_EQ(0x20304050u, value);
}

TEST_F(PpuTest, VRamLoadStore32Fails) {
  EXPECT_FALSE(Store32LE(vram_, VRAM_SIZE - 3u, 0x20304050u));
  uint32_t value;
  EXPECT_FALSE(Load32LE(vram_, VRAM_SIZE - 3u, &value));
}

TEST_F(PpuTest, OamStore8Fails) {
  EXPECT_FALSE(Store8(oam_, OAM_SIZE - 1u, 0x20u));
}

TEST_F(PpuTest, OamLoad8Succeeds) {
  EXPECT_TRUE(Store16LE(oam_, OAM_SIZE - 2u, 0x2030u));
  uint8_t value;
  EXPECT_TRUE(Load8(oam_, OAM_SIZE - 1u, &value));
  EXPECT_EQ(0x20u, value);
}

TEST_F(PpuTest, OamLoadStore8Fails) {
  EXPECT_FALSE(Store8(oam_, OAM_SIZE, 0x20u));
  uint8_t value;
  EXPECT_FALSE(Load8(oam_, OAM_SIZE, &value));
}

TEST_F(PpuTest, OamLoadStore16Succeeds) {
  EXPECT_TRUE(Store16LE(oam_, OAM_SIZE - 2u, 0x2030u));
  uint16_t value;
  EXPECT_TRUE(Load16LE(oam_, OAM_SIZE - 2u, &value));
  EXPECT_EQ(0x2030u, value);
}

TEST_F(PpuTest, OamLoadStore16Fails) {
  EXPECT_FALSE(Store16LE(oam_, OAM_SIZE - 1u, 0x2030u));
  uint16_t value;
  EXPECT_FALSE(Load16LE(oam_, OAM_SIZE - 1u, &value));
}

TEST_F(PpuTest, OamLoadStore32Succeeds) {
  EXPECT_TRUE(Store32LE(oam_, OAM_SIZE - 4u, 0x20304050u));
  uint32_t value;
  EXPECT_TRUE(Load32LE(oam_, OAM_SIZE - 4u, &value));
  EXPECT_EQ(0x20304050u, value);
}

TEST_F(PpuTest, OamLoadStore32Fails) {
  EXPECT_FALSE(Store32LE(oam_, OAM_SIZE - 3u, 0x20304050u));
  uint32_t value;
  EXPECT_FALSE(Load32LE(oam_, OAM_SIZE - 3u, &value));
}

TEST_F(PpuTest, GbaPpuRegistersLoad16LE) {
  for (uint32_t address = 0; address < REGISTERS_SIZE; address += 2) {
    uint16_t contents;
    if (address == DISPCNT_OFFSET || address == GREENSWP_OFFSET ||
        address == DISPSTAT_OFFSET || address == VCOUNT_OFFSET ||
        address == BG0CNT_OFFSET || address == BG1CNT_OFFSET ||
        address == BG2CNT_OFFSET || address == BG3CNT_OFFSET ||
        address == WININ_OFFSET || address == WINOUT_OFFSET ||
        address == BLDCNT_OFFSET) {
      EXPECT_TRUE(Load16LE(regs_, address, &contents));
      if (address == DISPCNT_OFFSET) {
        EXPECT_EQ(0x80u, contents);
      } else {
        EXPECT_EQ(0u, contents);
      }
    } else {
      EXPECT_FALSE(Load16LE(regs_, address, &contents));
    }
  }
}

TEST_F(PpuTest, GbaPpuRegistersLoad32LEUnusedLow) {
  uint32_t contents;
  EXPECT_FALSE(Load32LE(regs_, 0x4Eu, &contents));
}

TEST_F(PpuTest, GbaPpuRegistersLoad32LEUnusedHigh) {
  uint32_t contents = 0xFFFFFFFFu;
  EXPECT_TRUE(Store16LE(regs_, WINOUT_OFFSET, 16u));
  EXPECT_TRUE(Load32LE(regs_, WINOUT_OFFSET, &contents));
  EXPECT_EQ(0u, contents);
}

TEST_F(PpuTest, GbaPpuRegistersLoad32LE) {
  uint32_t contents;
  EXPECT_TRUE(Store32LE(regs_, WININ_OFFSET, 0xFFFFFFFFu));
  EXPECT_TRUE(Load32LE(regs_, WININ_OFFSET, &contents));
  EXPECT_EQ(0xFFFFFFFFu, contents);
}

TEST_F(PpuTest, GbaPpuRegistersLoad8Aligned) {
  uint8_t contents;
  EXPECT_TRUE(Store16LE(regs_, WININ_OFFSET, 0x1122u));
  EXPECT_TRUE(Load8(regs_, WININ_OFFSET, &contents));
  EXPECT_EQ(0x22, contents);
}

TEST_F(PpuTest, GbaPpuRegistersLoad8Unaligned) {
  uint8_t contents;
  EXPECT_TRUE(Store16LE(regs_, WININ_OFFSET, 0x1122u));
  EXPECT_TRUE(Load8(regs_, WININ_OFFSET + 1u, &contents));
  EXPECT_EQ(0x11, contents);
}

TEST_F(PpuTest, GbaPpuRegistersLoad8Fails) {
  uint8_t contents;
  EXPECT_FALSE(Load8(regs_, 0x4Eu, &contents));
}

TEST_F(PpuTest, GbaPpuRegistersStore16LEVCount) {
  uint16_t contents;
  EXPECT_TRUE(Store16LE(regs_, VCOUNT_OFFSET, 0xFFFFu));
  EXPECT_TRUE(Load16LE(regs_, VCOUNT_OFFSET, &contents));
  EXPECT_EQ(0u, contents);
}

TEST_F(PpuTest, GbaPpuRegistersStore16LEDispstat) {
  uint16_t contents;
  EXPECT_TRUE(Store16LE(regs_, DISPSTAT_OFFSET, 0xFFFFu));
  EXPECT_TRUE(Load16LE(regs_, DISPSTAT_OFFSET, &contents));
  EXPECT_EQ(0xFFF8u, contents);
}

TEST_F(PpuTest, GbaPpuRegistersStore16LE) {
  uint16_t contents;
  EXPECT_TRUE(Store16LE(regs_, WININ_OFFSET, 0xFFFFu));
  EXPECT_TRUE(Load16LE(regs_, WININ_OFFSET, &contents));
  EXPECT_EQ(0xFFFFu, contents);
}

TEST_F(PpuTest, GbaPpuRegistersStore8Aligned) {
  uint16_t contents;
  EXPECT_TRUE(Store16LE(regs_, WININ_OFFSET, 0x1122u));
  EXPECT_TRUE(Store8(regs_, WININ_OFFSET, 0x33u));
  EXPECT_TRUE(Load16LE(regs_, WININ_OFFSET, &contents));
  EXPECT_EQ(0x1133u, contents);
}

TEST_F(PpuTest, GbaPpuRegistersStore8Unaligned) {
  uint16_t contents;
  EXPECT_TRUE(Store16LE(regs_, WININ_OFFSET, 0x1122u));
  EXPECT_TRUE(Store8(regs_, WININ_OFFSET + 1u, 0x33u));
  EXPECT_TRUE(Load16LE(regs_, WININ_OFFSET, &contents));
  EXPECT_EQ(0x3322u, contents);
}