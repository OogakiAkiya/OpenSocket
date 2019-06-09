#include"../../include.h"

#include"BaseSocket.h"
#include"BaseRoutine.h"
#include"BaseServer.h"

//===============================================================
//Class BaseServer
//===============================================================

std::pair<SOCKET, std::vector<char>> BaseServer::TCP_GetRecvData()
{
	std::pair<SOCKET, std::vector<char>> returnData;
	returnData = recvDataQueList.front();
	recvDataQueList.pop();
	return returnData;
}

std::pair<sockaddr, std::vector<char>> BaseServer::UDP_GetRecvData()
{
	std::pair<sockaddr, std::vector<char>> returnData;
	returnData = U_recvDataQueList.front();
	U_recvDataQueList.pop();
	return returnData;
}

void BaseServer::SwitchIpv(std::shared_ptr<BaseSocket> _socket,int _ipv){
	if (_ipv == IPV4)_socket->SetProtocolVersion_IPv4();
	if (_ipv == IPV6)_socket->SetProtocolVersion_IPv6();
	if (_ipv == IPVD)_socket->SetProtocolVersion_Dual();
}

//===============================================================
//Class TCP_Server
//===============================================================
std::shared_ptr<BaseServer> TCP_Server::GetInstance(const std::string _addrs, const std::string _port, const int _ipv, const bool _asynchronous)
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
		recvDataMap.insert({ client->GetSocket(),recvDataList });
	}
	
	m_routine->Update(clientList, recvDataMap, recvDataQueList);	
}

int TCP_Server::GetRecvDataSize()
{
	return recvDataQueList.size();
}

int TCP_Server::SendOnlyClient(SOCKET _socket, char * _buf, int _bufSize)
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

int TCP_Server::SendAllClient(char * _buf, int _bufSize)
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

//===============================================================
//Class UDP_Server
//===============================================================
std::shared_ptr<BaseServer> UDP_Server::GetInstance(const std::string _addrs, const std::string _port, const int _ipv, const bool _asynchronous)
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
	m_routine->Update(m_socket,U_recvDataQueList);
}

int UDP_Server::GetRecvDataSize()
{
	return U_recvDataQueList.size();
}

int UDP_Server::SendOnlyClient(sockaddr * _addr, char * _buf, int _bufSize)
{
	int sendDataSize = 0;
	char sendBuf[TCP_BUFFERSIZE];

	//ヘッダーを付加し送信
	memcpy(sendBuf, &sequence, sizeof(unsigned int));
	memcpy(&sendBuf[sizeof(unsigned int)], _buf, _bufSize);

	//送信処理
	int len = m_socket->Sendto(_addr, &sendBuf[0], _bufSize+sizeof(unsigned int));

	//シーケンス番号管理
	sequence++;
	if (sequence > SEQUENCEMAX) { sequence = 0; }

	return len;
}

int UDP_Server::SendMultiClient(std::vector<sockaddr> _addrList, char * _buf, int _bufSize)
{
	int sendDataSize = 0;
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
