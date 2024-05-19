#include "WrapperOpenSSL_Def.h"
#include "WrapperOpenSSL_STD.h"

#include <openssl/evp.h>

#include "WrapperOpenSSL_Utility.h"

namespace WrapperOpenSSL {

std::string createMD5Hash(const std::string _data) {
   std::string decodeHash;
   EVP_MD_CTX* mdCtx = EVP_MD_CTX_new();
   unsigned char hashValue[EVP_MAX_MD_SIZE];
   unsigned int hashSize;

   // MD5ハッシュ作成用初期設定
   EVP_DigestInit_ex2(mdCtx, EVP_md5(), NULL);

   // ハッシュデータの登録
   EVP_DigestUpdate(mdCtx, _data.c_str(), _data.size());

   // データのハッシュ化
   EVP_DigestFinal_ex(mdCtx, hashValue, &hashSize);

   // EVP_MD_CTXを解放
   EVP_MD_CTX_free(mdCtx);

   // バイナリデータを16進のデータへ変換
   decodeHash.resize(hashSize * 2);
   for (unsigned int i = 0; i < hashSize; ++i) sprintf_s(&decodeHash[i * 2], decodeHash.size() - i * 2 + 1, "%02x", hashValue[i]);

   return decodeHash;
}
}  // namespace WrapperOpenSSL
