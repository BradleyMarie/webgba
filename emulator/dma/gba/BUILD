load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//emulator:__subpackages__"])

cc_library(
    name = "dma",
    srcs = ["dma.c"],
    hdrs = ["dma.h"],
    deps = [
        "//emulator/memory",
        "//emulator/platform/gba:platform",
    ],
)

cc_test(
    name = "dma_test",
    srcs = ["dma_test.cc"],
    deps = [
        ":dma",
        "@com_google_googletest//:gtest_main",
    ],
)
