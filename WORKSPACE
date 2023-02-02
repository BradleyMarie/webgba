load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "com_google_googletest",
    sha256 = "b4870bf121ff7795ba20d20bcdd8627b8e088f2d1dab299a031c1034eddc93d5",
    strip_prefix = "googletest-release-1.11.0",
    url = "https://github.com/google/googletest/archive/refs/tags/release-1.11.0.tar.gz",
)

http_archive(
    name = "emsdk",
    sha256 = "1d38b7375e12e85197165a4c51d76d90e1d9db8c2c593b64cfaec4338af54750",
    strip_prefix = "emsdk-3.1.31/bazel",
    url = "https://github.com/emscripten-core/emsdk/archive/refs/tags/3.1.31.tar.gz",
)

load("@emsdk//:deps.bzl", emsdk_deps = "deps")
emsdk_deps()

load("@emsdk//:emscripten_deps.bzl", emsdk_emscripten_deps = "emscripten_deps")
emsdk_emscripten_deps(emscripten_version = "3.1.31")