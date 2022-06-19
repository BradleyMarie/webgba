load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:private"])

cc_library(
    name = "bg_affine",
    srcs = ["bg_affine.c"],
    hdrs = ["bg_affine.h"],
    deps = [
        "//emulator/memory",
        "//emulator/ppu/gba:memory",
        "//emulator/ppu/gba:registers",
    ],
)

cc_library(
    name = "bg_bitmap",
    srcs = ["bg_bitmap.c"],
    hdrs = ["bg_bitmap.h"],
    deps = [
        "//emulator/memory",
        "//emulator/ppu/gba:memory",
        "//emulator/ppu/gba:registers",
    ],
)

cc_library(
    name = "bg_scrolling",
    srcs = ["bg_scrolling.c"],
    hdrs = ["bg_scrolling.h"],
    deps = [
        "//emulator/memory",
        "//emulator/ppu/gba:memory",
        "//emulator/ppu/gba:registers",
    ],
)

cc_library(
    name = "blend",
    srcs = ["blend.c"],
    hdrs = ["blend.h"],
    deps = [
        "//emulator/ppu/gba:registers",
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
    name = "render",
    srcs = ["render.c"],
    hdrs = ["render.h"],
    visibility = ["//emulator/ppu:__subpackages__"],
    deps = [
        ":bg_affine",
        ":bg_bitmap",
        ":bg_scrolling",
        ":blend",
        ":obj",
        ":window",
        "//emulator/ppu/gba:dirty",
        "//emulator:screen",
    ],
)

cc_library(
    name = "obj",
    srcs = ["obj.c"],
    hdrs = ["obj.h"],
    deps = [
        "//emulator/memory",
        "//emulator/ppu/gba:memory",
        "//emulator/ppu/gba:registers",
    ],
)

cc_library(
    name = "window",
    srcs = ["window.c"],
    hdrs = ["window.h"],
    deps = [
        "//emulator/ppu/gba:registers",
    ],
)
