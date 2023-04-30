#ifndef TCP_SERVER_h
#define TCP_SERVER_h

class TCP_Server :public BaseServer {
public:
	TCP_Server() {}
	~TCP_Server() { m_socket->Close(); }
	static std::shared_ptr<TCP_Server> GetInstance(
		const std::string _addrs,
		const std::string _port,
		const int _ipv,
		const bool _asynchronous = false
	);
	virtual void Update() override;
	virtual int GetRecvDataSize() override;
	virtual int GetFileDescriptor(fd_set* _fds) override;
	std::pair<int, std::vector<char>> GetRecvData();													//クライアントから受信したデータを取り出す

	int SendOnlyClient(const int _socket, const char* _buf, const int _bufSize);						//特定のクライアントに送信する場合使用する
	int SendAllClient(const char* _buf, const int _bufSize);											//全てのクライアントに送信する場合使用する
private:
	//メンバ関数
	void AcceptProcessing();
	void DataProcessing();

	//メンバ変数
	std::unordered_map<int, std::vector<char>> recvDataMap;		   //各クライアントごとのrecvData
	std::vector<std::shared_ptr<BaseSocket>> clientList;		   //クライアントのソケット情報を管理
	std::queue<std::pair<int, std::vector<char>>> recvDataQueList; //クライアントから受信した情報が入る

};

#endif