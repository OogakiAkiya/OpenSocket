#ifndef OPENSOCKET_UTILITY_H
#define OPENSOCKET_UTILITY_H

namespace OpenSocket {
/**
 *  @brief ファイルディスクリプタを引数として渡すことでselect関数を実行する
 *
 *  @param _fds   selectで監視するファイルディスクリプタ
 **/
void OpenSocket_Select(fd_set* _fds, int _maxfds = -1);

/**
 *  @brief 記号なしランダム文字列を生成する(記号なし)
 *
 *  @param _len   生成する文字列の長さ
 **/
std::string GenerateRandomStringNoSymbol(int _len);

}  // namespace OpenSocket
#endif