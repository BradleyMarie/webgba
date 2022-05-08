load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//visibility:private"])

cc_library(
    name = "bg_bitmap",
    srcs = ["bg_bitmap.c"],
    hdrs = ["bg_bitmap.h"],
    linkopts = ["-lGL"],
    deps = [
        "//emulator/ppu/gba:memory",
        "//emulator/ppu/gba:registers",
    ],
)