#ifndef BASESOCKET_h
#define BASESOCKET_h
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
	int Recv(char* _recvbuf, int recvbuf_size, const int flg = 0);									//データ受信(TCP)
	int Recvfrom(B_ADDRESS_IN* _senderAddr, char* _recvbuf, int recvbuf_size, const int flg = 0);		//データ受信(UDP)
	int Send(const char* _sendData, const int _sendDataSize);										//データ送信(TCP)
	int Send(const int _socket, const char* _sendData, const int _sendDataSize);					//データ送信(TCP)
	int Sendto(const char* _sendData, const int _sendDataSize);										//データ送信(UDP)
	int Sendto(const B_ADDRESS_IN* _addr, const char* _sendData, const int _sendDataSize);				//データ送信(UDP)

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
	B_SOCKET m_socket;										//ソケット
	bool is_available = false;
	struct addrinfo* result = NULL, hints;
	B_ADDRESS addr;
};

#endif


