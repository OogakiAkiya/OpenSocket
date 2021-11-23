#include"../OpenSocket/OpenSocket_STD.h"
#include"FreeSocket_Def.h"
#include"FreeSocket.h"

void FreeSocket::Init(const std::string _addrs, const std::string _port)
{
	ip = _addrs;
	port = _port;
}

void FreeSocket::SetProtocolVersion_IPv4()
{
	hints.ai_family = AF_INET;
}

void FreeSocket::SetProtocolVersion_IPv6()
{
	hints.ai_family = AF_INET6;
}

void FreeSocket::SetProtocolVersion_Dual()
{
	//要確認
	hints.ai_family = AF_UNSPEC;
}

void FreeSocket::SetProtocol_TCP()
{
#ifdef _MSC_VER
	hints.ai_protocol = IPPROTO_TCP;
#endif
	hints.ai_socktype = SOCK_STREAM;
}

void FreeSocket::SetProtocol_UDP()
{
#ifdef _MSC_VER
	hints.ai_protocol = IPPROTO_UDP;
#endif
	hints.ai_socktype = SOCK_DGRAM;
}

void FreeSocket::SetAsynchronous()
{
#ifdef _MSC_VER
	unsigned long value = 1;
	ioctlsocket(m_socket, FIONBIO, &value);					//非同期通信化
#else
	int value = 1;
	int result = ioctl(m_socket, FIONBIO, &value); //非同期通信化
#endif

}

bool FreeSocket::AddressSet()
{
	int error;

#ifdef _MSC_VER
	WSADATA wsaData;
	//socket使用可能かのチェック
	error = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if ( error != 0) {
		is_available = true;
		return false;
	}
#endif
	hints.ai_flags = AI_PASSIVE;



	if ((error = getaddrinfo(ip.c_str(), port.c_str(), &hints, &result)) != 0)
	{
		printf("getaddrinfo failed %d : %s\n", error, gai_strerror(error));
#ifdef _MSC_VER
		WSACleanup();
#endif
		return false;
	}

	//ソケットの作成
	m_socket = B_INIT_SOCKET;
	m_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (m_socket == INVALID_SOCKET) {
		freeaddrinfo(result);							//メモリの解放

#ifdef _MSC_VER
		printf("Error at socket():%ld\n", WSAGetLastError());
		WSACleanup();									//ソケットの解放
#else
		printf("Error at socket()\n");
#endif
		is_available = true;
		return false;
	}
	return true;
}

void FreeSocket::Close()
{
	int error = shutdown(m_socket, SD_SEND);			//今送っている情報を送りきって終わる
	if (error == SOCKET_ERROR) printf("socket close error\n");
	freeaddrinfo(result);

#ifdef _MSC_VER
	closesocket(m_socket);
	WSACleanup();
#else
	close(m_socket);
#endif

}

int FreeSocket::Recv(char* _recvbuf, int recvbuf_size, const int flg)
{
	if (this == nullptr)return 0;
	int bytesize = 0;
	bytesize = recv(m_socket, _recvbuf, recvbuf_size, 0);
	return bytesize;
}

int FreeSocket::Recvfrom(sockaddr* _senderAddr, char* _recvbuf, int recvbuf_size, const int flg)
{
	int bytesize = 0;
	int sendAddrSize = sizeof(sockaddr);
	bytesize = recvfrom(m_socket, _recvbuf, recvbuf_size, 0, _senderAddr, &sendAddrSize);
	return bytesize;
}

int FreeSocket::Send(const char* _sendData, const int _sendDataSize)
{
	int result = 0;
	result = send(m_socket, _sendData, _sendDataSize, 0);
	return result;
}

int FreeSocket::Send(const int _socket, const char* _sendData, const int _sendDataSize)
{
	int result = send(_socket, _sendData, _sendDataSize, 0);
	return result;
}

int FreeSocket::Sendto(const char* _sendData, const int _sendDataSize)
{
	int len = sendto(m_socket, _sendData, _sendDataSize, 0, result->ai_addr, result->ai_addrlen);
	return len;
}

int FreeSocket::Sendto(const sockaddr* _addr, const char* _sendData, const int _sendDataSize)
{
	int len = sendto(m_socket, _sendData, _sendDataSize, 0, _addr, sizeof(sockaddr));
	return len;
}

bool FreeSocket::Bind()
{
	if (bind(m_socket, result->ai_addr, result->ai_addrlen) != 0) {
		
		freeaddrinfo(result);																//メモリの解放
#ifdef _MSC_VER
		printf("Bind failed with error: %d\n", WSAGetLastError());
		closesocket(m_socket);
		WSACleanup();
#else
		printf("Bind failed with error\n");
		close(m_socket);
#endif
		return false;
	}
	return true;
}

bool FreeSocket::Listen()
{
	if (listen(m_socket, SOMAXCONN) != 0) {										//バックログのサイズを設定
#ifdef _MSC_VER
		printf("Listen failed with error:%ld\n", WSAGetLastError());
		closesocket(m_socket);
		WSACleanup();
#else
		printf("Listen failed with error\n");
		close(m_socket);
#endif
		return false;
	}
	return true;
}

std::shared_ptr<BaseSocket> FreeSocket::Accept()
{
	std::shared_ptr<BaseSocket> client = std::make_shared<BaseSocket>();

	//accept処理
	B_SOCKET recvSocket;
	B_ADDRESS_LEN len = sizeof(addr);
	recvSocket = accept(m_socket, &addr, &len);

#ifdef _MSC_VER
	if (recvSocket == INVALID_SOCKET) return nullptr;
#else
	if (recvSocket < 0) return nullptr;
#endif

	printf("Accept success\n");
	client->SetSocket(recvSocket);
	return client;
}

bool FreeSocket::Connect()
{
	int error = connect(m_socket, result->ai_addr, result->ai_addrlen)
	if (error != 0)
	{
		freeaddrinfo(result);																//メモリの解放
#ifdef _MSC_VER
		if (error == SOCKET_ERROR) {
			closesocket(m_socket);
			m_socket = INVALID_SOCKET;
		}
		if (m_socket == INVALID_SOCKET) {
			printf("Unable to server!\n");
			WSACleanup();
		}
#else
		printf("Unable to server!\n");
		close(m_socket);
#endif
		return false;

	}
	return true;
}
