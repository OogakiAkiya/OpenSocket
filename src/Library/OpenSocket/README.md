
# OpenSocket

## 非暗号化通信について
### 概要
極力単純に通信するための通信クラス
同期,非同期の両方に対応しており、最低限インスタンス生成後Updateを呼び出すだけで使用が可能。
GetRecvDataを使用すると受信したデータを受け取れ、Send~を使用するコトでデータを送信できる。

```sh
--TCP パケット全体構成--
ボディーデータサイズ...4byte
ボディーデータ...最大1MByte(TCP_BODY_MAX_SIZEで定数指定しているが任意の値に修正可能,必要数程度の定数に修正しよう)
エンドマーカー...6byte

--UDP パケット全体構成--
シーケンス番号...4byte
ボディーデータ...最大1000Byte(UDP_BODY_MAX_SIZEで定数指定しているが任意の値に修正可能,正しMTUを意識したサイズ感にする)
```

### 該当サンプル
- tcp_client_Asynchronous.cpp
- tcp_client_Synchronization.cpp
- tcp_server_Asynchronous.cpp
- tcp_server_Synchronization.cpp
- udp_client_Asynchronous.cpp
- udp_client_Synchronization.cpp
- udp_server_Asynchronous.cpp
- udp_server_Synchronization.cpp

## 暗号化通信のについて(TCP)
### 概要
基本的な通信部分はTCP_Server,TCP_Clientと同じヘッダーを使ってパケットのやり取りをする。
ただ、さらに下層で暗号化通信のためのやり取りをするヘッダーが含まれている
基本的にはサンプル通りだがKeyChangeConnectionStartを呼び出すことで鍵交換を自動で行ってくれる。
その後は他サンプル同様にUpdate関数の呼び出しで諸々の処理が実施される。

```sh
--TCP パケット全体構成--
ボディーデータサイズ...4byte
パケット大分類...1byte
パケット中分類...1byte
オプション1...1byte
オプション2...1byte
ボディーデータ...最大1MByte(TCP_BODY_MAX_SIZEで定数指定しているが任意の値に修正可能,必要数程度の定数に修正しよう)
エンドマーカー...6byte
```

### 該当サンプル
- tcp_cipher_client.cpp
- tcp_cipher_server.cpp

### 注意事項
- openssl3.0以上がビルド環境にインストールされている必要がある。
- 送信処理にはCipherSendOnlyClient,CiphserSendServerを使用する必要がある
- 引数に指定する値は暗号化通信プロコトコルヘッダーの解説の項を参考に設定する必要がある。

### 鍵交換ヘッダー解説
第一セグメント(1byte)...通信パケットの大分類(現状暗号の有無)
第二セグメント(1byte)...通信パケットの中分類(やり取りの詳細など)
オプション1(1byte)...必要に応じて付与されるオプション情報(必要なければパディング用データを挟む)
オプション2(1byte)...必要に応じて付与されるオプション情報(必要なければパディング用データを挟む)

