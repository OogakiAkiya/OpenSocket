#include"../OpenSocket_STD.h"
#include"../OpenSocket_Def.h"
#include"BaseSocket.h"

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
	//要確認
	hints.ai_family = AF_UNSPEC;
}

void BaseSocket::SetProtocol_TCP()
{
#ifdef _MSC_VER
	hints.ai_protocol = IPPROTO_TCP;
#endif
	hints.ai_socktype = SOCK_STREAM;
}

void BaseSocket::SetProtocol_UDP()
{
#ifdef _MSC_VER
	hints.ai_protocol = IPPROTO_UDP;
#endif
	hints.ai_socktype = SOCK_DGRAM;
}

void BaseSocket::SetAsynchronous()
{
#ifdef _MSC_VER
	unsigned long value = 1;
	ioctlsocket(m_socket, FIONBIO, &value);					//非同期通信化
#else
	int value = 1;
	int code = ioctl(m_socket, FIONBIO, &value); //非同期通信化
#endif

}

bool BaseSocket::AddressSet()
{
	int error;
	try {
#ifdef _MSC_VER
		WSADATA wsaData;
		//socket使用可能かのチェック
		error = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (error != 0) {
			is_available = true;
			return false;
		}
#endif
		hints.ai_flags = AI_PASSIVE;

		if ((error = getaddrinfo(ip.c_str(), port.c_str(), &hints, &result)) != 0)
		{
			std::cerr << "getaddrinfo failed " << error << " " << gai_strerror(error) << std::endl;

#ifdef _MSC_VER
			WSACleanup();
#endif
			return false;
		}

		//ソケットの作成
		m_socket = B_INIT_SOCKET;
		m_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

#ifdef _MSC_VER
		if (m_socket == INVALID_SOCKET) {
			std::cerr << "Error at socket() : " << WSAGetLastError() << std::endl;

			std::cerr <<
				WSACleanup();									//ソケットの解放
#else
		if (m_socket < 0) {
			std::cerr << "Error at socket()" << std::endl;
#endif
			freeaddrinfo(result);							//メモリの解放
			is_available = true;
			return false;
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Exception Error at AddressSet():" << e.what() << std::endl;
		return false;
	}
	return true;
}

void BaseSocket::Close()
{
	int error = shutdown(m_socket, B_SHUTDOWN);			//今送っている情報を送りきって終わる
#ifdef _MSC_VER
	if (error == SOCKET_ERROR) std::cerr << "socket close error" << std::endl;

	closesocket(m_socket);
	WSACleanup();
#else
	if (error < 0) std::cerr << "socket close error" << std::endl;
	close(m_socket);
#endif

}

int BaseSocket::Recv(char* _recvbuf, int recvbuf_size, const int flg)
{
	int byteSize = 0;
	try {
		byteSize = recv(m_socket, _recvbuf, recvbuf_size, 0);
	}
	catch (const std::exception& e) {
		std::cerr << "Exception Error at Recv():" << e.what() << std::endl;
	}

	return byteSize;
}

int BaseSocket::Recvfrom(B_ADDRESS_IN* _senderAddr, char* _recvbuf, int recvbuf_size, const int flg)
{
	int byteSize = 0;

	try {
		B_ADDRESS_LEN sendAddrSize = sizeof(B_ADDRESS);

#ifdef _MSC_VER
		//byteSize = recvfrom(m_socket, _recvbuf, recvbuf_size, 0, _senderAddr, &sendAddrSize);
		byteSize = recvfrom(m_socket, _recvbuf, recvbuf_size, 0, (struct sockaddr*)_senderAddr, &sendAddrSize);
#else
		byteSize = recvfrom(m_socket, _recvbuf, recvbuf_size, 0, (struct sockaddr*)_senderAddr, &sendAddrSize);
#endif
	}
	catch (const std::exception& e) {
		std::cerr << "Exception Error at Recvfrom():" << e.what() << std::endl;
	}

	return byteSize;
}

int BaseSocket::Send(const char* _sendData, const int _sendDataSize)
{
	int len = 0;
	try {
		len = send(m_socket, _sendData, _sendDataSize, 0);
	}
	catch (const std::exception& e) {
		std::cerr << "Exception Error at Send():" << e.what() << std::endl;
	}
	return len;
}

int BaseSocket::Send(const int _socket, const char* _sendData, const int _sendDataSize)
{
	int len = 0;
	try {
		len = send(_socket, _sendData, _sendDataSize, 0);
	}
	catch (const std::exception& e) {
		std::cerr << "Exception Error at Send():" << e.what() << std::endl;
	}
	return len;
}

int BaseSocket::Sendto(const char* _sendData, const int _sendDataSize)
{
	int len = 0;
	try {
		len = sendto(m_socket, _sendData, _sendDataSize, 0, result->ai_addr, result->ai_addrlen);
	}
	catch (const std::exception& e) {
		std::cerr << "Exception Error at Sendto():" << e.what() << std::endl;
	}

	return len;
}

int BaseSocket::Sendto(const B_ADDRESS_IN* _addr, const char* _sendData, const int _sendDataSize)
{
	int len = 0;
	try {
#ifdef _MSC_VER
	//int len = sendto(m_socket, _sendData, _sendDataSize, 0, _addr, sizeof(B_ADDRESS_IN));
		len = sendto(m_socket, _sendData, _sendDataSize, 0, (struct sockaddr*)_addr, sizeof(sockaddr));
#else
		len = sendto(m_socket, _sendData, _sendDataSize, 0, (struct sockaddr *)_addr, sizeof(sockaddr));
#endif
	}
	catch (const std::exception& e) {
		std::cerr << "Exception Error at Sendto():" << e.what() << std::endl;
	}
	return len;
}

bool BaseSocket::Bind()
{
	try {
		if (bind(m_socket, result->ai_addr, result->ai_addrlen) != 0) {

			freeaddrinfo(result);																//メモリの解放
#ifdef _MSC_VER
			std::cerr << "Bind failed with error: " << WSAGetLastError() << std::endl;
			closesocket(m_socket);
			WSACleanup();
#else
			std::cerr << "Bind failed with error" << std::endl;
			close(m_socket);
#endif
			return false;
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Exception Error at Bind():" << e.what() << std::endl;
		return false;
	}
	return true;
}

bool BaseSocket::Listen()
{
	try {
		if (listen(m_socket, SOMAXCONN) != 0) {										//バックログのサイズを設定
#ifdef _MSC_VER
			std::cerr << "Listen failed with error: " << WSAGetLastError() << std::endl;
			closesocket(m_socket);
			WSACleanup();
#else
			std::cerr << "Listen failed with error" << std::endl;
			close(m_socket);
#endif
			return false;
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Exception Error at Listen():" << e.what() << std::endl;
		return false;
	}

	return true;
}

std::shared_ptr<BaseSocket> BaseSocket::Accept()
{
	std::shared_ptr<BaseSocket> client = std::make_shared<BaseSocket>();

	//accept処理
	B_SOCKET recvSocket;
	B_ADDRESS_LEN len = sizeof(addr);
	try {
		recvSocket = accept(m_socket, &addr, &len);
	}
	catch (const std::exception& e) {
		std::cerr << "Exception Error at Accept():" << e.what() << std::endl;
		return nullptr;
	}


#ifdef _MSC_VER
	if (recvSocket == INVALID_SOCKET) return nullptr;
#else
	if (recvSocket < 0) return nullptr;
#endif
	std::cout << "Accept success" << std::endl;
	client->SetSocket(recvSocket);
	return client;
}

bool BaseSocket::Connect()
{
	int error = 0;
	try {
		error = connect(m_socket, result->ai_addr, result->ai_addrlen);
	}
	catch (const std::exception& e) {
		std::cerr << "Exception Error at Connect():" << e.what() << std::endl;
		return false;
	}

	if (error != 0) {
			freeaddrinfo(result);																//メモリの解放
#ifdef _MSC_VER
			if (error == SOCKET_ERROR) {
				closesocket(m_socket);
				m_socket = INVALID_SOCKET;
			}
			if (m_socket == INVALID_SOCKET) {
				std::cerr << "Unable to server!" << std::endl;
				WSACleanup();
			}
#else
			std::cerr << "Unable to server!" << std::endl;
#endif
			return false;

		}
	return true;
}
