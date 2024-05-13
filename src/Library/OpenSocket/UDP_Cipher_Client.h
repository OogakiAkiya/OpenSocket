#ifndef UDP_CIPHER_CLIENT_h
#define UDP_CIPHER_CLIENT_h

namespace OpenSocket {
class UDP_Cipher_Client : public UDP_Client {
public:
   UDP_Cipher_Client() {}
   ~UDP_Cipher_Client() {}
   // UpdateのoverrideからDataProcessingのoverrideに修正する。
   virtual void Update() override;
   static std::shared_ptr<UDP_Cipher_Client> GetInstance(const std::string _addrs, const std::string _port, const int _ipv, const bool _asynchronous = false);
   int CipherSendServer(const char* _buf, const int _bufSize, const char _firstClass, const char _secondClass, const char _firstOption, const char _secondOption);  // 特定のサーバーに送信する場合使用する
   bool KeyChangeConnectionStart(int _rsaKeyByteSize, int _aesKeyByteSize);

private:
   void DataProcessing();
   void CipherProcessing(std::pair<B_ADDRESS_IN, std::vector<char>> _data);

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