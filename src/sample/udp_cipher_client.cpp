#include "../Library/OpenSocket/OpenSocket.h"
int main() {
   //=============================================================
   // UDP Client暗号通信サンプル
   //=============================================================
   std::shared_ptr<OpenSocket::UDP_Cipher_Client> client = OpenSocket::UDP_Cipher_Client::GetInstance("127.0.0.1", "12345", OpenSocket::IPVD, true);

   // 暗号化通信のための鍵交換処理開始
   if (!client->KeyChangeConnectionStart(OpenSocket::RSA_KEY_2048_BYTE_SIZE, OpenSocket::AES_KEY_LEN_256)) {
      std::cout << "Key change faild" << std::endl;
      return 0;
   }

   char sendMsg[6] = "HELLO";
   int dataSize = client->CipherSendServer(&sendMsg[0], sizeof(sendMsg), OpenSocket::CIPHER_PACKET, OpenSocket::CIPHER_PACKET_SEND_DATA, OpenSocket::PADDING_DATA, OpenSocket::PADDING_DATA);
   std::cout << "SendDataSize=" << dataSize << std::endl;

   while (1) {
      client->Update();
      if (client->GetRecvDataSize() > 0) {
         // 受信データ取得
         std::vector<char> recvData = client->GetRecvData();
         unsigned int sequence;
         std::memcpy(&sequence, &recvData[0], OpenSocket::UDP_SEQUENCE_SIZE);
         std::cout << "Recv(" << sequence << ")=" << &recvData[OpenSocket::UDP_SEQUENCE_SIZE] << std::endl;

         // 送信処理
         int sendDataSize = client->CipherSendServer(&recvData[OpenSocket::UDP_SEQUENCE_SIZE], recvData.size() - OpenSocket::UDP_SEQUENCE_SIZE, OpenSocket::CIPHER_PACKET, OpenSocket::CIPHER_PACKET_SEND_DATA, OpenSocket::PADDING_DATA,
                                                     OpenSocket::PADDING_DATA);
         std::cout << "SendDataSize=" << sendDataSize << std::endl;
      }
   }
}
