# OpenSocket
## 概要
windows(VC++)環境とLinux等の他環境で同様のソースコードを使用しソケット通信を簡略化し使用するためのライブラリです。

## 必要なミドルウェア(制作環境)
- cmake(3.28.3)
- gcc/g++(12.0.5)
- visual studio 2022
- openssl(3.1.3)...暗号化通信をするために必要だが、なくても一部サンプルビルド可能


## ビルドについて
### Linuxコマンドラインによるビルド方法について
以下コマンドによりビルドされます。  
実行サンプルmain.cpp,サンプルディレクトリ配下のコードについては、実行後buildファイルが生成されそこに.outの拡張子で実行ファイルが生成されるので使用したいものを選んで実行ください。
```sh
// ビルドコマンド
$ cmake -S . -B build
$ cmake --build build

// 実行ファイルの実行方法(*は任意のファイル名に置換)
$ ./build/*.out
```

### visual studioでのビルド方法(GUI)
#### 前準備 (OpenSSLが不要の場合はここはスキップ)
1. visual studioをインストール
2. Strawberry Perlをインストール
3. OpenSSLのサイトより3.0以上のOpenSSLをダウンロードし、OpenSSLをビルドする。
    
    ビルド手順は以下の通り
    ```
    $ perl Configure VC-WIN64A --prefix=[インストール先ディレクトリ] --openssldir=[opensslのソースコード] no-asm no-shared
    $ nmake
    $ nmake test
    $ nmake install
    ```

#### visual studioのプロジェクトへの設定追加
1. Visual Studioのプロパティへインクルードディレクトリの設定追加
    visual studioでプロジェクトを開き 以下の通り移動  
    `プロジェクト>[Project名]のプロパティ>構成プロパティ>C/C++>全般>追加のインクルードディレクトリ`  
    プルダウンより編集でOpenSSLをビルドしたディレクトリ配下の『include』ディレクトリまでのパスを指定

2. Visual Studioのライブラリディレクトリへのパスを設定追加
    visual studioでプロジェクトを開き 以下の通り移動  
    `プロジェクト>[Project名]のプロパティ>構成プロパティ>リンカー>全般>追加のライブラリディレクトリ`  
    プルダウンより編集でOpenSSLをビルドしたディレクトリ配下の『lib』ディレクトリまでのパスを指定

3. Visual Studioの依存ファイルを設定追加
    visual studioでプロジェクトを開き 以下の通り移動      
    `プロジェクト>[Project名]のプロパティ>構成プロパティ>リンカー>入力>追加の依存ファイル`  
    プルダウンより編集で『libcrypto.lib』と『libssl.lib』を追加

4. Visual Studioのプリプロセッサ設定追加(OpenSSLが不要の場合はここはスキップしてよい)
    visual studioでプロジェクトを開き 以下の通り移動      
    `プロジェクト>[Project名]のプロパティ>構成プロパティ>C/C++>プリプロセッサ>プリプロセッサの定義`  
    プルダウンより編集で『OPENSSL_ENABLED』を追加

### visual studioでのビルド方法(CLI)
`Developer Command Prompt for VS [version名]`のコマンドプロンプト内で以下コマンドを実行することにビルドされます。  
もし、OpenSSLを使用する場合は前項の『visual studioでのビルド方法(GUI)』より『前準備』を実行しておいてください  
実行サンプルmain.cpp,サンプルディレクトリ配下のコードについては、実行後buildファイルが生成されそこに.exeの拡張子で実行ファイルが生成されるので使用したいものを選んで実行ください。
```sh
//　カレントディレクトリへの移動
$ cd 『ビルドプロジェクトのディレクトリ』

// OpenSSLへのパスを環境変数に追加(暗号化通信をしない場合はこの設定は不要です。)
$ set CMAKE_PREFIX_PATH=[OpenSSLをビルドしたディレクトリへのパス]

// ビルドコマンド
$ cmake -S . -B build
$ cmake --build build

// 実行ファイルの実行方法(*は任意のファイル名に置換)
$ .\build\Debug\*.out.exe
```

### windowsマシーン上でテストする際の注意点
wsl2上でserverを実行しclient側をwindowsで動作させるとudpの通信をする際client側で127.0.0.1を指定すると通信が届かない  
windowsでnetstatで確認すると解放されていないのがわかる  
対策として以下コマンドよりwsl2のipを確認し確認できたipをclientで指定する
```sh
$ ip a show dev eth0
```

## ライブラリの詳細について
- [OpenSocket](./src/Library/OpenSocket/README.md)
- [WrapperOpenSSL](./src/Library/WrapperOpenSSL/README.md)