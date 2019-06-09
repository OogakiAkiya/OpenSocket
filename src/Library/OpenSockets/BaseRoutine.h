#pragma once
class BaseSocket;

class BaseRoutine {
public:
	virtual void Update(std::shared_ptr<BaseSocket> _socket,std::vector<char>& _recvData, std::queue<std::vector<char>>& _recvDataQueList) {}
	virtual void Update(std::vector<std::shared_ptr<BaseSocket>>& _clientList, std::unordered_map<SOCKET, std::vector<char>>& _recvDataMap, std::queue<std::pair<SOCKET, std::vector<char>>>& _recvDataQueList) {}
	virtual void Update(std::shared_ptr<BaseSocket> _socket,std::queue<std::pair<sockaddr, std::vector<char>>>& _recvDataQueList) {};

private:
};

class TCP_Routine:public BaseRoutine {
public:
	//TCP_Client�Ŏg�p
	virtual void Update(std::shared_ptr<BaseSocket> _socket, 
						std::vector<char>& _recvData, 
						std::queue<std::vector<char>>& _recvDataQueList
						)override;
	//TCP_Server�Ŏg�p
	virtual void Update(std::vector<std::shared_ptr<BaseSocket>>& _clientList,
						std::unordered_map<SOCKET, std::vector<char>>& _recvDataMap, 
						std::queue<std::pair<SOCKET, std::vector<char>>>& _recvDataQueList
						) override;
private:

};

class UDP_Routine :public BaseRoutine {
public:
	//UDP_Server�Ŏg�p
	virtual void Update(std::shared_ptr<BaseSocket> _socket,std::queue<std::pair<sockaddr, std::vector<char>>>& _recvDataQueList) override ;
private:

};