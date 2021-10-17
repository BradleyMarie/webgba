extern "C" {
#include "emulator/sound/gba/direct_sound.h"
}

#include "googletest/include/gtest/gtest.h"

class DirectSoundTest : public testing::Test {
 public:
  void SetUp() override {
    channel_ = DirectSoundChannelAllocate();
    ASSERT_NE(nullptr, channel_);
  }

  void TearDown() override { DirectSoundChannelFree(channel_); }

 protected:
  DirectSoundChannel *channel_;
};

TEST_F(DirectSoundTest, EmptyPeek) {
  uint32_t value = DirectSoundChannelPeekBack(channel_);
  EXPECT_EQ(0u, value);
}

TEST_F(DirectSoundTest, EmptyPop) {
  int16_t value;
  ASSERT_TRUE(DirectSoundChannelPop(channel_, &value));
  EXPECT_EQ(0, value);
}

TEST_F(DirectSoundTest, FillBufferWith8BitSamples) {
  DirectSoundChannelPush8BitSamples(channel_, 0xFFFFFFFFu);
  uint32_t back = DirectSoundChannelPeekBack(channel_);
  EXPECT_EQ(0u, back);

  DirectSoundChannelPush8BitSamples(channel_, 0xFFFFFFFFu);
  back = DirectSoundChannelPeekBack(channel_);
  EXPECT_EQ(0u, back);

  DirectSoundChannelPush8BitSamples(channel_, 0xFFFFFFFFu);
  back = DirectSoundChannelPeekBack(channel_);
  EXPECT_EQ(0u, back);

  DirectSoundChannelPush8BitSamples(channel_, 0xFFFFFFFFu);
  back = DirectSoundChannelPeekBack(channel_);
  EXPECT_EQ(0u, back);

  DirectSoundChannelPush8BitSamples(channel_, 0xFFFFFFFFu);
  back = DirectSoundChannelPeekBack(channel_);
  EXPECT_EQ(0u, back);

  DirectSoundChannelPush8BitSamples(channel_, 0xFFFFFFFFu);
  back = DirectSoundChannelPeekBack(channel_);
  EXPECT_EQ(0u, back);

  DirectSoundChannelPush8BitSamples(channel_, 0xFFFFFFFFu);
  back = DirectSoundChannelPeekBack(channel_);
  EXPECT_EQ(0xFFFFFFFFu, back);
}

TEST_F(DirectSoundTest, DrainBufferWith8BitSamples) {
  DirectSoundChannelPush8BitSamples(channel_, 0x44332211u);
  DirectSoundChannelPush8BitSamples(channel_, 0x44332211u);
  DirectSoundChannelPush8BitSamples(channel_, 0x44332211u);
  DirectSoundChannelPush8BitSamples(channel_, 0x44332211u);
  DirectSoundChannelPush8BitSamples(channel_, 0x44332211u);
  DirectSoundChannelPush8BitSamples(channel_, 0x44332211u);
  DirectSoundChannelPush8BitSamples(channel_, 0x44332211u);

  int16_t sample;
  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x1100, sample);
  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x2200, sample);
  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x3300, sample);
  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x4400, sample);

  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x1100, sample);
  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x2200, sample);
  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x3300, sample);
  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x4400, sample);

  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x1100, sample);
  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x2200, sample);
  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x3300, sample);
  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x4400, sample);

  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x1100, sample);
  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x2200, sample);
  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x3300, sample);
  EXPECT_TRUE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x4400, sample);

  EXPECT_TRUE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x1100, sample);
  EXPECT_TRUE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x2200, sample);
  EXPECT_TRUE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x3300, sample);
  EXPECT_TRUE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x4400, sample);

  EXPECT_TRUE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x1100, sample);
  EXPECT_TRUE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x2200, sample);
  EXPECT_TRUE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x3300, sample);
  EXPECT_TRUE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x4400, sample);

  EXPECT_TRUE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x1100, sample);
  EXPECT_TRUE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x2200, sample);
  EXPECT_TRUE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x3300, sample);
  EXPECT_TRUE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x4400, sample);

  EXPECT_TRUE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0, sample);
}

