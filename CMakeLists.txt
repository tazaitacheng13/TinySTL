cmake_minimum_required(VERSION 3.5.0)
project(MiniSTL VERSION 0.1.0 LANGUAGES C CXX)

file(GLOB TEST_SOURCES test/*.cpp)
add_executable(MiniSTL alloc.cpp test.cpp ${TEST_SOURCES})

# 添加gtest路径
set(GTEST_ROOT /usr/local/)
set(GTEST_LIB_DIR ${GTEST_ROOT}/lib)
set(GTEST_INCLUDE_DIR ${GTEST_ROOT}/include)

# 查找 Google Test 库
find_library(GTEST_LIB gtest HINTS ${GTEST_LIB_DIR})
find_library(GTEST_MAIN_LIB gtest_main HINTS ${GTEST_LIB_DIR})

# 包含 Google Test 头文件路径
include_directories(${GTEST_INCLUDE_DIR})

# 链接 Google Test 库
target_link_libraries(MiniSTL ${GTEST_LIB} ${GTEST_MAIN_LIB} pthread)