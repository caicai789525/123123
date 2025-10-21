cmake_minimum_required(VERSION 3.16)
project(trpc_file_transfer_experiment CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ----------------- 包含目录 -----------------
# tRPC 头文件
include_directories("/home/kevin/trpc-cpp/trpc/include")

# Proto 生成目录
set(PROTO_GEN_DIR "${CMAKE_CURRENT_BINARY_DIR}/gen_proto")
file(MAKE_DIRECTORY ${PROTO_GEN_DIR})
include_directories(${PROTO_GEN_DIR})

# ----------------- 链接目录 -----------------
link_directories("/home/kevin/trpc-cpp/build/lib")

# ----------------- 查找 Protobuf -----------------
find_package(Protobuf REQUIRED)

# ----------------- Proto 文件生成 -----------------
file(GLOB PROTO_FILES "${CMAKE_CURRENT_SOURCE_DIR}/proto/*.proto")

add_custom_command(
    OUTPUT "${PROTO_GEN_DIR}/file_transfer.pb.cc"
           "${PROTO_GEN_DIR}/file_transfer.pb.h"
           "${PROTO_GEN_DIR}/file_transfer.trpc.pb.cc"
           "${PROTO_GEN_DIR}/file_transfer.trpc.pb.h"
    COMMAND ${Protobuf_PROTOC_EXECUTABLE}
            --proto_path=${CMAKE_CURRENT_SOURCE_DIR}/proto
            --cpp_out=${PROTO_GEN_DIR}
            --trpc_out=${PROTO_GEN_DIR}
            ${PROTO_FILES}
    DEPENDS ${PROTO_FILES}
    COMMENT "从 .proto 文件生成 C++ 源代码"
)

add_custom_target(generate_proto ALL
    DEPENDS "${PROTO_GEN_DIR}/file_transfer.pb.cc"
            "${PROTO_GEN_DIR}/file_transfer.pb.h"
            "${PROTO_GEN_DIR}/file_transfer.trpc.pb.cc"
            "${PROTO_GEN_DIR}/file_transfer.trpc.pb.h"
)

# ----------------- 服务端可执行文件 -----------------
add_executable(file_transfer_server
    server/main.cc
    server/file_transfer_service.cc
    server/file_transfer_service.h
)

add_dependencies(file_transfer_server generate_proto)

target_link_libraries(file_transfer_server
    "/home/kevin/trpc-cpp/build/lib/libtrpc.a"
    protobuf::libprotobuf
    pthread
)

# ----------------- 客户端可执行文件 -----------------
add_executable(file_transfer_client
    client/main.cc
)

add_dependencies(file_transfer_client generate_proto)

target_link_libraries(file_transfer_client
    "/home/kevin/trpc-cpp/build/lib/libtrpc.a"
    protobuf::libprotobuf
    pthread
)

# ----------------- 安装 -----------------
install(TARGETS file_transfer_server file_transfer_client RUNTIME DESTINATION bin)
install(FILES
    server/trpc_server_config.yaml
    client/trpc_client_config.yaml
    sample.txt
    DESTINATION bin
)
