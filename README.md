# OpenSocket
## build
./src/sample.cppのコメントアウトを外しserverとclientそれぞれでbuildを実施し実行ファイルを作成する

```sh
$ cmake -S . -B build
$ cmake --build build
$ ./build/OpenSocket.out
```

※windowsマシーン上でテストする際の注意点
wsl2上でserverを実行しclient側をwindowsで動作させるとudpの通信をする際client側で127.0.0.1を指定すると通信が届かない
windowsでnetstatで確認すると解放されていないのがわかる
対策として以下コマンドよりwsl2のipを確認し確認できたipをclientで指定する
```sh
$ ip a show dev eth0
```
## パケット図
![TCP Base packet image](readme_img/tcp_basepacket.png)
