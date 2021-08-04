load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//emulator:__subpackages__"])

cc_library(
    name = "dma",
    hdrs = ["dma.h"],
    srcs = ["dma.c"],
    deps = [
        "//emulator/memory:memory",
        "//emulator/platform/gba:platform",
    ],
)