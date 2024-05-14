#include "OpenSocket_Def.h"
#include "OpenSocket_STD.h"
#include "base/BaseSocket.h"

#include "base/BaseClient.h"

#include "UDP_Client.h"

namespace OpenSocket {
void UDP_Client::Update() { DataProcessing(); }

std::shared_ptr<BaseClient> UDP_Client::GetInstance(const std::string _addrs, const std::string _port, const int _ipv, const bool _asynchronous) {
   std::shared_ptr<UDP_Client> temp = std::make_shared<UDP_Client>();

   temp->m_socket = std::make_shared<BaseSocket>();
   temp->m_socket->Init(_addrs, _port);                // IPアドレスとポート番号の設定
   SwitchIpv(temp->m_socket, _ipv);                    // IPvの設定
   temp->m_socket->SetProtocol_UDP();                  // UDP通信に設定
   if (!temp->m_socket->AddressSet()) return nullptr;  // ソケット生成
   if (_asynchronous) temp->m_socket->SetAsynchronous();
   return temp;
}

std::vector<char> UDP_Client::GetRecvData() {
   std::pair<B_ADDRESS_IN, std::vector<char>> returnData;
   returnData = recvDataQueList.front();
   recvDataQueList.pop();
   return returnData.second;
}

int UDP_Client::SendServer(const char* _buf, const int _bufSize) {
   int sendDataSize = 0;
   char sendBuf[UDP_SEND_BUFFERSIZE];

   try {
      // ヘッダーを付加し送信
      std::memcpy(&sendBuf[0], &sequence, UDP_SEQUENCE_SIZE);
      std::memcpy(&sendBuf[UDP_SEQUENCE_SIZE], &_buf[0], _bufSize);

      // 送信処理
      sendDataSize = m_socket->Sendto(&sendBuf[0], _bufSize + UDP_SEQUENCE_SIZE);

      // シーケンス番号管理
      sequence++;
      if (sequence > SEQUENCEMAX) { sequence = 0; }
   } catch (const std::exception& e) {
      std::cerr << "Exception Error at TCP_Client::SendServer():" << e.what() << std::endl;
      return sendDataSize;
   }
   return sendDataSize;
}

void UDP_Client::DataProcessing() {
   // ファイルディスクリプタが設定されておりビットフラグが立っていない場合抜けるようにする
   if (fds != nullptr) {
      if (!FD_ISSET(m_socket->GetSocket(), fds)) { return; }
      // 同期通信用の受信処理
      char buf[RECV_PACKET_MAX_SIZE];
      std::pair<B_ADDRESS_IN, std::vector<char>> addData;

      // 受信処理
      int dataSize = m_socket->Recvfrom(&addData.first, &buf[0], RECV_PACKET_MAX_SIZE, 0);

      if (dataSize > 0) {
         addData.second.resize(dataSize);
         std::memcpy(&addData.second[0], &buf[0], dataSize);
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
         std::memcpy(&addData.second[0], &buf[0], dataSize);
         recvDataQueList.push(addData);
      } else {
         // 受信できるデータがなくなったのでループを抜ける
         break;
      }
   }
}
}  // namespace OpenSocket
