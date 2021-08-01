load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//emulator:__subpackages__"])

cc_library(
    name = "sound",
    hdrs = ["sound.h"],
    srcs = ["sound.c"],
    deps = [
        "//emulator/memory:memory",
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