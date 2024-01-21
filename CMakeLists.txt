# CMakeのバージョンを設定
cmake_minimum_required(VERSION 3.13)

# プロジェクト名と使用する言語を設定
project(OpenSocket)

#C++バージョン指定
add_definitions( -std=c++11 -Wall )

#Libraryパス一覧取得
file(GLOB LIBRARY_PAS_LIST RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "src/Library/*")
message(STATUS "source_dir: ${CMAKE_CURRENT_SOURCE_DIR}")

#subdirectoryの追加
foreach(ITEM ${LIBRARY_PAS_LIST})
	add_subdirectory(${ITEM})
endforeach()

#ライブラリのディレクトリ名取得
file(GLOB LIBRARY_LIST RELATIVE "${CMAKE_SOURCE_DIR}/src/Library/" "src/Library/*")

#実行ファイルの設定
add_executable(OpenSocket.out  ${PROJECT_SOURCE_DIR}/src/sample.cpp)

#ライブラリのリンク設定(opensslが見つかった場合)
get_property(OPENSSL_ENABLED GLOBAL PROPERTY OPENSSL_ENABLED)

if(OPENSSL_ENABLED)
	# プリプロセッサ識別子追加
	target_compile_definitions(OpenSocket.out PRIVATE OPENSSL_ENABLED)
else()
	message(WARNING "OpenSSL not found. Building without OpenSSL support.")
endif()

target_link_libraries(
	OpenSocket.out
	PRIVATE
	${LIBRARY_LIST}
	${Boost_LIBRARIES}
)