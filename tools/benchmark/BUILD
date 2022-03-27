cc_binary(
    name = "benchmark",
    srcs = ["benchmark.cc"],
    linkopts = [
        "-lGL",
        "-lprofiler",
        "-lpthread",
    ],
    deps = [
        "//emulator:gba",
    ],
)
