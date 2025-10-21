cmake_minimum_required(VERSION 3.16)
project(trpc_file_transfer_experiment CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ----------------- tRPC 路径 -----------------
# trpc_cpp_plugin 绝对路径
set(TRPC_PLUGIN "/home/kevin/trpc-cpp/build/bin/trpc_cpp_plugin")
# trpc include 和 lib 路径
set(TRPC_INCLUDE "/usr/local/trpc-cpp/trpc/include")
set(TRPC_LIB "/home/kevin/trpc-cpp/build/lib/libtrpc.a")

include_directories(${TRPC_INCLUDE})
link_directories("/home/kevin/trpc-cpp/build/lib")

# ----------------- Protobuf -----------------
find_package(Protobuf REQUIRED)
message(STATUS "Protobuf_INCLUDE_DIR: ${Protobuf_INCLUDE_DIR}")
message(STATUS "Protobuf_LIBRARIES: ${Protobuf_LIBRARIES}")

# ----------------- Proto 代码生成 -----------------
file(GLOB PROTO_FILES "${CMAKE_CURRENT_SOURCE_DIR}/proto/*.proto")
set(PROTO_GEN_DIR "${CMAKE_CURRENT_BINARY_DIR}/gen_proto")
file(MAKE_DIRECTORY ${PROTO_GEN_DIR})
include_directories(${PROTO_GEN_DIR})

add_custom_command(
    OUTPUT "${PROTO_GEN_DIR}/file_transfer.pb.cc"
           "${PROTO_GEN_DIR}/file_transfer.pb.h"
           "${PROTO_GEN_DIR}/file_transfer.trpc.pb.cc"
           "${PROTO_GEN_DIR}/file_transfer.trpc.pb.h"
    COMMAND ${Protobuf_PROTOC_EXECUTABLE}
            --plugin=protoc-gen-trpc=${TRPC_PLUGIN}
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

# ----------------- 服务端 -----------------
include_directories(server)

add_executable(file_transfer_server
    server/main.cc
    server/file_transfer_service.cc
    server/file_transfer_service.h
)

add_dependencies(file_transfer_server generate_proto)

target_link_libraries(file_transfer_server
    ${TRPC_LIB}
    ${Protobuf_LIBRARIES}
    pthread
)

# ----------------- 客户端 -----------------
add_executable(file_transfer_client
    client/main.cc
)

add_dependencies(file_transfer_client generate_proto)

target_link_libraries(file_transfer_client
    ${TRPC_LIB}
    ${Protobuf_LIBRARIES}
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
