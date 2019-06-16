#ifndef Include_h
#define Include_h

#ifdef _MSC_VER
#include<stdio.h>
#include<iostream>
#include<sstream>
#include<string>
#include<WinSock2.h>
#include<Ws2tcpip.h>
#include<vector>
#include<mutex>
#include<process.h>
#include<math.h>
#include<time.h>
#include<stdlib.h>
#include<algorithm>
#include<thread>
#include<queue>
#include <unordered_map>

#endif
#ifdef __GNUC__
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <queue>
#include <list>
#include <cstring>
#include <sys/ioctl.h>
#include <fstream>

#endif



#endif