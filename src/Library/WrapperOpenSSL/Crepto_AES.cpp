#include "WrapperOpenSSL_Def.h"
#include "WrapperOpenSSL_STD.h"

#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#include "Crepto_AES.h"

namespace WrapperOpenSSL {

int Crepto_AES::GenerateKey(std::vector<unsigned char>& _key, const unsigned int _keyLen) {
   // 格納先の引数を鍵の長さへリサイズ
   _key.resize(_keyLen);

   // 鍵の生成
   if (RAND_bytes(_key.data(), _keyLen) != 1) {
      OpenSSLErrorMessage(__FILE__, __LINE__, "RAND_bytes failed.");
      return 1;
   }

   return 0;
}

void Crepto_AES::OpenSSLErrorMessage(const std::string _fileName, const int _line, const std::string _msg) {
   std::cerr << "Error at " << _fileName << " : " << _line << " : " << _msg << std::endl;
   unsigned long errCode = ERR_get_error();
   if (errCode != 0) {
      char errBuf[256];
      ERR_error_string(errCode, errBuf);
      std::cerr << "OpenSSL ErrorCode: " << errCode << ",OpenSSL ErrorMessage: " << errBuf << std::endl;
   }
}

std::string Crepto_AES::Encrypt(const std::vector<unsigned char>& _key, const unsigned int _keyLen, const std::string& _plaintext) {
   std::string ciphertext;

   EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
   if (ctx == nullptr) {
      OpenSSLErrorMessage(__FILE__, __LINE__, "EVP_CIPHER_CTX_new function error in Crepto_AES.");
      return ciphertext;
   }

   // 鍵の長さに基づく暗号化アルゴリズムの決定
   const EVP_CIPHER* alg = nullptr;
   if (_keyLen == AES_KEY_LEN_128) alg = EVP_aes_128_cbc();
   if (_keyLen == AES_KEY_LEN_192) alg = EVP_aes_192_cbc();
   if (_keyLen == AES_KEY_LEN_256) alg = EVP_aes_256_cbc();
   if (alg == nullptr) {
      OpenSSLErrorMessage(__FILE__, __LINE__, "_keyLen is incorrect.");
      EVP_CIPHER_CTX_free(ctx);
      return ciphertext;
   }

   // 暗号コンテキスト初期化
   if (EVP_EncryptInit_ex(ctx, alg, nullptr, _key.data(), nullptr) != 1) {
      OpenSSLErrorMessage(__FILE__, __LINE__, "EVP_EncryptInit_ex function error in Crepto_AES.");
      EVP_CIPHER_CTX_free(ctx);
      return ciphertext;
   }

   // プレーンテキストのサイズ+暗号ブロックサイズで暗号化データ保蔵用の変数作成
   int ciphertextLen = _plaintext.length() + EVP_CIPHER_CTX_block_size(ctx);
   ciphertext.resize(ciphertextLen);

   // 暗号化処理
   if (EVP_EncryptUpdate(ctx, reinterpret_cast<unsigned char*>(&ciphertext[0]), &ciphertextLen, reinterpret_cast<const unsigned char*>(_plaintext.c_str()), _plaintext.length()) != 1) {
      OpenSSLErrorMessage(__FILE__, __LINE__, "EVP_EncryptUpdate function error in Crepto_AES.");
      EVP_CIPHER_CTX_free(ctx);
      return ciphertext;
   }

   // 最後のブロック長の暗号化+パディング処理を実施
   int finalLen;
   if (EVP_EncryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(&ciphertext[ciphertextLen]), &finalLen) != 1) {
      OpenSSLErrorMessage(__FILE__, __LINE__, "EVP_EncryptFinal_ex function error in Crepto_AES.");
      EVP_CIPHER_CTX_free(ctx);
      return ciphertext;
   }

   // 暗号化データのリサイズ処理
   ciphertext.resize(ciphertextLen + finalLen);

   // ctxのメモリ解放
   EVP_CIPHER_CTX_free(ctx);

   return ciphertext;
}

std::string Crepto_AES::Decrypt(const std::vector<unsigned char>& _key, const unsigned int _keyLen, const std::string& _ciphertext) {
   std::string plaintext;
   EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
   if (ctx == nullptr) {
      OpenSSLErrorMessage(__FILE__, __LINE__, "EVP_CIPHER_CTX_new function error in Crepto_AES.");
      return plaintext;
   }

   // 鍵の長さに基づく暗号化アルゴリズムの決定
   const EVP_CIPHER* alg = nullptr;
   if (_keyLen == AES_KEY_LEN_128) alg = EVP_aes_128_cbc();
   if (_keyLen == AES_KEY_LEN_192) alg = EVP_aes_192_cbc();
   if (_keyLen == AES_KEY_LEN_256) alg = EVP_aes_256_cbc();
   if (alg == nullptr) {
      OpenSSLErrorMessage(__FILE__, __LINE__, "_keyLen is incorrect.");
      EVP_CIPHER_CTX_free(ctx);
      return plaintext;
   }

   // 暗号コンテキスト初期化
   if (EVP_DecryptInit_ex(ctx, alg, nullptr, _key.data(), nullptr) != 1) {
      OpenSSLErrorMessage(__FILE__, __LINE__, "EVP_DecryptInit_ex function error in Crepto_AES.");
      EVP_CIPHER_CTX_free(ctx);
      return plaintext;
   }

   int plaintextLen = _ciphertext.length();
   plaintext.resize(plaintextLen);

   // 復号処理
   if (EVP_DecryptUpdate(ctx, reinterpret_cast<unsigned char*>(&plaintext[0]), &plaintextLen, reinterpret_cast<const unsigned char*>(_ciphertext.c_str()), _ciphertext.length()) != 1) {
      OpenSSLErrorMessage(__FILE__, __LINE__, "EVP_DecryptUpdate function error in Crepto_AES.");
      EVP_CIPHER_CTX_free(ctx);
      return plaintext;
   }

   // 最後のブロック長の復号+パディング除去を実施
   int final_len;
   if (EVP_DecryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(&plaintext[plaintextLen]), &final_len) != 1) {
      OpenSSLErrorMessage(__FILE__, __LINE__, "EVP_DecryptFinal_ex function error in Crepto_AES.");
      EVP_CIPHER_CTX_free(ctx);
      return plaintext;
   }

   // 復号データのリサイズ処理
   plaintext.resize(plaintextLen + final_len);

   // ctxのメモリ解放
   EVP_CIPHER_CTX_free(ctx);

   return plaintext;
}
}  // namespace WrapperOpenSSL