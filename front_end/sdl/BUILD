load("@emsdk//emscripten_toolchain:wasm_rules.bzl", "wasm_cc_binary")
load("@rules_cc//cc:defs.bzl", "cc_binary")

config_setting(
    name = "wasm_build",
    values = {"cpu": "wasm"},
)

cc_binary(
    name = "webgba",
    srcs = ["webgba.c"],
    data = ["//third_party/varooom:game.gba"],
    additional_linker_inputs = ["//third_party/varooom:game.gba"],
    linkopts = select({
        "//conditions:default": ["-lSDL"],
        ":wasm_build": [
            "--oformat=html",
            "--preload-file=$(location //third_party/varooom:game.gba)@/game.gba",
            "-s MINIFY_HTML=0",
            "-s INITIAL_MEMORY=134217728",
            "-s ASSERTIONS=1",
        ],
    }),
    deps = select({
        "//conditions:default": [
            "//emulator:gba",
            "//third_party/libsamplerate:libsamplerate",
        ],
        ":wasm_build": [
            "//emulator:gba",
            "//third_party/libsamplerate:libsamplerate",
        ],
    }),
)

wasm_cc_binary(
    name = "webgba_wasm",
    cc_target = ":webgba",
)
