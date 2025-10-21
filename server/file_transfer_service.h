#pragma once

#include <string>
#include "trpc/server/server.h"
#include "proto/file_transfer.trpc.pb.h"

// 继承自 tRPC 框架根据 proto 文件自动生成的服务基类
class FileTransferServiceImpl : public trpc::exp::FileTransfer {
 public:
  // 重写基类中的 `UploadFile` 虚函数，以实现我们自己的业务逻辑
  trpc::Status UploadFile(trpc::ServerContextPtr context,
                          trpc::stream::ServerStreamReader<trpc::exp::FileChunk>* stream,
                          trpc::exp::UploadResponse* response) override;
};