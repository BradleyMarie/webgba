extern "C" {
#include "emulator/peripherals/gba/peripherals.h"
}

#include "googletest/include/gtest/gtest.h"

#define IE_OFFSET 0x0u
#define IF_OFFSET 0x2u
#define IME_OFFSET 0x8u

#define SIODATA32S_OFFSET 0x00u
#define SIOMULTI0_OFFSET 0x00u
#define SIOMULTI1_OFFSET 0x02u
#define SIOMULTI2_OFFSET 0x04u
#define SIOMULTI3_OFFSET 0x06u
#define SIOCNT_OFFSET 0x08u
#define SIOMLT_SEND_OFFSET 0x0Au
#define SIODATA8_OFFSET 0x0Au
#define KEYINPUT_OFFSET 0x10u
#define KEYCNT_OFFSET 0x12u
#define RCNT_OFFSET 0x14u
#define IR_OFFSET 0x16u
#define JOYCNT_OFFSET 0x20u
#define JOY_RECV_L_OFFSET 0x30u
#define JOY_RECV_H_OFFSET 0x32u
#define JOY_TRANS_L_OFFSET 0x34u
#define JOY_TRANS_H_OFFSET 0x36u
#define JOYSTAT_OFFSET 0x38u

class PeripheralsTest : public testing::Test {
 public:
  void SetUp() override {
    ASSERT_TRUE(GbaPlatformAllocate(&plat_, &plat_regs_, &rst_, &fiq_, &irq_));
    ASSERT_TRUE(
        GbaPeripheralsAllocate(plat_, &peripherals_, &gamepad_, &regs_));

    ASSERT_TRUE(Store16LE(plat_regs_, IE_OFFSET, 0xFFFFu));
    ASSERT_TRUE(Store16LE(plat_regs_, IF_OFFSET, 0xFFFFu));
    ASSERT_TRUE(Store16LE(plat_regs_, IME_OFFSET, 0xFFFFu));
  }

  void TearDown() override {
    GbaPlatformRelease(plat_);
    MemoryFree(plat_regs_);
    InterruptLineFree(rst_);
    InterruptLineFree(fiq_);
    InterruptLineFree(irq_);
    GbaPeripheralsFree(peripherals_);
    GamePadFree(gamepad_);
    MemoryFree(regs_);
  }

 protected:
  GbaPlatform *plat_;
  Memory *plat_regs_;
  InterruptLine *rst_;
  InterruptLine *fiq_;
  InterruptLine *irq_;
  GbaPeripherals *peripherals_;
  GamePad *gamepad_;
  Memory *regs_;
};

