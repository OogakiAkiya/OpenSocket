#include"OpenSocket_STD.h"
#include"OpenSocket_Def.h"
#include"base/BaseSocket.h"
#include"base/BaseServer.h"
#include"TCP_Server.h"

std::shared_ptr<TCP_Server> TCP_Server::GetInstance(const std::string _addrs, const std::string _port, const int _ipv, const bool _asynchronous)
{
	std::shared_ptr<TCP_Server> temp = std::make_shared<TCP_Server>();

	temp->m_socket = std::make_shared<BaseSocket>();
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
	//ファイルディスクリプタが設定されていない場合
	if (fds == nullptr) {
		AcceptProcessing();
		if (clientList.size() > 0) DataProcessing();
		return;
	}

	//ファイルディスクリプタが設定されている場合
	if (FD_ISSET(m_socket->GetSocket(), fds)) AcceptProcessing();
	if (clientList.size() > 0) DataProcessing();
}

int TCP_Server::GetRecvDataSize()
{
	return recvDataQueList.size();
}

int TCP_Server::GetFileDescriptor(fd_set* _fds)
{
	int maxfds = BaseServer::GetFileDescriptor(_fds);
	for (int i = 0; i < clientList.size(); i++) {
		int socket = clientList.at(i)->GetSocket();
		FD_SET(socket, _fds);
		if (socket > maxfds) maxfds = socket;
	}
	return maxfds;
}

