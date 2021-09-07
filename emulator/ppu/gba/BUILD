load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:private"])

cc_library(
    name = "memory",
    hdrs = ["memory.h"],
    visibility = ["//emulator/ppu:__subpackages__"],
)

cc_library(
    name = "object_state",
    srcs = ["object_state.c"],
    hdrs = ["object_state.h"],
    deps = [
        ":memory",
        ":object_set",
    ],
)

cc_test(
    name = "object_state_test",
    srcs = ["object_state_test.cc"],
    deps = [
        ":object_state",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "object_set",
    hdrs = ["object_set.h"],
)

cc_test(
    name = "object_set_test",
    srcs = ["object_set_test.cc"],
    deps = [
        ":object_set",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "ppu",
    hdrs = ["ppu.h"],
    srcs = ["ppu.c"],
    linkopts = ["-lGL"],
    visibility = ["//emulator:__subpackages__"],
    deps = [
        ":memory",
        ":registers",
        ":screen",
        "//emulator/dma/gba:dma",
        "//emulator/memory:memory",
        "//emulator/ppu/gba/io:io",
        "//emulator/platform/gba:platform",
        "//emulator/ppu/gba/bg:affine",
        "//emulator/ppu/gba/bg:bitmap",
        "//emulator/ppu/gba/bg:scrolling",
        "//emulator/ppu/gba/oam:oam",
        "//emulator/ppu/gba/palette:palette",
        "//emulator/ppu/gba/vram:vram",
    ],
)

cc_test(
    name = "ppu_test",
    srcs = ["ppu_test.cc"],
    deps = [
        ":ppu",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "registers",
    hdrs = ["registers.h"],
    visibility = ["//emulator/ppu:__subpackages__"],
)

cc_library(
    name = "screen",
    hdrs = ["screen.h"],
    srcs = ["screen.c"],
    visibility = ["//emulator/ppu:__subpackages__"],
    linkopts = ["-lGL"],
    deps = [
        ":memory",
        ":registers",
    ],
)

cc_test(
    name = "screen_test",
    srcs = ["screen_test.cc"],
    linkopts = ["-lGL"],
    deps = [
        ":screen",
        "@com_google_googletest//:gtest_main",
    ],
)