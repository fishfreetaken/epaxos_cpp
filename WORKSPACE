load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
  name = "com_google_googletest",
  urls = ["https://github.com/google/googletest/archive/b796f7d44681514f58a683a3a71ff17c94edb0c1.zip"],
  strip_prefix = "googletest-b796f7d44681514f58a683a3a71ff17c94edb0c1",
)

http_archive(
    name = "com_google_protobuf",
    strip_prefix = "protobuf-3f13a1de2cfa2d6788b803da82c03233e8f315e6",
    url = "https://github.com/protocolbuffers/protobuf/archive/3f13a1de2cfa2d6788b803da82c03233e8f315e6.zip",
)

new_local_repository(
    name = "leveldb",
    path = "/data/home/vapourchen/github/leveldb",
    build_file = "BUILD.leveldb",
)

new_local_repository(
    name = "spdlog",
    path = "/data/home/vapourchen/github/spdlog",
    build_file = "BUILD.spdlog",
)

load("@com_google_protobuf//:protobuf_deps.bzl", "protobuf_deps")

protobuf_deps()
