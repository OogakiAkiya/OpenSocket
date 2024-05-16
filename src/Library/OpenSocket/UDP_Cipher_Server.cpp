#include "OpenSocket_Cipher_Def.h"
#include "OpenSocket_Def.h"
#include "OpenSocket_STD.h"
#include "Utility.h"
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

      // 送信データの付与(共通鍵交換済みの場合)
      if (aesKeyList.find(GetUDPSocketID(_addr)) != aesKeyList.end()) {
         // データの型変換
         std::string plainData(_buf, _bufSize);
         // データの暗号化
         std::vector<unsigned char> aesKey = aesKeyList[GetUDPSocketID(_addr)];
         std::string encodeData = aes->Encrypt(aesKey, aesKey.size(), plainData);

         // 送信データの付与
         std::memcpy(&sendBuf[sendDataSize], encodeData.data(), encodeData.size());
         sendDataSize += encodeData.size();

      } else {
         std::memcpy(&sendBuf[sendDataSize], &_buf[0], _bufSize);
         sendDataSize += _bufSize;
      }

      // 送信処理
      sendDataSize = UDP_Server::SendOnlyClient(_addr, &sendBuf[0], sendDataSize);

      // シーケンス番号管理
      sequence++;
      if (sequence > SEQUENCEMAX) { sequence = 0; }
   } catch (const std::exception& e) {
      std::cerr << "Exception Error at UDP_Cipher_Server::CipherSendOnlyClient:" << e.what() << std::endl;
      return sendDataSize;
   }

   return sendDataSize;
}

int UDP_Cipher_Server::CipherSendMultiClient(const std::vector<B_ADDRESS_IN> _addrList, const char* _buf, const int _bufSize, const char _firstClass, const char _secondClass, const char _firstOption, const char _secondOption) { return 0; }

