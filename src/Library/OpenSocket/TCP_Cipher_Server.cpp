#include "OpenSocket_Cipher_Def.h"
#include "OpenSocket_Def.h"
#include "OpenSocket_STD.h"
#include "Utility.h"
#include "base/BaseSocket.h"

#include "base/BaseServer.h"

#include "TCP_Server.h"

#include "../WrapperOpenSSL/WrapperOpenSSL.h"
#include "TCP_Cipher_Server.h"

namespace OpenSocket {
std::shared_ptr<TCP_Cipher_Server> TCP_Cipher_Server::GetInstance(const std::string _addrs, const std::string _port, const int _ipv, const bool _asynchronous) {
   std::shared_ptr<TCP_Cipher_Server> temp = std::make_shared<TCP_Cipher_Server>();

   temp->m_socket = std::make_shared<BaseSocket>();
   temp->m_socket->Init(_addrs, _port);
   SwitchIpv(temp->m_socket, _ipv);
   temp->m_socket->SetProtocol_TCP();
   if (!temp->m_socket->AddressSet()) return nullptr;
   if (!temp->m_socket->Bind()) return nullptr;
   if (!temp->m_socket->Listen()) return nullptr;
   if (_asynchronous) temp->m_socket->SetAsynchronous();
   return temp;
}

void TCP_Cipher_Server::Update() {
   // ファイルディスクリプタが設定されていない場合
   if (fds == nullptr) {
      // socketへの接続要求のチェック
      AcceptProcessing();

      // クライアントからの送付データの整形等を実施
      if (clientList.size() > 0) DataProcessing();
      return;
   }

   // ファイルディスクリプタが設定されている場合
   if (FD_ISSET(m_socket->GetSocket(), fds)) AcceptProcessing();
   if (clientList.size() > 0) DataProcessing();
}

int TCP_Cipher_Server::CipherSendOnlyClient(const int _socket, const char* _buf, const int _bufSize, const char _firstClass, const char _secondClass, const char _firstOption, const char _secondOption) {
   if (sendBuf.empty()) { sendBuf.resize(TCP_BASE_HEADER_SIZE + TCP_BODY_MAX_SIZE + ENDMARKERSIZE); }
   int sendDataSize = 0;

   // 暗号化処理付きプロトコル用ヘッダー付与
   std::memcpy(&sendBuf[0], &_firstClass, sizeof(_firstClass));
   std::memcpy(&sendBuf[sizeof(_firstClass)], &_secondClass, sizeof(_secondClass));
   std::memcpy(&sendBuf[sizeof(_firstClass) + sizeof(_secondClass)], &_firstOption, sizeof(_firstClass));
   std::memcpy(&sendBuf[sizeof(_firstClass) + sizeof(_secondClass) + sizeof(_firstOption)], &_secondOption, sizeof(_secondOption));
   sendDataSize = sizeof(_firstClass) + sizeof(_secondClass) + sizeof(_firstOption) + sizeof(_secondOption);

   // 送信データの付与(共通鍵交換済みの場合)
   if (aesKeyList.find(_socket) != aesKeyList.end()) {
      // データの型変換
      std::string plainData(_buf, _bufSize);
      // データの暗号化
      std::string encodeData = aes->Encrypt(aesKeyList[_socket], aesKeyList[_socket].size(), plainData);

      // 送信データの付与
      std::memcpy(&sendBuf[sendDataSize], encodeData.data(), encodeData.size());
      sendDataSize += encodeData.size();

      // 暗号化したデータの送信
      return TCP_Server::SendOnlyClient(_socket, &sendBuf[0], sendDataSize);
   }
   // 送信データの付与
   std::memcpy(&sendBuf[sendDataSize], _buf, _bufSize);
   sendDataSize += _bufSize;

   return TCP_Server::SendOnlyClient(_socket, &sendBuf[0], sendDataSize);
}

void TCP_Cipher_Server::DataProcessing() {
   // clietnListのindex,socket番号の順で値を格納
   std::list<std::pair<std::shared_ptr<BaseSocket>, int>> deleteList;

   for (int i = 0; i < clientList.size(); i++) {
      char buf[RECV_PACKET_MAX_SIZE];
      int socket = clientList.at(i)->GetSocket();

      // fdsがセットされておりsocketにイベントが発生しているか確認し、発生していなければスキップ
      if (fds) {
         if (!FD_ISSET(socket, fds)) continue;
      }

      // データを受信した際はそのバイト数が入り切断された場合は0,ノンブロッキングモードでデータを受信してない間は-1がdataSizeに入る
      int dataSize = clientList.at(i)->Recv(buf, RECV_PACKET_MAX_SIZE);
      if (dataSize > 0) {
         // 受信データを格納
         int nowSize = recvDataMap[socket].size();
         recvDataMap[socket].resize(nowSize + dataSize);
         std::memcpy((char*)&recvDataMap[socket][nowSize], &buf[0], dataSize);

         while (recvDataMap[socket].size() > TCP_CIPHER_HEADER_SIZE) {
            int bodySize;
            try {
               // 先頭パケットの解析
               std::memcpy(&bodySize, &recvDataMap[(B_SOCKET)socket][0], sizeof(int));

               // 先頭パケットが想定しているよりも小さいまたは大きいパケットの場合は不正パケットとして解釈する。
               if (bodySize < 0 || bodySize > TCP_BODY_MAX_SIZE) {
                  // TODO:不正パケットとみなした場合パケットをすべて削除しているが何かいい手がないか考える
                  recvDataMap[(B_SOCKET)socket].clear();
                  return;
               }

               // 受信データが一塊分あればレシーブキューに追加
               if (recvDataMap[socket].size() >= TCP_CIPHER_HEADER_SIZE + bodySize + ENDMARKERSIZE) {
                  // エンドマーカーの値が正常値かチェック()
                  if (memcmp(&recvDataMap[(B_SOCKET)socket][bodySize + TCP_CIPHER_HEADER_SIZE], &ENDMARKER, ENDMARKERSIZE) != 0) {
                     // TODO:不正パケットとみなした場合パケットをすべて削除しているが何かいい手がないか考える
                     recvDataMap[(B_SOCKET)socket].clear();
                     return;
                  }

                  std::pair<B_SOCKET, std::vector<char>> addData;
                  addData.first = socket;
                  addData.second.resize(bodySize);
                  std::memcpy(&addData.second[0], &recvDataMap[socket][TCP_CIPHER_HEADER_SIZE], bodySize);

                  CipherProcessing(addData);
                  recvDataMap[socket].erase(recvDataMap[socket].begin(), recvDataMap[socket].begin() + bodySize + TCP_CIPHER_HEADER_SIZE + ENDMARKERSIZE);
               }

            } catch (const std::exception& e) {
               std::cerr << "Exception Error at TCP_Cipher_Server::DataProcessing:" << e.what() << std::endl;

               // TODO:不正パケットなどで先頭データがintでmemcpyできなかった際はパケットをすべて削除しているが何かいい手がないか考える
               recvDataMap[(B_SOCKET)socket].clear();
               return;
            }
         }
      } else if (dataSize == 0) {
         // 接続を終了するとき
         std::cout << "connection is lost" << std::endl;
         deleteList.push_back(std::make_pair(clientList.at(i), socket));
      }
#ifdef _MSC_VER
      else if (WSAGetLastError() == WSAEWOULDBLOCK) {
         // clientがsendしていなかったときにおこるエラー
      }
#endif
      else {
#ifdef _MSC_VER

         // 接続エラーが起こった時
         std::cerr << "recv failed:" << WSAGetLastError() << std::endl;
         deleteList.push_back(std::make_pair(clientList.at(i), socket));
#else
         // errnoはシステムコールや標準ライブラリのエラーが格納される変数
         if (errno == EAGAIN) {
            // 非同期だとここを通ることがあるが無視して良い
            break;
         }

         if (errno == EBADF) {}
         if (errno == ECONNRESET) {
            // クライアント接続リセット
            std::cout << "connection is lost" << std::endl;
            deleteList.push_back(std::make_pair(clientList.at(i), socket));
            break;
         }
         // 接続エラーが起こった時
         std::cerr << "recv failed:" << errno << std::endl;
         deleteList.push_back(std::make_pair(clientList.at(i), socket));
#endif
      }
   }

   try {
      for (auto element : deleteList) {
         // 受信データ用配列の初期化
         recvDataMap[element.second].erase(recvDataMap[element.second].begin(), recvDataMap[element.second].end());
         // クライアントリストから削除
         auto it = std::remove(clientList.begin(), clientList.end(), element.first);
         clientList.erase(it, clientList.end());
      }
   } catch (const std::exception& e) { std::cerr << "Exception Error at TCP_Cipher_Server::DataProcessing.delete:" << e.what() << std::endl; }
}

void TCP_Cipher_Server::CipherProcessing(std::pair<B_SOCKET, std::vector<char>> _data) {
   if (_data.second.size() - TCP_CIPHER_HEADER_SIZE < 0) return;
   char bodyData[_data.second.size() - TCP_CIPHER_HEADER_SIZE];
   std::memcpy(&bodyData[0], &_data.second[TCP_CIPHER_HEADER_SIZE], _data.second.size() - TCP_CIPHER_HEADER_SIZE);

   // 暗号化プロトコル用のパケット解析
   if (_data.second[0] == CIPHER_PACKET) {
      switch (_data.second[sizeof(CIPHER_PACKET)]) {
         case CIPHER_PACKET_CREATE_PUBLICKEY_REQUEST:
            // 公開鍵の作成
            if (_data.second[sizeof(CIPHER_PACKET) + sizeof(CIPHER_PACKET_CREATE_PUBLICKEY_REQUEST)] == CIPHER_PACKET_RSA_KEY_SIZE_2048) {
               // 引数がbit数指定なので注意
               rsa->GenerateKey(RSA_KEY_2048_BYTE_SIZE * 8);
               std::string pubKey = rsa->GetPAMPublicKey();

               rsaKeyList.insert({_data.first, rsa->GetPAMPrivateKey()});

               // 公開鍵をclientへ送信
               CipherSendOnlyClient(_data.first, pubKey.data(), pubKey.size(), CIPHER_PACKET, CIPHER_PACKET_SEND_PUBLICKEY, PADDING_DATA, PADDING_DATA);
            }
            break;
         case CIPHER_PACKET_RECREATE_PUBLICKEY_REQUEST:
            break;
         case CIPHER_PACKET_REGISTRY_SHAREDKEY_REQUEST: {
            // 受信データより暗号化プロトコル用のヘッダーを除去しデコードのために型変換
            std::string cipherData(bodyData, sizeof(bodyData) / sizeof(bodyData[0]));

            // 送られてきた共通鍵をデコード
            std::string sharedKey = rsa->Decrypt(rsaKeyList[_data.first], cipherData);
            std::vector<unsigned char> decodeSharedKey(sharedKey.begin(), sharedKey.begin() + WrapperOpenSSL::AES_KEY_LEN_256);

            // 共通鍵を登録(ここ以降で通信は暗号化される)
            aesKeyList.insert({_data.first, decodeSharedKey});

            // 鍵登録完了の返送
            char sendBuf[0];
            CipherSendOnlyClient(_data.first, sendBuf, sizeof(sendBuf) / sizeof(sendBuf[0]), CIPHER_PACKET, CIPHER_PACKET_REGISTRIED_SHAREDKEY, PADDING_DATA, PADDING_DATA);
         } break;
         case CIPHER_PACKET_CHECK_SHAREDKEY_REQUEST: {
            // ランダム文字列の作成しチェックデータの登録
            std::string checkData = GenerateRandomStringNoSymbol(CHECK_DATA_SIZE);
            checkDataList.insert({_data.first, checkData});

            // チェックデータをクライアントに送信(パケットは暗号化されている)
            CipherSendOnlyClient(_data.first, checkData.data(), checkData.size(), CIPHER_PACKET, CIPHER_PACKET_SEND_CHECKDATA, PADDING_DATA, PADDING_DATA);

         } break;
         case CIPHER_PACKET_CHECK_CHECKDATA_REQUEST: {
            // 受信データの復号処理
            std::string encodeData(bodyData, sizeof(bodyData) / sizeof(bodyData[0]));
            std::string clientHashData = aes->Decrypt(aesKeyList[_data.first], aesKeyList[_data.first].size(), encodeData);

            // サーバ側に登録されているチェックデータをハッシュ化
            std::string serverHashData = WrapperOpenSSL::createMD5Hash(checkDataList[_data.first]);

            char sendBuf[0];
            if (serverHashData == clientHashData) {
               // サーバに登録されているチェックデータとクライアントから送付されたチェックデータが一致したため成功
               CipherSendOnlyClient(_data.first, sendBuf, sizeof(sendBuf) / sizeof(sendBuf[0]), CIPHER_PACKET, CIPHER_PACKET_CHECK_SUCCESS, PADDING_DATA, PADDING_DATA);
            } else {
               // 登録した共通鍵を削除
               aesKeyList.erase(_data.first);

               // サーバに登録されているチェックデータとクライアントから送付されたチェックデータが一致していないため失敗
               CipherSendOnlyClient(_data.first, sendBuf, sizeof(sendBuf) / sizeof(sendBuf[0]), CIPHER_PACKET, CIPHER_PACKET_CHECK_FAILD, PADDING_DATA, PADDING_DATA);
            }

            // 鍵交換に使用した登録データの削除
            rsaKeyList.erase(_data.first);
            checkDataList.erase(_data.first);
         } break;
         case CIPHER_PACKET_SEND_DATA:
            // データの復号処理
            std::string encodeData(bodyData, sizeof(bodyData) / sizeof(bodyData[0]));
            std::string decodeData = aes->Decrypt(aesKeyList[_data.first], aesKeyList[_data.first].size(), encodeData);

            // データの追加処理
            std::pair<B_SOCKET, std::vector<char>> addData;
            addData.first = _data.first;
            addData.second.resize(decodeData.size());
            std::memcpy(&addData.second[0], decodeData.data(), decodeData.size());

            recvDataQueList.push(addData);
            break;
      }
   }
}

}  // namespace OpenSocket
