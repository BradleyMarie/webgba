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
    name = "obj_visibility",
    srcs = ["obj_visibility.c"],
    hdrs = ["obj_visibility.h"],
    deps = [
        "//emulator/ppu/gba:memory",
        "//emulator/ppu/gba:registers",
        "//emulator/ppu/gba:set",
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
        ":obj_visibility",
        ":obj",
        ":screen",
        ":window",
        "//emulator/ppu/gba:dirty",
    ],
)

cc_test(
    name = "obj_visibility_test",
    srcs = ["obj_visibility_test.cc"],
    deps = [
        ":obj_visibility",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "obj",
    srcs = ["obj.c"],
    hdrs = ["obj.h"],
    deps = [
        ":obj_visibility",
        "//emulator/memory",
        "//emulator/ppu/gba:memory",
        "//emulator/ppu/gba:registers",
    ],
)

cc_library(
    name = "screen",
    srcs = ["screen.c"],
    hdrs = ["screen.h"],
    linkopts = ["-lGL"],
    deps = [
        "//emulator/ppu/gba:memory",
        "//emulator/ppu/gba:registers",
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
        "//emulator/ppu/gba:registers",
    ],
)
