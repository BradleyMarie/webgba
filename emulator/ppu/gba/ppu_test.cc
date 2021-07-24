extern "C" {
#include "emulator/ppu/gba/ppu.h"
}

#include "googletest/include/gtest/gtest.h"

#define PRAM_SIZE 1024u
#define VRAM_SIZE (96u * 1024u)
#define OAM_SIZE 1024u

class PpuTest : public testing::Test {
 public:
  void SetUp() override {
    ASSERT_TRUE(GbaInterruptControllerAllocate(&ic_, &rst_, &fiq_, &irq_));
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