#include "OpenSocket_Def.h"
#include "OpenSocket_STD.h"
#include "base/BaseSocket.h"

#include "base/BaseServer.h"

#include "UDP_Server.h"

namespace OpenSocket {

std::shared_ptr<UDP_Server> UDP_Server::GetInstance(const std::string _addrs, const std::string _port, const int _ipv, const bool _asynchronous) {
   std::shared_ptr<UDP_Server> temp = std::make_shared<UDP_Server>();

   temp->m_socket = std::make_shared<BaseSocket>();
   temp->m_socket->Init(_addrs, _port);
   SwitchIpv(temp->m_socket, _ipv);
   temp->m_socket->SetProtocol_UDP();
   if (!temp->m_socket->AddressSet()) return nullptr;
   if (!temp->m_socket->Bind()) return nullptr;
   if (_asynchronous) temp->m_socket->SetAsynchronous();

   return temp;
}

void UDP_Server::Update() { DataProcessing(); }

int UDP_Server::GetRecvDataSize() { return recvDataQueList.size(); }

int UDP_Server::SendOnlyClient(const B_ADDRESS_IN* _addr, const char* _buf, const int _bufSize) {
   int sendDataSize = 0;
   char sendBuf[UDP_SEND_BUFFERSIZE];

   try {
      // ヘッダーを付加し送信
      std::memcpy(sendBuf, &sequence, UDP_SEQUENCE_SIZE);
      std::memcpy(&sendBuf[UDP_SEQUENCE_SIZE], _buf, _bufSize);

      // 送信処理
      sendDataSize = m_socket->Sendto(_addr, &sendBuf[0], _bufSize + UDP_SEQUENCE_SIZE);
      // シーケンス番号管理
      sequence++;
      if (sequence > SEQUENCEMAX) { sequence = 0; }
   } catch (const std::exception& e) {
      std::cerr << "Exception Error at UDP_Server::SendOnlyClient():" << e.what() << std::endl;
      return sendDataSize;
   }

   return sendDataSize;
}

int UDP_Server::SendMultiClient(const std::vector<B_ADDRESS_IN> _addrList, const char* _buf, const int _bufSize) {
   char sendBuf[UDP_SEND_BUFFERSIZE];
   int sendDataSize = 0;

   try {
      // ヘッダーを付加し送信
      memcpy(sendBuf, &sequence, UDP_SEQUENCE_SIZE);
      memcpy(&sendBuf[UDP_SEQUENCE_SIZE], _buf, _bufSize);

      for (auto&& addr : _addrList) {
         // 送信処理
         sendDataSize = m_socket->Sendto(&addr, &sendBuf[0], _bufSize + UDP_SEQUENCE_SIZE);
      }

      // シーケンス番号管理
      sequence++;
      if (sequence > SEQUENCEMAX) { sequence = 0; }
   } catch (const std::exception& e) {
      std::cerr << "Exception Error at UDP_Server::SendMultiClient():" << e.what() << std::endl;
      return 0;
   }

   return sendDataSize;
}

void UDP_Server::DataProcessing() {
   // ファイルディスクリプタが設定されておりビットフラグが立っていない場合抜けるようにする
   if (fds != nullptr) {
      // 同期通信用の受信処理
      if (!FD_ISSET(m_socket->GetSocket(), fds)) { return; }
      char buf[RECV_PACKET_MAX_SIZE];
      std::pair<B_ADDRESS_IN, std::vector<char>> addData;

      // 受信処理
      int dataSize = m_socket->Recvfrom(&addData.first, &buf[0], RECV_PACKET_MAX_SIZE, 0);

      if (dataSize > 0) {
         addData.second.resize(dataSize);
         memcpy(&addData.second[0], &buf[0], dataSize);
         recvDataQueList.push(addData);
      }
      return;
   }
   // 非同期通信用の受信ループ
   char buf[RECV_PACKET_MAX_SIZE];
   while (true) {
      std::pair<B_ADDRESS_IN, std::vector<char>> addData;

      // 受信処理
      int dataSize = m_socket->Recvfrom(&addData.first, &buf[0], RECV_PACKET_MAX_SIZE, 0);

      if (dataSize > 0) {
         addData.second.resize(dataSize);
         memcpy(&addData.second[0], &buf[0], dataSize);
         recvDataQueList.push(addData);
      } else {
         // 受信できるデータがなくなったのでループを抜ける
         break;
      }
   }
}

std::pair<B_ADDRESS_IN, std::vector<char>> UDP_Server::GetRecvData() {
   std::pair<B_ADDRESS_IN, std::vector<char>> returnData;
   returnData = recvDataQueList.front();
   recvDataQueList.pop();
   return returnData;
}
}  // namespace OpenSocket
