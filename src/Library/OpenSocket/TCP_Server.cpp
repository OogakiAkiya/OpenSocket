#include"OpenSocket_STD.h"
#include"OpenSocket_Def.h"
#include"base/BaseSocket.h"
#include"base/BaseRoutine.h"
#include"base/BaseServer.h"
#include"TCP_Routine.h"
#include"TCP_Server.h"

std::shared_ptr<TCP_Server> TCP_Server::GetInstance(const std::string _addrs, const std::string _port, const int _ipv, const bool _asynchronous)
{
	std::shared_ptr<TCP_Server> temp = std::make_shared<TCP_Server>();

	temp->m_socket = std::make_shared<BaseSocket>();
	temp->m_routine = std::make_shared<TCP_Routine>();
	temp->m_socket->Init(_addrs, _port);
	SwitchIpv(temp->m_socket, _ipv);
	temp->m_socket->SetProtocol_TCP();
	if (!temp->m_socket->AddressSet())return nullptr;
	if (!temp->m_socket->Bind())return nullptr;
	if (!temp->m_socket->Listen())return nullptr;
	if (_asynchronous)temp->m_socket->SetAsynchronous();
	return temp;
}

void TCP_Server::Update()
{
	std::shared_ptr<BaseSocket> client = nullptr;
	client = m_socket->Accept();
	if (client != nullptr) {
		clientList.push_back(client);
		std::vector<char> recvDataList;

		//既存ソケットの有無確認
		auto ite = recvDataMap.find(client->GetSocket());
		if (ite != recvDataMap.end()) recvDataMap.erase(ite);

		//クライアント作成
		recvDataMap.insert({ client->GetSocket(),recvDataList });

	}

	if (clientList.size() > 0) m_routine->Update(clientList, recvDataMap, recvDataQueList);
}

int TCP_Server::GetRecvDataSize()
{
	return recvDataQueList.size();
}

int TCP_Server::SendOnlyClient(const int _socket, const char* _buf, const int _bufSize)
{
	int sendDataSize = 0;
	char sendBuf[TCP_BUFFERSIZE];

	try {
		//ヘッダーを付加
		memcpy(sendBuf, &_bufSize, TCP_HEADERSIZE);
		memcpy(&sendBuf[TCP_HEADERSIZE], _buf, _bufSize);

		//エンドマーカーを付与
		memcpy(&sendBuf[TCP_HEADERSIZE + _bufSize], ENDMARKER, ENDMARKERSIZE);

		for (auto&& clients : clientList) {
			if (clients->GetSocket() == _socket)sendDataSize = clients->Send(sendBuf, _bufSize + TCP_HEADERSIZE + ENDMARKERSIZE);
		}
	}
	catch (std::exception e) {
		std::cerr << "Exception Error at TCP_Server::SendOnlyClient():" << e.what() << std::endl;
		return sendDataSize;
	}

	return sendDataSize;
}

int TCP_Server::SendAllClient(const char* _buf, const int _bufSize)
{
	int sendDataSize = 0;
	char sendBuf[TCP_BUFFERSIZE];

	try {
		//ヘッダーを付加
		memcpy(sendBuf, &_bufSize, TCP_HEADERSIZE);
		memcpy(&sendBuf[TCP_HEADERSIZE], _buf, _bufSize);

		//エンドマーカーを付与
		memcpy(&sendBuf[TCP_HEADERSIZE + _bufSize], ENDMARKER, ENDMARKERSIZE);

		for (auto&& clients : clientList) {
			sendDataSize = clients->Send(sendBuf, _bufSize + TCP_HEADERSIZE + ENDMARKERSIZE);
		}
	}
	catch (std::exception e) {
		std::cerr << "Exception Error at TCP_Server::SendAllClient():" << e.what() << std::endl;
		return sendDataSize;
	}

	return sendDataSize;
}

std::pair<int, std::vector<char>> TCP_Server::GetRecvData()
{
	std::pair<int, std::vector<char>> returnData;
	returnData = recvDataQueList.front();
	recvDataQueList.pop();
	return returnData;
}
