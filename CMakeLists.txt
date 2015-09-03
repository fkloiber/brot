cmake_minimum_required(VERSION 2.8.8)
project(Brot)
enable_language(ASM_NASM)

find_package(OpenMP REQUIRED)
find_package(MPI REQUIRED)
find_package(ZLIB REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -Wall -Wextra -Wno-literal-suffix -march=native -std=gnu++11")
set(CMAKE_ASM_NASM_FLAGS "${CMAKE_ASM_NASM_FLAGS} -felf64")

set(CMAKE_ASM_NASM_FLAGS_DEBUG "${CMAKE_ASM_NSAM_FLAGS_DEBUG} -g -F dwarf -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -D_GLIBCXX_PARALLEL")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

include_directories(include)
include_directories(${MPI_INCLUDE_PATH})
include_directories(${ZLIB_INCLUDE_DIRS})

add_subdirectory(googletest)
include_directories(${gtest_SOURCE_DIR}/include)
enable_testing()
add_subdirectory(test)

add_executable(brot src/brot.cpp src/options.cpp src/math/rand.nasm src/math/iterate.nasm)
target_link_libraries(brot ${MPI_LIBRARIES} ${ZLIB_LIBRARIES})

set_target_properties(brot PROPERTIES COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")

set_target_properties(brot PROPERTIES LINK_FLAGS "${MPI_LINK_FLAGS}")
