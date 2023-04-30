#ifndef OPENSOCKET_UTILITY_h
#define OPENSOCKET_UTILITY_h

/**
*  @brief ファイルディスクリプタを引数として渡すことでselect関数を実行する
* 
*  @param _fds		selectで監視するファイルディスクリプタ
**/
void OpenSocket_Select(fd_set* _fds, int _maxfds = -1);

#endif