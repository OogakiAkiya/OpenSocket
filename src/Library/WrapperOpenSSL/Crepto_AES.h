#ifndef CREPTO_AES_h
#define CREPTO_AES_h

namespace WrapperOpenSSL {

class Crepto_AES {
public:
   Crepto_AES() {}
   ~Crepto_AES() {}
   int GenerateKey(std::vector<unsigned char>& _key, const unsigned int _keyLen);
   std::string Encrypt(const std::vector<unsigned char>& _key, const unsigned int _keyLen, const std::string& _plaintext);
   std::string Decrypt(const std::vector<unsigned char>& _key, const unsigned int _keyLen, const std::string& _ciphertext);

private:
   // メンバ関数
   void OpenSSLErrorMessage(const std::string _fileName, const int _line, const std::string _msg);
};

}  // namespace WrapperOpenSSL
#endif