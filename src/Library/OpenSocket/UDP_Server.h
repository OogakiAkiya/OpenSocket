#ifndef UDP_SERVER_h
#define UDP_SERVER_h

class UDP_Server :public BaseServer {
public:
	UDP_Server() {}
	~UDP_Server() {}
	static std::shared_ptr<UDP_Server> GetInstance(const std::string _addrs, const std::string _port, const int _ipv, const bool _asynchronous = false);
	virtual void Update() override;
	virtual int GetRecvDataSize() override;																	//受信データの数を取得
	std::pair<B_ADDRESS_IN, std::vector<char>> GetRecvData();												//クライアントから受信したデータを取り出す

	int SendOnlyClient(const B_ADDRESS_IN* _addr, const char* _buf, const int _bufSize);						//特定のクライアントに送信する場合使用する
	int SendMultiClient(const std::vector<B_ADDRESS_IN> _addrList, const char* _buf, const int _bufSize);

private:
	unsigned int sequence = 0;																				//シーケンス番号
	std::queue<std::pair<B_ADDRESS_IN, std::vector<char>>> recvDataQueList;									//クライアントから受信した情報が入る

};

#endif