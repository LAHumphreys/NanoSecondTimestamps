#
# Project Definition
#
cmake_minimum_required(VERSION 3.8.2)
project(UtilTime)

#
# Exported Library
#
add_library(Time STATIC src/util_time.cpp include/util_time.h)
target_include_directories(Time PUBLIC
    $<BUILD_INTERFACE:${UtilTime_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
set_target_properties(Time PROPERTIES
    PUBLIC_HEADER ${UtilTime_SOURCE_DIR}/include/util_time.h
)

#
# Benchmark Utility
#
add_executable(benchmark src/benchmark.cpp)
target_link_libraries(benchmark Time)

#
# Test Configuration
#
enable_testing()
find_package(GTest REQUIRED)

add_executable(timeTests test/util_time_tests.cpp)
target_link_libraries(timeTests Time GTest::GTest GTest::Main)

#
# Installation instructions
#
install(TARGETS Time EXPORT UtilTimeTargets
    ARCHIVE  DESTINATION lib
    INCLUDES DESTINATION include
    PUBLIC_HEADER DESTINATION include
)

install (EXPORT UtilTimeTargets
    FILE         UtilTimeTargets.cmake
    NAMESPACE    UtilTime::
    DESTINATION  lib/cmake/UtilTime
)

#
# Define our package version
#
include(CMakePackageConfigHelpers)
write_basic_package_version_file("UtilTimeConfigVersion.cmake"
    VERSION       0.1
    COMPATIBILITY SameMajorVersion
)
install (
    FILES       ${UtilTime_BINARY_DIR}/UtilTimeConfigVersion.cmake
    DESTINATION lib/cmake/UtilTime
)

#
# Finally export the Config which CMake will use
# when looking for matching libraries to link against.
#
install (
    FILES       UtilTimeConfig.cmake
    DESTINATION lib/cmake/UtilTime
)
