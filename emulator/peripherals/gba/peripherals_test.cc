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
    raised_ = false;
    InterruptLine *irq =
        InterruptLineAllocate(nullptr, InterruptSetLevel, nullptr);
    ASSERT_NE(irq, nullptr);
    ASSERT_TRUE(GbaPlatformAllocate(irq, &plat_, &plat_regs_));
    ASSERT_TRUE(
        GbaPeripheralsAllocate(plat_, &peripherals_, &gamepad_, &regs_));

    ASSERT_TRUE(Store16LE(plat_regs_, IE_OFFSET, 0xFFFFu));
    ASSERT_TRUE(Store16LE(plat_regs_, IF_OFFSET, 0xFFFFu));
    ASSERT_TRUE(Store16LE(plat_regs_, IME_OFFSET, 0xFFFFu));
  }

  void TearDown() override {
    GbaPlatformRelease(plat_);
    MemoryFree(plat_regs_);
    GbaPeripheralsFree(peripherals_);
    GamePadFree(gamepad_);
    MemoryFree(regs_);
  }

 protected:
  static void InterruptSetLevel(void *context, bool raised) {
    raised_ = raised;
  }

  static bool raised_;

  GbaPlatform *plat_;
  Memory *plat_regs_;
  GbaPeripherals *peripherals_;
  GamePad *gamepad_;
  Memory *regs_;
};

bool PeripheralsTest::raised_ = false;

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
  EXPECT_EQ(0x03FFu, contents);
  EXPECT_TRUE(Store16LE(regs_, KEYCNT_OFFSET, 0x1122u));
  EXPECT_TRUE(Load16LE(regs_, KEYCNT_OFFSET, &contents));
  EXPECT_EQ(0x1122u, contents);
  EXPECT_TRUE(Load16LE(regs_, RCNT_OFFSET, &contents));
  EXPECT_EQ(0x8000u, contents);
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
  EXPECT_EQ(0xFFu, contents);
  EXPECT_TRUE(Store8(regs_, KEYINPUT_OFFSET + 1u, 0x22u));
  EXPECT_TRUE(Load8(regs_, KEYINPUT_OFFSET + 1u, &contents));
  EXPECT_EQ(0x03u, contents);
  EXPECT_TRUE(Store8(regs_, KEYCNT_OFFSET, 0x11u));
  EXPECT_TRUE(Load8(regs_, KEYCNT_OFFSET, &contents));
  EXPECT_EQ(0x11u, contents);
  EXPECT_TRUE(Store8(regs_, KEYCNT_OFFSET + 1u, 0x22u));
  EXPECT_TRUE(Load8(regs_, KEYCNT_OFFSET + 1u, &contents));
  EXPECT_EQ(0x22u, contents);
  EXPECT_TRUE(Load8(regs_, RCNT_OFFSET, &contents));
  EXPECT_EQ(0x00u, contents);
  EXPECT_TRUE(Load8(regs_, RCNT_OFFSET + 1u, &contents));
  EXPECT_EQ(0x80u, contents);
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
  EXPECT_EQ(0x112203FFu, contents);
  EXPECT_TRUE(Load32LE(regs_, RCNT_OFFSET, &contents));
  EXPECT_EQ(0x00008000u, contents);
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

TEST_F(PeripheralsTest, GamePadInterruptDisabled) {
  GamePadToggleSelect(gamepad_, true);
  EXPECT_FALSE(raised_);
}

TEST_F(PeripheralsTest, GamePadInterruptOrA) {
  EXPECT_FALSE(raised_);
  EXPECT_TRUE(Store16LE(regs_, KEYCNT_OFFSET, 0x4003u));
  GamePadToggleSelect(gamepad_, true);
  EXPECT_FALSE(raised_);
  GamePadToggleSelect(gamepad_, false);
  EXPECT_FALSE(raised_);
  GamePadToggleA(gamepad_, true);
  EXPECT_TRUE(raised_);
}

TEST_F(PeripheralsTest, GamePadInterruptOrB) {
  EXPECT_FALSE(raised_);
  EXPECT_TRUE(Store16LE(regs_, KEYCNT_OFFSET, 0x4003u));
  GamePadToggleSelect(gamepad_, true);
  EXPECT_FALSE(raised_);
  GamePadToggleB(gamepad_, true);
  EXPECT_TRUE(raised_);
}

TEST_F(PeripheralsTest, GamePadInterruptAnd) {
  EXPECT_FALSE(raised_);
  EXPECT_TRUE(Store16LE(regs_, KEYCNT_OFFSET, 0xC003u));
  GamePadToggleSelect(gamepad_, true);
  EXPECT_FALSE(raised_);
  GamePadToggleSelect(gamepad_, false);
  EXPECT_FALSE(raised_);
  GamePadToggleA(gamepad_, true);
  EXPECT_FALSE(raised_);
  GamePadToggleB(gamepad_, true);
  EXPECT_TRUE(raised_);
  GamePadToggleSelect(gamepad_, true);
  EXPECT_TRUE(raised_);
}

TEST_F(PeripheralsTest, GamePadUp) {
  GamePadToggleUp(gamepad_, true);
  uint16_t value;
  EXPECT_TRUE(Load16LE(regs_, KEYINPUT_OFFSET, &value));
  EXPECT_EQ(0x03BF, value);

  EXPECT_TRUE(Store16LE(regs_, KEYCNT_OFFSET, 0x41FFu));
  GamePadToggleUp(gamepad_, true);
  EXPECT_FALSE(raised_);
  GamePadToggleUp(gamepad_, false);
  EXPECT_FALSE(raised_);
}

