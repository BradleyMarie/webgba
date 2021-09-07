load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//emulator/ppu/gba:__subpackages__"])

cc_library(
    name = "affine",
    srcs = ["affine.c"],
    hdrs = ["affine.h"],
    deps = [
        "//emulator/memory",
        "//emulator/ppu/gba:memory",
        "//emulator/ppu/gba:registers",
        "//emulator/ppu/gba:screen",
    ],
)

cc_library(
    name = "bitmap",
    srcs = ["bitmap.c"],
    hdrs = ["bitmap.h"],
    deps = [
        "//emulator/memory",
        "//emulator/ppu/gba:memory",
        "//emulator/ppu/gba:registers",
        "//emulator/ppu/gba:screen",
    ],
)

cc_library(
    name = "scrolling",
    srcs = ["scrolling.c"],
    hdrs = ["scrolling.h"],
    deps = [
        "//emulator/memory",
        "//emulator/ppu/gba:memory",
        "//emulator/ppu/gba:registers",
        "//emulator/ppu/gba:screen",
    ],
)
