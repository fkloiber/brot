cmake_minimum_required(VERSION 2.6)
project(Brot)
enable_language(ASM_NASM)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -fopenmp -Wall -Wextra -std=gnu++0x -march=native")
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas -Wall -Wextra -std=gnu++0x -march=native")
#SET(CMAKE_ASM_NASM_FLAGS "${CMAKE_ASM_NSAM_FLAGS} -f elf64 -g -F dwarf")

include_directories(include)

add_subdirectory(googletest)
include_directories(${gtest_SOURCE_DIR}/include)
enable_testing()

add_subdirectory(test)
