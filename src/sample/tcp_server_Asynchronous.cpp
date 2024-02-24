#include "Library/OpenSocket/OpenSocket.h"
int main() {
   //=============================================================
   // TCP Server非同期通信サンプル
   //=============================================================
   auto server = OpenSocket::TCP_Server::GetInstance("0.0.0.0", "12345", IPV4, true);
   while (1) {
      server->Update();

      // データ送信処理
      while (server->GetRecvDataSize() > 0) {
         auto recvData = server->GetRecvData();
         int sendDataSize = server->SendOnlyClient(recvData.first, &recvData.second[0], recvData.second.size());
      }
   }
}
