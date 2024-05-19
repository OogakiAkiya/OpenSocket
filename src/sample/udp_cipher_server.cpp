#include "../Library/OpenSocket/OpenSocket.h"
int main() {
   //=============================================================
   // UDP Server暗号通信サンプル
   //=============================================================
   std::shared_ptr<OpenSocket::UDP_Cipher_Server> server = OpenSocket::UDP_Cipher_Server::GetInstance("0.0.0.0", "12345", OpenSocket::IPV4, true);
   while (1) {
      server->Update();
      if (server->GetRecvDataSize() > 0) {
         // 受信データ取得
         std::pair<B_ADDRESS_IN, std::vector<char>> recvData = server->GetRecvData();
         unsigned int sequence;
         std::memcpy(&sequence, &recvData.second[0], OpenSocket::UDP_SEQUENCE_SIZE);
         std::cout << "Recv(" << sequence << ")=" << &recvData.second[OpenSocket::UDP_SEQUENCE_SIZE] << std::endl;

         // 送信処理
         int sendDataSize = server->CipherSendOnlyClient(&recvData.first, &recvData.second[OpenSocket::UDP_SEQUENCE_SIZE], recvData.second.size() - OpenSocket::UDP_SEQUENCE_SIZE, OpenSocket::CIPHER_PACKET,
                                                         OpenSocket::CIPHER_PACKET_SEND_DATA, OpenSocket::PADDING_DATA, OpenSocket::PADDING_DATA);
         std::cout << "SendDataSize=" << sendDataSize << std::endl;
      }
   }
}
