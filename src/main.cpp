#include "Library/OpenSocket/OpenSocket.h"

int main() {
   //=============================================================
   // TCP Server暗号通信サンプル
   //=============================================================
   std::shared_ptr<OpenSocket::TCP_Cipher_Server> server = OpenSocket::TCP_Cipher_Server::GetInstance("0.0.0.0", "12345", OpenSocket::IPV4, true);

   while (1) {
      server->Update();

      while (server->GetRecvDataSize() > 0) {
         // 受信データ取得
         std::pair<int, std::vector<char>> recvData = server->GetRecvData();
         std::cout << "Recv=" << recvData.second.data() << std::endl;

         // 送信処理
         int sendDataSize = server->CipherSendOnlyClient(recvData.first, &recvData.second[0], recvData.second.size(), OpenSocket::CIPHER_PACKET, OpenSocket::CIPHER_PACKET_SEND_DATA, OpenSocket::PADDING_DATA, OpenSocket::PADDING_DATA);
         std::cout << "SendDataSize=" << sendDataSize << std::endl;
      }
   }
}