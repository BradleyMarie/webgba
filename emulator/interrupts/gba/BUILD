load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//emulator:__subpackages__"])

cc_library(
    name = "interrupt_controller",
    hdrs = ["interrupt_controller.h"],
    srcs = ["interrupt_controller.c"],
    deps = [
        "//emulator/interrupts:interrupt_line",
    ],
)