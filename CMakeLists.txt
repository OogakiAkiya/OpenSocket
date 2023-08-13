# CMakeのバージョンを設定
cmake_minimum_required(VERSION 3.13)

# プロジェクト名と使用する言語を設定
project(OpenSocket)

#C++バージョン指定
add_definitions( -std=c++11 -Wall )

#Libraryパス一覧取得
file(GLOB LIBRARY_PAS_LIST RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "src/Library/*")

#subdirectoryの追加
foreach(ITEM ${LIBRARY_PAS_LIST})
   add_subdirectory(${ITEM})
endforeach()

#ライブラリのディレクトリ名取得
file(GLOB LIBRARY_LIST RELATIVE "${CMAKE_SOURCE_DIR}/src/Library/" "src/Library/*")

#opensslのライブラリ検索
find_package(OpenSSL REQUIRED)

#実行ファイルの設定
add_executable(OpenSocket.out  ${PROJECT_SOURCE_DIR}/src/sample.cpp)

#ライブラリのリンク設定(opensslが見つかった場合)
if(OpenSSL_FOUND)
   #読み込んだOpenSSLの情報を出力
   message(STATUS "OpenSSL Version: ${OPENSSL_VERSION}")
   message(STATUS "OpenSSL Library Path: ${OPENSSL_LIBRARIES}")
   #プログラム側にOPENSSLを使用していることを表すプリプロセッサディレクティブを追加
   add_definitions(-DUSE_OPENSSL)
   
   target_link_libraries(
      OpenSocket.out
      PRIVATE
      ${LIBRARY_LIST}
      ${Boost_LIBRARIES}
      OpenSSL::SSL
      OpenSSL::Crypto
   )
else()
   message(WARNING "OpenSSL not found. Building without OpenSSL support.")
   target_link_libraries(
      OpenSocket.out
      PRIVATE
      ${LIBRARY_LIST}
      ${Boost_LIBRARIES}
   )
endif()

