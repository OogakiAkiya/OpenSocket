#include"include.h"

#include"Library/OpenSockets/BaseSocket.h"
#include"Library/OpenSockets/BaseRoutine.h"
#include"Library/OpenSockets/BaseServer.h"
#include"Library/OpenSockets/BaseClient.h"

int main() {
	//=============================================================
	//TCPサンプル
	//=============================================================
	//TCP_Server
	/*
	auto server = TCP_Server::GetInstance("0.0.0.0", "12345", IPV4,true);
	while (1) {
		server->Update();

		//データ送信処理
		while (server->GetRecvDataSize()>0) {
			auto recvData = server->TCP_GetRecvData();
			int sendDataSize = server->SendOnlyClient(recvData.first, &recvData.second[0], recvData.second.size());
		}
	}
	*/

	//TCP_Client
	/*
	std::shared_ptr<BaseClient> client;
	while (1) {
		client = TCP_Client::GetInstance("127.0.0.1", "12345", IPV4,true);
		if (client) {
			break;
		}
		else {
			client = nullptr;
		}
	}

	//送信処理(仮)
	char tem[6] = "HELLO";
	int dataSize=client->SendServer(tem, sizeof(tem));
	printf("SendData=%d\n", dataSize);
	while (client->GetRecvDataSize() == 0) {
		client->Update();
	}
	//受信処理
	std::vector<char> temp=client->GetRecvData();
	char buf[100];
	memcpy(buf, &temp[0], temp.size());
	printf("%s\n\n", buf);
	*/


	//=============================================================
	//UDPサンプル
	//=============================================================
	//Server
	/*
	auto server = UDP_Server::GetInstance("0.0.0.0", "12345", IPV4, true);
	while (1) {
		server->Update();
		if (server->GetRecvDataSize() > 0) {
			auto temp=server->UDP_GetRecvData();
			unsigned int sequence;
			memcpy(&sequence, &temp.second[0], sizeof(unsigned int));
			printf("Recv(%d)=%s\n", sequence, &temp.second[sizeof(unsigned int)]);
			int sendDataSize=server->SendOnlyClient(&temp.first, &temp.second[sizeof(unsigned int)], temp.second.size()- sizeof(unsigned int));
			printf("Send=%d\n", sendDataSize);
		}
	}
	*/
	//Client
	/*
	std::shared_ptr<BaseClient> client;
	client = UDP_Client::GetInstance("127.0.0.1", "12345", IPVD, true);
	char tem[6] = "HELLO";
	int len=client->SendServer(&tem[0], sizeof(tem));
	printf("Send=%d\n", len);
	while (1) {
		client->Update();
		if (client->GetRecvDataSize() > 0) {
			std::vector<char> temp = client->GetRecvData();
			unsigned int sequence;
			memcpy(&sequence, &temp[0], sizeof(unsigned int));
			printf("Recv(%d)=%s\n", sequence, &temp[sizeof(unsigned int)]);

			len=client->SendServer(&temp[sizeof(unsigned int)], temp.size()-sizeof(unsigned int));
		}
	}
	*/
	return 0;
}