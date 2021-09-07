load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//emulator/ppu/gba:__subpackages__"])

cc_library(
    name = "objects",
    srcs = ["objects.c"],
    hdrs = ["objects.h"],
    deps = [
        "//emulator/memory",
        "//emulator/ppu/gba:memory",
        "//emulator/ppu/gba:object_state",
        "//emulator/ppu/gba:registers",
        "//emulator/ppu/gba:screen",
    ],
)
