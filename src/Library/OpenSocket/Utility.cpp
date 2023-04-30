#include"OpenSocket_STD.h"
#include"OpenSocket_Def.h"
#include"Utility.h"

void OpenSocket_Select(fd_set* _fds,int _maxfds)
{
	
#ifdef _MSC_VER
	//winsockでは第一引数は無視される
	select(0, _fds, NULL, NULL, NULL);

#else
	//最大のファイルディスクリプタが判明している場合
	if (_maxfds != -1) {
		select(_maxfds + 1, _fds, NULL, NULL, NULL);
		return;
	}

	//最大のファイルディスクリプタを取得(ファイルディスクリプタのとりうる範囲を探索)
	int maxfds = -1;
	for (int i = 0; i < FD_SETSIZE; i++) {
		if (FD_ISSET(i,_fds)) {
			maxfds = i;
		}
	}
	select(maxfds + 1, _fds, NULL, NULL, NULL);

#endif


}
