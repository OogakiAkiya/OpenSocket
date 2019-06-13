#ifndef BASESERVER_h
#define BASESERVER_h

#ifdef _MSC_VER

class BaseServer {
public:
	virtual ~BaseServer() { m_socket->Close(); }
	virtual void Update() {};
	virtual int GetRecvDataSize() = 0;																		//クライアントから受信したデータがいくつあるか
	virtual int SendOnlyClient(int _socket, char* _buf, int _bufSize) { return 0; }						//特定のクライアントに送信する場合使用する(TCP)
	virtual int SendOnlyClient(sockaddr* _addr, char* _buf, int _bufSize) { return 0; }						//特定のクライアントに送信する場合使用する(UDP)
	virtual int SendMultiClient(std::vector<sockaddr> _addrList, char* _buf, int _bufSize) { return 0; }	//特定の複数クライアントに送信する場合使用する(UDP)
	virtual int SendAllClient(char* _buf, int _bufSize) { return 0; }										//全てのクライアントに送信する場合使用する(TCP)

	//TCP専用
	std::pair<int, std::vector<char>> TCP_GetRecvData();													//クライアントから受信したデータを取り出す

	//UDP専用
	std::pair<sockaddr, std::vector<char>> UDP_GetRecvData();												//クライアントから受信したデータを取り出す

protected:
	std::shared_ptr<BaseSocket> m_socket;																	//ソケット通信用
	std::shared_ptr<BaseRoutine> m_routine;																	//recv処理などのルーティン
	static void SwitchIpv(std::shared_ptr<BaseSocket> _socket, int _ipv);									//IPvの設定

	//TCP専用
	std::queue<std::pair<int, std::vector<char>>> recvDataQueList;										//クライアントから受信した情報が入る

	//UDP専用
	std::queue<std::pair<sockaddr, std::vector<char>>> U_recvDataQueList;									//クライアントから受信した情報が入る

};


//==============================================================
//derived class
//==============================================================

class TCP_Server :public BaseServer {
public:
	TCP_Server() {}
	~TCP_Server() {}
	static std::shared_ptr<BaseServer> GetInstance(const std::string _addrs, const std::string _port, const int _ipv, const bool _asynchronous = false);
	virtual void Update() override;
	virtual int GetRecvDataSize() override;

	virtual int SendOnlyClient(int _socket, char* _buf, int _bufSize)override;							//特定のクライアントに送信する場合使用する
	virtual int SendAllClient(char* _buf, int _bufSize)override;											//全てのクライアントに送信する場合使用する

private:
	std::unordered_map<int, std::vector<char>> recvDataMap;												//各クライアントごとのrecvData
	std::vector<std::shared_ptr<BaseSocket>> clientList;													//クライアントのソケット情報を管理

};

class UDP_Server :public BaseServer {
public:
	UDP_Server() {}
	~UDP_Server() {}
	static std::shared_ptr<BaseServer> GetInstance(const std::string _addrs, const std::string _port, const int _ipv, const bool _asynchronous = false);
	virtual void Update() override;
	virtual int GetRecvDataSize() override;																	//受信データの数を取得
	virtual int SendOnlyClient(sockaddr* _addr, char* _buf, int _bufSize)override;							//特定のクライアントに送信する場合使用する
	virtual int SendMultiClient(std::vector<sockaddr> _addrList, char* _buf, int _bufSize)override;
private:
	unsigned int sequence = 0;																				//シーケンス番号

};


#endif
#ifdef __GNUC__
class BaseServer
{
public:
	virtual ~BaseServer() { m_socket->Close(); }
	virtual void Update() {};
	virtual int GetRecvDataSize() = 0;																		//クライアントから受信したデータがいくつあるか
	virtual int SendOnlyClient(int _socket, char *_buf, int _bufSize) { return 0; }							//特定のクライアントに送信する場合使用する(TCP)
	virtual int SendOnlyClient(sockaddr_in *_addr, char *_buf, int _bufSize) { return 0; }					//特定のクライアントに送信する場合使用する(UDP)
	virtual int SendMultiClient(std::vector<sockaddr_in> _addrList, char *_buf, int _bufSize) { return 0; } //特定の複数クライアントに送信する場合使用する(UDP)
	virtual int SendAllClient(char *_buf, int _bufSize) { return 0; }										//全てのクライアントに送信する場合使用する(TCP)

	//TCP専用
	std::pair<int, std::vector<char>> TCP_GetRecvData(); //クライアントから受信したデータを取り出す

	//UDP専用
	std::pair<sockaddr_in, std::vector<char>> UDP_GetRecvData(); //クライアントから受信したデータを取り出す

protected:
	std::shared_ptr<BaseSocket> m_socket;								  //ソケット通信用
	std::shared_ptr<BaseRoutine> m_routine;								  //recv処理などのルーティン
	static void SwitchIpv(std::shared_ptr<BaseSocket> _socket, int _ipv); //IPvの設定

	//TCP専用
	std::queue<std::pair<int, std::vector<char>>> recvDataQueList; //クライアントから受信した情報が入る

	//UDP専用
	std::queue<std::pair<sockaddr_in, std::vector<char>>> U_recvDataQueList; //クライアントから受信した情報が入る
};

//==============================================================
//derived class
//==============================================================

class TCP_Server : public BaseServer
{
public:
	TCP_Server() {}
	~TCP_Server() {}
	static std::shared_ptr<BaseServer> GetInstance(const std::string _addrs, const std::string _port, const int _ipv, const bool _asynchronous = false);
	virtual void Update() override;
	virtual int GetRecvDataSize() override;

	virtual int SendOnlyClient(int _socket, char *_buf, int _bufSize) override; //特定のクライアントに送信する場合使用する
	virtual int SendAllClient(char *_buf, int _bufSize) override;				//全てのクライアントに送信する場合使用する

private:
	std::unordered_map<int, std::vector<char>> recvDataMap; //各クライアントごとのrecvData
	std::vector<std::shared_ptr<BaseSocket>> clientList;	//クライアントのソケット情報を管理
};

class UDP_Server : public BaseServer
{
public:
	UDP_Server() {}
	~UDP_Server() {}
	static std::shared_ptr<BaseServer> GetInstance(const std::string _addrs, const std::string _port, const int _ipv, const bool _asynchronous = false);
	virtual void Update() override;
	virtual int GetRecvDataSize() override;											   //受信データの数を取得
	virtual int SendOnlyClient(sockaddr_in *_addr, char *_buf, int _bufSize) override; //特定のクライアントに送信する場合使用する
	virtual int SendMultiClient(std::vector<sockaddr_in> _addrList, char *_buf, int _bufSize) override;

private:
	unsigned int sequence = 0; //シーケンス番号
};

#endif

#endif