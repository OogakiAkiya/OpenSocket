#ifndef BASESERVER_h
#define BASESERVER_h

class BaseServer {
public:
	virtual ~BaseServer() { m_socket->Close(); }
	virtual void Update() {};
	virtual int GetRecvDataSize() = 0;								      //クライアントから受信したデータがいくつあるか
	virtual int GetFileDescriptor(fd_set* _fds);
	void SetFileDescriptorPointer(fd_set* _fds);
protected:
	static void SwitchIpv(std::shared_ptr<BaseSocket> _socket, int _ipv); //IPvの設定

	std::shared_ptr<BaseSocket> m_socket;								  //ソケット通信用
	fd_set* fds = nullptr;
};

#endif