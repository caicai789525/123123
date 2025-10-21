cmake_minimum_required(VERSION 3.16)
project(trpc_file_transfer_experiment CXX)

# ----------------------------- 编译选项 -----------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ----------------------------- include / lib 路径 -----------------------------
# tRPC
set(TRPC_INCLUDE_DIR "/usr/local/trpc-cpp/trpc/include")
set(TRPC_LIB_DIR "/usr/local/trpc-cpp/trpc/lib")

include_directories(${TRPC_INCLUDE_DIR})
link_directories(${TRPC_LIB_DIR})

# Protobuf
find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})

# ----------------------------- proto 文件生成 -----------------------------
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
          --proto_path=${CMAKE_CURRENT_SOURCE_DIR}/proto
          --cpp_out=${PROTO_GEN_DIR}
          --trpc_out=${PROTO_GEN_DIR}
          ${PROTO_FILES}
  DEPENDS ${PROTO_FILES}
  COMMENT "生成 proto C++ 代码"
)

add_custom_target(generate_proto ALL
  DEPENDS "${PROTO_GEN_DIR}/file_transfer.pb.cc"
          "${PROTO_GEN_DIR}/file_transfer.pb.h"
          "${PROTO_GEN_DIR}/file_transfer.trpc.pb.cc"
          "${PROTO_GEN_DIR}/file_transfer.trpc.pb.h"
)

# ----------------------------- 服务端 -----------------------------
add_executable(file_transfer_server
  server/main.cc
  server/file_transfer_service.cc
  server/file_transfer_service.h
)

add_dependencies(file_transfer_server generate_proto)

target_link_libraries(file_transfer_server
    ${TRPC_LIB_DIR}/libtrpc.a
    protobuf::libprotobuf
    pthread
)

# ----------------------------- 客户端 -----------------------------
add_executable(file_transfer_client
  client/main.cc
)

add_dependencies(file_transfer_client generate_proto)

target_link_libraries(file_transfer_client
    ${TRPC_LIB_DIR}/libtrpc.a
    protobuf::libprotobuf
    pthread
)

# ----------------------------- 安装 -----------------------------
install(TARGETS file_transfer_server file_transfer_client RUNTIME DESTINATION bin)

install(FILES
    server/trpc_server_config.yaml
    client/trpc_client_config.yaml
    sample.txt
    DESTINATION bin
)
