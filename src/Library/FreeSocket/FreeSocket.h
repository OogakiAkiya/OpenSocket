#ifndef BASECLIENT_h
#define BASECLIENT_h

class FreeSocket {
public:
	//�ݒ�
	void Init(const std::string _addrs, const std::string _port);
	void SetProtocolVersion_IPv4();							//IPv4
	void SetProtocolVersion_IPv6();							//IPv6
	void SetProtocolVersion_Dual();							//IPv4��IPv6�̗����ƒʐM�\(�N���C�A���g�ł̂ݎg�p�\)
	void SetProtocol_TCP();									//TCP
	void SetProtocol_UDP();									//UDP
	void SetAsynchronous();									//�񓯊�
	bool AddressSet();										//socket����
	void Close();

	//recv,send
	int Recv(char* _recvbuf, int recvbuf_size, const int flg = 0);									//�f�[�^��M(TCP)
	int Recvfrom(sockaddr* _senderAddr, char* _recvbuf, int recvbuf_size, const int flg = 0);		//�f�[�^��M(UDP)
	int Send(const char* _sendData, const int _sendDataSize);										//�f�[�^���M(TCP)
	int Send(const int _socket, const char* _sendData, const int _sendDataSize);					//�f�[�^���M(TCP)
	int Sendto(const char* _sendData, const int _sendDataSize);										//�f�[�^���M(UDP)
	int Sendto(const sockaddr* _addr, const char* _sendData, const int _sendDataSize);				//�f�[�^���M(UDP)

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
	std::string ip = "";									//IP�A�h���X
	std::string port = "";									//�|�[�g�ԍ�
	B_SOCKET m_socket;										//�\�P�b�g
	bool is_available = false;
	struct addrinfo* result = NULL, hints;
	B_ADDRESS addr;
};

#endif