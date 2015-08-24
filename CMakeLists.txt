cmake_minimum_required(VERSION 2.8.8)
project(Brot)
enable_language(ASM_NASM)

find_package(OpenMP REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -Wall -Wextra -march=native -std=gnu++11")
set(CMAKE_ASM_NASM_FLAGS "${CMAKE_ASM_NASM_FLAGS} -felf64")

set(CMAKE_ASM_NASM_FLAGS_DEBUG "${CMAKE_ASM_NSAM_FLAGS_DEBUG} -g -F dwarf -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -D_GLIBCXX_PARALLEL")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

include_directories(include)

add_subdirectory(googletest)
include_directories(${gtest_SOURCE_DIR}/include)
enable_testing()
add_subdirectory(test)
