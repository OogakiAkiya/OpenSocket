#ifndef TCP_CIPHER_SERVER_h
#define TCP_CIPHER_SERVER_h

namespace OpenSocket {
class TCP_Cipher_Server : public TCP_Server {
public:
   TCP_Cipher_Server() {}
   ~TCP_Cipher_Server() { m_socket->Close(); }
   static std::shared_ptr<TCP_Cipher_Server> GetInstance(const std::string _addrs, const std::string _port, const int _ipv, const bool _asynchronous = false);
   virtual void Update() override;
   int CipherSendOnlyClient(const int _socket, const char* _buf, const int _bufSize, const char _firstClass, const char _secondClass, const char _firstOption, const char _secondOption);  // 特定のクライアントに送信する場合使用する

private:
   void DataProcessing();
   void CipherProcessing(std::pair<B_SOCKET, std::vector<char>> _data);

   // 公開鍵暗号
   std::shared_ptr<WrapperOpenSSL::Crepto_RSA> rsa = std::make_shared<WrapperOpenSSL::Crepto_RSA>();
   std::unordered_map<B_SOCKET, std::string> rsaKeyList;

   // 共通鍵暗号
   std::shared_ptr<WrapperOpenSSL::Crepto_AES> aes = std::make_shared<WrapperOpenSSL::Crepto_AES>();
   std::unordered_map<B_SOCKET, std::vector<unsigned char>> aesKeyList;

   // チェックデータリスト
   std::unordered_map<B_SOCKET, std::string> checkDataList;
};
}  // namespace OpenSocket
#endif