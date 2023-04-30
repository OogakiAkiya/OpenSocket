#ifndef BASECLIENT_h
#define BASECLIENT_h

class BaseClient {
public:
	virtual ~BaseClient() { m_socket->Close(); }
	virtual void Update() {};
	virtual int GetRecvDataSize() { return 0; }											//サーバーから受信したデータがいくつあるか
	virtual std::vector<char> GetRecvData() = 0;										//サーバーから受信したデータを取り出す
	virtual int SendServer(const char* _buf, const int _bufSize) { return 0; }			//特定のサーバーに送信する場合使用する
	virtual int GetFileDescriptor(fd_set* _fds);
	void SetFileDescriptorPointer(fd_set* _fds);
protected:
	static void SwitchIpv(std::shared_ptr<BaseSocket> _socket, int _ipv);

	std::shared_ptr<BaseSocket> m_socket;
	std::queue<std::vector<char>> recvDataQueList;										//サーバーから受信した情報が入る
	fd_set* fds = nullptr;

};
#endif
