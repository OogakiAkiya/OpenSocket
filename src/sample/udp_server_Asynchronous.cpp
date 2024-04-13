#include "../Library/OpenSocket/OpenSocket.h"
int main() {
   //=============================================================
   // UDP Server非同期通信サンプル
   //=============================================================
   std::shared_ptr<OpenSocket::UDP_Server> server = OpenSocket::UDP_Server::GetInstance("0.0.0.0", "12345", OpenSocket::IPV4, true);
   while (1) {
      server->Update();
      if (server->GetRecvDataSize() > 0) {
         std::pair<B_ADDRESS_IN, std::vector<char>> recvData = server->GetRecvData();
         unsigned int sequence;
         std::memcpy(&sequence, &recvData.second[0], sizeof(unsigned int));
         printf("Recv(%d)=%s\n", sequence, &recvData.second[sizeof(unsigned int)]);
         int sendDataSize = server->SendOnlyClient(&recvData.first, &recvData.second[sizeof(unsigned int)], recvData.second.size() - sizeof(unsigned int));
         printf("Send=%d\n", sendDataSize);
      }
   }
}
