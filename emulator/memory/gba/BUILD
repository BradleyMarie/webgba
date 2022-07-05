load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//emulator:__subpackages__"])

cc_library(
    name = "memory",
    srcs = ["memory.c"],
    hdrs = ["memory.h"],
    deps = [
        "//emulator/memory/gba/bad",
        "//emulator/memory/gba/bios",
        "//emulator/memory/gba/io",
        "//emulator/memory/gba/open_bus",
    ],
)

cc_test(
    name = "memory_test",
    srcs = ["memory_test.cc"],
    shard_count = 13,
    deps = [
        ":memory",
        "//tools/bios_data:data",
        "@com_google_googletest//:gtest_main",
    ],
)
