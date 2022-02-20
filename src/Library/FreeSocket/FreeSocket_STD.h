#ifndef FREESOCKET_STD_h
#define FREESOCKET_STD_h

#include<stdio.h>
#include<vector>
#include<mutex>
#include <string>
#include <list>
#include<queue>

#ifdef _MSC_VER
#include<iostream>
#include<sstream>
#include<WinSock2.h>
#include<Ws2tcpip.h>
#include<process.h>
#include<math.h>
#include<time.h>
#include<stdlib.h>
#include<algorithm>
#include<thread>

#include <unordered_map>

#endif
#ifdef __GNUC__
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



#endif