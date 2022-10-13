#define IPV4 1
#define IPV6 2
#define IPVD 3
#define SEQUENCEMAX 4200000000
#define RECVPACKETMAXSIZE 1000000

//�o�b�t�@�T�C�Y��1Mbyte
//#define TCP_BUFFERSIZE 1048576
#define TCP_BUFFERSIZE 2048

//TODO 型部分についてはtypedefに書き換え予定
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