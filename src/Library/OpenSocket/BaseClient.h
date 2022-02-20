#ifndef BASECLIENT_h
#define BASECLIENT_h

class BaseClient {
public:
	virtual ~BaseClient() { m_socket->Close(); }
	virtual void Update() {};
	virtual int GetRecvDataSize() { return 0; }											//サーバーから受信したデータがいくつあるか
	virtual std::vector<char> GetRecvData() = 0;										//サーバーから受信したデータを取り出す
	virtual int SendServer(const char* _buf, const int _bufSize) { return 0; }			//特定のサーバーに送信する場合使用する

protected:
	std::shared_ptr<BaseSocket> m_socket;
	std::shared_ptr<BaseRoutine> m_routine;
	static void SwitchIpv(std::shared_ptr<BaseSocket> _socket, int _ipv);
	std::queue<std::vector<char>> recvDataQueList;										//サーバーから受信した情報が入る


};


//==============================================================
//derived class
//==============================================================

class TCP_Client :public BaseClient {
public:
	TCP_Client() {}
	~TCP_Client() {}
	virtual void Update() override;
	static std::shared_ptr<BaseClient> GetInstance(const std::string _addrs, const std::string _port, const int _ipv, const bool _asynchronous = false);
	virtual int GetRecvDataSize()override { return recvDataQueList.size(); }			//サーバーから受信したデータがいくつあるか
	virtual std::vector<char> GetRecvData()override;									//サーバーから受信したデータを取り出す
	virtual int SendServer(const char* _buf, const int _bufSize)override;				//特定のサーバーに送信する場合使用する

private:
	std::vector<char> recvData;
};

class UDP_Client :public BaseClient {
public:
	UDP_Client() {}
	~UDP_Client() {}
	virtual void Update() override;
	static std::shared_ptr<BaseClient> GetInstance(const std::string _addrs, const std::string _port, const int _ipv, const bool _asynchronous = false);

	virtual int GetRecvDataSize()override { return recvDataQueList.size(); }			//サーバーから受信したデータがいくつあるか
	virtual std::vector<char> GetRecvData()override;									//サーバーから受信したデータを取り出す
	virtual int SendServer(const char* _buf, const int _bufSize)override;				//特定のサーバーに送信する場合使用する

private:
	unsigned int sequence = 0;															//シーケンス番号
	std::queue<std::pair<B_ADDRESS_IN, std::vector<char>>> recvDataQueList;					//クライアントから受信した情報が入る

};

#endif
