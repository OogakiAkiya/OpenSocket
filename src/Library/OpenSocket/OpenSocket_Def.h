﻿namespace OpenSocket {

// プログラム中で使用する定数一覧
constexpr int IPV4 = 1;
constexpr int IPV6 = 2;
constexpr int IPVD = 3;
constexpr long SEQUENCEMAX = 4200000000;
constexpr int TCP_BASE_HEADER_SIZE = sizeof(int);
constexpr int UDP_SEQUENCE_SIZE = sizeof(unsigned int);

constexpr int RECV_PACKET_MAX_SIZE = 4194304;
constexpr int TCP_BODY_MAX_SIZE = 1048576;
constexpr int UDP_BODY_MAX_SIZE = 1000;

#define ENDMARKER u8"¥r¥n"
#define ENDMARKERSIZE strlen(ENDMARKER)

// MSとそれ以外のOSで使用する型の統一用マクロ
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