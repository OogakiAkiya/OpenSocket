#include <iostream>
#include <string>

#include "Library/WrapperOpenSSL/WrapperOpenSSL.h"
#include "Library/OpenSocket/OpenSocket.h"
/*
#define OPENSSL_NO_DEPRECATED

#define CHECK_ERROR(expr)                               \
	if (!(expr))                                        \
	{                                                   \
		std::cerr << "Error in " << #expr << std::endl; \
		exit(EXIT_FAILURE);                             \
	}

// 共通鍵のサイズ（バイト単位）
const int KEY_SIZE = 32;

// 共通鍵の生成
void generateKey(unsigned char *key)
{
	CHECK_ERROR(RAND_bytes(key, KEY_SIZE) == 1);
}

// データを暗号化する
std::string encrypt(const unsigned char *key, const std::string &plaintext)
{
	EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
	CHECK_ERROR(ctx != nullptr);

	CHECK_ERROR(EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key, nullptr));

	int ciphertext_len = plaintext.length() + EVP_CIPHER_CTX_block_size(ctx);
	std::string ciphertext(ciphertext_len, '\0');

	CHECK_ERROR(EVP_EncryptUpdate(ctx, reinterpret_cast<unsigned char *>(&ciphertext[0]), &ciphertext_len,
								  reinterpret_cast<const unsigned char *>(plaintext.c_str()), plaintext.length()));

	int final_len;
	CHECK_ERROR(EVP_EncryptFinal_ex(ctx, reinterpret_cast<unsigned char *>(&ciphertext[ciphertext_len]), &final_len));

	EVP_CIPHER_CTX_free(ctx);
	ciphertext.resize(ciphertext_len + final_len);

	return ciphertext;
}

// 暗号文を復号する
std::string decrypt(const unsigned char *key, const std::string &ciphertext)
{
	EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
	CHECK_ERROR(ctx != nullptr);

	CHECK_ERROR(EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key, nullptr));

	int plaintext_len = ciphertext.length();
	std::string plaintext(plaintext_len, '\0');

	CHECK_ERROR(EVP_DecryptUpdate(ctx, reinterpret_cast<unsigned char *>(&plaintext[0]), &plaintext_len,
								  reinterpret_cast<const unsigned char *>(ciphertext.c_str()), ciphertext.length()));

	int final_len;
	CHECK_ERROR(EVP_DecryptFinal_ex(ctx, reinterpret_cast<unsigned char *>(&plaintext[plaintext_len]), &final_len));

	EVP_CIPHER_CTX_free(ctx);
	plaintext.resize(plaintext_len + final_len);

	return plaintext;
}
void printKey(const unsigned char *key, int size)
{
	std::cout << "Key: ";
	for (int i = 0; i < size; ++i)
	{
		std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(key[i]);
	}
	std::cout << std::dec << std::endl;
}
*/
int main()
{
	std::shared_ptr<Crepto_AES> temp = std::make_shared<Crepto_AES>();
	std::cout << temp->GenerateKey() << std::endl;

	/*
	OpenSSL_add_all_algorithms();

	// 共通鍵の生成
	unsigned char key[KEY_SIZE];
	generateKey(key);
	printKey(key, KEY_SIZE);

	// 暗号化対象のデータ
	std::string plaintext = "Hello, OpenSSL 3.0!";

	// 暗号化
	std::string ciphertext = encrypt(key, plaintext);
	std::cout << "Ciphertext: " << ciphertext << std::endl;

	// 復号
	std::string decryptedText = decrypt(key, ciphertext);
	std::cout << "Decrypted text: " << decryptedText << std::endl;

	EVP_cleanup();
	*/

	//=============================================================
	// TCP Server非同期通信サンプル
	//=============================================================
	/*
	auto server = TCP_Server::GetInstance("0.0.0.0", "12345", IPV4,true);
	while (1) {
		server->Update();

		//データ送信処理
		while (server->GetRecvDataSize()>0) {
			auto recvData = server->GetRecvData();
			int sendDataSize = server->SendOnlyClient(recvData.first, &recvData.second[0], recvData.second.size());
		}
	}
	*/
}
