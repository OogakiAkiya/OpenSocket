#ifndef BASESOCKET_h
#define BASESOCKET_h

#define IPV4 1
#define IPV6 2
#define IPVD 3
#define SEQUENCEMAX 4200000000

//バッファサイズは1Mbyte
//#define TCP_BUFFERSIZE 1048576
#define TCP_BUFFERSIZE 2048


#ifdef _MSC_VER
class BaseSocket {
public:
	//設定
	void Init(const std::string _addrs, const std::string _port);
	void SetProtocolVersion_IPv4();							//IPv4
	void SetProtocolVersion_IPv6();							//IPv6
	void SetProtocolVersion_Dual();							//IPv4とIPv6の両方と通信可能(クライアントでのみ使用可能)
	void SetProtocol_TCP();									//TCP
	void SetProtocol_UDP();									//UDP
	void SetAsynchronous();									//非同期
	bool AddressSet();										//socket処理
	void Close();

	//recv,send
	int Recv(char* _recvbuf,int recvbuf_size, const int flg = 0);									//データ受信(TCP)
	int Recvfrom(sockaddr* _senderAddr, char* _recvbuf, int recvbuf_size,const int flg = 0);			//データ受信(UDP)
	int Send(const char* _sendData, const int _sendDataSize);											//データ送信(TCP)
	int Send(const int _socket, const char* _sendData, const int _sendDataSize);							//データ送信(TCP)
	int Sendto(const char* _sendData, const int _sendDataSize);										//データ送信(UDP)
	int Sendto(const sockaddr* _addr, const char* _sendData, const int _sendDataSize);						//データ送信(UDP)

	//get,set
	int GetSocket() { return m_socket; }
	void SetSocket(int _socket) { m_socket = _socket; }

	//server
	bool Bind();
	bool Listen();
	std::shared_ptr<BaseSocket> Accept();

	//client
	bool Connect();

protected:
	//==================================================
	//Winsock2.0 variable
	//==================================================
	std::string ip = "";									//IPアドレス
	std::string port = "";									//ポート番号
	SOCKET m_socket;										//ソケット
	bool is_available = false;
	struct addrinfo *result = NULL, hints;
};

#endif
#ifdef __GNUC__
class BaseSocket
{
public:
	~BaseSocket() { Close(); }
	void Init(const std::string _addrs, const std::string _port);
	void SetProtocolVersion_IPv4(); //IPv4
	void SetProtocolVersion_IPv6(); //IPv6
	void SetProtocolVersion_Dual(); //IPv4とIPv6の両方と通信可能(クライアントでのみ使用可能)
	void SetProtocol_TCP();			//TCP
	void SetProtocol_UDP();			//UDP
	void SetAsynchronous();			//非同期
	bool AddressSet();				//socket処理
	void Close();

	//get,set
	int GetSocket() { return m_socket; }
	void SetSocket(int _socket) { m_socket = _socket; }

	//server
	bool Bind();
	bool Listen();
	std::shared_ptr<BaseSocket> Accept();
	//client
	bool Connect();

	int Recv(char *_recvbuf, int recvbuf_size, const int flg = 0);							   //データ受信(TCP)
	int Recvfrom(sockaddr_in *_senderAddr, char *_recvbuf, int recvbuf_size, const int flg = 0); //データ受信(UDP)
	int Send(const char *_sendData, const int _sendDataSize);									   //データ送信(TCP)
	int Send(const int _socket, const char *_sendData, const int _sendDataSize);					   //データ送信(TCP)
	int Sendto(const char *_sendData, const int _sendDataSize);								   //データ送信(UDP)
	int Sendto(const sockaddr_in *_addr, const char *_sendData, const int _sendDataSize);			   //データ送信(UDP)

private:
	int m_socket;
	std::string ip;
	std::string port;
	struct addrinfo hints, *result;
	struct sockaddr_in client;
};

#endif

#endif


