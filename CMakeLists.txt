cmake_minimum_required(VERSION 2.6)
project(Brot)
enable_language(ASM_NASM)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=gnu++0x -mavx")

include_directories(include)

add_subdirectory(googletest)
include_directories(${gtest_SOURCE_DIR}/include)
enable_testing()

add_subdirectory(test)
