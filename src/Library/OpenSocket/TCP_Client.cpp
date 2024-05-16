#include "OpenSocket_Def.h"
#include "OpenSocket_STD.h"
#include "base/BaseSocket.h"

#include "base/BaseClient.h"

#include "TCP_Client.h"

namespace OpenSocket {

void TCP_Client::Update() { DataProcessing(); }

std::shared_ptr<BaseClient> TCP_Client::GetInstance(const std::string _addrs, const std::string _port, const int _ipv, const bool _asynchronous) {
   std::shared_ptr<TCP_Client> temp = std::make_shared<TCP_Client>();

   temp->m_socket = std::make_shared<BaseSocket>();
   temp->m_socket->Init(_addrs, _port);                // IPアドレスとポート番号の設定
   SwitchIpv(temp->m_socket, _ipv);                    // IPvの設定
   temp->m_socket->SetProtocol_TCP();                  // TCP通信に設定
   if (!temp->m_socket->AddressSet()) return nullptr;  // ソケット生成
   if (!temp->m_socket->Connect()) return nullptr;     // コネクト処理
   if (_asynchronous) temp->m_socket->SetAsynchronous();

   return temp;
}

std::vector<char> TCP_Client::GetRecvData() {
   std::vector<char> returnData;
   returnData = recvDataQueList.front();
   recvDataQueList.pop();
   return returnData;
}

int TCP_Client::SendServer(const char* _buf, const int _bufSize) {
   int sendDataSize = 0;
   char sendBuf[TCP_SEND_BUFFERSIZE];
   try {
      // ヘッダーを付加
      std::memcpy(sendBuf, &_bufSize, sizeof(int));

      // データの付与
      std::memcpy(&sendBuf[TCP_BASE_HEADER_SIZE], _buf, _bufSize);

      // エンドマーカーを付与
      std::memcpy(&sendBuf[TCP_BASE_HEADER_SIZE + _bufSize], ENDMARKER, ENDMARKERSIZE);

      // 送信
      sendDataSize = m_socket->Send(sendBuf, _bufSize + TCP_BASE_HEADER_SIZE + ENDMARKERSIZE);
   } catch (const std::exception& e) {
      std::cerr << "Exception Error at TCP_Client::SendServer():" << e.what() << std::endl;
      return sendDataSize;
   }
   return sendDataSize;
}

void TCP_Client::DataProcessing() {
   // ファイルディスクリプタが設定されておりビットフラグが立っていない場合抜けるようにする
   if (fds != nullptr) {
      if (!FD_ISSET(m_socket->GetSocket(), fds)) { return; }
   }

   char buf[RECV_PACKET_MAX_SIZE];
   int dataSize = m_socket->Recv(buf, RECV_PACKET_MAX_SIZE);

   if (dataSize > 0) {
      // 受信データを格納
      int nowSize = recvData.size();
      recvData.resize(nowSize + dataSize);
      std::memcpy((char*)&recvData[nowSize], &buf[0], dataSize);

      while (recvData.size() > TCP_BASE_HEADER_SIZE) {
         int bodySize;
         try {
            // 先頭パケットの解析
            std::memcpy(&bodySize, &recvData[0], sizeof(int));
            // 先頭パケットが想定しているよりも小さいまたは大きいパケットの場合は不正パケットとして解釈する。
            if (bodySize < 0 || bodySize > TCP_BODY_MAX_SIZE) {
               // TODO:不正パケットとみなした場合パケットをすべて削除しているが何かいい手がないか考える
               recvData.clear();
               return;
            }

            // 受信データが一塊分あればキューに追加
            if (recvData.size() >= TCP_BASE_HEADER_SIZE + bodySize + ENDMARKERSIZE) {
               // エンドマーカーの値が正常値かチェック
               if (memcmp(&recvData[bodySize + TCP_BASE_HEADER_SIZE], &ENDMARKER, ENDMARKERSIZE) != 0) {
                  // TODO:不正パケットとみなした場合パケットをすべて削除しているが何かいい手がないか考える
                  recvData.clear();
                  return;
               }

               // データの追加処理
               std::vector<char> addData;
               addData.resize(bodySize);
               std::memcpy(&addData[0], &recvData[TCP_BASE_HEADER_SIZE], bodySize);
               recvDataQueList.push(addData);
               recvData.erase(recvData.begin(), recvData.begin() + bodySize + TCP_BASE_HEADER_SIZE + ENDMARKERSIZE);
            }
         } catch (const std::exception& e) {
            std::cerr << "Exception Error at TCP_Routine::Update():" << e.what() << std::endl;

            // TODO:不正パケットなどで先頭データがintでmemcpyできなかった際はパケットをすべて削除しているが何かいい手がないか考える
            recvData.clear();
            return;
         }
      }
   } else if (dataSize == 0) {
      // 接続を終了するとき
      std::cout << "connection is lost" << std::endl;
   }
#ifdef _MSC_VER
   else if (WSAGetLastError() == WSAEWOULDBLOCK) {
      // clientがsendしていなかったときにおこるエラー(returnで良いかも)
   }
#endif
   else {
#ifdef _MSC_VER
      // 接続エラーが起こった時
      std::cerr << "recv failed:" << WSAGetLastError() << std::endl;
#else
      if (errno == EAGAIN) {
         // 非同期だとここを基本は通る
         return;
      }

      // 接続エラーが起こった時
      std::cerr << "recv failed" << std::endl;

#endif
   }
}

}  // namespace OpenSocket
