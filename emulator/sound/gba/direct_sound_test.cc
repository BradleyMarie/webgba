extern "C" {
#include "emulator/sound/gba/direct_sound.h"
}

#include "googletest/include/gtest/gtest.h"

class DirectSoundTest : public testing::Test {
 public:
  void SetUp() override { memset(&channel_, 0, sizeof(DirectSoundChannel)); }

 protected:
  DirectSoundChannel channel_;
};

TEST_F(DirectSoundTest, EmptyPop) {
  int8_t value;
  EXPECT_TRUE(DirectSoundChannelPop(&channel_, &value));
  EXPECT_EQ(0, value);
}

TEST_F(DirectSoundTest, PushPop) {
  DirectSoundChannelPush(&channel_, 1);

  int8_t value;
  EXPECT_TRUE(DirectSoundChannelPop(&channel_, &value));
  EXPECT_EQ(1, value);
}

/* Disabled because of increased buffer size
TEST_F(DirectSoundTest, PushToPastFull) {
  for (int8_t i = 1; i <= 33; i++) {
    DirectSoundChannelPush(&channel_, i);
  }

  int8_t value;
  EXPECT_FALSE(DirectSoundChannelPop(&channel_, &value));
  EXPECT_EQ(2, value);
  EXPECT_FALSE(DirectSoundChannelPop(&channel_, &value));
  EXPECT_EQ(3, value);
  EXPECT_FALSE(DirectSoundChannelPop(&channel_, &value));
  EXPECT_EQ(4, value);
  EXPECT_FALSE(DirectSoundChannelPop(&channel_, &value));
  EXPECT_EQ(5, value);
  EXPECT_FALSE(DirectSoundChannelPop(&channel_, &value));
  EXPECT_EQ(6, value);
  EXPECT_FALSE(DirectSoundChannelPop(&channel_, &value));
  EXPECT_EQ(7, value);
  EXPECT_FALSE(DirectSoundChannelPop(&channel_, &value));
  EXPECT_EQ(8, value);
  EXPECT_FALSE(DirectSoundChannelPop(&channel_, &value));
  EXPECT_EQ(9, value);
  EXPECT_FALSE(DirectSoundChannelPop(&channel_, &value));
  EXPECT_EQ(10, value);
  EXPECT_FALSE(DirectSoundChannelPop(&channel_, &value));
  EXPECT_EQ(11, value);
  EXPECT_FALSE(DirectSoundChannelPop(&channel_, &value));
  EXPECT_EQ(12, value);
  EXPECT_FALSE(DirectSoundChannelPop(&channel_, &value));
  EXPECT_EQ(13, value);
  EXPECT_FALSE(DirectSoundChannelPop(&channel_, &value));
  EXPECT_EQ(14, value);
  EXPECT_FALSE(DirectSoundChannelPop(&channel_, &value));
  EXPECT_EQ(15, value);
  EXPECT_FALSE(DirectSoundChannelPop(&channel_, &value));
  EXPECT_EQ(16, value);
  EXPECT_TRUE(DirectSoundChannelPop(&channel_, &value));
  EXPECT_EQ(17, value);
  EXPECT_TRUE(DirectSoundChannelPop(&channel_, &value));
  EXPECT_EQ(18, value);
  EXPECT_TRUE(DirectSoundChannelPop(&channel_, &value));
  EXPECT_EQ(19, value);
  EXPECT_TRUE(DirectSoundChannelPop(&channel_, &value));
  EXPECT_EQ(20, value);
  EXPECT_TRUE(DirectSoundChannelPop(&channel_, &value));
  EXPECT_EQ(21, value);
  EXPECT_TRUE(DirectSoundChannelPop(&channel_, &value));
  EXPECT_EQ(22, value);
  EXPECT_TRUE(DirectSoundChannelPop(&channel_, &value));
  EXPECT_EQ(23, value);
  EXPECT_TRUE(DirectSoundChannelPop(&channel_, &value));
  EXPECT_EQ(24, value);
  EXPECT_TRUE(DirectSoundChannelPop(&channel_, &value));
  EXPECT_EQ(25, value);
  EXPECT_TRUE(DirectSoundChannelPop(&channel_, &value));
  EXPECT_EQ(26, value);
  EXPECT_TRUE(DirectSoundChannelPop(&channel_, &value));
  EXPECT_EQ(27, value);
  EXPECT_TRUE(DirectSoundChannelPop(&channel_, &value));
  EXPECT_EQ(28, value);
  EXPECT_TRUE(DirectSoundChannelPop(&channel_, &value));
  EXPECT_EQ(29, value);
  EXPECT_TRUE(DirectSoundChannelPop(&channel_, &value));
  EXPECT_EQ(30, value);
  EXPECT_TRUE(DirectSoundChannelPop(&channel_, &value));
  EXPECT_EQ(31, value);
  EXPECT_TRUE(DirectSoundChannelPop(&channel_, &value));
  EXPECT_EQ(32, value);
  EXPECT_TRUE(DirectSoundChannelPop(&channel_, &value));
  EXPECT_EQ(33, value);
}

TEST_F(DirectSoundTest, RepeatedPushAndPops) {
  for (int i = 0; i < 100; i++) {
    for (int8_t i = 1; i <= 33; i++) {
      DirectSoundChannelPush(&channel_, i);
    }

    int8_t value;
    EXPECT_FALSE(DirectSoundChannelPop(&channel_, &value));
    EXPECT_EQ(2, value);
    EXPECT_FALSE(DirectSoundChannelPop(&channel_, &value));
    EXPECT_EQ(3, value);
    EXPECT_FALSE(DirectSoundChannelPop(&channel_, &value));
    EXPECT_EQ(4, value);
    EXPECT_FALSE(DirectSoundChannelPop(&channel_, &value));
    EXPECT_EQ(5, value);
    EXPECT_FALSE(DirectSoundChannelPop(&channel_, &value));
    EXPECT_EQ(6, value);
    EXPECT_FALSE(DirectSoundChannelPop(&channel_, &value));
    EXPECT_EQ(7, value);
    EXPECT_FALSE(DirectSoundChannelPop(&channel_, &value));
    EXPECT_EQ(8, value);
    EXPECT_FALSE(DirectSoundChannelPop(&channel_, &value));
    EXPECT_EQ(9, value);
    EXPECT_FALSE(DirectSoundChannelPop(&channel_, &value));
    EXPECT_EQ(10, value);
    EXPECT_FALSE(DirectSoundChannelPop(&channel_, &value));
    EXPECT_EQ(11, value);
    EXPECT_FALSE(DirectSoundChannelPop(&channel_, &value));
    EXPECT_EQ(12, value);
    EXPECT_FALSE(DirectSoundChannelPop(&channel_, &value));
    EXPECT_EQ(13, value);
    EXPECT_FALSE(DirectSoundChannelPop(&channel_, &value));
    EXPECT_EQ(14, value);
    EXPECT_FALSE(DirectSoundChannelPop(&channel_, &value));
    EXPECT_EQ(15, value);
    EXPECT_FALSE(DirectSoundChannelPop(&channel_, &value));
    EXPECT_EQ(16, value);
    EXPECT_TRUE(DirectSoundChannelPop(&channel_, &value));
    EXPECT_EQ(17, value);
    EXPECT_TRUE(DirectSoundChannelPop(&channel_, &value));
    EXPECT_EQ(18, value);
    EXPECT_TRUE(DirectSoundChannelPop(&channel_, &value));
    EXPECT_EQ(19, value);
    EXPECT_TRUE(DirectSoundChannelPop(&channel_, &value));
    EXPECT_EQ(20, value);
    EXPECT_TRUE(DirectSoundChannelPop(&channel_, &value));
    EXPECT_EQ(21, value);
    EXPECT_TRUE(DirectSoundChannelPop(&channel_, &value));
    EXPECT_EQ(22, value);
    EXPECT_TRUE(DirectSoundChannelPop(&channel_, &value));
    EXPECT_EQ(23, value);
    EXPECT_TRUE(DirectSoundChannelPop(&channel_, &value));
    EXPECT_EQ(24, value);
    EXPECT_TRUE(DirectSoundChannelPop(&channel_, &value));
    EXPECT_EQ(25, value);
    EXPECT_TRUE(DirectSoundChannelPop(&channel_, &value));
    EXPECT_EQ(26, value);
    EXPECT_TRUE(DirectSoundChannelPop(&channel_, &value));
    EXPECT_EQ(27, value);
    EXPECT_TRUE(DirectSoundChannelPop(&channel_, &value));
    EXPECT_EQ(28, value);
    EXPECT_TRUE(DirectSoundChannelPop(&channel_, &value));
    EXPECT_EQ(29, value);
    EXPECT_TRUE(DirectSoundChannelPop(&channel_, &value));
    EXPECT_EQ(30, value);
    EXPECT_TRUE(DirectSoundChannelPop(&channel_, &value));
    EXPECT_EQ(31, value);
    EXPECT_TRUE(DirectSoundChannelPop(&channel_, &value));
    EXPECT_EQ(32, value);
    EXPECT_TRUE(DirectSoundChannelPop(&channel_, &value));
    EXPECT_EQ(33, value);

    for (int8_t i = 1; i <= 7; i++) {
      DirectSoundChannelPush(&channel_, i);
    }

    EXPECT_TRUE(DirectSoundChannelPop(&channel_, &value));
    EXPECT_EQ(1, value);
    EXPECT_TRUE(DirectSoundChannelPop(&channel_, &value));
    EXPECT_EQ(2, value);
    EXPECT_TRUE(DirectSoundChannelPop(&channel_, &value));
    EXPECT_EQ(3, value);
    EXPECT_TRUE(DirectSoundChannelPop(&channel_, &value));
    EXPECT_EQ(4, value);
    EXPECT_TRUE(DirectSoundChannelPop(&channel_, &value));
    EXPECT_EQ(5, value);
    EXPECT_TRUE(DirectSoundChannelPop(&channel_, &value));
    EXPECT_EQ(6, value);
    EXPECT_TRUE(DirectSoundChannelPop(&channel_, &value));
    EXPECT_EQ(7, value);
    EXPECT_TRUE(DirectSoundChannelPop(&channel_, &value));
    EXPECT_EQ(0, value);
  }
}
*/

