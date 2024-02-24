#include "Library/OpenSocket/OpenSocket.h"
int main() {
   //=============================================================
   // UDP Client同期通信サンプル
   //=============================================================
   fd_set readfds;
   std::shared_ptr<OpenSocket::BaseClient> client;
   client = OpenSocket::UDP_Client::GetInstance("127.0.0.1", "12345", IPVD, false);
   char tem[6] = "HELLO";
   int len = client->SendServer(&tem[0], sizeof(tem));
   printf("Send=%d\n", len);
   while (1) {
      FD_ZERO(&readfds);
      int maxfds = client->GetFileDescriptor(&readfds);
      // ソケットの設定で非同期設定を有効にしていない場合ここでブロッキングされる
      OpenSocket::OpenSocket_Select(&readfds, maxfds);
      client->SetFileDescriptorPointer(&readfds);

      client->Update();
      if (client->GetRecvDataSize() > 0) {
         std::vector<char> temp = client->GetRecvData();
         unsigned int sequence;
         std::memcpy(&sequence, &temp[0], sizeof(unsigned int));
         printf("Recv(%d)=%s\n", sequence, &temp[sizeof(unsigned int)]);

         len = client->SendServer(&temp[sizeof(unsigned int)], temp.size() - sizeof(unsigned int));
      }
   }
}
