#include"OpenSocket_STD.h"
#include"OpenSocket_Def.h"
#include"BaseSocket.h"
#include"BaseRoutine.h"
#include"BaseServer.h"

//===============================================================
//Class BaseServer
//===============================================================

void BaseServer::SwitchIpv(std::shared_ptr<BaseSocket> _socket, int _ipv) {
	if (_ipv == IPV4)_socket->SetProtocolVersion_IPv4();
	if (_ipv == IPV6)_socket->SetProtocolVersion_IPv6();
	if (_ipv == IPVD)_socket->SetProtocolVersion_Dual();
}

//===============================================================
//Class TCP_Server
//===============================================================
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

int TCP_Server::SendOnlyClient(const int _socket, const char * _buf, const int _bufSize)
{
	int sendDataSize = 0;
	char sendBuf[TCP_BUFFERSIZE];

	//ヘッダーを付加し送信
	memcpy(sendBuf, &_bufSize, sizeof(int));
	memcpy(&sendBuf[sizeof(int)], _buf, _bufSize);

	for (auto&& clients : clientList) {
		if (clients->GetSocket() == _socket)sendDataSize = clients->Send(sendBuf, _bufSize + sizeof(int));
	}
	return sendDataSize;
}

int TCP_Server::SendAllClient(const char * _buf, const int _bufSize)
{
	int sendDataSize = 0;
	char sendBuf[TCP_BUFFERSIZE];

	//ヘッダーを付加し送信
	memcpy(sendBuf, &_bufSize, sizeof(int));
	memcpy(&sendBuf[sizeof(int)], _buf, _bufSize);

	for (auto&& clients : clientList) {
		sendDataSize = clients->Send(sendBuf, _bufSize + sizeof(int));
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

//===============================================================
//Class UDP_Server
//===============================================================
std::shared_ptr<UDP_Server> UDP_Server::GetInstance(const std::string _addrs, const std::string _port, const int _ipv, const bool _asynchronous)
{
	std::shared_ptr<UDP_Server> temp = std::make_shared<UDP_Server>();

	temp->m_socket = std::make_shared<BaseSocket>();
	temp->m_routine = std::make_shared<UDP_Routine>();
	temp->m_socket->Init(_addrs, _port);
	SwitchIpv(temp->m_socket, _ipv);
	temp->m_socket->SetProtocol_UDP();
	if (!temp->m_socket->AddressSet())return nullptr;
	if (!temp->m_socket->Bind())return nullptr;
	if (_asynchronous)temp->m_socket->SetAsynchronous();

	return temp;
}

void UDP_Server::Update()
{
	m_routine->Update(m_socket, recvDataQueList);
}

int UDP_Server::GetRecvDataSize()
{
	return recvDataQueList.size();
}

int UDP_Server::SendOnlyClient(const B_ADDRESS_IN* _addr, const char * _buf, const int _bufSize)
{
	char sendBuf[TCP_BUFFERSIZE];

	//ヘッダーを付加し送信
	memcpy(sendBuf, &sequence, sizeof(unsigned int));
	memcpy(&sendBuf[sizeof(unsigned int)], _buf, _bufSize);

	//送信処理
	int len = m_socket->Sendto(_addr, &sendBuf[0], _bufSize + sizeof(unsigned int));

	//シーケンス番号管理
	sequence++;
	if (sequence > SEQUENCEMAX) { sequence = 0; }

	return len;
}

int UDP_Server::SendMultiClient(const std::vector<B_ADDRESS_IN> _addrList, const char * _buf, const int _bufSize)
{
	char sendBuf[TCP_BUFFERSIZE];
	int len = 0;

	//ヘッダーを付加し送信
	memcpy(sendBuf, &sequence, sizeof(unsigned int));
	memcpy(&sendBuf[sizeof(unsigned int)], _buf, _bufSize);

	for (auto&& addr : _addrList) {
		//送信処理
		len = m_socket->Sendto(&addr, &sendBuf[0], _bufSize + sizeof(unsigned int));
	}

	//シーケンス番号管理
	sequence++;
	if (sequence > SEQUENCEMAX) { sequence = 0; }

	return len;
}

std::pair<B_ADDRESS_IN, std::vector<char>> UDP_Server::GetRecvData()
{
	std::pair<B_ADDRESS_IN, std::vector<char>> returnData;
	returnData = recvDataQueList.front();
	recvDataQueList.pop();
	return returnData;
}

