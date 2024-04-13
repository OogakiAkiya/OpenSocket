#include "../Library/OpenSocket/OpenSocket.h"
int main() {
   //=============================================================
   // UDP Client同期通信サンプル
   //=============================================================
   fd_set readfds;
   std::shared_ptr<OpenSocket::BaseClient> client = OpenSocket::UDP_Client::GetInstance("127.0.0.1", "12345", OpenSocket::IPVD, false);
   char sendMsg[6] = "HELLO";
   int len = client->SendServer(&sendMsg[0], sizeof(sendMsg));
   printf("Send=%d\n", len);
   while (1) {
      FD_ZERO(&readfds);
      int maxfds = client->GetFileDescriptor(&readfds);
      // ソケットの設定で非同期設定を有効にしていない場合ここでブロッキングされる
      OpenSocket::OpenSocket_Select(&readfds, maxfds);
      client->SetFileDescriptorPointer(&readfds);

      client->Update();
      if (client->GetRecvDataSize() > 0) {
         std::vector<char> recvData = client->GetRecvData();
         unsigned int sequence;
         std::memcpy(&sequence, &recvData[0], sizeof(unsigned int));
         printf("Recv(%d)=%s\n", sequence, &recvData[sizeof(unsigned int)]);

         len = client->SendServer(&recvData[sizeof(unsigned int)], recvData.size() - sizeof(unsigned int));
      }
   }
}
