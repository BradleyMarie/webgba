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
        "//conditions:default": ["-lSDL2"],
        ":wasm_build": [
            "--oformat=html",
            "--preload-file=$(location //third_party/varooom:game.gba)@/game.gba",
            "-s DEFAULT_LIBRARY_FUNCS_TO_INCLUDE=$$autoResumeAudioContext,$$dynCall",
            "-s MINIFY_HTML=0",
            "-s INITIAL_MEMORY=134217728",
            "-s MAX_WEBGL_VERSION=2",
            "-s MIN_WEBGL_VERSION=2",
        ],
    }),
    deps = select({
        "//conditions:default": [
            "//emulator:gba",
            "//emulator:screen",
        ],
        ":wasm_build": [
            "//emulator:gba",
            "//emulator:screen",
            "//third_party/sdl2",
        ],
    }),
)

wasm_cc_binary(
    name = "webgba_wasm",
    cc_target = ":webgba",
)