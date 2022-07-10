load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "com_google_googletest",
    sha256 = "b4870bf121ff7795ba20d20bcdd8627b8e088f2d1dab299a031c1034eddc93d5",
    strip_prefix = "googletest-release-1.11.0",
    url = "https://github.com/google/googletest/archive/refs/tags/release-1.11.0.tar.gz",
)

http_archive(
    name = "emsdk",
    sha256 = "74c7c54b3544555ec38d1e9dcc7e90b9f49ed0e04f2cc3fd44663c598af24124",
    strip_prefix = "emsdk-3.1.15/bazel",
    url = "https://github.com/emscripten-core/emsdk/archive/refs/tags/3.1.15.tar.gz",
)

load("@emsdk//:deps.bzl", emsdk_deps = "deps")
emsdk_deps()

load("@emsdk//:emscripten_deps.bzl", emsdk_emscripten_deps = "emscripten_deps")
emsdk_emscripten_deps(emscripten_version = "3.1.15")