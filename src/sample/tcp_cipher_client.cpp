#include "../Library/OpenSocket/OpenSocket.h"

int main() {
   //=============================================================
   // TCP Client暗号通信サンプル
   //=============================================================
   std::shared_ptr<OpenSocket::TCP_Cipher_Client> client;
   while (1) {
      client = OpenSocket::TCP_Cipher_Client::GetInstance("127.0.0.1", "12345", OpenSocket::IPV4, true);
      if (client) {
         break;
      } else {
         client = nullptr;
      }
   }

   // 暗号化通信のための鍵交換処理開始
   client->KeyChangeConnectionStart(OpenSocket::RSA_KEY_2048_BYTE_SIZE, OpenSocket::AES_KEY_LEN_256);

   // 暗号化用データの送信処理(仮)
   char sendMsg[6] = "HELLO";
   int dataSize = client->CipherSendServer(sendMsg, sizeof(sendMsg) / sizeof(sendMsg[0]), OpenSocket::CIPHER_PACKET, OpenSocket::CIPHER_PACKET_SEND_DATA, OpenSocket::PADDING_DATA, OpenSocket::PADDING_DATA);

   printf("SendData=%d\n", dataSize);

   while (1) {
      client->Update();

      if (client->GetRecvDataSize() > 0) {
         // 受信処理
         std::vector<char> recvData = client->GetRecvData();
         char buf[100];
         std::memcpy(buf, &recvData[0], recvData.size());
         printf("%s\n", buf);
         client->CipherSendServer(&recvData[0], recvData.size(), OpenSocket::CIPHER_PACKET, OpenSocket::CIPHER_PACKET_SEND_DATA, OpenSocket::PADDING_DATA, OpenSocket::PADDING_DATA);
      }
   }
}