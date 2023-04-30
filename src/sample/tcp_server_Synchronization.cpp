#include "Library/OpenSocket/OpenSocket.h"
int main() {
	//=============================================================
	//TCP Server同期通信サンプル
	//=============================================================
	fd_set readfds;
	auto server = TCP_Server::GetInstance("0.0.0.0", "12345", IPV4,false);
	while (1) {
		FD_ZERO(&readfds);
		int maxfds = server->GetFileDescriptor(&readfds);
		//ソケットの設定で非同期設定を有効にしていない場合ここでブロッキングされる
		OpenSocket_Select(&readfds,maxfds);
		server->SetFileDescriptorPointer(&readfds);
		server->Update();

		//データ送信処理
		while (server->GetRecvDataSize()>0) {
			auto recvData = server->GetRecvData();
			int sendDataSize = server->SendOnlyClient(recvData.first, &recvData.second[0], recvData.second.size());
		}
	}	
}
