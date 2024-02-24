#include "Library/OpenSocket/OpenSocket.h"
int main() {
   //=============================================================
   // TCP Client同期通信サンプル
   //=============================================================
   fd_set readfds;
   std::shared_ptr<OpenSocket::BaseClient> client;
   while (1) {
      client = OpenSocket::TCP_Client::GetInstance("127.0.0.1", "12345", IPV4, false);
      if (client) {
         break;
      } else {
         client = nullptr;
      }
   }
   // 送信処理(仮)
   char tem[6] = "HELLO";
   int dataSize = client->SendServer(tem, sizeof(tem));
   printf("SendData=%d\n", dataSize);

   while (1) {
      FD_ZERO(&readfds);
      int maxfds = client->GetFileDescriptor(&readfds);
      // ソケットの設定で非同期設定を有効にしていない場合ここでブロッキングされる
      OpenSocket::OpenSocket_Select(&readfds, maxfds);
      client->SetFileDescriptorPointer(&readfds);

      client->Update();
      if (client->GetRecvDataSize() > 0) {
         // 受信処理
         std::vector<char> temp = client->GetRecvData();
         char buf[100];
         std::memcpy(buf, &temp[0], temp.size());
         printf("%s\n", buf);
         client->SendServer(&temp[0], temp.size());
      }
   }
}
