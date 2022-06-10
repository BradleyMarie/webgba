load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//emulator:__subpackages__"])

cc_library(
    name = "gba",
    srcs = ["gba.c"],
    hdrs = ["gba.h"],
    linkopts = ["-lGL"],
    visibility = [
        "//front_end:__subpackages__",
        "//tools/benchmark:__subpackages__",
    ],
    deps = [
        ":screen",
        "//emulator/cpu/arm7tdmi",
        "//emulator/dma/gba:dma",
        "//emulator/game/gba:game",
        "//emulator/memory/gba:memory",
        "//emulator/peripherals:gamepad",
        "//emulator/peripherals/gba:peripherals",
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
        ":screen",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "screen",
    srcs = ["screen.c"],
    hdrs = ["screen.h"],
    linkopts = ["-lGL"],
    visibility = [
        "//emulator:__subpackages__",
        "//front_end:__subpackages__",
        "//tools/benchmark:__subpackages__",
    ],
)