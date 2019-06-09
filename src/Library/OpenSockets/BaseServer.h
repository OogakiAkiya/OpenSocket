#pragma once

class BaseServer {
public:
	virtual ~BaseServer() { m_socket->Close(); }
	virtual void Update() {};
	virtual int GetRecvDataSize() = 0;																		//�N���C�A���g�����M�����f�[�^���������邩
	virtual int SendOnlyClient(SOCKET _socket, char* _buf, int _bufSize) { return 0; }						//����̃N���C�A���g�ɑ��M����ꍇ�g�p����(TCP)
	virtual int SendOnlyClient(sockaddr* _addr, char* _buf, int _bufSize) { return 0; }						//����̃N���C�A���g�ɑ��M����ꍇ�g�p����(UDP)
	virtual int SendMultiClient(std::vector<sockaddr> _addrList, char* _buf, int _bufSize) { return 0; }	//����̕����N���C�A���g�ɑ��M����ꍇ�g�p����(UDP)
	virtual int SendAllClient(char* _buf, int _bufSize) { return 0; }										//�S�ẴN���C�A���g�ɑ��M����ꍇ�g�p����(TCP)

	//TCP��p
	std::pair<SOCKET, std::vector<char>> TCP_GetRecvData();													//�N���C�A���g�����M�����f�[�^�����o��
	
	//UDP��p
	std::pair<sockaddr, std::vector<char>> UDP_GetRecvData();												//�N���C�A���g�����M�����f�[�^�����o��

protected:
	std::shared_ptr<BaseSocket> m_socket;																	//�\�P�b�g�ʐM�p
	std::shared_ptr<BaseRoutine> m_routine;																	//recv�����Ȃǂ̃��[�e�B��
	static void SwitchIpv(std::shared_ptr<BaseSocket> _socket, int _ipv);									//IPv�̐ݒ�

	//TCP��p
	std::queue<std::pair<SOCKET, std::vector<char>>> recvDataQueList;										//�N���C�A���g�����M������񂪓���

	//UDP��p
	std::queue<std::pair<sockaddr, std::vector<char>>> U_recvDataQueList;									//�N���C�A���g�����M������񂪓���

};


//==============================================================
//derived class
//==============================================================

class TCP_Server :public BaseServer {
public:
	TCP_Server() {}
	~TCP_Server() {}
	static std::shared_ptr<BaseServer> GetInstance(const std::string _addrs, const std::string _port, const int _ipv, const bool _asynchronous=false);
	virtual void Update() override;
	virtual int GetRecvDataSize() override;

	virtual int SendOnlyClient(SOCKET _socket, char* _buf, int _bufSize)override;							//����̃N���C�A���g�ɑ��M����ꍇ�g�p����
	virtual int SendAllClient(char* _buf, int _bufSize)override;											//�S�ẴN���C�A���g�ɑ��M����ꍇ�g�p����

private:
	std::unordered_map<SOCKET, std::vector<char>> recvDataMap;												//�e�N���C�A���g���Ƃ�recvData
	std::vector<std::shared_ptr<BaseSocket>> clientList;													//�N���C�A���g�̃\�P�b�g�����Ǘ�

};

class UDP_Server :public BaseServer {
public:
	UDP_Server() {}
	~UDP_Server() {}
	static std::shared_ptr<BaseServer> GetInstance(const std::string _addrs, const std::string _port, const int _ipv, const bool _asynchronous = false);
	virtual void Update() override;
	virtual int GetRecvDataSize() override;																	//��M�f�[�^�̐����擾
	virtual int SendOnlyClient(sockaddr* _addr, char* _buf, int _bufSize)override;							//����̃N���C�A���g�ɑ��M����ꍇ�g�p����
	virtual int SendMultiClient(std::vector<sockaddr> _addrList, char* _buf, int _bufSize)override;
private:
	unsigned int sequence = 0;																				//�V�[�P���X�ԍ�

};

