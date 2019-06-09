#pragma once

#define IPV4 1
#define IPV6 2
#define IPVD 3
#define SEQUENCEMAX 4200000000

//�o�b�t�@�T�C�Y��1Mbyte
//#define TCP_BUFFERSIZE 1048576
#define TCP_BUFFERSIZE 2048

class BaseSocket {
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
	int Recv(char* _recvbuf, int recvbuf_size, int flg = 0);									//�f�[�^��M(TCP)
	int Recvfrom(sockaddr* _senderAddr,char* _recvbuf, int recvbuf_size, int flg = 0);			//�f�[�^��M(UDP)
	int Send(char* _sendData, const int _sendDataSize);											//�f�[�^���M(TCP)
	int Send(SOCKET _socket,char* _sendData, const int _sendDataSize);							//�f�[�^���M(TCP)
	int Sendto(char* _sendData, const int _sendDataSize);										//�f�[�^���M(UDP)
	int Sendto(sockaddr* _addr,char* _sendData, const int _sendDataSize);						//�f�[�^���M(UDP)

	//get,set
	SOCKET GetSocket() { return m_socket; }
	void SetSocket(SOCKET _socket) { m_socket = _socket; }

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
	SOCKET m_socket;										//�\�P�b�g
	bool is_available = false;
	struct addrinfo *result = NULL, hints;	
};
