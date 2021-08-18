load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:private"])

cc_library(
    name = "bitmap",
    hdrs = ["bitmap.h"],
    srcs = ["bitmap.c"],
    visibility = ["//emulator:__subpackages__"],
    deps = [
        ":types",
    ],
)

cc_library(
    name = "copy_to_fbo",
    hdrs = ["copy_to_fbo.h"],
    srcs = ["copy_to_fbo.c"],
    linkopts = ["-lGL"],
    deps = [
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
    name = "types",
    hdrs = ["types.h"],
    visibility = ["//emulator/ppu/gba:__subpackages__"],
)