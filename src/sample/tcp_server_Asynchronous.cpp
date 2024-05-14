#include "../Library/OpenSocket/OpenSocket.h"
int main() {
   //=============================================================
   // TCP Server非同期通信サンプル
   //=============================================================
   std::shared_ptr<OpenSocket::TCP_Server> server = OpenSocket::TCP_Server::GetInstance("0.0.0.0", "12345", OpenSocket::IPV4, true);
   while (1) {
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
