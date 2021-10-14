load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//emulator:__subpackages__"])

cc_library(
    name = "direct_sound",
    srcs = ["direct_sound.c"],
    hdrs = ["direct_sound.h"],
    deps = [
        "//emulator/dma/gba:dma",
    ],
)

cc_library(
    name = "sound",
    srcs = ["sound.c"],
    hdrs = ["sound.h"],
    deps = [
        "//emulator/memory",
    ],
)

cc_test(
    name = "sound_test",
    srcs = ["sound_test.cc"],
    deps = [
        ":sound",
        "@com_google_googletest//:gtest_main",
    ],
)
