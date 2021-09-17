load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:private"])

cc_library(
    name = "blend",
    srcs = ["blend.c"],
    hdrs = ["blend.h"],
    deps = [
        ":registers",
    ],
)

cc_test(
    name = "blend_test",
    srcs = ["blend_test.cc"],
    deps = [
        ":blend",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "memory",
    hdrs = ["memory.h"],
    visibility = ["//emulator/ppu:__subpackages__"],
)

cc_library(
    name = "ppu",
    srcs = ["ppu.c"],
    hdrs = ["ppu.h"],
    linkopts = ["-lGL"],
    visibility = ["//emulator:__subpackages__"],
    deps = [
        ":blend",
        ":memory",
        ":registers",
        ":screen",
        ":window",
        "//emulator/dma/gba:dma",
        "//emulator/memory",
        "//emulator/platform/gba:platform",
        "//emulator/ppu/gba/bg:affine",
        "//emulator/ppu/gba/bg:bitmap",
        "//emulator/ppu/gba/bg:scrolling",
        "//emulator/ppu/gba/io",
        "//emulator/ppu/gba/oam",
        "//emulator/ppu/gba/obj:draw",
        "//emulator/ppu/gba/palette",
        "//emulator/ppu/gba/vram",
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
    srcs = ["screen.c"],
    hdrs = ["screen.h"],
    linkopts = ["-lGL"],
    visibility = ["//emulator/ppu:__subpackages__"],
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

cc_library(
    name = "window",
    srcs = ["window.c"],
    hdrs = ["window.h"],
    deps = [
        ":registers",
    ],
)
