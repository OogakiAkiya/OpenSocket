#ifndef TCP_ROUTINE_h
#define TCP_ROUTINE_h

class TCP_Routine :public BaseRoutine {
public:
	//特定のSocketだけUpdate処理を行う
	virtual void Update(const std::shared_ptr<BaseSocket> _socket,
		std::vector<char>& _recvData,
		std::queue<std::vector<char>>& _recvDataQueList
	)override;

	//socketリスト分Update処理を行う
	virtual void Update(std::vector<std::shared_ptr<BaseSocket>>& _clientList,
		std::unordered_map<int, std::vector<char>>& _recvDataMap,
		std::queue<std::pair<int, std::vector<char>>>& _recvDataQueList
	) override;

private:

};

#endif