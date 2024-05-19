# cmakeバージョン指定
cmake_minimum_required(VERSION 3.13)

# ビルド対象ファイル検索
file(GLOB_RECURSE CPPLIST RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "./*.cpp")

# OpenSSLが見つからない場合
if(NOT OPENSSL_FOUND)
	add_library(
		OpenSocket
		STATIC
		Utility.cpp
		base/BaseSocket.cpp
		base/BaseClient.cpp
		base/BaseServer.cpp
		TCP_Client.cpp
		TCP_Server.cpp
		UDP_Client.cpp
		UDP_Server.cpp
	)
	return()
endif()

# OpenSSLが見つかった場合の処理
add_library(
	OpenSocket
	STATIC
	${CPPLIST}
)
