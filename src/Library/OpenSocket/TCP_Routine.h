#ifndef TCP_ROUTINE_h
#define TCP_ROUTINE_h

class TCP_Routine :public BaseRoutine {
public:
	virtual void Update(const std::shared_ptr<BaseSocket> _socket,
		std::vector<char>& _recvData,
		std::queue<std::vector<char>>& _recvDataQueList
	)override;

	virtual void Update(std::vector<std::shared_ptr<BaseSocket>>& _clientList,
		std::unordered_map<int, std::vector<char>>& _recvDataMap,
		std::queue<std::pair<int, std::vector<char>>>& _recvDataQueList
	) override;

private:

};

#endif