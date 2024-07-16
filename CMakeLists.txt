cmake_minimum_required(VERSION 3.5.0)
project(MiniSTL VERSION 0.1.0 LANGUAGES C CXX)

add_executable(MiniSTL alloc.cpp test.cpp test_vector.cpp)

find_package(GTest REQUIRED)