load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//emulator/ppu/gba:__subpackages__"])

cc_library(
    name = "draw",
    srcs = ["draw.c"],
    hdrs = ["draw.h"],
    deps = [
        ":visibility",
        "//emulator/memory",
        "//emulator/ppu/gba:memory",
        "//emulator/ppu/gba:registers",
        "//emulator/ppu/gba:screen",
    ],
)

cc_library(
    name = "set",
    hdrs = ["set.h"],
)

cc_test(
    name = "set_test",
    srcs = ["set_test.cc"],
    deps = [
        ":set",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "visibility",
    srcs = ["visibility.c"],
    hdrs = ["visibility.h"],
    visibility = ["//emulator/ppu:__subpackages__"],
    deps = [
        ":set",
        "//emulator/ppu/gba:memory",
        "//emulator/ppu/gba:registers",
    ],
)

cc_test(
    name = "visibility_test",
    srcs = ["visibility_test.cc"],
    deps = [
        ":visibility",
        "@com_google_googletest//:gtest_main",
    ],
)