TEST_F(DirectSoundTest, FillAndDrainBufferWith16BitSamples) {
  DirectSoundChannelPush16BitSamples(channel_, 0x44332211u);
  DirectSoundChannelPush16BitSamples(channel_, 0x44332211u);
  DirectSoundChannelPush16BitSamples(channel_, 0x44332211u);
  DirectSoundChannelPush16BitSamples(channel_, 0x44332211u);
  DirectSoundChannelPush16BitSamples(channel_, 0x44332211u);
  DirectSoundChannelPush16BitSamples(channel_, 0x44332211u);
  DirectSoundChannelPush16BitSamples(channel_, 0x44332211u);
  DirectSoundChannelPush16BitSamples(channel_, 0x44332211u);
  DirectSoundChannelPush16BitSamples(channel_, 0x44332211u);
  DirectSoundChannelPush16BitSamples(channel_, 0x44332211u);
  DirectSoundChannelPush16BitSamples(channel_, 0x44332211u);
  DirectSoundChannelPush16BitSamples(channel_, 0x44332211u);
  DirectSoundChannelPush16BitSamples(channel_, 0x44332211u);
  DirectSoundChannelPush16BitSamples(channel_, 0x44332211u);

  int16_t sample;
  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x2211, sample);
  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x4433, sample);
  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x2211, sample);
  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x4433, sample);

  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x2211, sample);
  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x4433, sample);
  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x2211, sample);
  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x4433, sample);

  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x2211, sample);
  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x4433, sample);
  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x2211, sample);
  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x4433, sample);

  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x2211, sample);
  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x4433, sample);
  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x2211, sample);
  EXPECT_TRUE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x4433, sample);

  EXPECT_TRUE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x2211, sample);
  EXPECT_TRUE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x4433, sample);
  EXPECT_TRUE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x2211, sample);
  EXPECT_TRUE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x4433, sample);

  EXPECT_TRUE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x2211, sample);
  EXPECT_TRUE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x4433, sample);
  EXPECT_TRUE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x2211, sample);
  EXPECT_TRUE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x4433, sample);

  EXPECT_TRUE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x2211, sample);
  EXPECT_TRUE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x4433, sample);
  EXPECT_TRUE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x2211, sample);
  EXPECT_TRUE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x4433, sample);

  EXPECT_TRUE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0, sample);
}

TEST_F(DirectSoundTest, DrainBufferWith8BitSamplesWithRefill) {
  DirectSoundChannelPush8BitSamples(channel_, 0x44332211u);
  DirectSoundChannelPush8BitSamples(channel_, 0x44332211u);
  DirectSoundChannelPush8BitSamples(channel_, 0x44332211u);
  DirectSoundChannelPush8BitSamples(channel_, 0x44332211u);
  DirectSoundChannelPush8BitSamples(channel_, 0x44332211u);
  DirectSoundChannelPush8BitSamples(channel_, 0x44332211u);
  DirectSoundChannelPush8BitSamples(channel_, 0x44332211u);

  int16_t sample;
  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x1100, sample);
  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x2200, sample);
  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x3300, sample);
  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x4400, sample);

  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x1100, sample);
  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x2200, sample);
  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x3300, sample);
  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x4400, sample);

  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x1100, sample);
  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x2200, sample);
  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x3300, sample);
  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x4400, sample);

  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x1100, sample);
  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x2200, sample);
  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x3300, sample);
  EXPECT_TRUE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x4400, sample);

  DirectSoundChannelPush8BitSamples(channel_, 0x88776655u);
  DirectSoundChannelPush8BitSamples(channel_, 0x88776655u);
  DirectSoundChannelPush8BitSamples(channel_, 0x88776655u);
  DirectSoundChannelPush8BitSamples(channel_, 0x88776655u);

  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x1100, sample);
  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x2200, sample);
  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x3300, sample);
  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x4400, sample);

  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x1100, sample);
  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x2200, sample);
  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x3300, sample);
  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x4400, sample);

  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x1100, sample);
  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x2200, sample);
  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x3300, sample);
  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x4400, sample);

  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x5500, sample);
  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x6600, sample);
  EXPECT_FALSE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x7700, sample);
  EXPECT_TRUE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(-30720, sample);

  EXPECT_TRUE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x5500, sample);
  EXPECT_TRUE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x6600, sample);
  EXPECT_TRUE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x7700, sample);
  EXPECT_TRUE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(-30720, sample);

  EXPECT_TRUE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x5500, sample);
  EXPECT_TRUE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x6600, sample);
  EXPECT_TRUE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x7700, sample);
  EXPECT_TRUE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(-30720, sample);

  EXPECT_TRUE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x5500, sample);
  EXPECT_TRUE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x6600, sample);
  EXPECT_TRUE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0x7700, sample);
  EXPECT_TRUE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(-30720, sample);

  EXPECT_TRUE(DirectSoundChannelPop(channel_, &sample));
  EXPECT_EQ(0, sample);
}