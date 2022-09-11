#ifndef OPENSOCKET_STD_h
#define OPENSOCKET_STD_h



#ifdef _MSC_VER
#pragma comment(lib,"ws2_32.lib")
#include<sstream>
#include<WinSock2.h>
#include<Ws2tcpip.h>
#include<process.h>
#include<math.h>
#include<time.h>
#include<stdlib.h>
#include<algorithm>
#include<thread>


#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <memory>
#include <unordered_map>
#include <cstring>
#include <sys/ioctl.h>
#include <fstream>

#endif
#include<iostream>
#include<stdio.h>
#include<vector>
#include<mutex>
#include<string>
#include<list>
#include<queue>
#include<unordered_map>

#endif