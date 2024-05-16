﻿#ifndef TCP_CLIENT_h
#define TCP_CLIENT_h

namespace OpenSocket {
class TCP_Client : public BaseClient {
public:
   TCP_Client() {}
   ~TCP_Client() { m_socket->Close(); }
   virtual void Update() override;
   static std::shared_ptr<BaseClient> GetInstance(const std::string _addrs, const std::string _port, const int _ipv, const bool _asynchronous = false);
   virtual int GetRecvDataSize() override { return recvDataQueList.size(); }  // サーバーから受信したデータがいくつあるか
   virtual std::vector<char> GetRecvData() override;                          // サーバーから受信したデータを取り出す
   virtual int SendServer(const char* _buf, const int _bufSize) override;     // 特定のサーバーに送信する場合使用する

protected:
   // メンバ変数
   std::vector<char> recvData;

private:
   // メンバ関数
   void DataProcessing();
};
}  // namespace OpenSocket
#endif