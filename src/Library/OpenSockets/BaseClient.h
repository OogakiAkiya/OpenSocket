#pragma once

class BaseClient {
public:
	virtual ~BaseClient() { m_socket->Close(); }
	virtual void Update() {};
	virtual int GetRecvDataSize() { return 0; }											//�T�[�o�[�����M�����f�[�^���������邩
	virtual std::vector<char> GetRecvData() = 0;										//�T�[�o�[�����M�����f�[�^�����o��
	virtual int SendServer(char* _buf, int _bufSize) { return 0; }						//����̃T�[�o�[�ɑ��M����ꍇ�g�p����

protected:
	std::shared_ptr<BaseSocket> m_socket;
	std::shared_ptr<BaseRoutine> m_routine;
	static void SwitchIpv(std::shared_ptr<BaseSocket> _socket, int _ipv);
	std::queue<std::vector<char>> recvDataQueList;										//�T�[�o�[�����M������񂪓���

	//UDP
	std::queue<std::pair<sockaddr, std::vector<char>>> U_recvDataQueList;				//�N���C�A���g�����M������񂪓���

};


//==============================================================
//derived class
//==============================================================

class TCP_Client :public BaseClient {
public:
	TCP_Client() {}
	~TCP_Client() {}
	virtual void Update() override;
	static std::shared_ptr<BaseClient> GetInstance(const std::string _addrs, const std::string _port,const int _ipv, const bool _asynchronous = false);
	virtual int GetRecvDataSize()override { return recvDataQueList.size(); }			//�T�[�o�[�����M�����f�[�^���������邩
	virtual std::vector<char> GetRecvData()override;									//�T�[�o�[�����M�����f�[�^�����o��
	virtual int SendServer(char* _buf, int _bufSize)override;							//����̃T�[�o�[�ɑ��M����ꍇ�g�p����

private:
	std::vector<char> recvData;
};

class UDP_Client :public BaseClient {
public:
	UDP_Client() {}
	~UDP_Client() {}
	virtual void Update() override;
	static std::shared_ptr<BaseClient> GetInstance(const std::string _addrs, const std::string _port, const int _ipv, const bool _asynchronous = false);

	virtual int GetRecvDataSize()override { return U_recvDataQueList.size(); }			//�T�[�o�[�����M�����f�[�^���������邩
	virtual std::vector<char> GetRecvData()override;									//�T�[�o�[�����M�����f�[�^�����o��
	virtual int SendServer(char* _buf, int _bufSize)override;							//����̃T�[�o�[�ɑ��M����ꍇ�g�p����

private:
	unsigned int sequence = 0;															//�V�[�P���X�ԍ�
};
