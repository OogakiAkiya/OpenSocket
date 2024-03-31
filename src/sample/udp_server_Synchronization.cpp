#include "Library/OpenSocket/OpenSocket.h"
int main() {
   //=============================================================
   // UDP Server同期通信サンプル
   //=============================================================
   fd_set readfds;
   auto server = OpenSocket::UDP_Server::GetInstance("0.0.0.0", "12345", OpenSocket::IPV4, false);
   while (1) {
      FD_ZERO(&readfds);
      int maxfds = server->GetFileDescriptor(&readfds);
      // ソケットの設定で非同期設定を有効にしていない場合ここでブロッキングされる
      OpenSocket::OpenSocket_Select(&readfds, maxfds);
      server->SetFileDescriptorPointer(&readfds);

      server->Update();
      if (server->GetRecvDataSize() > 0) {
         auto temp = server->GetRecvData();
         unsigned int sequence;
         std::memcpy(&sequence, &temp.second[0], sizeof(unsigned int));
         printf("Recv(%d)=%s\n", sequence, &temp.second[sizeof(unsigned int)]);
         int sendDataSize = server->SendOnlyClient(&temp.first, &temp.second[sizeof(unsigned int)], temp.second.size() - sizeof(unsigned int));
         printf("Send=%d\n", sendDataSize);
      }
   }
}
