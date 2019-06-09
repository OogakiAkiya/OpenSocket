#include"../../include.h"
#include"BaseSocket.h"

#pragma comment(lib,"ws2_32.lib")

void BaseSocket::Init(const std::string _addrs, const std::string _port)
{
	ip = _addrs;
	port = _port;

}

void BaseSocket::SetProtocolVersion_IPv4()
{
	hints.ai_family = AF_INET;
}

void BaseSocket::SetProtocolVersion_IPv6()
{
	hints.ai_family = AF_INET6;
}

void BaseSocket::SetProtocolVersion_Dual()
{
	hints.ai_family = AF_UNSPEC;
}

void BaseSocket::SetProtocol_TCP()
{
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_socktype = SOCK_STREAM;

}

void BaseSocket::SetProtocol_UDP()
{
	hints.ai_protocol = IPPROTO_UDP;
	hints.ai_socktype = SOCK_DGRAM;
}

void BaseSocket::SetAsynchronous()
{
	unsigned long value = 1;
	ioctlsocket(m_socket, FIONBIO, &value);					//非同期通信化

}


bool BaseSocket::AddressSet()
{
	WSADATA wsaData;
	//socket使用可能かのチェック
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		is_available = true;
		return false;
	}

	hints.ai_flags = AI_PASSIVE;
	iResult = getaddrinfo(ip.c_str(), port.c_str(), &hints, &result);

	if (iResult != 0) {
		printf("getaddrinfo failed:%d\n", iResult);
		WSACleanup();
		is_available = true;
		return false;
	}

	//ソケットの作成
	m_socket = INVALID_SOCKET;
	m_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (m_socket == INVALID_SOCKET) {
		printf("Error at socket():%ld\n", WSAGetLastError());
		freeaddrinfo(result);							//メモリの解放
		WSACleanup();									//ソケットの解放
		is_available = true;
		return false;
	}
	return true;


}

void BaseSocket::Close()
{
	int iResult = shutdown(m_socket, SD_SEND);			//今送っている情報を送りきって終わる
	if (iResult == SOCKET_ERROR) {
		closesocket(m_socket);
		WSACleanup();
		return;
	}
	closesocket(m_socket);
	WSACleanup();
}

int BaseSocket::Recv(char * _recvbuf, int recvbuf_size, int flg)
{
	if (this == nullptr)return 0;
	int bytesize = 0;
	bytesize = recv(m_socket, _recvbuf, recvbuf_size, 0);
	return bytesize;
}

int BaseSocket::Recvfrom(sockaddr* _senderAddr,char * _recvbuf, int recvbuf_size, int flg)
{
	;
	int bytesize = 0;
	int sendAddrSize = sizeof(sockaddr);
	bytesize = recvfrom(m_socket, _recvbuf, recvbuf_size, 0,_senderAddr,&sendAddrSize);
	return bytesize;
}

int BaseSocket::Send(char * _sendData, const int _sendDataSize)
{
	int result = 0;
	result = send(m_socket, _sendData, _sendDataSize, 0);
	return result;
	
}

int BaseSocket::Send(SOCKET _socket, char * _sendData, const int _sendDataSize)
{
	int result = send(_socket, _sendData, _sendDataSize, 0);
	return result;
}

int BaseSocket::Sendto(char * _sendData, const int _sendDataSize)
{
	int len = sendto(m_socket, _sendData, _sendDataSize, 0, result->ai_addr, result->ai_addrlen);
	return len;
}

int BaseSocket::Sendto(sockaddr * _addr, char * _sendData, const int _sendDataSize)
{
	int len = sendto(m_socket, _sendData, _sendDataSize, 0, _addr, sizeof(sockaddr));
	return len;
}

bool BaseSocket::Bind()
{
	//bind
	int iResult = ::bind(m_socket, result->ai_addr, (int)result->ai_addrlen);				//IPアドレス(ローカルアドレスが入る)とポートの指定
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);																//メモリの解放
		closesocket(m_socket);																//ソケットのクローズ
		WSACleanup();																		//ソケットの解放
		return false;
	}
	return true;

}

bool BaseSocket::Listen()
{
	//listen
	if (listen(m_socket, SOMAXCONN) == SOCKET_ERROR) {										//バックログのサイズを設定
		printf("Listen failed with error:%ld\n", WSAGetLastError());
		closesocket(m_socket);
		WSACleanup();
		return false;
	}
	return true;
}

std::shared_ptr<BaseSocket> BaseSocket::Accept()
{
	std::shared_ptr<BaseSocket> client = std::make_shared<BaseSocket>();

	//accept処理
	SOCKET recvSocket;
	recvSocket=accept(m_socket, NULL, NULL);
	if (recvSocket == INVALID_SOCKET) {
		return nullptr;
	}

	printf("接続あり\n");
	client->SetSocket(recvSocket);
	return client;
}

bool BaseSocket::Connect()
{
	//接続
	int iResult = connect(m_socket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}
	freeaddrinfo(result);
	if (m_socket == INVALID_SOCKET) {
		printf("Unable to server!\n");
		WSACleanup();
		return false;
	}
	return true;
}