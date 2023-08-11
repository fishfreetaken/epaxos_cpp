
load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "src_lib",
    srcs = [
        "instance_mng.cc",
        "leveldb_kv.cc",
        "instance_node.cc",
    ],
    hdrs = [
        "instance_mng.h",
        "leveldb_kv.h",
        "node_num_cfg.h",
        "instance_node.h",
    ],
    deps = [
        "//proto:instance_cc_proto",
        "//include:include-header",
        "@spdlog//:spdlog_lib",
        "@leveldb//:leveldb_lib",
    ],
    visibility=["//visibility:public"],
)
