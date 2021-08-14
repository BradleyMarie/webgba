load("@rules_cc//cc:defs.bzl", "cc_library")

cc_binary(
    name = "webgba.so",
    srcs = ["webgba.c"],
    linkopts = [
        "-Wl,-lGL,--version-script",
        "$(location :webgba.lds)",
    ],
    linkshared = 1,
    deps = [
        ":webgba.lds",
        "//emulator:gba",
        "//third_party/libretro:libretro",
    ],
)