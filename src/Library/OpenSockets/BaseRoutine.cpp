#include"../../include.h"
#include"BaseSocket.h"
#include"BaseRoutine.h"


//===============================================================
//Class TCP_Routine
//===============================================================

void TCP_Routine::Update(std::shared_ptr<BaseSocket> _socket, std::vector<char>& _recvData, std::queue<std::vector<char>>& _recvDataQueList)
{
	char buf[TCP_BUFFERSIZE];
	int dataSize = _socket->Recv(buf, TCP_BUFFERSIZE);

	if (dataSize > 0) {
		//受信データを格納
		int nowSize = _recvData.size();
		_recvData.resize(nowSize + dataSize);
		memcpy((char*)&_recvData[nowSize], &buf[0], dataSize);

		while (_recvData.size() > sizeof(int)) {
				int dataSize;
				memcpy(&dataSize, &_recvData[0], sizeof(int));

				//受信データが一塊分あればキューに追加
				if (_recvData.size() > dataSize) {
					std::vector<char> addData;
					addData.resize(dataSize);
					memcpy(&addData[0], &_recvData[sizeof(int)], dataSize);
					_recvDataQueList.push(addData);
					_recvData.erase(_recvData.begin(), _recvData.begin() + dataSize + sizeof(int));
				}
			}
		}else if (dataSize == 0) {
		//接続を終了するとき
		printf("切断されました\n");
	}
	else if (WSAGetLastError() == 10035) {
		//clientがsendしていなかったときにおこるエラー
	}
	else {
		//接続エラーが起こった時
		printf("recv failed:%d\n%d", WSAGetLastError(), dataSize);
	}
}

void TCP_Routine::Update(std::vector<std::shared_ptr<BaseSocket>>& _clientList, std::unordered_map<int, std::vector<char>>& _recvDataMap, std::queue<std::pair<int, std::vector<char>>>& _recvDataQueList)
{
	std::list<int> deleteList;

	for (int i = 0; i < _clientList.size(); i++) {
		char buf[TCP_BUFFERSIZE];
		int socket = _clientList.at(i)->GetSocket();

		int dataSize = _clientList.at(i)->Recv(buf, TCP_BUFFERSIZE);

		if (dataSize > 0) {

			//受信データを格納
			int nowSize = _recvDataMap[socket].size();
			_recvDataMap[socket].resize(nowSize + dataSize);
			memcpy((char*)&_recvDataMap[socket][nowSize], &buf[0], dataSize);

			while (_recvDataMap[socket].size() > sizeof(int)) {
				int dataSize;
				memcpy(&dataSize, &_recvDataMap[(SOCKET)socket][0], sizeof(int));

				//受信データが一塊分あればキューリストに追加
				if (_recvDataMap[socket].size() > dataSize) {
					std::pair<SOCKET, std::vector<char>> addData;
					addData.first = socket;
					addData.second.resize(dataSize);
					memcpy(&addData.second[0], &_recvDataMap[socket][sizeof(int)], dataSize);
					_recvDataQueList.push(addData);
					_recvDataMap[socket].erase(_recvDataMap[socket].begin(), _recvDataMap[socket].begin() + dataSize + sizeof(int));
				}
			}

		}
		else if (dataSize == 0) {
			//接続を終了するとき
			printf("切断されました\n");
			deleteList.push_back(i);
		}
		else if (WSAGetLastError() == 10035) {
			//clientがsendしていなかったときにおこるエラー
		}
		else {
			//接続エラーが起こった時
			printf("recv failed:%d\n%d", WSAGetLastError(), dataSize);
			deleteList.push_back(i);
		}
	}

	//切断されたソケット処理
	for (auto element : deleteList) {
		//受信データ削除
		int socket = _clientList.at(element)->GetSocket();
		_recvDataMap[socket].erase(_recvDataMap[socket].begin(), _recvDataMap[socket].end());
		
		//ソケット削除
		_clientList.erase(_clientList.begin() + element);

	}


}

//===============================================================
//Class UDP_Routine
//===============================================================
void UDP_Routine::Update(std::shared_ptr<BaseSocket> _socket,std::queue<std::pair<sockaddr, std::vector<char>>>& _recvDataQueList)
{
	std::pair<sockaddr, std::vector<char>> addData;
	char buf[TCP_BUFFERSIZE];

	//受信処理
	int dataSize=_socket->Recvfrom(&addData.first,&buf[0],TCP_BUFFERSIZE,0);

	if (dataSize > 0) {
		addData.second.resize(dataSize);
		memcpy(&addData.second[0], &buf[0],dataSize);
		_recvDataQueList.push(addData);
	}
	
	
}
