#include"../OpenSocket_STD.h"
#include"../OpenSocket_Def.h"
#include"BaseSocket.h"
#include"BaseServer.h"

int BaseServer::GetFileDescriptor(fd_set* _fds)
{
	FD_SET(m_socket->GetSocket(), _fds);
	return m_socket->GetSocket();
}

void BaseServer::SetFileDescriptorPointer(fd_set* _fds)
{
	fds = _fds;
}

void BaseServer::SwitchIpv(std::shared_ptr<BaseSocket> _socket, int _ipv) {
	if (_ipv == IPV4)_socket->SetProtocolVersion_IPv4();
	if (_ipv == IPV6)_socket->SetProtocolVersion_IPv6();
	if (_ipv == IPVD)_socket->SetProtocolVersion_Dual();
}