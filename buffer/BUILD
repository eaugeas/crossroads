package(default_visibility = ["//visibility:public"])

cc_library(
    name = "buffer",
    srcs = ["buffered_reader.cc", "buffered_writer.cc",
            "msg_buffer.cc", "scanner.cc",
            "stream_buffer.cc", "status.cc"],
    hdrs = ["buffered_reader.hpp", "buffered_writer.hpp",
            "buffer.hpp", "dispose_func.hpp", "memio.hpp",
            "msg_buffer.hpp", "scanner.hpp", "stream_buffer.hpp", "status.hpp"],
    deps = ["//status", "//io", "//value"],
)

cc_test(
    name = "buffer_test",
    srcs = ["buffer_test.cc"],
    deps = [":buffer", "//test"],
)

cc_test(
    name = "scanner_test",
    srcs = ["scanner_test.cc"],
    deps = [":buffer", "//test"],
)
