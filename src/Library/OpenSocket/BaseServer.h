#ifndef BASESERVER_h
#define BASESERVER_h

#ifdef _MSC_VER

class BaseServer {
public:
	virtual ~BaseServer() { m_socket->Close(); }
	virtual void Update() {};
	virtual int GetRecvDataSize() = 0;																	//クライアントから受信したデータがいくつあるか
protected:
	std::shared_ptr<BaseSocket> m_socket;																//ソケット通信用
	std::shared_ptr<BaseRoutine> m_routine;																//recv処理などのルーティン
	static void SwitchIpv(std::shared_ptr<BaseSocket> _socket, int _ipv);								//IPvの設定
};


//==============================================================
//derived class
//==============================================================

class TCP_Server :public BaseServer {
public:
	TCP_Server() {}
	~TCP_Server() {}
	static std::shared_ptr<TCP_Server> GetInstance(const std::string _addrs, const std::string _port, const int _ipv, const bool _asynchronous = false);
	virtual void Update() override;
	virtual int GetRecvDataSize() override;
	std::pair<int, std::vector<char>> GetRecvData();													//クライアントから受信したデータを取り出す

	int SendOnlyClient(const int _socket, const char* _buf, const int _bufSize);						//特定のクライアントに送信する場合使用する
	int SendAllClient(const char* _buf, const int _bufSize);											//全てのクライアントに送信する場合使用する

private:
	std::unordered_map<int, std::vector<char>> recvDataMap;												//各クライアントごとのrecvData
	std::vector<std::shared_ptr<BaseSocket>> clientList;												//クライアントのソケット情報を管理
	std::queue<std::pair<int, std::vector<char>>> recvDataQueList;										//クライアントから受信した情報が入る

};

class UDP_Server :public BaseServer {
public:
	UDP_Server() {}
	~UDP_Server() {}
	static std::shared_ptr<UDP_Server> GetInstance(const std::string _addrs, const std::string _port, const int _ipv, const bool _asynchronous = false);
	virtual void Update() override;
	virtual int GetRecvDataSize() override;																//受信データの数を取得
	std::pair<sockaddr, std::vector<char>> GetRecvData();												//クライアントから受信したデータを取り出す

	int SendOnlyClient(const sockaddr* _addr, const char* _buf, const int _bufSize);					//特定のクライアントに送信する場合使用する
	int SendMultiClient(const std::vector<sockaddr> _addrList, const char* _buf, const int _bufSize);

private:
	unsigned int sequence = 0;																			//シーケンス番号
	std::queue<std::pair<sockaddr, std::vector<char>>> recvDataQueList;									//クライアントから受信した情報が入る

};


#endif
#ifdef __GNUC__
class BaseServer
{
public:
	virtual ~BaseServer() { m_socket->Close(); }
	virtual void Update() {};
	virtual int GetRecvDataSize() = 0; //クライアントから受信したデータがいくつあるか
protected:
	std::shared_ptr<BaseSocket> m_socket;								  //ソケット通信用
	std::shared_ptr<BaseRoutine> m_routine;								  //recv処理などのルーティン
	static void SwitchIpv(std::shared_ptr<BaseSocket> _socket, int _ipv); //IPvの設定
};

//==============================================================
//derived class
//==============================================================

class TCP_Server : public BaseServer
{
public:
	TCP_Server() {}
	~TCP_Server() { m_socket->Close(); }
	static std::shared_ptr<TCP_Server> GetInstance(const std::string _addrs, const std::string _port, const int _ipv, const bool _asynchronous = false);
	virtual void Update() override;
	virtual int GetRecvDataSize() override;
	std::pair<int, std::vector<char>> GetRecvData(); //クライアントから受信したデータを取り出す

	int SendOnlyClient(const int _socket, const char *_buf, const int _bufSize); //特定のクライアントに送信する場合使用する
	int SendAllClient(const char *_buf, const int _bufSize);					 //全てのクライアントに送信する場合使用する

private:
	std::unordered_map<int, std::vector<char>> recvDataMap;		   //各クライアントごとのrecvData
	std::vector<std::shared_ptr<BaseSocket>> clientList;		   //クライアントのソケット情報を管理
	std::queue<std::pair<int, std::vector<char>>> recvDataQueList; //クライアントから受信した情報が入る
};

class UDP_Server : public BaseServer
{
public:
	UDP_Server() {}
	~UDP_Server() {}
	static std::shared_ptr<UDP_Server> GetInstance(const std::string _addrs, const std::string _port, const int _ipv, const bool _asynchronous = false);
	virtual void Update() override;
	virtual int GetRecvDataSize() override;					 //受信データの数を取得
	std::pair<sockaddr_in, std::vector<char>> GetRecvData(); //クライアントから受信したデータを取り出す

	int SendOnlyClient(const sockaddr_in *_addr, const char *_buf, const int _bufSize); //特定のクライアントに送信する場合使用する
	int SendMultiClient(const std::vector<sockaddr_in> _addrList, const char *_buf, const int _bufSize);

private:
	unsigned int sequence = 0;											   //シーケンス番号
	std::queue<std::pair<sockaddr_in, std::vector<char>>> recvDataQueList; //クライアントから受信した情報が入る
};
#endif

#endif