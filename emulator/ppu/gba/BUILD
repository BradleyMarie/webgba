load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:private"])

cc_library(
    name = "bitmap",
    hdrs = ["bitmap.h"],
    srcs = ["bitmap.c"],
    visibility = ["//emulator:__subpackages__"],
    deps = [
        ":screen",
        ":types",
    ],
)

cc_library(
    name = "ppu",
    hdrs = ["ppu.h"],
    srcs = ["ppu.c"],
    linkopts = ["-lGL"],
    visibility = ["//emulator:__subpackages__"],
    deps = [
        ":bitmap",
        ":screen",
        ":types",
        "//emulator/memory:memory",
        "//emulator/platform/gba:platform",
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
    name = "screen",
    hdrs = ["screen.h"],
    srcs = ["screen.c"],
    linkopts = ["-lGL"],
    deps = [
        ":types",
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
    name = "types",
    hdrs = ["types.h"],
    visibility = ["//emulator/ppu/gba:__subpackages__"],
)