load("@rules_cc//cc:defs.bzl", "cc_proto_library")
load("@rules_proto//proto:defs.bzl", "proto_library")

cc_proto_library(
    name = "conf_cc_proto",
    deps = [":conf_proto"],
)

cc_proto_library(
    name = "instance_cc_proto",
    deps = [":instance_proto"],
)

proto_library(
    name = "instance_proto",
    srcs = ["instance.proto"],
    deps = [":conf_proto"],
)

proto_library(
    name = "conf_proto",
    srcs = ["conf.proto"],
)

package(default_visibility = ["//visibility:public"])
