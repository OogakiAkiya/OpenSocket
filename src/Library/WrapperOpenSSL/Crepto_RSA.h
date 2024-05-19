#ifndef CREPTO_RSA_h
#define CREPTO_RSA_h

namespace WrapperOpenSSL {

class Crepto_RSA {
public:
   Crepto_RSA() {}
   ~Crepto_RSA();
   int GenerateKey(const int _keyBit);  //_keyBitは『暗号化したいバイト数/8bit-11byte(ヘッダーとパディング)』を指定する
   std::string GetPAMPublicKey();
   std::string GetPAMPrivateKey();
   std::string Encrypt(const std::string _pemKey, const std::string& _plaintext);
   std::string Encrypt(const std::string _pemKey, const std::vector<unsigned char>& _plaintext);
   std::string Decrypt(const std::string _pemKey, const std::string& _ciphertext);

private:
   // メンバ関数
   void OpenSSLErrorMessage(const std::string _fileName, const int _line, const std::string _msg);

   // メンバ変数
   std::string publicKey;
   std::string privateKey;
};

}  // namespace WrapperOpenSSL
#endif