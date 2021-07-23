load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:private"])

cc_library(
    name = "ppu",
    hdrs = ["ppu.h"],
    srcs = ["ppu.c"],
    visibility = ["//emulator:__subpackages__"],
    deps = [
        ":types",
        "//emulator/interrupts/gba:interrupt_controller",
        "//emulator/memory:memory",
    ],
)

cc_library(
    name = "types",
    hdrs = ["types.h"],
    visibility = ["//emulator/ppu/gba:__subpackages__"],
)