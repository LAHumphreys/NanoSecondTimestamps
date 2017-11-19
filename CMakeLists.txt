cmake_minimum_required(VERSION 3.8.2)
project(UtilTime)

add_library(Time STATIC src/util_time.cpp include/util_time.h)
target_include_directories(Time PUBLIC include)

enable_testing()
find_package(GTest REQUIRED)

add_executable(timeTests test/util_time_tests.cpp)
target_link_libraries(timeTests Time GTest::GTest GTest::Main)