int TCP_Server::SendOnlyClient(const int _socket, const char* _buf, const int _bufSize)
{
	int sendDataSize = 0;
	char sendBuf[TCP_BUFFERSIZE];

	try {
		//ヘッダーを付加
		memcpy(sendBuf, &_bufSize, TCP_HEADERSIZE);
		memcpy(&sendBuf[TCP_HEADERSIZE], _buf, _bufSize);

		//エンドマーカーを付与
		memcpy(&sendBuf[TCP_HEADERSIZE + _bufSize], ENDMARKER, ENDMARKERSIZE);

		for (auto&& clients : clientList) {
			if (clients->GetSocket() == _socket)sendDataSize = clients->Send(sendBuf, _bufSize + TCP_HEADERSIZE + ENDMARKERSIZE);
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Exception Error at TCP_Server::SendOnlyClient():" << e.what() << std::endl;
		return sendDataSize;
	}

	return sendDataSize;
}

int TCP_Server::SendAllClient(const char* _buf, const int _bufSize)
{
	int sendDataSize = 0;
	char sendBuf[TCP_BUFFERSIZE];

	try {
		//ヘッダーを付加
		memcpy(sendBuf, &_bufSize, TCP_HEADERSIZE);
		memcpy(&sendBuf[TCP_HEADERSIZE], _buf, _bufSize);

		//エンドマーカーを付与
		memcpy(&sendBuf[TCP_HEADERSIZE + _bufSize], ENDMARKER, ENDMARKERSIZE);

		for (auto&& clients : clientList) {
			sendDataSize = clients->Send(sendBuf, _bufSize + TCP_HEADERSIZE + ENDMARKERSIZE);
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Exception Error at TCP_Server::SendAllClient():" << e.what() << std::endl;
		return sendDataSize;
	}

	return sendDataSize;
}

void TCP_Server::AcceptProcessing()
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
}

void TCP_Server::DataProcessing()
{
	//clietnListのindex,socket番号の順で値を格納
	std::list<std::pair<std::shared_ptr<BaseSocket>, int>> deleteList;

	for (int i = 0; i < clientList.size(); i++) {
		char buf[TCP_BUFFERSIZE];
		int socket = clientList.at(i)->GetSocket();

		//fdsがセットされておりsocketにイベントが発生しているか確認し、発生していなければスキップ
		if (fds) {
			if (!FD_ISSET(socket, fds)) continue;
		}

		//データを受信した際はそのバイト数が入り切断された場合は0,ノンブロッキングモードでデータを受信してない間は-1がdataSizeに入る
		int dataSize = clientList.at(i)->Recv(buf, TCP_BUFFERSIZE);

		if (dataSize > 0) {

			//受信データを格納
			int nowSize = recvDataMap[socket].size();
			recvDataMap[socket].resize(nowSize + dataSize);
			memcpy((char*)&recvDataMap[socket][nowSize], &buf[0], dataSize);

			while (recvDataMap[socket].size() > sizeof(int)) {

				int dataSize = 0;
				try {
					memcpy(&dataSize, &recvDataMap[(B_SOCKET)socket][0], sizeof(int));

					//先頭パケットが想定しているよりも小さいまたは大きいパケットの場合は不正パケットとして解釈する。
					if (dataSize < 0 || dataSize > TCP_BUFFERSIZE - sizeof(int) - ENDMARKERSIZE) {
						//TODO 不正パケットとみなした場合パケットをすべて削除しているが何かいい手がないか考える
						recvDataMap[(B_SOCKET)socket].clear();
						return;
					}

					//エンドマーカーの値が正常値かチェック
					if (memcmp(&recvDataMap[(B_SOCKET)socket][dataSize + sizeof(int)], &ENDMARKER, ENDMARKERSIZE) != 0) {
						//TODO:不正パケットとみなした場合パケットをすべて削除しているが何かいい手がないか考える
						recvDataMap[(B_SOCKET)socket].clear();
						return;
					}
				}
				catch (const std::exception& e) {
					std::cerr << "Exception Error at TCP_Server::DataProcessing:" << e.what() << std::endl;

					//TODO:不正パケットなどで先頭データがintでmemcpyできなかった際はパケットをすべて削除しているが何かいい手がないか考える
					recvDataMap[(B_SOCKET)socket].clear();
					return;
				}

				//受信データが一塊分あればレシーブキューに追加
				if (recvDataMap[socket].size() > dataSize) {
					std::pair<B_SOCKET, std::vector<char>> addData;
					addData.first = socket;
					addData.second.resize(dataSize);
					memcpy(&addData.second[0], &recvDataMap[socket][sizeof(int)], dataSize);
					recvDataQueList.push(addData);
					recvDataMap[socket].erase(recvDataMap[socket].begin(), recvDataMap[socket].begin() + dataSize + sizeof(int) + ENDMARKERSIZE);
				}
			}

		}
		else if (dataSize == 0) {
			//接続を終了するとき
			std::cout << "connection is lost" << std::endl;
			deleteList.push_back(std::make_pair(clientList.at(i),socket));
		}
#ifdef _MSC_VER
		else if (WSAGetLastError() == WSAEWOULDBLOCK) {
			//clientがsendしていなかったときにおこるエラー
		}
#endif
		else {
#ifdef _MSC_VER

			//接続エラーが起こった時
			std::cerr << "recv failed:" << WSAGetLastError() << std::endl;
			deleteList.push_back(std::make_pair(clientList.at(i), socket));
#else
			//errnoはシステムコールや標準ライブラリのエラーが格納される変数
			if (errno == EAGAIN)
			{
				//非同期だとここを通ることがあるが無視して良い
				break;
			}

			if (errno == EBADF)
			{
			}
			if (errno == ECONNRESET)
			{
				//クライアント接続リセット
				std::cout << "connection is lost" << std::endl;
				deleteList.push_back(std::make_pair(clientList.at(i), socket));
				break;
			}
			//接続エラーが起こった時
			std::cerr << "recv failed:" << errno << std::endl;
			deleteList.push_back(std::make_pair(clientList.at(i), socket));
#endif

		}
	}

	try {
		for (auto element : deleteList) {
			//受信データ用配列の初期化
			recvDataMap[element.second].erase(recvDataMap[element.second].begin(), recvDataMap[element.second].end());
			//クライアントリストから削除
			auto it = std::remove(clientList.begin(), clientList.end(), element.first);
			clientList.erase(it, clientList.end());

		}
	}
	catch (const std::exception& e) {
		std::cerr << "Exception Error at TCP_Server::DataProcessing.delete:" << e.what() << std::endl;
	}


}


std::pair<int, std::vector<char>> TCP_Server::GetRecvData()
{
	std::pair<int, std::vector<char>> returnData;
	returnData = recvDataQueList.front();
	recvDataQueList.pop();
	return returnData;
}
