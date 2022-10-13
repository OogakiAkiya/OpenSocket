#ifndef BASEROUTINE_h
#define BASEROUTINE_h

class BaseRoutine {
public:
	virtual void Update(const std::shared_ptr<BaseSocket> _socket, std::vector<char>& _recvData, std::queue<std::vector<char>>& _recvDataQueList) {}
	virtual void Update(std::vector<std::shared_ptr<BaseSocket>>& _clientList, std::unordered_map<int, std::vector<char>>& _recvDataMap, std::queue<std::pair<int, std::vector<char>>>& _recvDataQueList) {}
	virtual void Update(const std::shared_ptr<BaseSocket> _socket, std::queue<std::pair<B_ADDRESS_IN, std::vector<char>>>& _recvDataQueList) {};
private:
};

#endif