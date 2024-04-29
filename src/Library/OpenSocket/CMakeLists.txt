cmake_minimum_required(VERSION 3.13)

file(GLOB_RECURSE CPPLIST RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "./*.cpp")

add_library(
	OpenSocket
	STATIC
	${CPPLIST}
)
# ライブラリのリンク設定(opensslが見つかった場合)
get_property(OPENSSL_ENABLED GLOBAL PROPERTY OPENSSL_ENABLED)

if(OPENSSL_ENABLED)
	# プリプロセッサ識別子追加
	target_compile_definitions(OpenSocket PUBLIC OPENSSL_ENABLED)
endif()
