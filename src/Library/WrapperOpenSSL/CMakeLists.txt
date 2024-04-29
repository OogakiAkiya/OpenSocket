cmake_minimum_required(VERSION 3.13)

file(GLOB_RECURSE CPPLIST RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "./*.cpp")

# opensslのライブラリ検索
find_package(OpenSSL REQUIRED)
add_library(
	WrapperOpenSSL
	STATIC
	${CPPLIST}
)

if(OpenSSL_FOUND)
	# 読み込んだOpenSSLの情報を出力
	message(STATUS "OpenSSL Version: ${OPENSSL_VERSION}")
	message(STATUS "OpenSSL Library Path: ${OPENSSL_LIBRARIES}")

	# OPENSSLが有効なことを変数に格納
	set(OPENSSL_ENABLED ${OpenSSL_FOUND} CACHE INTERNAL "OpenSSL found in WrapperOpenSSL")

	target_link_libraries(
		WrapperOpenSSL 
		PUBLIC 
		OpenSSL::SSL 
		OpenSSL::Crypto
	)

endif()
