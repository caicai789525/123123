# include tRPC
include_directories("/usr/local/trpc-cpp/trpc/include")
link_directories("/usr/local/trpc-cpp/trpc/lib")

# Protobuf 生成目录
set(PROTO_GEN_DIR "${CMAKE_CURRENT_BINARY_DIR}/gen_proto")
file(MAKE_DIRECTORY ${PROTO_GEN_DIR})
include_directories(${PROTO_GEN_DIR})

# 生成 proto 代码
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

# 链接服务端
add_executable(file_transfer_server
  server/main.cc
  server/file_transfer_service.cc
  server/file_transfer_service.h
)
add_dependencies(file_transfer_server generate_proto)
target_link_libraries(file_transfer_server
    /usr/local/trpc-cpp/trpc/lib/libtrpc.a
    protobuf::libprotobuf
    pthread
)

# 链接客户端
add_executable(file_transfer_client
  client/main.cc
)
add_dependencies(file_transfer_client generate_proto)
target_link_libraries(file_transfer_client
    /usr/local/trpc-cpp/trpc/lib/libtrpc.a
    protobuf::libprotobuf
    pthread
)