TEST_F(PeripheralsTest, GamePadDown) {
  GamePadToggleDown(gamepad_, true);
  uint16_t value;
  EXPECT_TRUE(Load16LE(regs_, KEYINPUT_OFFSET, &value));
  EXPECT_EQ(0x037F, value);

  EXPECT_TRUE(Store16LE(regs_, KEYCNT_OFFSET, 0x41FFu));
  GamePadToggleDown(gamepad_, true);
  EXPECT_FALSE(raised_);
  GamePadToggleDown(gamepad_, false);
  EXPECT_FALSE(raised_);
}

TEST_F(PeripheralsTest, GamePadLeft) {
  GamePadToggleLeft(gamepad_, true);
  uint16_t value;
  EXPECT_TRUE(Load16LE(regs_, KEYINPUT_OFFSET, &value));
  EXPECT_EQ(0x03DF, value);

  EXPECT_TRUE(Store16LE(regs_, KEYCNT_OFFSET, 0x41FFu));
  GamePadToggleLeft(gamepad_, true);
  EXPECT_FALSE(raised_);
  GamePadToggleLeft(gamepad_, false);
  EXPECT_FALSE(raised_);
}

TEST_F(PeripheralsTest, GamePadRight) {
  GamePadToggleRight(gamepad_, true);
  uint16_t value;
  EXPECT_TRUE(Load16LE(regs_, KEYINPUT_OFFSET, &value));
  EXPECT_EQ(0x03EF, value);

  EXPECT_TRUE(Store16LE(regs_, KEYCNT_OFFSET, 0x41FFu));
  GamePadToggleRight(gamepad_, true);
  EXPECT_FALSE(raised_);
  GamePadToggleRight(gamepad_, false);
  EXPECT_FALSE(raised_);
}

TEST_F(PeripheralsTest, GamePadA) {
  GamePadToggleA(gamepad_, true);
  uint16_t value;
  EXPECT_TRUE(Load16LE(regs_, KEYINPUT_OFFSET, &value));
  EXPECT_EQ(0x03FE, value);

  EXPECT_TRUE(Store16LE(regs_, KEYCNT_OFFSET, 0x41FFu));
  GamePadToggleA(gamepad_, true);
  EXPECT_FALSE(raised_);
  GamePadToggleA(gamepad_, false);
  EXPECT_FALSE(raised_);
}

TEST_F(PeripheralsTest, GamePadB) {
  GamePadToggleB(gamepad_, true);
  uint16_t value;
  EXPECT_TRUE(Load16LE(regs_, KEYINPUT_OFFSET, &value));
  EXPECT_EQ(0x03FD, value);

  EXPECT_TRUE(Store16LE(regs_, KEYCNT_OFFSET, 0x41FFu));
  GamePadToggleB(gamepad_, true);
  EXPECT_FALSE(raised_);
  GamePadToggleB(gamepad_, false);
  EXPECT_FALSE(raised_);
}

TEST_F(PeripheralsTest, GamePadL) {
  GamePadToggleL(gamepad_, true);
  uint16_t value;
  EXPECT_TRUE(Load16LE(regs_, KEYINPUT_OFFSET, &value));
  EXPECT_EQ(0x01FF, value);

  EXPECT_TRUE(Store16LE(regs_, KEYCNT_OFFSET, 0x41FFu));
  GamePadToggleL(gamepad_, true);
  EXPECT_FALSE(raised_);
  GamePadToggleL(gamepad_, false);
  EXPECT_FALSE(raised_);
}

TEST_F(PeripheralsTest, GamePadR) {
  GamePadToggleR(gamepad_, true);
  uint16_t value;
  EXPECT_TRUE(Load16LE(regs_, KEYINPUT_OFFSET, &value));
  EXPECT_EQ(0x02FF, value);

  EXPECT_TRUE(Store16LE(regs_, KEYCNT_OFFSET, 0x41FFu));
  GamePadToggleR(gamepad_, true);
  EXPECT_FALSE(raised_);
  GamePadToggleR(gamepad_, false);
  EXPECT_FALSE(raised_);
}

TEST_F(PeripheralsTest, GamePadStart) {
  GamePadToggleStart(gamepad_, true);
  uint16_t value;
  EXPECT_TRUE(Load16LE(regs_, KEYINPUT_OFFSET, &value));
  EXPECT_EQ(0x03F7, value);

  EXPECT_TRUE(Store16LE(regs_, KEYCNT_OFFSET, 0x41FFu));
  GamePadToggleStart(gamepad_, true);
  EXPECT_FALSE(raised_);
  GamePadToggleStart(gamepad_, false);
  EXPECT_FALSE(raised_);
}

TEST_F(PeripheralsTest, GamePadSelect) {
  GamePadToggleSelect(gamepad_, true);
  uint16_t value;
  EXPECT_TRUE(Load16LE(regs_, KEYINPUT_OFFSET, &value));
  EXPECT_EQ(0x03FB, value);

  EXPECT_TRUE(Store16LE(regs_, KEYCNT_OFFSET, 0x41FFu));
  GamePadToggleSelect(gamepad_, true);
  EXPECT_FALSE(raised_);
  GamePadToggleSelect(gamepad_, false);
  EXPECT_FALSE(raised_);
}