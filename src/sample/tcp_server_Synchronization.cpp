﻿#include "../Library/OpenSocket/OpenSocket.h"
int main() {
   //=============================================================
   // TCP Server同期通信サンプル
   //=============================================================
   fd_set readfds;
   std::shared_ptr<OpenSocket::TCP_Server> server = OpenSocket::TCP_Server::GetInstance("0.0.0.0", "12345", OpenSocket::IPV4, false);
   while (1) {
      FD_ZERO(&readfds);
      int maxfds = server->GetFileDescriptor(&readfds);
      // ソケットの設定で非同期設定を有効にしていない場合ここでブロッキングされる
      OpenSocket::OpenSocket_Select(&readfds, maxfds);
      server->SetFileDescriptorPointer(&readfds);
      server->Update();

      // データ送信処理
      while (server->GetRecvDataSize() > 0) {
         // 受信データ取得
         std::pair<int, std::vector<char>> recvData = server->GetRecvData();
         std::cout << "Recv=" << recvData.second.data() << std::endl;

         // 送信処理
         int sendDataSize = server->SendOnlyClient(recvData.first, &recvData.second[0], recvData.second.size());
         std::cout << "SendDataSize=" << sendDataSize << std::endl;
      }
   }
}
