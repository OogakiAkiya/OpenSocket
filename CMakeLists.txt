# CMakeのバージョンを設定
cmake_minimum_required(VERSION 3.13)

# プロジェクト名
project(OpenSocket)

# C++バージョン指定
add_definitions( -std=c++11 -Wall)

# ビルドに必要なopensslの検出
find_package(OpenSSL 3.0 REQUIRED)
if(OPENSSL_FOUND)
	message(STATUS "OpenSSL Version: ${OPENSSL_VERSION}")
	message(STATUS "OpenSSL Library Path: ${OPENSSL_LIBRARIES}")

	# プリプロセッサ識別子追加
    add_definitions( -DOPENSSL_ENABLED)
endif()

# 非暗号化サンプルファイル
set(SAMPLE_FILES tcp_client_Asynchronous)
list(APPEND SAMPLE_FILES tcp_client_Synchronization)
list(APPEND SAMPLE_FILES tcp_server_Asynchronous)
list(APPEND SAMPLE_FILES tcp_server_Synchronization)
list(APPEND SAMPLE_FILES udp_client_Asynchronous)
list(APPEND SAMPLE_FILES udp_client_Synchronization)
list(APPEND SAMPLE_FILES udp_server_Asynchronous)
list(APPEND SAMPLE_FILES udp_server_Synchronization)

# 暗号化サンプル
set(CIPHER_SAMPLE_FILES tcp_cipher_client)
list(APPEND CIPHER_SAMPLE_FILES tcp_cipher_server)


# 暗号化ライブラリも含めたビルド実施
if(OPENSSL_FOUND)
	# ライブラリの追加
	add_subdirectory(src/Library/OpenSocket)
	add_subdirectory(src/Library/WrapperOpenSSL)

	#メイン実行ファイルの設定
	add_executable(main.out  ${PROJECT_SOURCE_DIR}/src/main.cpp)
	target_link_libraries(main.out PRIVATE OpenSocket WrapperOpenSSL)

	# 非暗号化サンプル実行ファイルの設定
	foreach(FILE ${SAMPLE_FILES})
		add_executable(${FILE}.out  ${PROJECT_SOURCE_DIR}/src/sample/${FILE}.cpp)
		target_link_libraries(${FILE}.out PRIVATE OpenSocket WrapperOpenSSL)
	endforeach()

	# 暗号化サンプル実行ファイルの設定
	foreach(FILE ${CIPHER_SAMPLE_FILES})
		add_executable(${FILE}.out  ${PROJECT_SOURCE_DIR}/src/sample/${FILE}.cpp)
		target_link_libraries(${FILE}.out PRIVATE OpenSocket WrapperOpenSSL)
	endforeach()

	else()
	
	# ライブラリの追加
	add_subdirectory(src/Library/OpenSocket)

	#メイン実行ファイルの設定
	add_executable(main.out  ${PROJECT_SOURCE_DIR}/src/main.cpp)
	target_link_libraries(main.out PRIVATE OpenSocket)

	# 非暗号化サンプル実行ファイルの設定
	foreach(FILE ${SAMPLE_FILES})
		add_executable(${FILE}.out  ${PROJECT_SOURCE_DIR}/src/sample/${FILE}.cpp)
		target_link_libraries(${FILE}.out PRIVATE OpenSocket WrapperOpenSSL)
	endforeach()
endif()

