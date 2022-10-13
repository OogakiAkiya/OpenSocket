#include"OpenSocket_STD.h"
#include"OpenSocket_Def.h"
#include"base/BaseSocket.h"
#include"base/BaseRoutine.h"
#include"UDP_Routine.h"

void UDP_Routine::Update(const std::shared_ptr<BaseSocket> _socket, std::queue<std::pair<B_ADDRESS_IN, std::vector<char>>>& _recvDataQueList)
{
	std::pair<B_ADDRESS_IN, std::vector<char>> addData;
	char buf[TCP_BUFFERSIZE];

	//ŽóMˆ—
	int dataSize = _socket->Recvfrom(&addData.first, &buf[0], TCP_BUFFERSIZE, 0);

	if (dataSize > 0) {
		addData.second.resize(dataSize);
		memcpy(&addData.second[0], &buf[0], dataSize);
		_recvDataQueList.push(addData);
	}
}