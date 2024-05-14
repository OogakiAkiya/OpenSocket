#include "../Library/OpenSocket/OpenSocket.h"
int main() {
   //=============================================================
   // UDP Client暗号通信サンプル
   //=============================================================
   std::shared_ptr<OpenSocket::UDP_Cipher_Client> client = OpenSocket::UDP_Cipher_Client::GetInstance("127.0.0.1", "12345", OpenSocket::IPVD, true);

   // 暗号化通信のための鍵交換処理開始
   client->KeyChangeConnectionStart(OpenSocket::RSA_KEY_2048_BYTE_SIZE, OpenSocket::AES_KEY_LEN_256);

   char sendMsg[6] = "HELLO";
   int len = client->CipherSendServer(&sendMsg[0], sizeof(sendMsg), OpenSocket::CIPHER_PACKET, OpenSocket::CIPHER_PACKET_SEND_DATA, OpenSocket::PADDING_DATA, OpenSocket::PADDING_DATA);
   printf("Send=%d\n", len);
   while (1) {
      client->Update();
      if (client->GetRecvDataSize() > 0) {
         std::vector<char> recvData = client->GetRecvData();
         unsigned int sequence;
         std::memcpy(&sequence, &recvData[0], sizeof(unsigned int));
         printf("Recv(%d)=%s\n", sequence, &recvData[sizeof(unsigned int)]);
         len = client->CipherSendServer(&recvData[sizeof(unsigned int)], recvData.size() - sizeof(unsigned int), OpenSocket::CIPHER_PACKET, OpenSocket::CIPHER_PACKET_SEND_DATA, OpenSocket::PADDING_DATA, OpenSocket::PADDING_DATA);
      }
   }
}
