#include "Library/OpenSocket/OpenSocket.h"
int main() {
   //=============================================================
   // UDP Server非同期通信サンプル
   //=============================================================
   auto server = OpenSocket::UDP_Server::GetInstance("0.0.0.0", "12345", IPV4, true);
   while (1) {
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
