﻿# cmakeバージョン指定
cmake_minimum_required(VERSION 3.13)

# ビルド対象ファイル検索
file(GLOB_RECURSE CPPLIST RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "./*.cpp")

add_library(
	WrapperOpenSSL
	STATIC
	${CPPLIST}
)

# opensslをリンク
target_link_libraries(
	WrapperOpenSSL 
	PRIVATE
	OpenSSL::SSL 
	OpenSSL::Crypto
)