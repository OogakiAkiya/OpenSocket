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
   temp->m_socket->SetProtocol_UDP();                  // UDP通信に設定
   if (!temp->m_socket->AddressSet()) return nullptr;  // ソケット生成
   if (_asynchronous) temp->m_socket->SetAsynchronous();
   return temp;
}

int UDP_Cipher_Client::CipherSendServer(const char* _buf, const int _bufSize, const char _firstClass, const char _secondClass, const char _firstOption, const char _secondOption) {
   if (sendBuf.empty()) sendBuf.resize(UDP_SEQUENCE_SIZE + UDP_BODY_MAX_SIZE);

   int sendDataSize = 0;
   try {
      // 暗号化処理付きプロトコル用ヘッダー付与
      std::memcpy(&sendBuf[0], &_firstClass, sizeof(_firstClass));
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
         std::memcpy(&sendBuf[sendDataSize], encodeData.data(), encodeData.size());
         sendDataSize += encodeData.size();

      } else {
         std::memcpy(&sendBuf[sendDataSize], &_buf[0], _bufSize);
         sendDataSize += _bufSize;
      }

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
      // 同期通信用の受信処理
      char buf[RECV_PACKET_MAX_SIZE];
      std::pair<B_ADDRESS_IN, std::vector<char>> addData;

      // 受信処理
      int dataSize = m_socket->Recvfrom(&addData.first, &buf[0], RECV_PACKET_MAX_SIZE, 0);

      if (dataSize > 0) {
         addData.second.resize(dataSize);
         std::memcpy(&addData.second[0], &buf[0], dataSize);
         CipherProcessing(addData);
      }
      return;
   }
   // 非同期通信用の受信ループ
   char buf[RECV_PACKET_MAX_SIZE];
   while (1) {
      std::pair<B_ADDRESS_IN, std::vector<char>> addData;

      // 受信処理
      int dataSize = m_socket->Recvfrom(&addData.first, &buf[0], RECV_PACKET_MAX_SIZE, 0);

      if (dataSize > 0) {
         addData.second.resize(dataSize);
         std::memcpy(&addData.second[0], &buf[0], dataSize);
         CipherProcessing(addData);
      } else {
         // 受信できるデータがなくなったのでループを抜ける
         break;
      }
   }
}

void UDP_Cipher_Client::CipherProcessing(std::pair<B_ADDRESS_IN, std::vector<char>> _data) {
   // シーケンス番号作成
   unsigned int sequence;
   std::memcpy(&sequence, &_data.second[0], UDP_SEQUENCE_SIZE);

   // ボディーデータ作成
   char bodyData[_data.second.size() - UDP_SEQUENCE_SIZE - UDP_CIPHER_HEADER_SIZE];
   std::memcpy(&bodyData[0], &_data.second[UDP_SEQUENCE_SIZE + UDP_CIPHER_HEADER_SIZE], sizeof(bodyData) / sizeof(bodyData[0]));
   // 暗号化ヘッダーの第二セグメントを参照し分岐処理
   switch (_data.second[UDP_SEQUENCE_SIZE + sizeof(CIPHER_PACKET)]) {
      case CIPHER_PACKET_SEND_PUBLICKEY: {
         // 公開鍵型変換
         std::string pubKey(bodyData, sizeof(bodyData) / sizeof(bodyData[0]));

         // 暗号化した公開鍵の暗号化
         std::string sharedKey;
         // 共通鍵の作成
         aes->GenerateKey(aesKey, aesKeyByteSize);

         // 受信した公開鍵で共通鍵を暗号化
         sharedKey = rsa->Encrypt(pubKey, aesKey);
         CipherSendServer(sharedKey.data(), sharedKey.size(), CIPHER_PACKET, CIPHER_PACKET_REGISTRY_SHAREDKEY_REQUEST, PADDING_DATA, PADDING_DATA);
      } break;

      case CIPHER_PACKET_REGISTRIED_SHAREDKEY:
         // 鍵登録が無事できたかのチェックを要求
         char sendBuf[0];
         CipherSendServer(sendBuf, sizeof(sendBuf) / sizeof(sendBuf[0]), CIPHER_PACKET, CIPHER_PACKET_CHECK_SHAREDKEY_REQUEST, PADDING_DATA, PADDING_DATA);
         break;

      case CIPHER_PACKET_SEND_CHECKDATA: {
         // 受信データの型変換
         std::string checkData(bodyData, sizeof(bodyData) / sizeof(bodyData[0]));
         std::string decodeData = aes->Decrypt(aesKey, aesKeyByteSize, checkData);

         // サーバから送信されたチェックデータのハッシュ化
         std::string hashCheckData = WrapperOpenSSL::createMD5Hash(decodeData);

         // ハッシュ値の暗号化
         std::string encodeData = aes->Encrypt(aesKey, aesKeyByteSize, hashCheckData);

         // 受信データのハッシュ化
         CipherSendServer(encodeData.data(), encodeData.size(), CIPHER_PACKET, CIPHER_PACKET_CHECK_CHECKDATA_REQUEST, PADDING_DATA, PADDING_DATA);
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
            if (rsaKeyByteSize == RSA_KEY_2048_BYTE_SIZE) CipherSendServer(sendBuf, sendDataSize, CIPHER_PACKET, CIPHER_PACKET_CREATE_PUBLICKEY_REQUEST, CIPHER_PACKET_RSA_KEY_SIZE_2048, PADDING_DATA);
         }
      } break;
      case CIPHER_PACKET_SEND_DATA:
         std::string encodeData(bodyData, sizeof(bodyData) / sizeof(bodyData[0]));
         // データの復号処理
         std::string decodeData = aes->Decrypt(aesKey, aesKeyByteSize, encodeData);

         // 暗号化ヘッダーの除去したデータの作成
         std::vector<char> addData;
         addData.resize(UDP_SEQUENCE_SIZE + decodeData.size());
         std::memcpy(&addData[0], &sequence, UDP_SEQUENCE_SIZE);
         std::memcpy(&addData[UDP_SEQUENCE_SIZE], decodeData.data(), decodeData.size());

         // データの追加処理
         recvDataQueList.push({_data.first, addData});
         break;
   }
}
}  // namespace OpenSocket