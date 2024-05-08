#include "../Library/OpenSocket/OpenSocket.h"

int main() {
   //=============================================================
   // TCP Server暗号通信サンプル
   //=============================================================
   std::shared_ptr<OpenSocket::TCP_Cipher_Server> server = OpenSocket::TCP_Cipher_Server::GetInstance("0.0.0.0", "12345", OpenSocket::IPV4, true);

   while (1) {
      server->Update();

      // データ送信処理

      while (server->GetRecvDataSize() > 0) {
         std::pair<int, std::vector<char>> recvData = server->GetRecvData();
         printf("Recv=%s\n", &recvData.second[0]);
         int sendDataSize = server->CipherSendOnlyClient(recvData.first, &recvData.second[0], recvData.second.size(), OpenSocket::CIPHER_PACKET, OpenSocket::CIPHER_PACKET_SEND_DATA, OpenSocket::PADDING_DATA, OpenSocket::PADDING_DATA);
         printf("Send=%d\n", sendDataSize);
      }
   }
}