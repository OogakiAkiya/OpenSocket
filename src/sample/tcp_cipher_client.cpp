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
   if (!client->KeyChangeConnectionStart(OpenSocket::RSA_KEY_2048_BYTE_SIZE, OpenSocket::AES_KEY_LEN_256)) {
      std::cout << "Key change faild" << std::endl;
      return 1;
   }

   // 暗号化用データの送信処理
   char sendMsg[6] = "HELLO";
   int dataSize = client->CipherSendServer(sendMsg, sizeof(sendMsg) / sizeof(sendMsg[0]), OpenSocket::CIPHER_PACKET, OpenSocket::CIPHER_PACKET_SEND_DATA, OpenSocket::PADDING_DATA, OpenSocket::PADDING_DATA);
   std::cout << "SendDataSize=" << dataSize << std::endl;
   while (1) {
      client->Update();

      if (client->GetRecvDataSize() > 0) {
         // 受信データ取得
         std::vector<char> recvData = client->GetRecvData();
         std::cout << "Recv=" << recvData.data() << std::endl;

         // 送信処理
         int sendDataSize = client->CipherSendServer(&recvData[0], recvData.size(), OpenSocket::CIPHER_PACKET, OpenSocket::CIPHER_PACKET_SEND_DATA, OpenSocket::PADDING_DATA, OpenSocket::PADDING_DATA);
         std::cout << "SendDataSize=" << sendDataSize << std::endl;
      }
   }
}