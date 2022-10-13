cmake_minimum_required(VERSION 3.13)

file(GLOB_RECURSE CPPLIST RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "./*.cpp")

add_library(
  OpenSocket
  STATIC
  ${CPPLIST}
)
