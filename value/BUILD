package(default_visibility = ["//visibility:public"])

cc_library(
    name = "value",
    srcs = ["parser.cc", "string_utils.cc"],
    hdrs = ["parser.hpp", "string_utils.hpp"],
)

cc_test(
    name = "parser_test",
    srcs = ["parser_test.cc"],
    deps = [":value", "//test"],
)

cc_test(
    name = "string_utils_test",
    srcs = ["string_utils_test.cc"],
    deps = [":value", "//test"],
)
