cc_library(
    name="frames_lib",
    hdrs=["frames.h"],
    deps=[
        "@com_gitlab_eigen//:eigen",
    ]
)

cc_binary(
    name="frames_examples",
    srcs=["frames.cc"],
    deps=[
        "//frames:frames_lib",
    ]
)