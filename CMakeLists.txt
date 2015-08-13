cmake_minimum_required(VERSION 2.6)
project(Brot)
enable_language(ASM_NASM)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -fopenmp -Wall -Wextra -std=gnu++0x -march=native")
set(CMAKE_ASM_NASM_FLAGS_DEBUG "${CMAKE_ASM_NSAM_FLAGS_DEBUG} -felf64 -g -F dwarf -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -fopenmp -D_GLIBCXX_PARALLEL -Wall -Wextra -std=gnu++0x -march=native")
set(CMAKE_ASM_NASM_FLAGS_RELEASE "${CMAKE_ASM_NSAM_FLAGS_RELEASE} -felf64")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

include_directories(include)

add_subdirectory(googletest)
include_directories(${gtest_SOURCE_DIR}/include)
enable_testing()

add_subdirectory(test)
