#include "OpenSocket_Cipher_Def.h"
#include "OpenSocket_Def.h"
#include "OpenSocket_STD.h"
#include "base/BaseSocket.h"

#include "base/BaseClient.h"

#include "UDP_Client.h"

#include "../WrapperOpenSSL/WrapperOpenSSL.h"
#include "UDP_Cipher_Client.h"

namespace OpenSocket {
void UDP_Cipher_Client::Update() { UDP_Cipher_Client::DataProcessing(); }

std::shared_ptr<UDP_Cipher_Client> UDP_Cipher_Client::GetInstance(const std::string _addrs, const std::string _port, const int _ipv, const bool _asynchronous) {
   std::shared_ptr<UDP_Cipher_Client> temp = std::make_shared<UDP_Cipher_Client>();

   temp->m_socket = std::make_shared<BaseSocket>();
   temp->m_socket->Init(_addrs, _port);                // IPアドレスとポート番号の設定
   SwitchIpv(temp->m_socket, _ipv);                    // IPvの設定
   temp->m_socket->SetProtocol_UDP();                  // TCP通信に設定
   if (!temp->m_socket->AddressSet()) return nullptr;  // ソケット生成
   if (_asynchronous) temp->m_socket->SetAsynchronous();
   return temp;
}

int UDP_Cipher_Client::CipherSendServer(const char* _buf, const int _bufSize, const char _firstClass, const char _secondClass, const char _firstOption, const char _secondOption) {
   int sendDataSize = 0;
   char sendBuf[UDP_SEND_BUFFERSIZE];

   try {
      // 暗号化処理付きプロトコル用ヘッダー付与
      std::memcpy(&sendBuf, &_firstClass, sizeof(_firstClass));
      std::memcpy(&sendBuf[sizeof(_firstClass)], &_secondClass, sizeof(_secondClass));
      std::memcpy(&sendBuf[sizeof(_firstClass) + sizeof(_secondClass)], &_firstOption, sizeof(_firstClass));
      std::memcpy(&sendBuf[sizeof(_firstClass) + sizeof(_secondClass) + sizeof(_firstOption)], &_secondOption, sizeof(_secondOption));
      sendDataSize = sizeof(_firstClass) + sizeof(_secondClass) + sizeof(_firstOption) + sizeof(_secondOption);

      std::memcpy(&sendBuf[sendDataSize], &_buf[0], _bufSize);
      sendDataSize += _bufSize;

      // 送信処理
      sendDataSize = UDP_Client::SendServer(&sendBuf[0], sendDataSize);

      // シーケンス番号管理
      sequence++;
      if (sequence > SEQUENCEMAX) { sequence = 0; }
   } catch (const std::exception& e) {
      std::cerr << "Exception Error at TCP_Client::SendServer():" << e.what() << std::endl;
      return sendDataSize;
   }
   return sendDataSize;
}

bool UDP_Cipher_Client::KeyChangeConnectionStart(int _rsaKeyByteSize, int _aesKeyByteSize) {
   int sendDataSize = 0;
   char sendBuf[0];

   // 鍵サイズの登録
   aesKeyByteSize = _aesKeyByteSize;
   rsaKeyByteSize = _rsaKeyByteSize;

   // Serverへ鍵交換を要求
   if (rsaKeyByteSize == RSA_KEY_2048_BYTE_SIZE) CipherSendServer(sendBuf, sendDataSize, CIPHER_PACKET, CIPHER_PACKET_CREATE_PUBLICKEY_REQUEST, CIPHER_PACKET_RSA_KEY_SIZE_2048, PADDING_DATA);

   // 後続の鍵交換処理の実施
   while (iskeychange == false) { Update(); }

   // ToDo: 鍵交換のタイムアウト処理が追加されればfalseを返す処理を追加

   return true;
}
void UDP_Cipher_Client::DataProcessing() {
   // ファイルディスクリプタが設定されておりビットフラグが立っていない場合抜けるようにする
   if (fds != nullptr) {
      if (!FD_ISSET(m_socket->GetSocket(), fds)) { return; }
   }

   std::pair<B_ADDRESS_IN, std::vector<char>> addData;
   char buf[RECV_PACKET_MAX_SIZE];

   // 受信処理
   int dataSize = m_socket->Recvfrom(&addData.first, &buf[0], RECV_PACKET_MAX_SIZE, 0);

   if (dataSize > 0) {
      addData.second.resize(dataSize);
      std::memcpy(&addData.second[0], &buf[0], dataSize);
      CipherProcessing(addData);
   }
}

void UDP_Cipher_Client::CipherProcessing(std::pair<B_ADDRESS_IN, std::vector<char>> _data) {
   unsigned int sequence;
   std::memcpy(&sequence, &_data.second[0], UDP_SEQUENCE_SIZE);

   // 暗号化ヘッダーの除去したデータの作成
   std::vector<char> recvData;
   recvData.resize(_data.second.size() - UDP_CIPHER_HEADER_SIZE);
   std::memcpy(&recvData[0], &sequence, UDP_SEQUENCE_SIZE);
   std::memcpy(&recvData[UDP_SEQUENCE_SIZE], &_data.second[UDP_SEQUENCE_SIZE + UDP_CIPHER_HEADER_SIZE], _data.second.size() - UDP_CIPHER_HEADER_SIZE - UDP_SEQUENCE_SIZE);
   recvDataQueList.push({_data.first, recvData});
}
}  // namespace OpenSocket