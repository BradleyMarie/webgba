load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:private"])

cc_library(
    name = "bg_bitmap",
    srcs = ["bg_bitmap.c"],
    hdrs = ["bg_bitmap.h"],
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