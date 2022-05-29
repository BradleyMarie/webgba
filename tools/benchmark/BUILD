load("@emsdk//emscripten_toolchain:wasm_rules.bzl", "wasm_cc_binary")
load("@rules_cc//cc:defs.bzl", "cc_binary")

config_setting(
    name = "wasm_build",
    values = {"cpu": "wasm"},
)

cc_binary(
    name = "benchmark",
    srcs = ["benchmark.cc"],
    data = ["//third_party/varooom:game.gba"],
    additional_linker_inputs = ["//third_party/varooom:game.gba"],
    linkopts = select({
        "//conditions:default": [
            "-lGL",
            "-lprofiler",
        ],
        ":wasm_build": [
            "--oformat=html",
            "--preload-file=$(location //third_party/varooom:game.gba)@/game.gba",
            "-s MINIFY_HTML=0",
            "-s INITIAL_MEMORY=134217728",
            "-s ASSERTIONS=0",
            "-s MAX_WEBGL_VERSION=2",
            "-s MIN_WEBGL_VERSION=2",
        ],
    }),
    deps = [
        "//emulator:gba",
    ],
)

wasm_cc_binary(
    name = "benchmark_wasm",
    cc_target = ":benchmark",
)