load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//emulator/memory/gba:__subpackages__"])

cc_library(
    name = "bios",
    srcs = ["bios.c"],
    hdrs = ["bios.h"],
    deps = [
        "//emulator/memory",
        "//tools/bios_data:data",
    ],
)

cc_test(
    name = "bios_test",
    srcs = ["bios_test.cc"],
    deps = [
        ":bios",
        "@com_google_googletest//:gtest_main",
    ],
)