TEST_F(DirectSoundTest, PushTwo) {
  DirectSoundChannelPushTwo(&channel_, 0x2211);

  int8_t value;
  EXPECT_TRUE(DirectSoundChannelPop(&channel_, &value));
  EXPECT_EQ(0x11, value);
  EXPECT_TRUE(DirectSoundChannelPop(&channel_, &value));
  EXPECT_EQ(0x22, value);
}

TEST_F(DirectSoundTest, PushFour) {
  DirectSoundChannelPushFour(&channel_, 0x44332211);

  int8_t value;
  EXPECT_TRUE(DirectSoundChannelPop(&channel_, &value));
  EXPECT_EQ(0x11, value);
  EXPECT_TRUE(DirectSoundChannelPop(&channel_, &value));
  EXPECT_EQ(0x22, value);
  EXPECT_TRUE(DirectSoundChannelPop(&channel_, &value));
  EXPECT_EQ(0x33, value);
  EXPECT_TRUE(DirectSoundChannelPop(&channel_, &value));
  EXPECT_EQ(0x44, value);
}

TEST_F(DirectSoundTest, Clear) {
  for (int8_t i = 1; i <= 33; i++) {
    DirectSoundChannelPush(&channel_, i);
  }

  DirectSoundChannelClear(&channel_);

  int8_t value;
  EXPECT_TRUE(DirectSoundChannelPop(&channel_, &value));
  EXPECT_EQ(0, value);
}