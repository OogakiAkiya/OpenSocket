#include"../OpenSocket_STD.h"
#include"../OpenSocket_Def.h"
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
