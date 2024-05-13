#ifndef UDP_CIPHER_SERVER_h
#define UDP_CIPHER_SERVER_h

namespace OpenSocket {
class UDP_Cipher_Server : public UDP_Server {
public:
   UDP_Cipher_Server() {}
   ~UDP_Cipher_Server() {}
   virtual void Update() override;
   static std::shared_ptr<UDP_Cipher_Server> GetInstance(const std::string _addrs, const std::string _port, const int _ipv, const bool _asynchronous = false);

   int CipherSendOnlyClient(const B_ADDRESS_IN* _addr, const char* _buf, const int _bufSize, const char _firstClass, const char _secondClass, const char _firstOption, const char _secondOption);  // 特定のクライアントに送信する場合使用する
   int CipherSendMultiClient(const std::vector<B_ADDRESS_IN> _addrList, const char* _buf, const int _bufSize, const char _firstClass, const char _secondClass, const char _firstOption, const char _secondOption);

private:
   void DataProcessing();
   void CipherProcessing(std::pair<B_ADDRESS_IN, std::vector<char>> _data);
   std::shared_ptr<WrapperOpenSSL::Crepto_RSA> rsa = std::make_shared<WrapperOpenSSL::Crepto_RSA>();
   std::shared_ptr<WrapperOpenSSL::Crepto_AES> aes = std::make_shared<WrapperOpenSSL::Crepto_AES>();
   std::unordered_map<B_SOCKET, std::string> rsaKeyList;
   std::unordered_map<B_SOCKET, std::vector<unsigned char>> aesKeyList;
   std::unordered_map<B_SOCKET, std::string> checkDataList;
};
}  // namespace OpenSocket
#endif