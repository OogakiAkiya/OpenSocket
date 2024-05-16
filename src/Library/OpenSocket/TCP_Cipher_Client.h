#ifndef TCP_CIPHER_CLIENT_h
#define TCP_CIPHER_CLIENT_h

namespace OpenSocket {
class TCP_Cipher_Client : public TCP_Client {
public:
   TCP_Cipher_Client() {}
   ~TCP_Cipher_Client() { m_socket->Close(); }
   static std::shared_ptr<TCP_Cipher_Client> GetInstance(const std::string _addrs, const std::string _port, const int _ipv, const bool _asynchronous = false);

   // UpdateのoverrideからDataProcessingのoverrideに修正する。
   virtual void Update() override;
   int CipherSendServer(const char* _buf, const int _bufSize, const char _firstClass, const char _secondClass, const char _firstOption, const char _secondOption);  // 通信用の送信処理
   bool KeyChangeConnectionStart(int _rsaKeyByteSize, int _aesKeyByteSize);

private:
   void DataProcessing();
   void CipherProcessing(std::vector<char> _data);

   // 鍵交換完了フラグ
   bool iskeychange = false;

   // 公開暗号
   std::shared_ptr<WrapperOpenSSL::Crepto_RSA> rsa = std::make_shared<WrapperOpenSSL::Crepto_RSA>();
   int rsaKeyByteSize = 0;

   // 共通鍵
   std::shared_ptr<WrapperOpenSSL::Crepto_AES> aes = std::make_shared<WrapperOpenSSL::Crepto_AES>();
   std::vector<unsigned char> aesKey;
   int aesKeyByteSize = 0;
};
}  // namespace OpenSocket
#endif