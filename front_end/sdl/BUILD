load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "webgba",
    srcs = ["webgba.c"],
    linkopts = ["-lSDL2"],
    deps = [
        "//emulator:gba",
    ],
)