void UDP_Cipher_Server::DataProcessing() {
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
   while (true) {
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

void UDP_Cipher_Server::CipherProcessing(std::pair<B_ADDRESS_IN, std::vector<char>> _data) {
   // シーケンス番号取得
   unsigned int sequence;
   std::memcpy(&sequence, &_data.second[0], UDP_SEQUENCE_SIZE);

   // ボディーデータ取得
   char bodyData[_data.second.size() - UDP_SEQUENCE_SIZE - UDP_CIPHER_HEADER_SIZE];
   std::memcpy(&bodyData[0], &_data.second[UDP_SEQUENCE_SIZE + UDP_CIPHER_HEADER_SIZE], sizeof(bodyData) / sizeof(bodyData[0]));

   // UDPの擬似ソケットID作成(IPアドレス,port番号を元にIDを作成する)
   std::string socketID = GetUDPSocketID(&_data.first);

   switch (_data.second[UDP_SEQUENCE_SIZE + sizeof(CIPHER_PACKET)]) {
      case CIPHER_PACKET_CREATE_PUBLICKEY_REQUEST:

         // 公開鍵の作成
         if (_data.second[UDP_SEQUENCE_SIZE + sizeof(CIPHER_PACKET) + sizeof(CIPHER_PACKET_CREATE_PUBLICKEY_REQUEST)] == CIPHER_PACKET_RSA_KEY_SIZE_2048) {
            // 引数がbit数指定なので注意
            rsa->GenerateKey(RSA_KEY_2048_BYTE_SIZE * 8);
            std::string pubKey = rsa->GetPAMPublicKey();

            rsaKeyList.insert({socketID, rsa->GetPAMPrivateKey()});

            // 公開鍵をclientへ送信
            CipherSendOnlyClient(&_data.first, pubKey.data(), pubKey.size(), CIPHER_PACKET, CIPHER_PACKET_SEND_PUBLICKEY, PADDING_DATA, PADDING_DATA);
         }
         break;
      case CIPHER_PACKET_RECREATE_PUBLICKEY_REQUEST:
         break;
      case CIPHER_PACKET_REGISTRY_SHAREDKEY_REQUEST: {
         // 受信データより暗号化プロトコル用のヘッダーを除去しデコードのために型変換
         std::string cipherData(bodyData, sizeof(bodyData) / sizeof(bodyData[0]));

         // 送られてきた共通鍵をデコード
         std::string sharedKey = rsa->Decrypt(rsaKeyList[socketID], cipherData);
         std::vector<unsigned char> decodeSharedKey(sharedKey.begin(), sharedKey.begin() + WrapperOpenSSL::AES_KEY_LEN_256);

         // 共通鍵を登録(ここ以降で通信は暗号化される)
         aesKeyList.insert({socketID, decodeSharedKey});

         // 鍵登録完了の返送
         char sendBuf[0];
         CipherSendOnlyClient(&_data.first, sendBuf, sizeof(sendBuf) / sizeof(sendBuf[0]), CIPHER_PACKET, CIPHER_PACKET_REGISTRIED_SHAREDKEY, PADDING_DATA, PADDING_DATA);
      } break;
      case CIPHER_PACKET_CHECK_SHAREDKEY_REQUEST: {
         // ランダム文字列の作成しチェックデータの登録
         std::string checkData = GenerateRandomStringNoSymbol(CHECK_DATA_SIZE);
         checkDataList.insert({socketID, checkData});

         // チェックデータをクライアントに送信(パケットは暗号化されている)
         CipherSendOnlyClient(&_data.first, checkData.data(), checkData.size(), CIPHER_PACKET, CIPHER_PACKET_SEND_CHECKDATA, PADDING_DATA, PADDING_DATA);

      } break;
      case CIPHER_PACKET_CHECK_CHECKDATA_REQUEST: {
         // 受信データの復号処理
         std::string encodeData(bodyData, sizeof(bodyData) / sizeof(bodyData[0]));
         std::string clientHashData = aes->Decrypt(aesKeyList[socketID], aesKeyList[socketID].size(), encodeData);

         // サーバ側に登録されているチェックデータをハッシュ化
         std::string serverHashData = WrapperOpenSSL::createMD5Hash(checkDataList[socketID]);

         char sendBuf[0];
         if (serverHashData == clientHashData) {
            // サーバに登録されているチェックデータとクライアントから送付されたチェックデータが一致したため成功
            CipherSendOnlyClient(&_data.first, sendBuf, sizeof(sendBuf) / sizeof(sendBuf[0]), CIPHER_PACKET, CIPHER_PACKET_CHECK_SUCCESS, PADDING_DATA, PADDING_DATA);
         } else {
            // 登録した共通鍵を削除
            aesKeyList.erase(socketID);

            // サーバに登録されているチェックデータとクライアントから送付されたチェックデータが一致していないため失敗
            CipherSendOnlyClient(&_data.first, sendBuf, sizeof(sendBuf) / sizeof(sendBuf[0]), CIPHER_PACKET, CIPHER_PACKET_CHECK_FAILD, PADDING_DATA, PADDING_DATA);
         }

         // 鍵交換に使用した登録データの削除
         rsaKeyList.erase(socketID);
         checkDataList.erase(socketID);
      } break;
      case CIPHER_PACKET_SEND_DATA:
         // データの復号処理
         std::string encodeData(bodyData, sizeof(bodyData) / sizeof(bodyData[0]));
         std::string decodeData = aes->Decrypt(aesKeyList[socketID], aesKeyList[socketID].size(), encodeData);

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

std::string UDP_Cipher_Server::GetUDPSocketID(const B_ADDRESS_IN* _addr) {
   std::string ipAddress = inet_ntoa(_addr->sin_addr);  // IPv4アドレスを文字列に変換
   uint16_t port = ntohs(_addr->sin_port);              // ポート番号をホストバイトオーダーに変換

   return ipAddress + ":" + std::to_string(port);
}
}  // namespace OpenSocket