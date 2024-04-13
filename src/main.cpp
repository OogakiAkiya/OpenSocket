#include "Library/OpenSocket/OpenSocket.h"
#include "Library/WrapperOpenSSL/WrapperOpenSSL.h"

int main() {
   //=========================
   // 各種変数定義
   //=========================

   // サーバクライアント共通情報
   unsigned int rsa_key_len = 2048;  // 鍵の長さはデータの長さの最大値(bit)+パディングサイズ88(bit)を下回らないようにする
   int aes_key_len = WrapperOpenSSL::AES_KEY_LEN_256;
   std::string plaintext(rsa_key_len / 8 - WrapperOpenSSL::PKCS1_PADDING_SIZE, 'A');

   // クライアント公開暗号
   std::shared_ptr<WrapperOpenSSL::Crepto_RSA> client_a_rsa = std::make_shared<WrapperOpenSSL::Crepto_RSA>();
   std::string client_rsa_pub_key;

   // クライアント共通鍵
   std::shared_ptr<WrapperOpenSSL::Crepto_AES> client_a_aes = std::make_shared<WrapperOpenSSL::Crepto_AES>();
   std::vector<unsigned char> client_a_aes_key;

   // サーバ公開暗号
   std::shared_ptr<WrapperOpenSSL::Crepto_RSA> server_a_rsa = std::make_shared<WrapperOpenSSL::Crepto_RSA>();

   // サーバ共通鍵
   std::shared_ptr<WrapperOpenSSL::Crepto_AES> server_a_aes = std::make_shared<WrapperOpenSSL::Crepto_AES>();
   std::vector<unsigned char> server_a_aes_key;

   //=========================
   // ハイブリット暗号処理
   //=========================
   // サーバ側で公開鍵を生成しクライアントへ送付(本来は通信で実現)
   server_a_rsa->GenerateKey(rsa_key_len);
   client_rsa_pub_key = server_a_rsa->GetPAMPublicKey();
   std::cout << "pubkey:" << server_a_rsa->GetPAMPublicKey() << std::endl;
   std::cout << "privatekey:" << server_a_rsa->GetPAMPrivateKey() << std::endl;

   // クライアント側で使用したい共通鍵を生成
   client_a_aes->GenerateKey(client_a_aes_key, aes_key_len);

   // 受け取った公開鍵を使用し共通鍵を暗号化しサーバへ送付(本来は通信で実現)
   std::string cipher_shared_key = client_a_rsa->Encrypt(client_rsa_pub_key, client_a_aes_key);
   std::cout << "cipher_shared_key:" << cipher_shared_key << std::endl;
   std::cout << "cipher_shared_key.size():" << cipher_shared_key.size() << std::endl;

   // サーバ側で共通鍵の復号処理
   std::string decrypt_shared_key = server_a_rsa->Decrypt(server_a_rsa->GetPAMPrivateKey(), cipher_shared_key);
   std::vector<unsigned char> decode_shared_key(decrypt_shared_key.begin(), decrypt_shared_key.begin() + aes_key_len);
   server_a_aes_key = decode_shared_key;

   // 共通鍵のデータ比較
   std::cout << "Client AES Key:" << std::endl;
   for (unsigned char str : client_a_aes_key) { std::cout << static_cast<int>(str) << " "; }
   std::cout << std::endl;
   std::cout << "Client AES Key Size:" << client_a_aes_key.size() << std::endl;

   std::cout << "Server AES Key:" << std::endl;
   for (unsigned char str : server_a_aes_key) { std::cout << static_cast<int>(str) << " "; }
   std::cout << std::endl;
   std::cout << "Server AES Key Size:" << server_a_aes_key.size() << std::endl;

   // 受け渡した共通鍵でデータの暗号化と復号ができるかチェックする。
   std::string endata = client_a_aes->Encrypt(client_a_aes_key, aes_key_len, plaintext);
   std::cout << "plaintext :" << plaintext << std::endl;
   std::string decodetext = server_a_aes->Decrypt(server_a_aes_key, aes_key_len, endata);
   std::cout << "decodetext:" << decodetext << std::endl;

   if (plaintext.compare(decodetext) == 0) std::cout << "整合性チェック: OK" << std::endl;
}
