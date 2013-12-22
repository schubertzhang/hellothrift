# Build File
cc_binary (
    name = 'server',
    srcs = 'server.cpp',
    deps = [
        '//rpc2http/thrift/sample/idl:hello_thrift',
        '//thirdparty/boost:boost',
        '//thirdparty/glog:glog',
        '//thirdparty/thrift:thriftlibcpp',
    ],
)

cc_binary (
    name = 'client',
    srcs = 'client.cpp',
    deps = [
        '//rpc2http/thrift/sample/idl:hello_thrift',
        '//thirdparty/thrift:thriftlibcpp',
        '//thirdparty/boost:boost',
        '//thirdparty/glog:glog',
    ],
)
