#ifdef _MSC_VER
B_SOCKET SOCKET
B_INIT_SOCKET INVALID_SOCKE
B_ADDRESS sockaddr
B_ADDRESS_LEN int
#else
B_SOCKET int
B_INIT_SOCKET -1
B_ADDRESS struct sockaddr
B_ADDRESS_LEN socklen_t
#endif