load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:private"])

cc_library(
    name = "dirty",
    hdrs = ["dirty.h"],
    srcs = ["dirty.c"],
    visibility = ["//emulator/ppu:__subpackages__"],
    deps = [
        ":memory",
        ":set",
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
        ":dirty",
        ":memory",
        ":registers",
        "//emulator/dma/gba:dma",
        "//emulator/memory",
        "//emulator/platform/gba:platform",
        "//emulator/ppu/gba/io",
        "//emulator/ppu/gba/oam",
        "//emulator/ppu/gba/palette",
        "//emulator/ppu/gba/software:render",
        "//emulator/ppu/gba/vram",
        "//util:macros",
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
    name = "set",
    hdrs = ["set.h"],
    visibility = ["//emulator/ppu:__subpackages__"],
)

cc_test(
    name = "set_test",
    srcs = ["set_test.cc"],
    deps = [
        ":set",
        "@com_google_googletest//:gtest_main",
    ],
)