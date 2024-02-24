#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include "WrapperOpenSSL_Def.h"
#include "WrapperOpenSSL_STD.h"

#include "Crepto_RSA.h"

namespace WrapperOpenSSL {

Crepto_RSA::~Crepto_RSA() {}

int Crepto_RSA::GenerateKey(const int _keyBit) {
   EVP_PKEY* pkey = nullptr;
   EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);
   if (ctx == nullptr) {
      OpenSSLErrorMessage(__FILE__, __LINE__, "EVP_CIPHER_CTX_new function error in Crepto_RSA");
      EVP_PKEY_CTX_free(ctx);
      return 1;
   }

   // 鍵生成のパラメータ初期化/設定
   if (EVP_PKEY_keygen_init(ctx) < 1) {
      OpenSSLErrorMessage(__FILE__, __LINE__, "EVP_PKEY_keygen_init function error in Crepto_RSA");
      EVP_PKEY_CTX_free(ctx);
      return 1;
   }

   if (EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, _keyBit) < 1) {
      OpenSSLErrorMessage(__FILE__, __LINE__, "EVP_PKEY_CTX_set_rsa_keygen_bits function error in Crepto_RSA");
      EVP_PKEY_CTX_free(ctx);
      return 1;
   }

   // 鍵生成
   if (EVP_PKEY_keygen(ctx, &pkey) < 1) {
      OpenSSLErrorMessage(__FILE__, __LINE__, "EVP_PKEY_keygen function error in Crepto_RSA");
      EVP_PKEY_CTX_free(ctx);
      return 1;
   }

   // 公開鍵をPEM形式へ変更
   BIO* pubkeyBio = BIO_new(BIO_s_mem());
   char* pubkeyData;
   if (!PEM_write_bio_PUBKEY(pubkeyBio, pkey)) {
      OpenSSLErrorMessage(__FILE__, __LINE__, "PEM_write_bio_PUBKEY function error in Crepto_RSA");
      BIO_free(pubkeyBio);
      EVP_PKEY_free(pkey);
      return 1;
   }

   long pubkeyLen = BIO_get_mem_data(pubkeyBio, &pubkeyData);
   std::string pubkeyString(pubkeyData, pubkeyData + pubkeyLen);
   publicKey = pubkeyString;

   // 秘密鍵をPEM形式へ変更
   BIO* privkeyBio = BIO_new(BIO_s_mem());
   char* privkeyData;
   if (!PEM_write_bio_PrivateKey(privkeyBio, pkey, NULL, NULL, 0, NULL, NULL)) {
      OpenSSLErrorMessage(__FILE__, __LINE__, "PEM_write_bio_PrivateKey function error in Crepto_RSA");
      BIO_free(pubkeyBio);
      BIO_free(privkeyBio);
      EVP_PKEY_free(pkey);
      return 1;
   }

   long privkeyLen = BIO_get_mem_data(privkeyBio, &privkeyData);
   std::string privkeyString(privkeyData, privkeyData + privkeyLen);
   privateKey = privkeyString;

   // メモリー解放
   BIO_free(pubkeyBio);
   BIO_free(privkeyBio);
   EVP_PKEY_free(pkey);
   EVP_PKEY_CTX_free(ctx);

   return 0;
}

std::string Crepto_RSA::GetPAMPublicKey() { return publicKey; }

std::string Crepto_RSA::GetPAMPrivateKey() { return privateKey; }

std::string Crepto_RSA::Encrypt(const std::string _pemKey, const std::string& _plaintext) {
   // 変数定義
   BIO* keyBio = nullptr;
   EVP_PKEY* pkey = nullptr;
   EVP_PKEY_CTX* ctx = nullptr;
   std::string ciphertext;

   // 鍵読み込み
   keyBio = BIO_new_mem_buf(_pemKey.c_str(), static_cast<int>(_pemKey.size()));
   if (!keyBio) {
      OpenSSLErrorMessage(__FILE__, __LINE__, "keyBio is empty.");
      goto RELEASE_BIO;
   }

   pkey = PEM_read_bio_PUBKEY(keyBio, nullptr, nullptr, nullptr);
   if (!pkey) {
      OpenSSLErrorMessage(__FILE__, __LINE__, "pkey is empty.");
      goto RELEASE_BIO;
   }

   // 暗号化のための初期セッティング
   ctx = EVP_PKEY_CTX_new(pkey, NULL);
   if (!ctx) {
      OpenSSLErrorMessage(__FILE__, __LINE__, "ctx is empty.");
      goto RELEASE_PKEY;
   }

   if (EVP_PKEY_encrypt_init(ctx) != 1) {
      OpenSSLErrorMessage(__FILE__, __LINE__, "EVP_PKEY_encrypt_init function error in Crepto_RSA");
      goto RELEASE_CTX;
   }
   if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_PADDING) != 1) {
      OpenSSLErrorMessage(__FILE__, __LINE__, "EVP_PKEY_CTX_set_rsa_padding function error in Crepto_RSA");
      goto RELEASE_CTX;
   }

   // 暗号化後のサイズを確認する
   size_t ciphertextLen;
   if (EVP_PKEY_encrypt(ctx, nullptr, &ciphertextLen, reinterpret_cast<const unsigned char*>(_plaintext.c_str()), _plaintext.size()) != 1) {
      OpenSSLErrorMessage(__FILE__, __LINE__, "EVP_PKEY_encrypt function error in Crepto_RSA");
      goto RELEASE_CTX;
   }

   // 暗号化
   ciphertext.resize(ciphertextLen);
   if (EVP_PKEY_encrypt(ctx, reinterpret_cast<unsigned char*>(&ciphertext[0]), &ciphertextLen, reinterpret_cast<const unsigned char*>(_plaintext.c_str()), _plaintext.size()) != 1) {
      OpenSSLErrorMessage(__FILE__, __LINE__, "EVP_PKEY_encrypt function error in Crepto_RSA");
      goto RELEASE_CTX;
   }

   // メモリー解放
