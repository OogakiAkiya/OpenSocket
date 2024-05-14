#include "../Library/OpenSocket/OpenSocket.h"
int main() {
   //=============================================================
   // UDP Server同期通信サンプル
   //=============================================================
   fd_set readfds;
   std::shared_ptr<OpenSocket::UDP_Server> server = OpenSocket::UDP_Server::GetInstance("0.0.0.0", "12345", OpenSocket::IPV4, false);
   while (1) {
      FD_ZERO(&readfds);
      int maxfds = server->GetFileDescriptor(&readfds);
      // ソケットの設定で非同期設定を有効にしていない場合ここでブロッキングされる
      OpenSocket::OpenSocket_Select(&readfds, maxfds);
      server->SetFileDescriptorPointer(&readfds);

      server->Update();
      if (server->GetRecvDataSize() > 0) {
         // 受信データ取得
         std::pair<B_ADDRESS_IN, std::vector<char>> recvData = server->GetRecvData();
         unsigned int sequence;
         std::memcpy(&sequence, &recvData.second[0], OpenSocket::UDP_SEQUENCE_SIZE);
         std::cout << "Recv(" << sequence << ")=" << &recvData.second[OpenSocket::UDP_SEQUENCE_SIZE] << std::endl;

         // 送信処理
         int sendDataSize = server->SendOnlyClient(&recvData.first, &recvData.second[OpenSocket::UDP_SEQUENCE_SIZE], recvData.second.size() - OpenSocket::UDP_SEQUENCE_SIZE);
         std::cout << "SendDataSize=" << sendDataSize << std::endl;
      }
   }
}
