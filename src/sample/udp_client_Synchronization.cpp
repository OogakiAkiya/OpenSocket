#include "../Library/OpenSocket/OpenSocket.h"
int main() {
   //=============================================================
   // UDP Client同期通信サンプル
   //=============================================================
   fd_set readfds;
   std::shared_ptr<OpenSocket::BaseClient> client = OpenSocket::UDP_Client::GetInstance("127.0.0.1", "12345", OpenSocket::IPVD, false);
   char sendMsg[6] = "HELLO";
   int dataSize = client->SendServer(&sendMsg[0], sizeof(sendMsg));
   std::cout << "SendDataSize=" << dataSize << std::endl;

   while (1) {
      FD_ZERO(&readfds);
      int maxfds = client->GetFileDescriptor(&readfds);
      // ソケットの設定で非同期設定を有効にしていない場合ここでブロッキングされる
      OpenSocket::OpenSocket_Select(&readfds, maxfds);
      client->SetFileDescriptorPointer(&readfds);

      client->Update();
      if (client->GetRecvDataSize() > 0) {
         // 受信データ取得
         std::vector<char> recvData = client->GetRecvData();
         unsigned int sequence;
         std::memcpy(&sequence, &recvData[0], OpenSocket::UDP_SEQUENCE_SIZE);
         std::cout << "Recv(" << sequence << ")=" << &recvData[OpenSocket::UDP_SEQUENCE_SIZE] << std::endl;

         // 送信処理
         int sendDataSize = client->SendServer(&recvData[OpenSocket::UDP_SEQUENCE_SIZE], recvData.size() - OpenSocket::UDP_SEQUENCE_SIZE);
         std::cout << "SendDataSize=" << sendDataSize << std::endl;
      }
   }
}
