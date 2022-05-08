extern "C" {
#include "emulator/ppu/gba/oam/oam.h"
}

#include <cstring>

#include "googletest/include/gtest/gtest.h"

class OamTest : public testing::Test {
 public:
  void SetUp() override {
    memset(&oam_memory_, 0, sizeof(GbaPpuObjectAttributeMemory));
    memset(&internal_registers_, 0, sizeof(GbaPpuInternalRegisters));
    memset(&dirty_.objects, 0, sizeof(GbaPpuSet));
    memset(&dirty_.rotations, 0, sizeof(GbaPpuSet));
    memory_ = OamAllocate(&oam_memory_, &internal_registers_, &dirty_,
                          FreeRoutine, nullptr);
    ASSERT_NE(nullptr, memory_);
  }

  void TearDown() override { MemoryFree(memory_); }

  static void FreeRoutine(void* context) { ASSERT_EQ(nullptr, context); }

 protected:
  GbaPpuObjectAttributeMemory oam_memory_;
  GbaPpuInternalRegisters internal_registers_;
  GbaPpuOamDirtyBits dirty_;
  Memory* memory_;
};

TEST_F(OamTest, LoadStore32Succeeds) {
  EXPECT_TRUE(Store32LE(memory_, 0x0u, 0x20304050u));
  uint32_t value;
  EXPECT_TRUE(Load32LE(memory_, 0x0u, &value));
  EXPECT_EQ(0x20304050u, value);
  EXPECT_TRUE(Load32LE(memory_, OAM_SIZE, &value));
  EXPECT_EQ(0x20304050u, value);
}

TEST_F(OamTest, LoadStore16Succeeds) {
  EXPECT_TRUE(Store16LE(memory_, 0x0u, 0x2030u));
  uint16_t value;
  EXPECT_TRUE(Load16LE(memory_, 0x0u, &value));
  EXPECT_EQ(0x2030u, value);
  EXPECT_TRUE(Load16LE(memory_, OAM_SIZE, &value));
  EXPECT_EQ(0x2030u, value);
}

TEST_F(OamTest, LoadStore8Succeeds) {
  EXPECT_TRUE(Store8(memory_, 0x0u, 0x20u));
  uint8_t value;
  EXPECT_TRUE(Load8(memory_, 0x0u, &value));
  EXPECT_EQ(0x0u, value);
  EXPECT_TRUE(Load8(memory_, OAM_SIZE, &value));
  EXPECT_EQ(0x0u, value);
}

TEST_F(OamTest, Store16UpdatesAddsState0) {
  EXPECT_TRUE(Store16LE(memory_, 0x0u, 0u));
  EXPECT_EQ(0u, GbaPpuSetPop(&dirty_.objects));
  EXPECT_TRUE(GbaPpuSetEmpty(&dirty_.objects));
  EXPECT_TRUE(GbaPpuSetEmpty(&dirty_.rotations));
}

TEST_F(OamTest, Store16UpdatesAddsState2) {
  EXPECT_TRUE(Store16LE(memory_, 0x2u, 0u));
  EXPECT_EQ(0u, GbaPpuSetPop(&dirty_.objects));
  EXPECT_TRUE(GbaPpuSetEmpty(&dirty_.objects));
  EXPECT_TRUE(GbaPpuSetEmpty(&dirty_.rotations));
}

TEST_F(OamTest, Store16UpdatesAddsState4) {
  EXPECT_TRUE(Store16LE(memory_, 0x4u, 0u));
  EXPECT_TRUE(GbaPpuSetEmpty(&dirty_.objects));
  EXPECT_EQ(0u, GbaPpuSetPop(&dirty_.rotations));
  EXPECT_TRUE(GbaPpuSetEmpty(&dirty_.rotations));
}

TEST_F(OamTest, Store16UpdatesAddsState6) {
  EXPECT_TRUE(Store16LE(memory_, 0x6u, 0u));
  EXPECT_TRUE(GbaPpuSetEmpty(&dirty_.objects));
  EXPECT_EQ(0u, GbaPpuSetPop(&dirty_.rotations));
  EXPECT_TRUE(GbaPpuSetEmpty(&dirty_.rotations));
}

TEST_F(OamTest, Store16UpdatesAddsState8) {
  EXPECT_TRUE(Store16LE(memory_, 0x8u, 0u));
  EXPECT_EQ(1u, GbaPpuSetPop(&dirty_.objects));
  EXPECT_TRUE(GbaPpuSetEmpty(&dirty_.objects));
  EXPECT_TRUE(GbaPpuSetEmpty(&dirty_.rotations));
}

TEST_F(OamTest, Store16UpdatesAddsState14) {
  EXPECT_TRUE(Store16LE(memory_, 14u, 0u));
  EXPECT_TRUE(GbaPpuSetEmpty(&dirty_.objects));
  EXPECT_EQ(0u, GbaPpuSetPop(&dirty_.rotations));
  EXPECT_TRUE(GbaPpuSetEmpty(&dirty_.rotations));
}

TEST_F(OamTest, Store16UpdatesAddsState36) {
  EXPECT_TRUE(Store16LE(memory_, 36u, 0u));
  EXPECT_TRUE(GbaPpuSetEmpty(&dirty_.objects));
  EXPECT_EQ(1u, GbaPpuSetPop(&dirty_.rotations));
  EXPECT_TRUE(GbaPpuSetEmpty(&dirty_.rotations));
}

TEST_F(OamTest, Store32UpdatesAddsState0) {
  EXPECT_TRUE(Store16LE(memory_, 0x6u, 0u));
  EXPECT_TRUE(GbaPpuSetEmpty(&dirty_.objects));
  EXPECT_EQ(0u, GbaPpuSetPop(&dirty_.rotations));
  EXPECT_TRUE(GbaPpuSetEmpty(&dirty_.rotations));
}

TEST_F(OamTest, Store32UpdatesAddsState4) {
  EXPECT_TRUE(Store16LE(memory_, 0x4u, 0u));
  EXPECT_TRUE(GbaPpuSetEmpty(&dirty_.objects));
  EXPECT_EQ(0u, GbaPpuSetPop(&dirty_.rotations));
  EXPECT_TRUE(GbaPpuSetEmpty(&dirty_.rotations));
}

TEST_F(OamTest, Store32UpdatesAddsState8) {
  EXPECT_TRUE(Store16LE(memory_, 0x8u, 0u));
  EXPECT_EQ(1u, GbaPpuSetPop(&dirty_.objects));
  EXPECT_TRUE(GbaPpuSetEmpty(&dirty_.objects));
  EXPECT_TRUE(GbaPpuSetEmpty(&dirty_.rotations));
}