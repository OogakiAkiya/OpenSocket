﻿#include "../Library/OpenSocket/OpenSocket.h"
int main() {
   //=============================================================
   // UDP Client非同期通信サンプル
   //=============================================================
   std::shared_ptr<OpenSocket::BaseClient> client = OpenSocket::UDP_Client::GetInstance("127.0.0.1", "12345", OpenSocket::IPVD, true);
   char sendMsg[6] = "HELLO";
   int len = client->SendServer(&sendMsg[0], sizeof(sendMsg));
   printf("Send=%d\n", len);
   while (1) {
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
