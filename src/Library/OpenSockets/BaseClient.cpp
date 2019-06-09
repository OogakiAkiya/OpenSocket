#include"../../include.h"
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

std::shared_ptr<BaseClient> TCP_Client::GetInstance(const std::string _addrs, const std::string _port,const int _ipv,const bool _asynchronous)
{
	std::shared_ptr<TCP_Client> temp = std::make_shared<TCP_Client>();
	
	temp->m_socket = std::make_shared<BaseSocket>();
	temp->m_routine = std::make_shared<TCP_Routine>();

	temp->m_socket->Init(_addrs, _port);						//IP�A�h���X�ƃ|�[�g�ԍ��̐ݒ�
	SwitchIpv(temp->m_socket, _ipv);							//IPv�̐ݒ�
	temp->m_socket->SetProtocol_TCP();							//TCP�ʐM�ɐݒ�
	if (!temp->m_socket->AddressSet())return nullptr;			//�\�P�b�g����
	if (!temp->m_socket->Connect())return nullptr;				//�R�l�N�g����
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

int TCP_Client::SendServer(char * _buf, int _bufSize)
{
	int sendDataSize = 0;
	char sendBuf[TCP_BUFFERSIZE];

	//�w�b�_�[��t�������M
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
	m_routine->Update(m_socket, U_recvDataQueList);
}

std::shared_ptr<BaseClient> UDP_Client::GetInstance(const std::string _addrs, const std::string _port, const int _ipv, const bool _asynchronous)
{
	std::shared_ptr<UDP_Client> temp = std::make_shared<UDP_Client>();

	temp->m_socket = std::make_shared<BaseSocket>();
	temp->m_routine = std::make_shared<UDP_Routine>();

	temp->m_socket->Init(_addrs, _port);						//IP�A�h���X�ƃ|�[�g�ԍ��̐ݒ�
	SwitchIpv(temp->m_socket, _ipv);							//IPv�̐ݒ�
	temp->m_socket->SetProtocol_UDP();							//TCP�ʐM�ɐݒ�
	if (!temp->m_socket->AddressSet())return nullptr;			//�\�P�b�g����
	if (_asynchronous)temp->m_socket->SetAsynchronous();
	return temp;
}

std::vector<char> UDP_Client::GetRecvData()
{
	std::pair<sockaddr, std::vector<char>> returnData;
	returnData = U_recvDataQueList.front();
	U_recvDataQueList.pop();
	return returnData.second;
}

int UDP_Client::SendServer(char * _buf, int _bufSize)
{
	int sendDataSize = 0;
	char sendBuf[TCP_BUFFERSIZE];

	//�w�b�_�[��t�������M
	memcpy(&sendBuf[0], &sequence, sizeof(unsigned int));
	memcpy(&sendBuf[sizeof(unsigned int)], &_buf[0], _bufSize);
	
	//���M����
	int len = m_socket->Sendto(&sendBuf[0], _bufSize+sizeof(unsigned int));

	//�V�[�P���X�ԍ��Ǘ�
	sequence++;
	if (sequence > SEQUENCEMAX) { sequence = 0; }

	return len;
}
