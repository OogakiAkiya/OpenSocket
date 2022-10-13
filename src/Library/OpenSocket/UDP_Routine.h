#ifndef UDP_ROUTINE_h
#define UDP_ROUTINE_h

class UDP_Routine :public BaseRoutine {
public:
	virtual void Update(const std::shared_ptr<BaseSocket> _socket, std::queue<std::pair<B_ADDRESS_IN, std::vector<char>>>& _recvDataQueList) override;

private:

};

#endif