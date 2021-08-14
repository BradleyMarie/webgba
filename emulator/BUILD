load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//emulator:__subpackages__"])

cc_library(
    name = "gba",
    hdrs = ["gba.h"],
    srcs = ["gba.c"],
    linkopts = ["-lGL"],
    deps = [
        "//emulator/cpu/arm7tdmi:arm7tdmi",
        "//emulator/dma/gba:dma",
        "//emulator/game/gba:game",
        "//emulator/memory/gba:memory",
        "//emulator/peripherals/gba:peripherals",
        "//emulator/peripherals:gamepad",
        "//emulator/platform/gba:platform",
        "//emulator/ppu/gba:ppu",
        "//emulator/sound/gba:sound",
        "//emulator/timers/gba:timers",
    ],
)

cc_test(
    name = "gba_test",
    srcs = ["gba_test.cc"],
    deps = [
        ":gba",
        "@com_google_googletest//:gtest_main",
    ],
)