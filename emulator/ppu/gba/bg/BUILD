load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//emulator/ppu/gba:__subpackages__"])

cc_library(
    name = "affine",
    hdrs = ["affine.h"],
    srcs = ["affine.c"],
    deps = [
        "//emulator/memory:memory",
        "//emulator/ppu/gba:memory",
        "//emulator/ppu/gba:registers",
        "//emulator/ppu/gba:screen",
    ],
)

cc_library(
    name = "bitmap",
    hdrs = ["bitmap.h"],
    srcs = ["bitmap.c"],
    deps = [
        "//emulator/memory:memory",
        "//emulator/ppu/gba:memory",
        "//emulator/ppu/gba:registers",
        "//emulator/ppu/gba:screen",
    ],
)

cc_library(
    name = "linear",
    hdrs = ["linear.h"],
    srcs = ["linear.c"],
    deps = [
        "//emulator/memory:memory",
        "//emulator/ppu/gba:memory",
        "//emulator/ppu/gba:registers",
        "//emulator/ppu/gba:screen",
    ],
)