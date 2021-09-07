load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//emulator:__subpackages__"])

cc_library(
    name = "game",
    srcs = ["game.c"],
    hdrs = ["game.h"],
    deps = [
        "//emulator/memory",
    ],
)
