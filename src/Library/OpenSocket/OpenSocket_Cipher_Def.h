namespace OpenSocket {
// プログラム中で使用する定数一覧
constexpr int TCP_CIPHER_HEADER_SIZE = sizeof(char) + sizeof(char) + sizeof(char) + sizeof(char);
constexpr int UDP_CIPHER_HEADER_SIZE = sizeof(char) + sizeof(char) + sizeof(char) + sizeof(char);

constexpr int CHECK_DATA_SIZE = 64;

// KeyChangeConnectionStartの引数に設定する鍵交換パラメータ
constexpr int RSA_KEY_2048_BYTE_SIZE = 256;
constexpr int RSA_KEY_2048_BIT_SIZE = 2048;
constexpr int AES_KEY_LEN_128 = 16;
constexpr int AES_KEY_LEN_192 = 24;
constexpr int AES_KEY_LEN_256 = 32;

// ヘッダー定数
/* パディング用データ(全セグメント共通) */
constexpr char PADDING_DATA = 0;

/* パケット大分類(ヘッダーの第一セグメントに指定) */
constexpr char NORMAL_PACKET = 1;
constexpr char CIPHER_PACKET = 2;

/* パケット分類NORMAL_PACKET: パケット情報(ヘッダーの第二セグメントに指定) */
constexpr char NORMAL_PACKET_SEND_DATA = 10;
/* パケット分類CIPHER_PACKET: パケット情報(ヘッダーの第二セグメントに指定) */
constexpr char CIPHER_PACKET_SEND_DATA = 10;
constexpr char CIPHER_PACKET_CREATE_PUBLICKEY_REQUEST = 21;
constexpr char CIPHER_PACKET_RECREATE_PUBLICKEY_REQUEST = 22;
constexpr char CIPHER_PACKET_SEND_PUBLICKEY = 23;
constexpr char CIPHER_PACKET_REGISTRY_SHAREDKEY_REQUEST = 24;
constexpr char CIPHER_PACKET_REGISTRIED_SHAREDKEY = 25;
constexpr char CIPHER_PACKET_CHECK_SHAREDKEY_REQUEST = 26;
constexpr char CIPHER_PACKET_SEND_CHECKDATA = 27;
constexpr char CIPHER_PACKET_CHECK_CHECKDATA_REQUEST = 28;
constexpr char CIPHER_PACKET_CHECK_SUCCESS = 29;
constexpr char CIPHER_PACKET_CHECK_FAILD = 30;

/* パケット分類CIPHER_PACKET,パケット情報CIPHER_PACKET_CREATE_PUBLICKEY_REQUEST: オプション情報1(ヘッダーの第三セグメントに指定) */
constexpr char CIPHER_PACKET_RSA_KEY_SIZE_2048 = 10;

}  // namespace OpenSocket