load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//emulator:__subpackages__"])

cc_library(
    name = "memory",
    hdrs = ["memory.h"],
    srcs = ["memory.c"],
    deps = [
        "//emulator/memory/gba/bad:bad",
        "//emulator/memory/gba/bios:bios",
        "//emulator/memory/gba/io:io",
        "//emulator/memory/gba/iram:iram",
        "//emulator/memory/gba/wram:wram",
    ],
)

cc_test(
    name = "memory_test",
    srcs = ["memory_test.cc"],
    deps = [
        ":memory",
        "//emulator/memory/gba/bios:bios_data",
        "@com_google_googletest//:gtest_main",
    ],
)