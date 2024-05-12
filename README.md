# OpenSocket
## 概要
windows(VC++)環境とLinux等の他環境で同様のソースコードを使用しソケット通信を簡略化し使用するためのライブラリです。

## 必要なミドルウェア(制作環境)
- cmake(3.28.3)
- gcc/g++(12.0.5)
- openssl(3.1.3)...暗号化通信をするために必要だが、なくても一部サンプルビルド可能

## ビルドについて
以下コマンドによりビルドされます。
実行サンプル,main.cppについてはbuildファイルが生成されそこに.outの拡張子で実行ファイルが生成されるので使用したいものを選んで実行ください。
```sh
$ cmake -S . -B build
$ cmake --build build

// 実行ファイルの実行方法(*は任意のファイル名に置換)
$ ./build/*.out
```
※windowsマシーン上でテストする際の注意点
wsl2上でserverを実行しclient側をwindowsで動作させるとudpの通信をする際client側で127.0.0.1を指定すると通信が届かない
windowsでnetstatで確認すると解放されていないのがわかる
対策として以下コマンドよりwsl2のipを確認し確認できたipをclientで指定する
```sh
$ ip a show dev eth0
```

## ライブラリの詳細について
- [OpenSocket](./src/Library/OpenSocket/README.md)
- [WrapperOpenSSL](./src/Library/WrapperOpenSSL/README.md)