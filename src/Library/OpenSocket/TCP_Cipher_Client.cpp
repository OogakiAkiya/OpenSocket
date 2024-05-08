#include "OpenSocket_Cipher_Def.h"
#include "OpenSocket_Def.h"
#include "OpenSocket_STD.h"
#include "base/BaseSocket.h"

#include "base/BaseClient.h"

#include "TCP_Client.h"

#include "../WrapperOpenSSL/WrapperOpenSSL.h"
#include "TCP_Cipher_Client.h"

namespace OpenSocket {
std::shared_ptr<TCP_Cipher_Client> TCP_Cipher_Client::GetInstance(const std::string _addrs, const std::string _port, const int _ipv, const bool _asynchronous) {
   std::shared_ptr<TCP_Cipher_Client> temp = std::make_shared<TCP_Cipher_Client>();

   temp->m_socket = std::make_shared<BaseSocket>();
   temp->m_socket->Init(_addrs, _port);                // IPアドレスとポート番号の設定
   SwitchIpv(temp->m_socket, _ipv);                    // IPvの設定
   temp->m_socket->SetProtocol_TCP();                  // TCP通信に設定
   if (!temp->m_socket->AddressSet()) return nullptr;  // ソケット生成
   if (!temp->m_socket->Connect()) return nullptr;     // コネクト処理
   if (_asynchronous) temp->m_socket->SetAsynchronous();
   return temp;
}
void TCP_Cipher_Client::Update() { DataProcessing(); }

int TCP_Cipher_Client::CiphserSendServer(const char* _buf, const int _bufSize, const char _firstClass, const char _secondClass, const char _firstOption, const char _secondOption) {
   int sendDataSize = 0;
   char sendBuf[SEND_BUFFERSIZE];

   // 暗号化処理付きプロトコル用ヘッダー付与
   std::memcpy(&sendBuf, &_firstClass, sizeof(_firstClass));
   std::memcpy(&sendBuf[sizeof(_firstClass)], &_secondClass, sizeof(_secondClass));
   std::memcpy(&sendBuf[sizeof(_firstClass) + sizeof(_secondClass)], &_firstOption, sizeof(_firstClass));
   std::memcpy(&sendBuf[sizeof(_firstClass) + sizeof(_secondClass) + sizeof(_firstOption)], &_secondOption, sizeof(_secondOption));
   sendDataSize = sizeof(_firstClass) + sizeof(_secondClass) + sizeof(_firstOption) + sizeof(_secondOption);

   // 送信データの付与(共通鍵交換済みの場合)
   if (iskeychange) {
      // データの型変換
      std::string plainData(_buf, _bufSize);
      // データの暗号化
      std::string encodeData = aes->Encrypt(aesKey, aesKeyByteSize, plainData);

      // 送信データの付与
      memcpy(&sendBuf[sendDataSize], encodeData.data(), strlen(encodeData.data()));
      sendDataSize += strlen(encodeData.data());

      // 暗号化したデータの送信
      return TCP_Client::SendServer(sendBuf, sendDataSize);
   }
   // 送信データの付与
   memcpy(&sendBuf[sendDataSize], _buf, _bufSize);
   sendDataSize += _bufSize;

   // 通常フローの送信処理
   return TCP_Client::SendServer(sendBuf, sendDataSize);
}

bool TCP_Cipher_Client::KeyChangeConnectionStart(int _rsaKeyByteSize, int _aesKeyByteSize) {
   int sendDataSize = 0;
   char sendBuf[0];

   // 鍵サイズの登録
   aesKeyByteSize = _aesKeyByteSize;
   rsaKeyByteSize = _rsaKeyByteSize;

   // Serverへ鍵交換を要求
   if (rsaKeyByteSize == RSA_KEY_2048_BYTE_SIZE) CiphserSendServer(sendBuf, sendDataSize, CIPHER_PACKET, CIPHER_PACKET_CREATE_PUBLICKEY_REQUEST, CIPHER_PACKET_RSA_KEY_SIZE_2048, PADDING_DATA);

   // 後続の鍵交換処理の実施
   while (iskeychange == false) { Update(); }

   // ToDo: 鍵交換のタイムアウト処理が追加されればfalseを返す処理を追加

   return true;
}

void TCP_Cipher_Client::DataProcessing() {
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
      memcpy((char*)&recvData[nowSize], &buf[0], dataSize);

      while (recvData.size() > TCP_BASE_HEADER_SIZE) {
         int bodySize;
         try {
            // 先頭パケットの解析
            memcpy(&bodySize, &recvData[0], sizeof(int));
            // 先頭パケットが想定しているよりも小さいまたは大きいパケットの場合は不正パケットとして解釈する。
            if (bodySize < 0 || bodySize > BODY_MAX_SIZE) {
               // TODO:不正パケットとみなした場合パケットをすべて削除しているが何かいい手がないか考える
               recvData.clear();
               return;
            }
            // 受信データが一塊分あればキューに追加
            if (recvData.size() >= TCP_BASE_HEADER_SIZE + bodySize + ENDMARKERSIZE) {
               // エンドマーカーの値が正常値かチェック
               if (memcmp(&recvData[TCP_BASE_HEADER_SIZE + bodySize], &ENDMARKER, ENDMARKERSIZE) != 0) {
                  // TODO:不正パケットとみなした場合パケットをすべて削除しているが何かいい手がないか考える
                  recvData.clear();
                  return;
               }

               std::vector<char> addData;
               addData.resize(bodySize);
               memcpy(&addData[0], &recvData[TCP_BASE_HEADER_SIZE], bodySize);
               CipherProcessing(addData);
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

void TCP_Cipher_Client::CipherProcessing(std::vector<char> _data) {
   if (_data.size() - TCP_CIPHER_HEADER_SIZE < 0) return;
   char bodyData[_data.size() - TCP_CIPHER_HEADER_SIZE];
   memcpy(&bodyData[0], &_data[TCP_CIPHER_HEADER_SIZE], _data.size() - TCP_CIPHER_HEADER_SIZE);

   // 暗号化プロトコル用のパケット解析
   if (_data[0] == CIPHER_PACKET) {
      switch (_data[sizeof(CIPHER_PACKET)]) {
         case CIPHER_PACKET_SEND_PUBLICKEY: {
            // 公開鍵型変換
            std::string pubKey(bodyData, sizeof(bodyData) / sizeof(bodyData[0]));

            // 暗号化した公開鍵の暗号化
            std::string sharedKey;
            while (1) {
               // 共通鍵の作成
               aes->GenerateKey(aesKey, aesKeyByteSize);

               // 受信した公開鍵で共通鍵を暗号化
               sharedKey = rsa->Encrypt(pubKey, aesKey);
               if (strlen(sharedKey.data()) == rsaKeyByteSize) break;
            }
            CiphserSendServer(sharedKey.data(), strlen(sharedKey.data()), CIPHER_PACKET, CIPHER_PACKET_REGISTRY_SHAREDKEY_REQUEST, PADDING_DATA, PADDING_DATA);
         } break;
         case CIPHER_PACKET_REGISTRIED_SHAREDKEY:

            // 鍵登録が無事できたかのチェックを要求
            char sendBuf[0];
            CiphserSendServer(sendBuf, sizeof(sendBuf) / sizeof(sendBuf[0]), CIPHER_PACKET, CIPHER_PACKET_CHECK_SHAREDKEY_REQUEST, PADDING_DATA, PADDING_DATA);
            break;
         case CIPHER_PACKET_SEND_CHECKDATA: {
            std::cout << "check data start" << std::endl;

            // 受信データの型変換
            std::string checkData(bodyData, sizeof(bodyData) / sizeof(bodyData[0]));
            std::string decodeData = aes->Decrypt(aesKey, aesKeyByteSize, checkData);

            // サーバから送信されたチェックデータのハッシュ化
            // std::string hashCheckData = WrapperOpenSSL::createMD5Hash(checkData);
            std::string hashCheckData = WrapperOpenSSL::createMD5Hash(decodeData);

            // ハッシュ値の暗号化
            std::string encodeData = aes->Encrypt(aesKey, aesKeyByteSize, hashCheckData);

            // 受信データのハッシュ化
            CiphserSendServer(encodeData.data(), strlen(encodeData.data()), CIPHER_PACKET, CIPHER_PACKET_CHECK_CHECKDATA_REQUEST, PADDING_DATA, PADDING_DATA);

            // デバッグ
            std::cout << "plainData:" << checkData << std::endl;
            std::cout << "decodeData:" << decodeData << std::endl;
            std::cout << "hashCheckData:" << hashCheckData << std::endl;
            std::cout << "encodeData:" << encodeData << std::endl;
            std::cout << "encodeData.size:" << strlen(encodeData.data()) << std::endl;

         } break;
         case CIPHER_PACKET_CHECK_SUCCESS:
            iskeychange = true;
            break;
         case CIPHER_PACKET_CHECK_FAILD: {
            // もう一度鍵交換から挑戦
            int sendDataSize = 0;
            char sendBuf[0];

            // 共通鍵の作成
            if (aesKeyByteSize == WrapperOpenSSL::AES_KEY_LEN_256) {
               aes->GenerateKey(aesKey, WrapperOpenSSL::AES_KEY_LEN_256);
               // Serverへ鍵交換を要求
               if (rsaKeyByteSize == RSA_KEY_2048_BYTE_SIZE) CiphserSendServer(sendBuf, sendDataSize, CIPHER_PACKET, CIPHER_PACKET_CREATE_PUBLICKEY_REQUEST, CIPHER_PACKET_RSA_KEY_SIZE_2048, PADDING_DATA);
            }
         } break;
         case CIPHER_PACKET_SEND_DATA:
            std::string encodeData(bodyData, sizeof(bodyData) / sizeof(bodyData[0]));
            // データの復号処理
            std::string decodeData = aes->Decrypt(aesKey, aesKeyByteSize, encodeData);

            // 型変化
            std::vector<char> addData(decodeData.begin(), decodeData.end());

            // データの追加処理
            recvDataQueList.push(addData);
            break;
      }
   }
}

}  // namespace OpenSocket