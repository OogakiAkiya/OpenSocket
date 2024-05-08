#include "OpenSocket_Def.h"
#include "OpenSocket_STD.h"

#include "Utility.h"

namespace OpenSocket {
void OpenSocket_Select(fd_set* _fds, int _maxfds) {
#ifdef _MSC_VER
   // winsockでは第一引数は無視される
   select(0, _fds, NULL, NULL, NULL);

#else
   // 最大のファイルディスクリプタ(fd)が判明している場合
   // ※Unix系であれば0は標準入力,1は標準出力,2は標準エラー出力でfdが予約されているため、3が最低値のはずだがプラットフォームに依存するため負の値が入っていればfdの最大値が判明していないものとする。
   if (_maxfds >= 0) {
      select(_maxfds + 1, _fds, NULL, NULL, NULL);
      return;
   }

   // 最大のファイルディスクリプタを取得(ファイルディスクリプタのとりうる範囲を探索)
   int maxfds = -1;
   for (int i = 0; i < FD_SETSIZE; i++) {
      if (FD_ISSET(i, _fds)) { maxfds = i; }
   }

   if (maxfds < 0) return;
   select(maxfds + 1, _fds, NULL, NULL, NULL);

#endif
}

std::string GenerateRandomStringNoSymbol(int _len) {
   std::string charList = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

   // デバイスから乱数の取得
   std::random_device rd;

   // シード値の設定
   std::mt19937 mt(rd());

   // 乱数の範囲指定
   std::uniform_int_distribution<int> rand(0, charList.size() - 1);

   // 乱数を生成し文字列リストから文字を取得し文字列の生成
   std::string output;
   for (int i = 0; i < _len; ++i) { output += charList[rand(mt)]; }
   return output;
}

}  // namespace OpenSocket
