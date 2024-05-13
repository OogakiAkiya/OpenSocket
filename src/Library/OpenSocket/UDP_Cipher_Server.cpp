#include "OpenSocket_Cipher_Def.h"
#include "OpenSocket_Def.h"
#include "OpenSocket_STD.h"
#include "base/BaseSocket.h"

#include "base/BaseServer.h"

#include "UDP_Server.h"

#include "../WrapperOpenSSL/WrapperOpenSSL.h"
#include "UDP_Cipher_Server.h"

namespace OpenSocket {
void UDP_Cipher_Server::Update() { UDP_Cipher_Server::DataProcessing(); }

std::shared_ptr<UDP_Cipher_Server> UDP_Cipher_Server::GetInstance(const std::string _addrs, const std::string _port, const int _ipv, const bool _asynchronous) {
   std::shared_ptr<UDP_Cipher_Server> temp = std::make_shared<UDP_Cipher_Server>();

   temp->m_socket = std::make_shared<BaseSocket>();
   temp->m_socket->Init(_addrs, _port);
   SwitchIpv(temp->m_socket, _ipv);
   temp->m_socket->SetProtocol_UDP();
   if (!temp->m_socket->AddressSet()) return nullptr;
   if (!temp->m_socket->Bind()) return nullptr;
   if (_asynchronous) temp->m_socket->SetAsynchronous();

   return temp;
}

int UDP_Cipher_Server::CipherSendOnlyClient(const B_ADDRESS_IN* _addr, const char* _buf, const int _bufSize, const char _firstClass, const char _secondClass, const char _firstOption, const char _secondOption) {
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
      sendDataSize = UDP_Cipher_Server::SendOnlyClient(_addr, &sendBuf[0], sendDataSize);

      // シーケンス番号管理
      sequence++;
      if (sequence > SEQUENCEMAX) { sequence = 0; }
   } catch (const std::exception& e) {
      std::cerr << "Exception Error at UDP_Server::SendOnlyClient():" << e.what() << std::endl;
      return sendDataSize;
   }

   return sendDataSize;
}

int UDP_Cipher_Server::CipherSendMultiClient(const std::vector<B_ADDRESS_IN> _addrList, const char* _buf, const int _bufSize, const char _firstClass, const char _secondClass, const char _firstOption, const char _secondOption) { return 0; }

void UDP_Cipher_Server::DataProcessing() {
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

void UDP_Cipher_Server::CipherProcessing(std::pair<B_ADDRESS_IN, std::vector<char>> _data) {
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