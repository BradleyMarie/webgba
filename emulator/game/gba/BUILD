load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//emulator:__subpackages__"])

cc_library(
    name = "game",
    hdrs = ["game.h"],
    srcs = ["game.c"],
    deps = [
        "//emulator/memory:memory",
    ],
)