### 暗号化通信プロコトコルヘッダーの解説(基本的に上から順に実施されていく)
```sh
# 鍵要求
client -> serverへRSA鍵の発行要求
== ヘッダー設定値 ==
第一セグメント: CIPHER_PACKET
第二セグメンt: CIPHER_PACKET_CREATE_PUBLICKEY_REQUEST
オプション情報1: CIPHER_PACKET_RSA_KEY_SIZE_2048...RSAの鍵サイズ指定(現状2048ビットのみ対応,2048ビット以下になる用にするのは鍵強度的に非推奨)
オプション情報2: PADDING_DATA

# 鍵の再度要求(必要に応じて)
※現状未実装
client -> serverへRSA鍵の再発行要求
== ヘッダー設定値 ==
第一セグメント: CIPHER_PACKET
第二セグメンt: CIPHER_PACKET_RECREATE_PUBLICKEY_REQUEST
オプション情報1: CIPHER_PACKET_RSA_KEY_SIZE_2048...RSAの鍵サイズ指定(現状2048ビットのみ対応,2048ビット以下になる用にするのは鍵強度的に非推奨)
オプション情報2: PADDING_DATA

# 公開鍵の送付
server -> clientへRSA公開鍵の送付
== ヘッダー設定値 ==
第一セグメント: CIPHER_PACKET
第二セグメンt: CIPHER_PACKET_SEND_PUBLICKEY
オプション情報1: PADDING_DATA
オプション情報2: PADDING_DATA

# 共通鍵の登録要求
client -> serverへAES鍵の再発行要求
サーバから送られてきたRSA公開鍵を使用しクライアントで生成したAES鍵を送付しサーバに共通鍵登録を依頼
== ヘッダー設定値 ==
第一セグメント: CIPHER_PACKET
第二セグメンt: CIPHER_PACKET_REGISTRY_SHAREDKEY_REQUEST
オプション情報1: PADDING_DATA
オプション情報2: PADDING_DATA

# 共通鍵登録完了通知
server -> clientへ共通鍵の正常登録完了の連絡
== ヘッダー設定値 ==
第一セグメント: CIPHER_PACKET
第二セグメンt: CIPHER_PACKET_REGISTRIED_SHAREDKEY
オプション情報1: PADDING_DATA
オプション情報2: PADDING_DATA

# 共通鍵チェックリクエスト
client -> serverへ共通鍵が正常に登録できたかのチェックをリクエスト
== ヘッダー設定値 ==
第一セグメント: CIPHER_PACKET
第二セグメンt: CIPHER_PACKET_CHECK_SHAREDKEY_REQUEST
オプション情報1: PADDING_DATA
オプション情報2: PADDING_DATA

# チェックデータの送付
client -> serverへ共通鍵が正常に登録できたかのチェックをリクエスト
== ヘッダー設定値 ==
第一セグメント: CIPHER_PACKET
第二セグメンt: CIPHER_PACKET_SEND_CHECKDATA
オプション情報1: PADDING_DATA
オプション情報2: PADDING_DATA

# 共通鍵チェックリクエスト
server -> clientへサーバ側で生成したチェックデータを共通鍵で暗号化したデータを送付
== ヘッダー設定値 ==
第一セグメント: CIPHER_PACKET
第二セグメンt: CIPHER_PACKET_CHECK_SHAREDKEY_REQUEST
オプション情報1: PADDING_DATA
オプション情報2: PADDING_DATA

# チェックデータ正誤チェックリクエスト
client -> serverへ受け取ったチェックデータを共通鍵で復号しMD5でハッシュ化させたデータを共通鍵で送付
== ヘッダー設定値 ==
第一セグメント: CIPHER_PACKET
第二セグメンt: CIPHER_PACKET_CHECK_CHECKDATA_REQUEST
オプション情報1: PADDING_DATA
オプション情報2: PADDING_DATA

# 共通鍵チェックリクエストの成功
server -> clientへ受け取ったデータを共通鍵で復号しハッシュ値がサーバ側で生成した値と同値の場合送付
== ヘッダー設定値 ==
第一セグメント: CIPHER_PACKET
第二セグメンt: CIPHER_PACKET_CHECK_SUCCESS
オプション情報1: PADDING_DATA
オプション情報2: PADDING_DATA

# 共通鍵チェックリクエストの失敗
server -> clientへ受け取ったデータを共通鍵で復号しハッシュ値がサーバ側で生成した値と同値ではない場合送付(clientはRSA鍵交換から再度実施開始)
== ヘッダー設定値 ==
第一セグメント: CIPHER_PACKET
第二セグメンt: CIPHER_PACKET_CHECK_FAILD
オプション情報1: PADDING_DATA
オプション情報2: PADDING_DATA

# 暗号化データの通信
上記の鍵交換のやり取りが完了した場合に使用する。
serverとclientの双方で使用し、共通鍵で暗号化したデータを送ったことを示す。
== ヘッダー設定値 ==
第一セグメント: CIPHER_PACKET
第二セグメンt: CIPHER_PACKET_SEND_DATA
オプション情報1: PADDING_DATA
オプション情報2: PADDING_DATA
```



## 暗号化通信のについて(UDP)
### 概要
基本的な通信部分はUDP_Server,UDP_Clientと同じヘッダーを使ってパケットのやり取りをする。
ただ、さらに下層で暗号化通信のためのやり取りをするヘッダーが含まれている
基本的にはサンプル通りだがKeyChangeConnectionStartを呼び出すことで鍵交換を自動で行ってくれる。
その後は他サンプル同様にUpdate関数の呼び出しで諸々の処理が実施される。

```sh
--TCP パケット全体構成--
シーケンス番号...4byte
パケット大分類...1byte
パケット中分類...1byte
オプション1...1byte
オプション2...1byte
ボディーデータ...最大1000Byte(UDP_BODY_MAX_SIZEで定数指定しているが任意の値に修正可能,正しMTUを意識したサイズ感にする)
```
### 該当サンプル
- udp_cipher_client.cpp
- udp_cipher_server.cpp

### 注意事項
- openssl3.0以上がビルド環境にインストールされている必要がある。
- 送信処理にはCipherSendOnlyClient,CiphserSendServerを使用する必要がある
- 引数に指定する値は暗号化通信プロコトコルヘッダーの解説の項を参考に設定する必要がある。

### 鍵交換ヘッダー解説
第一セグメント(1byte)...通信パケットの大分類(現状暗号の有無)
第二セグメント(1byte)...通信パケットの中分類(やり取りの詳細など)
オプション1(1byte)...必要に応じて付与されるオプション情報(必要なければパディング用データを挟む)
オプション2(1byte)...必要に応じて付与されるオプション情報(必要なければパディング用データを挟む)