TEST_F(PeripheralsTest, GbaPeripheralsRegistersLoad16LE) {
  uint16_t contents;
  EXPECT_TRUE(Store16LE(regs_, SIOMULTI0_OFFSET, 0x1122u));
  EXPECT_TRUE(Load16LE(regs_, SIOMULTI0_OFFSET, &contents));
  EXPECT_EQ(0u, contents);
  EXPECT_TRUE(Store16LE(regs_, SIOMULTI1_OFFSET, 0x1122u));
  EXPECT_TRUE(Load16LE(regs_, SIOMULTI1_OFFSET, &contents));
  EXPECT_EQ(0u, contents);
  EXPECT_TRUE(Store16LE(regs_, SIOMULTI2_OFFSET, 0x1122u));
  EXPECT_TRUE(Load16LE(regs_, SIOMULTI2_OFFSET, &contents));
  EXPECT_EQ(0u, contents);
  EXPECT_TRUE(Store16LE(regs_, SIOMULTI3_OFFSET, 0x1122u));
  EXPECT_TRUE(Load16LE(regs_, SIOMULTI3_OFFSET, &contents));
  EXPECT_EQ(0u, contents);
  EXPECT_TRUE(Store16LE(regs_, SIOCNT_OFFSET, 0x1122u));
  EXPECT_TRUE(Load16LE(regs_, SIOCNT_OFFSET, &contents));
  EXPECT_EQ(0x1122u, contents);
  EXPECT_TRUE(Store16LE(regs_, SIOMLT_SEND_OFFSET, 0x3344u));
  EXPECT_TRUE(Load16LE(regs_, SIOMLT_SEND_OFFSET, &contents));
  EXPECT_EQ(0x3344u, contents);

  // Out of Bounds
  EXPECT_TRUE(Store16LE(regs_, SIOMLT_SEND_OFFSET + 2u, 0xAABBu));
  EXPECT_FALSE(Load16LE(regs_, SIOMLT_SEND_OFFSET + 2u, &contents));

  // In Bounds
  EXPECT_TRUE(Store16LE(regs_, KEYINPUT_OFFSET, 0x1122u));
  EXPECT_TRUE(Load16LE(regs_, KEYINPUT_OFFSET, &contents));
  EXPECT_EQ(0u, contents);
  EXPECT_TRUE(Store16LE(regs_, KEYCNT_OFFSET, 0x1122u));
  EXPECT_TRUE(Load16LE(regs_, KEYCNT_OFFSET, &contents));
  EXPECT_EQ(0x1122u, contents);
  EXPECT_TRUE(Store16LE(regs_, RCNT_OFFSET, 0x1122u));
  EXPECT_TRUE(Load16LE(regs_, RCNT_OFFSET, &contents));
  EXPECT_EQ(0x1122u, contents);
  EXPECT_TRUE(Store16LE(regs_, IR_OFFSET, 0xAABBu));
  EXPECT_TRUE(Load16LE(regs_, IR_OFFSET, &contents));
  EXPECT_EQ(0u, contents);

  // Out of Bounds
  EXPECT_TRUE(Store16LE(regs_, IR_OFFSET + 2u, 0xAABBu));
  EXPECT_FALSE(Load16LE(regs_, IR_OFFSET + 2u, &contents));

  // In Bounds
  EXPECT_TRUE(Store16LE(regs_, JOYCNT_OFFSET, 0x1122u));
  EXPECT_TRUE(Load16LE(regs_, JOYCNT_OFFSET, &contents));
  EXPECT_EQ(0x1122u, contents);

  // Out of Bounds
  EXPECT_TRUE(Store16LE(regs_, JOYCNT_OFFSET + 2u, 0xAABBu));
  EXPECT_FALSE(Load16LE(regs_, JOYCNT_OFFSET + 2u, &contents));

  // In Bounds
  EXPECT_TRUE(Store16LE(regs_, JOY_RECV_L_OFFSET, 0x1122u));
  EXPECT_TRUE(Load16LE(regs_, JOY_RECV_L_OFFSET, &contents));
  EXPECT_EQ(0u, contents);
  EXPECT_TRUE(Store16LE(regs_, JOY_RECV_H_OFFSET, 0x1122u));
  EXPECT_TRUE(Load16LE(regs_, JOY_RECV_H_OFFSET, &contents));
  EXPECT_EQ(0u, contents);
  EXPECT_TRUE(Store16LE(regs_, JOY_TRANS_L_OFFSET, 0x1122u));
  EXPECT_TRUE(Load16LE(regs_, JOY_TRANS_L_OFFSET, &contents));
  EXPECT_EQ(0x1122u, contents);
  EXPECT_TRUE(Store16LE(regs_, JOY_TRANS_H_OFFSET, 0x3344u));
  EXPECT_TRUE(Load16LE(regs_, JOY_TRANS_H_OFFSET, &contents));
  EXPECT_EQ(0x3344u, contents);
  EXPECT_TRUE(Store16LE(regs_, JOYSTAT_OFFSET, 0x1122u));
  EXPECT_TRUE(Load16LE(regs_, JOYSTAT_OFFSET, &contents));
  EXPECT_EQ(0u, contents);

  // Out of Bounds
  EXPECT_TRUE(Store16LE(regs_, JOYSTAT_OFFSET + 2u, 0xAABBu));
  EXPECT_FALSE(Load16LE(regs_, JOYSTAT_OFFSET + 2u, &contents));
}

