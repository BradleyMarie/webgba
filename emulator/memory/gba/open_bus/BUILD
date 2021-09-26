load("@rules_cc//cc:defs.bzl", "cc_library")

package(default_visibility = ["//emulator/memory/gba:__subpackages__"])

cc_library(
    name = "open_bus",
    srcs = ["open_bus.c"],
    hdrs = ["open_bus.h"],
    deps = [
        "//emulator/memory",
    ],
)

cc_test(
    name = "open_bus_test",
    srcs = ["open_bus_test.cc"],
    deps = [
        ":open_bus",
        "@com_google_googletest//:gtest_main",
    ],
)
