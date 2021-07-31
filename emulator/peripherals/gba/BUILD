load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//emulator:__subpackages__"])

cc_library(
    name = "peripherals",
    hdrs = ["peripherals.h"],
    srcs = ["peripherals.c"],
    deps = [
        "//emulator/memory:memory",
    ],
)