load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:private"])

cc_library(
    name = "bg_affine",
    srcs = ["bg_affine.c"],
    hdrs = ["bg_affine.h"],
    linkopts = ["-lGL"],
    deps = [
        "//emulator/ppu/gba:dirty",
        "//emulator/ppu/gba:memory",
        "//emulator/ppu/gba:registers",
    ],
)

cc_library(
    name = "bg_bitmap_mode3",
    srcs = ["bg_bitmap_mode3.c"],
    hdrs = ["bg_bitmap_mode3.h"],
    linkopts = ["-lGL"],
    deps = [
        "//emulator/ppu/gba:dirty",
        "//emulator/ppu/gba:memory",
        "//emulator/ppu/gba:registers",
    ],
)

cc_library(
    name = "bg_bitmap_mode4",
    srcs = ["bg_bitmap_mode4.c"],
    hdrs = ["bg_bitmap_mode4.h"],
    linkopts = ["-lGL"],
    deps = [
        "//emulator/ppu/gba:dirty",
        "//emulator/ppu/gba:memory",
        "//emulator/ppu/gba:registers",
    ],
)

cc_library(
    name = "bg_bitmap_mode5",
    srcs = ["bg_bitmap_mode5.c"],
    hdrs = ["bg_bitmap_mode5.h"],
    linkopts = ["-lGL"],
    deps = [
        "//emulator/ppu/gba:dirty",
        "//emulator/ppu/gba:memory",
        "//emulator/ppu/gba:registers",
    ],
)

cc_library(
    name = "mosaic",
    srcs = ["mosaic.c"],
    hdrs = ["mosaic.h"],
    linkopts = ["-lGL"],
    deps = [
        "//emulator/ppu/gba:dirty",
        "//emulator/ppu/gba:memory",
        "//emulator/ppu/gba:registers",
    ],
)

cc_library(
    name = "palette_large",
    srcs = ["palette_large.c"],
    hdrs = ["palette_large.h"],
    linkopts = ["-lGL"],
    deps = [
        "//emulator/ppu/gba:dirty",
        "//emulator/ppu/gba:memory",
    ],
)

cc_library(
    name = "palette_small",
    srcs = ["palette_small.c"],
    hdrs = ["palette_small.h"],
    linkopts = ["-lGL"],
    deps = [
        "//emulator/ppu/gba:dirty",
        "//emulator/ppu/gba:memory",
    ],
)

cc_library(
    name = "render",
    srcs = ["render.c"],
    hdrs = ["render.h"],
    visibility = ["//emulator/ppu:__subpackages__"],
    deps = [
        ":bg_affine",
        ":bg_bitmap_mode3",
        ":bg_bitmap_mode4",
        ":bg_bitmap_mode5",
        ":mosaic",
        ":palette_large",
        ":palette_small",
        "//emulator/ppu/gba:dirty",
        "//emulator/ppu/gba:draw_manager",
        "//emulator/ppu/gba:memory",
        "//emulator/ppu/gba:registers",
    ],
)