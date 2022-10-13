#include"OpenSocket_STD.h"
#include"OpenSocket_Def.h"
#include"base/BaseSocket.h"
#include"base/BaseRoutine.h"
#include"base/BaseClient.h"
#include"TCP_Routine.h"
#include"TCP_Client.h"


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

int TCP_Client::SendServer(const char* _buf, const int _bufSize)
{
	int sendDataSize = 0;
	char sendBuf[TCP_BUFFERSIZE];

	try {
		//ヘッダーを付加し送信
		memcpy(sendBuf, &_bufSize, sizeof(int));
		memcpy(&sendBuf[sizeof(int)], _buf, _bufSize);

		sendDataSize = m_socket->Send(sendBuf, _bufSize + sizeof(int));
	}
	catch (std::exception e) {
		std::cerr << "Exception Error at TCP_Client::SendServer():" << e.what() << std::endl;
		return sendDataSize;
	}

	return sendDataSize;
}
