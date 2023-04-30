#ifndef UDP_CLIENT_h
#define UDP_CLIENT_h

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
	//メンバ関数
	void DataProcessing();

	//メンバ変数
	unsigned int sequence = 0;															//シーケンス番号
	std::queue<std::pair<B_ADDRESS_IN, std::vector<char>>> recvDataQueList;				//クライアントから受信した情報が入る

};

#endif