#ifndef OPENSOCKET_STD_h
#define OPENSOCKET_STD_h

#ifdef _MSC_VER
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <math.h>
#include <process.h>
#include <stdlib.h>
#include <time.h>
#include <sstream>
#include <thread>

#else
#include <netdb.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <fstream>
#include <memory>
#include <unordered_map>

#endif
#include <algorithm>
#include <iostream>
#include <list>
#include <mutex>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

#endif