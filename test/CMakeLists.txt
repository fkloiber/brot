cmake_minimum_required(VERSION 2.6)
project(Brot-test)

add_executable(math_log math_log.cpp ../src/math/linux-x86-64-log.nasm)
target_link_libraries(math_log gtest_main)
add_test(Math_log math_log)

add_executable(rand rand.cpp ../src/math/rand.nasm)
target_link_libraries(rand gtest_main)
add_test(Rand rand)

add_executable(iterate iterate.cpp ../src/math/iterate.nasm)
target_link_libraries(iterate gtest_main)
add_test(Iterate iterate)

add_executable(math_log_timing math_log_timing.cpp ../src/math/linux-x86-64-log.nasm)
