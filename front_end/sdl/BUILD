load("@emsdk//emscripten_toolchain:wasm_rules.bzl", "wasm_cc_binary")
load("@rules_cc//cc:defs.bzl", "cc_binary")

cc_binary(
    name = "webgba",
    srcs = ["webgba.c"],
    linkopts = ["-lSDL"],
    deps = [
        "//emulator:gba",
    ],
)

wasm_cc_binary(
    name = "webgba-wasm",
    cc_target = ":webgba",
)