TEST_F(PeripheralsTest, GbaPeripheralsRegistersLoad8) {
  uint8_t contents;
  EXPECT_TRUE(Store8(regs_, SIOMULTI0_OFFSET, 0x11u));
  EXPECT_TRUE(Load8(regs_, SIOMULTI0_OFFSET, &contents));
  EXPECT_EQ(0u, contents);
  EXPECT_TRUE(Store8(regs_, SIOMULTI0_OFFSET + 1u, 0x11u));
  EXPECT_TRUE(Load8(regs_, SIOMULTI0_OFFSET + 1u, &contents));
  EXPECT_EQ(0u, contents);
  EXPECT_TRUE(Store8(regs_, SIOMULTI1_OFFSET, 0x11u));
  EXPECT_TRUE(Load8(regs_, SIOMULTI1_OFFSET, &contents));
  EXPECT_EQ(0u, contents);
  EXPECT_TRUE(Store8(regs_, SIOMULTI1_OFFSET + 1u, 0x11u));
  EXPECT_TRUE(Load8(regs_, SIOMULTI1_OFFSET + 1u, &contents));
  EXPECT_EQ(0u, contents);
  EXPECT_TRUE(Store8(regs_, SIOMULTI2_OFFSET, 0x11u));
  EXPECT_TRUE(Load8(regs_, SIOMULTI2_OFFSET, &contents));
  EXPECT_EQ(0u, contents);
  EXPECT_TRUE(Store8(regs_, SIOMULTI2_OFFSET + 1u, 0x11u));
  EXPECT_TRUE(Load8(regs_, SIOMULTI2_OFFSET + 1u, &contents));
  EXPECT_EQ(0u, contents);
  EXPECT_TRUE(Store8(regs_, SIOMULTI3_OFFSET, 0x11u));
  EXPECT_TRUE(Load8(regs_, SIOMULTI3_OFFSET, &contents));
  EXPECT_EQ(0u, contents);
  EXPECT_TRUE(Store8(regs_, SIOMULTI3_OFFSET + 1u, 0x11u));
  EXPECT_TRUE(Load8(regs_, SIOMULTI3_OFFSET + 1u, &contents));
  EXPECT_EQ(0u, contents);
  EXPECT_TRUE(Store8(regs_, SIOCNT_OFFSET, 0x11u));
  EXPECT_TRUE(Load8(regs_, SIOCNT_OFFSET, &contents));
  EXPECT_EQ(0x11u, contents);
  EXPECT_TRUE(Store8(regs_, SIOCNT_OFFSET + 1u, 0x22u));
  EXPECT_TRUE(Load8(regs_, SIOCNT_OFFSET + 1u, &contents));
  EXPECT_EQ(0x22u, contents);
  EXPECT_TRUE(Store8(regs_, SIOMLT_SEND_OFFSET, 0x33u));
  EXPECT_TRUE(Load8(regs_, SIOMLT_SEND_OFFSET, &contents));
  EXPECT_EQ(0x33u, contents);
  EXPECT_TRUE(Store8(regs_, SIOMLT_SEND_OFFSET + 1u, 0x44u));
  EXPECT_TRUE(Load8(regs_, SIOMLT_SEND_OFFSET + 1u, &contents));
  EXPECT_EQ(0x44u, contents);

  // Out of Bounds
  EXPECT_TRUE(Store8(regs_, SIOMLT_SEND_OFFSET + 2u, 0xAAu));
  EXPECT_FALSE(Load8(regs_, SIOMLT_SEND_OFFSET + 2u, &contents));

  // In Bounds
  EXPECT_TRUE(Store8(regs_, KEYINPUT_OFFSET, 0x22u));
  EXPECT_TRUE(Load8(regs_, KEYINPUT_OFFSET, &contents));
  EXPECT_EQ(0u, contents);
  EXPECT_TRUE(Store8(regs_, KEYINPUT_OFFSET + 1u, 0x22u));
  EXPECT_TRUE(Load8(regs_, KEYINPUT_OFFSET + 1u, &contents));
  EXPECT_EQ(0u, contents);
  EXPECT_TRUE(Store8(regs_, KEYCNT_OFFSET, 0x11u));
  EXPECT_TRUE(Load8(regs_, KEYCNT_OFFSET, &contents));
  EXPECT_EQ(0x11u, contents);
  EXPECT_TRUE(Store8(regs_, KEYCNT_OFFSET + 1u, 0x22u));
  EXPECT_TRUE(Load8(regs_, KEYCNT_OFFSET + 1u, &contents));
  EXPECT_EQ(0x22u, contents);
  EXPECT_TRUE(Store8(regs_, RCNT_OFFSET, 0x11u));
  EXPECT_TRUE(Load8(regs_, RCNT_OFFSET, &contents));
  EXPECT_EQ(0x11u, contents);
  EXPECT_TRUE(Store8(regs_, RCNT_OFFSET + 1u, 0x22u));
  EXPECT_TRUE(Load8(regs_, RCNT_OFFSET + 1u, &contents));
  EXPECT_EQ(0x22u, contents);
  EXPECT_TRUE(Store8(regs_, IR_OFFSET, 0x11u));
  EXPECT_TRUE(Load8(regs_, IR_OFFSET, &contents));
  EXPECT_EQ(0u, contents);
  EXPECT_TRUE(Store8(regs_, IR_OFFSET + 1u, 0x22u));
  EXPECT_TRUE(Load8(regs_, IR_OFFSET + 1u, &contents));
  EXPECT_EQ(0u, contents);

  // Out of Bounds
  EXPECT_TRUE(Store8(regs_, IR_OFFSET + 2u, 0xAAu));
  EXPECT_FALSE(Load8(regs_, IR_OFFSET + 2u, &contents));

  // In Bounds
  EXPECT_TRUE(Store8(regs_, JOYCNT_OFFSET, 0x11u));
  EXPECT_TRUE(Load8(regs_, JOYCNT_OFFSET, &contents));
  EXPECT_EQ(0x11u, contents);
  EXPECT_TRUE(Store8(regs_, JOYCNT_OFFSET + 1u, 0x22u));
  EXPECT_TRUE(Load8(regs_, JOYCNT_OFFSET + 1u, &contents));
  EXPECT_EQ(0x22u, contents);

  // Out of Bounds
  EXPECT_TRUE(Store8(regs_, JOYCNT_OFFSET + 2u, 0xAAu));
  EXPECT_FALSE(Load8(regs_, JOYCNT_OFFSET + 2u, &contents));

  // In Bounds
  EXPECT_TRUE(Store8(regs_, JOY_RECV_L_OFFSET, 0x11u));
  EXPECT_TRUE(Load8(regs_, JOY_RECV_L_OFFSET, &contents));
  EXPECT_EQ(0u, contents);
  EXPECT_TRUE(Store8(regs_, JOY_RECV_L_OFFSET + 1u, 0x11u));
  EXPECT_TRUE(Load8(regs_, JOY_RECV_L_OFFSET + 1u, &contents));
  EXPECT_EQ(0u, contents);
  EXPECT_TRUE(Store8(regs_, JOY_RECV_H_OFFSET, 0x11u));
  EXPECT_TRUE(Load8(regs_, JOY_RECV_H_OFFSET, &contents));
  EXPECT_EQ(0u, contents);
  EXPECT_TRUE(Store8(regs_, JOY_RECV_H_OFFSET + 1u, 0x11u));
  EXPECT_TRUE(Load8(regs_, JOY_RECV_H_OFFSET + 1u, &contents));
  EXPECT_EQ(0u, contents);
  EXPECT_TRUE(Store8(regs_, JOY_TRANS_L_OFFSET, 0x11u));
  EXPECT_TRUE(Load8(regs_, JOY_TRANS_L_OFFSET, &contents));
  EXPECT_EQ(0x11u, contents);
  EXPECT_TRUE(Store8(regs_, JOY_TRANS_L_OFFSET + 1u, 0x22u));
  EXPECT_TRUE(Load8(regs_, JOY_TRANS_L_OFFSET + 1u, &contents));
  EXPECT_EQ(0x22u, contents);
  EXPECT_TRUE(Store8(regs_, JOY_TRANS_H_OFFSET, 0x33u));
  EXPECT_TRUE(Load8(regs_, JOY_TRANS_H_OFFSET, &contents));
  EXPECT_EQ(0x33u, contents);
  EXPECT_TRUE(Store8(regs_, JOY_TRANS_H_OFFSET + 1u, 0x44u));
  EXPECT_TRUE(Load8(regs_, JOY_TRANS_H_OFFSET + 1u, &contents));
  EXPECT_EQ(0x44u, contents);
  EXPECT_TRUE(Store8(regs_, JOYSTAT_OFFSET, 0x11u));
  EXPECT_TRUE(Load8(regs_, JOYSTAT_OFFSET, &contents));
  EXPECT_EQ(0u, contents);
  EXPECT_TRUE(Store8(regs_, JOYSTAT_OFFSET + 1u, 0x11u));
  EXPECT_TRUE(Load8(regs_, JOYSTAT_OFFSET + 1u, &contents));
  EXPECT_EQ(0u, contents);

  // Out of Bounds
  EXPECT_TRUE(Store8(regs_, JOYSTAT_OFFSET + 2u, 0xAAu));
  EXPECT_FALSE(Load8(regs_, JOYSTAT_OFFSET + 2u, &contents));
}

