load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//emulator:__subpackages__"])

cc_library(
    name = "timers",
    hdrs = ["timers.h"],
    srcs = ["timers.c"],
    deps = [
        "//emulator/memory:memory",
    ],
)