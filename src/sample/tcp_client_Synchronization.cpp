#include "../Library/OpenSocket/OpenSocket.h"
int main() {
   //=============================================================
   // TCP Client同期通信サンプル
   //=============================================================
   fd_set readfds;
   std::shared_ptr<OpenSocket::BaseClient> client;
   while (1) {
      client = OpenSocket::TCP_Client::GetInstance("127.0.0.1", "12345", OpenSocket::IPV4, false);
      if (client) {
         break;
      } else {
         client = nullptr;
      }
   }
   // 送信処理(仮)
   char sendMsg[6] = "HELLO";
   int dataSize = client->SendServer(sendMsg, sizeof(sendMsg));
   std::cout << "SendDataSize=" << dataSize << std::endl;

   while (1) {
      FD_ZERO(&readfds);
      int maxfds = client->GetFileDescriptor(&readfds);
      // ソケットの設定で非同期設定を有効にしていない場合ここでブロッキングされる
      OpenSocket::OpenSocket_Select(&readfds, maxfds);
      client->SetFileDescriptorPointer(&readfds);

      client->Update();
      if (client->GetRecvDataSize() > 0) {
         // 受信データ取得
         std::vector<char> recvData = client->GetRecvData();
         std::cout << "Recv=" << recvData.data() << std::endl;

         // 送信処理
         int sendDataSize = client->SendServer(&recvData[0], recvData.size());
         std::cout << "SendDataSize=" << sendDataSize << std::endl;
      }
   }
}
