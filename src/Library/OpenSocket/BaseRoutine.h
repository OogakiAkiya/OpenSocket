#ifndef BASEROUTINE_h
#define BASEROUTINE_h

#ifdef _MSC_VER

class BaseRoutine {
public:
	virtual void Update(const std::shared_ptr<BaseSocket> _socket, std::vector<char>& _recvData, std::queue<std::vector<char>>& _recvDataQueList) {}
	virtual void Update(std::vector<std::shared_ptr<BaseSocket>>& _clientList, std::unordered_map<int, std::vector<char>>& _recvDataMap, std::queue<std::pair<int, std::vector<char>>>& _recvDataQueList) {}
	virtual void Update(const std::shared_ptr<BaseSocket> _socket, std::queue<std::pair<sockaddr, std::vector<char>>>& _recvDataQueList) {};

private:
};

class TCP_Routine :public BaseRoutine {
public:
	//TCP_Clientで使用
	virtual void Update(const std::shared_ptr<BaseSocket> _socket,
		std::vector<char>& _recvData,
		std::queue<std::vector<char>>& _recvDataQueList
	)override;
	//TCP_Serverで使用
	virtual void Update(std::vector<std::shared_ptr<BaseSocket>>& _clientList,
		std::unordered_map<int, std::vector<char>>& _recvDataMap,
		std::queue<std::pair<int, std::vector<char>>>& _recvDataQueList
	) override;
private:

};

class UDP_Routine :public BaseRoutine {
public:
	//UDP_Serverで使用
	virtual void Update(const std::shared_ptr<BaseSocket> _socket, std::queue<std::pair<sockaddr, std::vector<char>>>& _recvDataQueList) override;
private:

};
#endif
#ifdef __GNUC__
class BaseRoutine
{
public:
	virtual void Update(const std::shared_ptr<BaseSocket> _socket, std::vector<char> &_recvData, std::queue<std::vector<char>> &_recvDataQueList) {}
	virtual void Update(std::vector<std::shared_ptr<BaseSocket>> &_clientList, std::unordered_map<int, std::vector<char>> &_recvDataMap, std::queue<std::pair<int, std::vector<char>>> &_recvDataQueList) {}
	virtual void Update(std::shared_ptr<BaseSocket> _socket, std::queue<std::pair<sockaddr_in, std::vector<char>>> &_recvDataQueList) {};
};

class TCP_Routine : public BaseRoutine
{
public:
	//TCP_Clientで使用
	virtual void Update(const std::shared_ptr<BaseSocket> _socket,
		std::vector<char> &_recvData,
		std::queue<std::vector<char>> &_recvDataQueList) override;
	//TCP_Serverで使用
	virtual void Update(std::vector<std::shared_ptr<BaseSocket>> &_clientList,
		std::unordered_map<int, std::vector<char>> &_recvDataMap,
		std::queue<std::pair<int, std::vector<char>>> &_recvDataQueList) override;

private:
};

class UDP_Routine : public BaseRoutine
{
public:
	//UDP_Serverで使用
	virtual void Update(const std::shared_ptr<BaseSocket> _socket, std::queue<std::pair<sockaddr_in, std::vector<char>>> &_recvDataQueList) override;

private:
};
#endif
#endif