TEST_F(PeripheralsTest, GbaPeripheralsRegistersLoad32LE) {
  uint32_t contents;
  EXPECT_TRUE(Store32LE(regs_, SIOMULTI0_OFFSET, 0x11223344u));
  EXPECT_TRUE(Load32LE(regs_, SIOMULTI0_OFFSET, &contents));
  EXPECT_EQ(0u, contents);
  EXPECT_TRUE(Store32LE(regs_, SIOMULTI2_OFFSET, 0x11223344u));
  EXPECT_TRUE(Load32LE(regs_, SIOMULTI2_OFFSET, &contents));
  EXPECT_EQ(0u, contents);
  EXPECT_TRUE(Store32LE(regs_, SIOCNT_OFFSET, 0x11223344u));
  EXPECT_TRUE(Load32LE(regs_, SIOCNT_OFFSET, &contents));
  EXPECT_EQ(0x11223344u, contents);

  // Out of Bounds
  EXPECT_TRUE(Store32LE(regs_, SIOCNT_OFFSET + 4u, 0x11223344u));
  EXPECT_FALSE(Load32LE(regs_, SIOCNT_OFFSET + 4u, &contents));

  // In Bounds
  EXPECT_TRUE(Store32LE(regs_, KEYINPUT_OFFSET, 0x11223344u));
  EXPECT_TRUE(Load32LE(regs_, KEYINPUT_OFFSET, &contents));
  EXPECT_EQ(0x11220000u, contents);
  EXPECT_TRUE(Store32LE(regs_, RCNT_OFFSET, 0x11223344u));
  EXPECT_TRUE(Load32LE(regs_, RCNT_OFFSET, &contents));
  EXPECT_EQ(0x00003344u, contents);

  // Out of Bounds
  EXPECT_TRUE(Store32LE(regs_, RCNT_OFFSET + 4u, 0x11223344u));
  EXPECT_FALSE(Load32LE(regs_, RCNT_OFFSET + 4u, &contents));

  // In Bounds
  EXPECT_TRUE(Store32LE(regs_, JOYCNT_OFFSET, 0x11223344u));
  EXPECT_TRUE(Load32LE(regs_, JOYCNT_OFFSET, &contents));
  EXPECT_EQ(0u, contents);

  // Out of Bounds
  EXPECT_TRUE(Store32LE(regs_, JOYCNT_OFFSET + 4u, 0x11223344u));
  EXPECT_FALSE(Load32LE(regs_, JOYCNT_OFFSET + 4u, &contents));

  // In Bounds
  EXPECT_TRUE(Store32LE(regs_, JOY_RECV_L_OFFSET, 0x11223344u));
  EXPECT_TRUE(Load32LE(regs_, JOY_RECV_L_OFFSET, &contents));
  EXPECT_EQ(0u, contents);
  EXPECT_TRUE(Store32LE(regs_, JOY_TRANS_L_OFFSET, 0x11223344u));
  EXPECT_TRUE(Load32LE(regs_, JOY_TRANS_L_OFFSET, &contents));
  EXPECT_EQ(0x11223344u, contents);
  EXPECT_TRUE(Store32LE(regs_, JOYSTAT_OFFSET, 0x11223344u));
  EXPECT_TRUE(Load32LE(regs_, JOYSTAT_OFFSET, &contents));
  EXPECT_EQ(0u, contents);

  // Out of Bounds
  EXPECT_TRUE(Store32LE(regs_, JOYSTAT_OFFSET + 4u, 0x11223344u));
  EXPECT_FALSE(Load32LE(regs_, JOYSTAT_OFFSET + 4u, &contents));
}