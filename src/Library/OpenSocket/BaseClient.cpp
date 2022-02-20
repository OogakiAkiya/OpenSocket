#include"OpenSocket_STD.h"
#include"OpenSocket_Def.h"
#include"BaseSocket.h"
#include"BaseRoutine.h"
#include"BaseClient.h"

//===============================================================
//Class BaseClient
//===============================================================
void BaseClient::SwitchIpv(std::shared_ptr<BaseSocket> _socket, int _ipv) {
	if (_ipv == IPV4)_socket->SetProtocolVersion_IPv4();
	if (_ipv == IPV6)_socket->SetProtocolVersion_IPv6();
	if (_ipv == IPVD)_socket->SetProtocolVersion_Dual();
}

//===============================================================
//Class Client
//===============================================================
void TCP_Client::Update()
{
	m_routine->Update(m_socket, recvData, recvDataQueList);
}

std::shared_ptr<BaseClient> TCP_Client::GetInstance(const std::string _addrs, const std::string _port, const int _ipv, const bool _asynchronous)
{
	std::shared_ptr<TCP_Client> temp = std::make_shared<TCP_Client>();

	temp->m_socket = std::make_shared<BaseSocket>();
	temp->m_routine = std::make_shared<TCP_Routine>();

	temp->m_socket->Init(_addrs, _port);						//IPアドレスとポート番号の設定
	SwitchIpv(temp->m_socket, _ipv);							//IPvの設定
	temp->m_socket->SetProtocol_TCP();							//TCP通信に設定
	if (!temp->m_socket->AddressSet())return nullptr;			//ソケット生成
	if (!temp->m_socket->Connect())return nullptr;				//コネクト処理
	if (_asynchronous)temp->m_socket->SetAsynchronous();

	return temp;
}

std::vector<char> TCP_Client::GetRecvData()
{
	std::vector<char> returnData;
	returnData = recvDataQueList.front();
	recvDataQueList.pop();
	return returnData;
}

int TCP_Client::SendServer(const char * _buf, const int _bufSize)
{
	int sendDataSize = 0;
	char sendBuf[TCP_BUFFERSIZE];

	//ヘッダーを付加し送信
	memcpy(sendBuf, &_bufSize, sizeof(int));
	memcpy(&sendBuf[sizeof(int)], _buf, _bufSize);

	sendDataSize = m_socket->Send(sendBuf, _bufSize + sizeof(int));
	return sendDataSize;
}

//===============================================================
//UDP_Client
//===============================================================
void UDP_Client::Update()
{
	m_routine->Update(m_socket, recvDataQueList);
}

std::shared_ptr<BaseClient> UDP_Client::GetInstance(const std::string _addrs, const std::string _port, const int _ipv, const bool _asynchronous)
{
	std::shared_ptr<UDP_Client> temp = std::make_shared<UDP_Client>();

	temp->m_socket = std::make_shared<BaseSocket>();
	temp->m_routine = std::make_shared<UDP_Routine>();

	temp->m_socket->Init(_addrs, _port);						//IPアドレスとポート番号の設定
	SwitchIpv(temp->m_socket, _ipv);							//IPvの設定
	temp->m_socket->SetProtocol_UDP();							//TCP通信に設定
	if (!temp->m_socket->AddressSet())return nullptr;			//ソケット生成
	if (_asynchronous)temp->m_socket->SetAsynchronous();
	return temp;
}

std::vector<char> UDP_Client::GetRecvData()
{
	std::pair<B_ADDRESS_IN, std::vector<char>> returnData;
	returnData = recvDataQueList.front();
	recvDataQueList.pop();
	return returnData.second;
}

int UDP_Client::SendServer(const char * _buf, const int _bufSize)
{
	//int sendDataSize = 0;
	char sendBuf[TCP_BUFFERSIZE];

	//ヘッダーを付加し送信
	memcpy(&sendBuf[0], &sequence, sizeof(unsigned int));
	memcpy(&sendBuf[sizeof(unsigned int)], &_buf[0], _bufSize);

	//送信処理
	int len = m_socket->Sendto(&sendBuf[0], _bufSize + sizeof(unsigned int));

	//シーケンス番号管理
	sequence++;
	if (sequence > SEQUENCEMAX) { sequence = 0; }

	return len;
}

