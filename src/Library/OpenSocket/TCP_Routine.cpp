#include"OpenSocket_STD.h"
#include"OpenSocket_Def.h"
#include"base/BaseSocket.h"
#include"base/BaseRoutine.h"
#include"TCP_Routine.h"

void TCP_Routine::Update(const std::shared_ptr<BaseSocket> _socket, std::vector<char>& _recvData, std::queue<std::vector<char>>& _recvDataQueList)
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
			try {
				//先頭パケットの解析
				memcpy(&dataSize, &_recvData[0], sizeof(int));

				//先頭パケットが想定しているよりも小さいまたは大きいパケットの場合は不正パケットとして解釈する。
				if (dataSize < 0 || dataSize > TCP_BUFFERSIZE - sizeof(int) - ENDMARKERSIZE) {
					//TODO:不正パケットとみなした場合パケットをすべて削除しているが何かいい手がないか考える
					_recvData.clear();
					return;
				}

				//エンドマーカーの値が正常値かチェック
				if (memcmp(&_recvData[dataSize + sizeof(int)], &ENDMARKER, ENDMARKERSIZE) != 0) {
					//TODO:不正パケットとみなした場合パケットをすべて削除しているが何かいい手がないか考える
					_recvData.clear();
					return;
				}

			}
			catch (std::exception e) {
				std::cerr << "Exception Error at TCP_Routine::Update():" << e.what() << std::endl;

				//TODO:不正パケットなどで先頭データがintでmemcpyできなかった際はパケットをすべて削除しているが何かいい手がないか考える
				_recvData.clear();
				return;
			}

			//受信データが一塊分あればキューに追加
			if (_recvData.size() > dataSize) {
				std::vector<char> addData;
				addData.resize(dataSize);
				memcpy(&addData[0], &_recvData[sizeof(int)], dataSize);
				_recvDataQueList.push(addData);
				_recvData.erase(_recvData.begin(), _recvData.begin() + dataSize + sizeof(int) + ENDMARKERSIZE);
			}
		}
	}
	else if (dataSize == 0) {
		//接続を終了するとき
		std::cout << "connection is lost" << std::endl;
	}
#ifdef _MSC_VER
	else if (WSAGetLastError() == WSAEWOULDBLOCK) {
		//clientがsendしていなかったときにおこるエラー(returnで良いかも)
	}
#endif
	else {
#ifdef _MSC_VER
		//接続エラーが起こった時
		std::cerr << "recv failed:" << WSAGetLastError() << std::endl;
#else
		if (errno == EAGAIN)
		{
			//非同期だとここを基本は通る
			return;
		}

		//接続エラーが起こった時
		std::cerr << "recv failed" << std::endl;

#endif

	}
}

void TCP_Routine::Update(std::vector<std::shared_ptr<BaseSocket>>& _clientList, std::unordered_map<int, std::vector<char>>& _recvDataMap, std::queue<std::pair<int, std::vector<char>>>& _recvDataQueList)
{
	std::list<int> deleteList;

	for (int i = 0; i < _clientList.size(); i++) {
		char buf[TCP_BUFFERSIZE];
		int socket = _clientList.at(i)->GetSocket();

		//データを受信した際はそのバイト数が入り切断された場合は0,ノンブロッキングモードでデータを受信してない間は-1がdataSizeに入る
		int dataSize = _clientList.at(i)->Recv(buf, TCP_BUFFERSIZE);

		if (dataSize > 0) {

			//受信データを格納
			int nowSize = _recvDataMap[socket].size();
			_recvDataMap[socket].resize(nowSize + dataSize);
			memcpy((char*)&_recvDataMap[socket][nowSize], &buf[0], dataSize);

			while (_recvDataMap[socket].size() > sizeof(int)) {

				int dataSize = 0;
				try {
					memcpy(&dataSize, &_recvDataMap[(B_SOCKET)socket][0], sizeof(int));

					//先頭パケットが想定しているよりも小さいまたは大きいパケットの場合は不正パケットとして解釈する。
					if (dataSize < 0 || dataSize > TCP_BUFFERSIZE - sizeof(int) - ENDMARKERSIZE) {
						//TODO 不正パケットとみなした場合パケットをすべて削除しているが何かいい手がないか考える
						_recvDataMap[(B_SOCKET)socket].clear();
						return;
					}

					//エンドマーカーの値が正常値かチェック
					if (memcmp(&_recvDataMap[(B_SOCKET)socket][dataSize + sizeof(int)], &ENDMARKER, ENDMARKERSIZE) != 0) {
						//TODO:不正パケットとみなした場合パケットをすべて削除しているが何かいい手がないか考える
						_recvDataMap[(B_SOCKET)socket].clear();
						return;
					}
				}
				catch (std::exception e) {
					std::cerr << "Exception Error at TCP_Routine::Update():" << e.what() << std::endl;

					//TODO:不正パケットなどで先頭データがintでmemcpyできなかった際はパケットをすべて削除しているが何かいい手がないか考える
					_recvDataMap[(B_SOCKET)socket].clear();
					return;
				}

				//受信データが一塊分あればキューリストに追加
				if (_recvDataMap[socket].size() > dataSize) {
					std::pair<B_SOCKET, std::vector<char>> addData;
					addData.first = socket;
					addData.second.resize(dataSize);
					memcpy(&addData.second[0], &_recvDataMap[socket][sizeof(int)], dataSize);
					_recvDataQueList.push(addData);
					_recvDataMap[socket].erase(_recvDataMap[socket].begin(), _recvDataMap[socket].begin() + dataSize + sizeof(int) + ENDMARKERSIZE);
				}
			}

		}
		else if (dataSize == 0) {
			//接続を終了するとき
			std::cout << "connection is lost" << std::endl;
			deleteList.push_back(i);
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
			deleteList.push_back(i);
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
				deleteList.push_back(i);
				break;
			}
			//接続エラーが起こった時
			std::cerr << "recv failed:" << errno << std::endl;
			deleteList.push_back(i);
#endif

		}
	}

	//切断されたソケット処理
	for (auto element : deleteList) {
		//受信データ削除
		int socket = _clientList.at(element)->GetSocket();

#ifdef _MSC_VER
		_recvDataMap[socket].erase(_recvDataMap[socket].begin(), _recvDataMap[socket].end());
#else
		_recvDataMap.erase(socket);
#endif

		//ソケット削除
		_clientList.erase(_clientList.begin() + element);

	}


}
