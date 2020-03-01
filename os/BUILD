package(default_visibility = ["//visibility:public"])

cc_library(
    name = "os",
    srcs = ["socket.cc", "aio.cc", "file_stream.cc", "status.cc", "pipe.cc",
         "event_loop.cc"],
    hdrs = ["socket.hpp", "aio.hpp", "status.hpp", "file_stream.hpp", "pipe.hpp",
         "channel.hpp", "event_loop.hpp"],
    deps = ["//status", "//io"],
)