RELEASE_CTX:
   EVP_PKEY_CTX_free(ctx);
RELEASE_PKEY:
   EVP_PKEY_free(pkey);
RELEASE_BIO:
   BIO_free(keyBio);

   return ciphertext;
}

std::string Crepto_RSA::Decrypt(const std::string _pemKey, const std::string& _ciphertext) {
   // 変数定義
   BIO* keyBio = nullptr;
   EVP_PKEY* pkey = nullptr;
   EVP_PKEY_CTX* ctx = nullptr;
   std::string plaintext;

   // 鍵読み込み
   keyBio = BIO_new_mem_buf(_pemKey.c_str(), static_cast<int>(_pemKey.size()));
   if (!keyBio) {
      OpenSSLErrorMessage(__FILE__, __LINE__, "keyBio is empty.");
      goto RELEASE_BIO;
   }

   pkey = PEM_read_bio_PrivateKey(keyBio, nullptr, nullptr, nullptr);
   if (!pkey) {
      OpenSSLErrorMessage(__FILE__, __LINE__, "pkey is empty.");
      goto RELEASE_BIO;
   }

   // 暗号化のための初期セッティング
   ctx = EVP_PKEY_CTX_new(pkey, NULL);
   if (!ctx) {
      OpenSSLErrorMessage(__FILE__, __LINE__, "ctx is empty.");
      goto RELEASE_PKEY;
   }

   if (EVP_PKEY_decrypt_init(ctx) != 1) {
      OpenSSLErrorMessage(__FILE__, __LINE__, "EVP_PKEY_decrypt_init function error in Crepto_RSA");
      goto RELEASE_CTX;
   }

   if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_PADDING) != 1) {
      OpenSSLErrorMessage(__FILE__, __LINE__, "EVP_PKEY_CTX_set_rsa_padding function error in Crepto_RSA");
      goto RELEASE_CTX;
   }

   // 暗号化後のサイズを確認する
   size_t plaintextLen;
   if (EVP_PKEY_decrypt(ctx, nullptr, &plaintextLen, reinterpret_cast<const unsigned char*>(_ciphertext.c_str()), _ciphertext.size()) != 1) {
      OpenSSLErrorMessage(__FILE__, __LINE__, "EVP_PKEY_decrypt function error in Crepto_RSA");
      goto RELEASE_CTX;
   }

   // 暗号化
   plaintext.resize(plaintextLen);
   if (EVP_PKEY_decrypt(ctx, reinterpret_cast<unsigned char*>(&plaintext[0]), &plaintextLen, reinterpret_cast<const unsigned char*>(_ciphertext.c_str()), _ciphertext.size()) != 1) {
      OpenSSLErrorMessage(__FILE__, __LINE__, "EVP_PKEY_decrypt function error in Crepto_RSA");
      goto RELEASE_CTX;
   }

   // メモリー解放
RELEASE_CTX:
   EVP_PKEY_CTX_free(ctx);
RELEASE_PKEY:
   EVP_PKEY_free(pkey);
RELEASE_BIO:
   BIO_free(keyBio);

   return plaintext;
}

void Crepto_RSA::OpenSSLErrorMessage(const std::string _fileName, const int _line, const std::string _msg) {
   std::cerr << "Error at " << _fileName << " : " << _line << " : " << _msg << std::endl;
   unsigned long errCode = ERR_get_error();
   if (errCode != 0) {
      char errBuf[256];
      ERR_error_string(errCode, errBuf);
      std::cerr << "OpenSSL ErrorCode: " << errCode << ",OpenSSL ErrorMessage: " << errBuf << std::endl;
   }
}
}  // namespace WrapperOpenSSL