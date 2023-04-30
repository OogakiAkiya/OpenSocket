#include "Library/OpenSocket/OpenSocket.h"
int main() {
	//=============================================================
	//UDP Client非同期通信サンプル
	//=============================================================
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
			std::memcpy(&sequence, &temp[0], sizeof(unsigned int));
			printf("Recv(%d)=%s\n", sequence, &temp[sizeof(unsigned int)]);

			len=client->SendServer(&temp[sizeof(unsigned int)], temp.size()-sizeof(unsigned int));
		}
	}
}
