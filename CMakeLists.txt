# CMakeのバージョンを設定
cmake_minimum_required(VERSION 3.13)

# プロジェクト名と使用する言語を設定
project(OpenSocket)

# C++バージョン指定
add_definitions( -std=c++11 -Wall )

# サンプル用の実行ファイル変数作成
set(SAMPLE_FILES tcp_client_Asynchronous)
list(APPEND SAMPLE_FILES tcp_client_Synchronization)
list(APPEND SAMPLE_FILES tcp_server_Asynchronous)
list(APPEND SAMPLE_FILES tcp_server_Synchronization)
list(APPEND SAMPLE_FILES udp_client_Asynchronous)
list(APPEND SAMPLE_FILES udp_client_Synchronization)
list(APPEND SAMPLE_FILES udp_server_Asynchronous)
list(APPEND SAMPLE_FILES udp_server_Synchronization)

# Libraryパス一覧取得
file(GLOB LIBRARY_PATH_LIST RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "src/Library/*")
message(STATUS "source_dir: ${CMAKE_CURRENT_SOURCE_DIR}")

# subdirectoryの追加
foreach(ITEM ${LIBRARY_PATH_LIST})
	add_subdirectory(${ITEM})
endforeach()

# ライブラリのディレクトリ名取得
file(GLOB LIBRARY_LIST RELATIVE "${CMAKE_SOURCE_DIR}/src/Library/" "src/Library/*")

# メイン実行ファイルの設定
add_executable(OpenSocket.out  ${PROJECT_SOURCE_DIR}/src/main.cpp)

# サンプル実行ファイルの設定
foreach(FILE ${SAMPLE_FILES})
	add_executable(${FILE}.out  ${PROJECT_SOURCE_DIR}/src/sample/${FILE}.cpp)
endforeach()

# ライブラリのリンク設定(opensslが見つかった場合)
get_property(OPENSSL_ENABLED GLOBAL PROPERTY OPENSSL_ENABLED)

if(OPENSSL_ENABLED)
	# プリプロセッサ識別子追加
	target_compile_definitions(OpenSocket.out PRIVATE OPENSSL_ENABLED)

	# サンプル実行ファイル用
	foreach(FILE ${SAMPLE_FILES})
		target_compile_definitions(${FILE}.out PRIVATE OPENSSL_ENABLED)
	endforeach()
	
else()
	message(WARNING "OpenSSL not found. Building without OpenSSL support.")
endif()

target_link_libraries(OpenSocket.out PRIVATE ${LIBRARY_LIST})

# サンプル実行ファイル用
foreach(FILE ${SAMPLE_FILES})
	target_link_libraries(${FILE}.out PRIVATE ${LIBRARY_LIST})
endforeach()
