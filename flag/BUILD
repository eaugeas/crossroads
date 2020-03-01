package(default_visibility = ["//visibility:public"])

cc_library(
    name = "flag",
    srcs = ["flag.cc", "config.cc", "status.cc"],
    hdrs = ["flag.hpp", "config.hpp", "status.hpp"],
    deps = ["//value", "//buffer"],
)

cc_test(
    name = "flag_test",
    srcs = ["flag_test.cc"],
    deps = [":flag", "//value", "//test"],
)

cc_test(
    name = "config_test",
    srcs = ["config_test.cc"],
    deps = [":flag", "//value", "//test"],
)
