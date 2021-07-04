load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//emulator:__subpackages__"])

cc_library(
    name = "bios_data",
    hdrs = ["bios_data.h"],
    visibility = ["//visibility:private"],
)

cc_library(
    name = "bios",
    hdrs = ["bios.h"],
    srcs = ["bios.c"],
    deps = [
        ":bios_data",
        "//emulator/memory:memory",
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