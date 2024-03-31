namespace OpenSocket {
constexpr int IPV4 = 1;
constexpr int IPV6 = 2;
constexpr int IPVD = 3;
constexpr long SEQUENCEMAX = 4200000000;
// #define RECVPACKETMAXSIZE 1000000
constexpr int TCP_HEADERSIZE = sizeof(int);

// #define TCP_BUFFERSIZE 1048576
constexpr int TCP_BUFFERSIZE = 2048;

#define ENDMARKER u8"¥r¥n"
#define ENDMARKERSIZE strlen(ENDMARKER)

// TODO 型部分についてはtypedefに書き換えが可能か検証
#ifdef _MSC_VER
#define B_SOCKET SOCKET
#define B_INIT_SOCKET INVALID_SOCKET
#define B_ADDRESS sockaddr
#define B_ADDRESS_LEN int
#define B_ADDRESS_IN sockaddr_in
#define B_SHUTDOWN SD_BOTH

#else
#define B_SOCKET int
#define B_INIT_SOCKET -1
#define B_ADDRESS struct sockaddr
#define B_ADDRESS_LEN socklen_t
#define B_ADDRESS_IN sockaddr_in
#define B_SHUTDOWN SHUT_RDWR
#endif

}  // namespace OpenSocket