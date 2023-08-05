
load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "src_lib",
    srcs = [
        "instance_mng.cc"
    ],
    hdrs = [
        "instance_mng.h",
    ],
    deps = [
        "//proto:instance_cc_proto",
        "//include:include-header",
        "@spdlog//:spdlog_lib",
        "//src:src_level_lib",
    ],
    visibility=["//visibility:public"],
)

cc_library(
    name = "src_level_lib",
    srcs = [
        "leveldb_kv.cc",
    ],
    hdrs = [
        "leveldb_kv.h",
    ],
    deps = [
        "//include:include-header",
        "@leveldb//:leveldb_lib",
        "@spdlog//:spdlog_lib"
    ],
    visibility=["//visibility:public"],
)


