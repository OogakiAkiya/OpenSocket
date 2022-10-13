#ifndef BASESERVER_h
#define BASESERVER_h

class BaseServer {
public:
	virtual ~BaseServer() { m_socket->Close(); }
	virtual void Update() {};
	virtual int GetRecvDataSize() = 0;																	//クライアントから受信したデータがいくつあるか
protected:
	std::shared_ptr<BaseSocket> m_socket;								  //ソケット通信用
	std::shared_ptr<BaseRoutine> m_routine;								  //recv処理などのルーティン
	static void SwitchIpv(std::shared_ptr<BaseSocket> _socket, int _ipv); //IPvの設定
};

#endif