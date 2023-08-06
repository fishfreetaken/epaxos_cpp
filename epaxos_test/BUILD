load("@rules_cc//cc:defs.bzl", "cc_proto_library") 
load("@rules_proto//proto:defs.bzl", "proto_library") 

cc_test(
  name = "hello_test",
  size = "small",
  srcs = ["hello_test.cc"],
  deps = ["@com_google_googletest//:gtest_main"],
)

cc_test(
    name = "instance_test",
    size = "small",
    srcs = ["instance_test.cc"],
    deps = ["@com_google_googletest//:gtest_main","//src:src_lib"],
)
    
