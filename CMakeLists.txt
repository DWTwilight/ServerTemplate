cmake_minimum_required(VERSION 3.12.0)

project(ServerTemplate VERSION 1.0.0)

# Enable C++11
set(CMAKE_CXX_STANDARD 11)

# external libraries
add_subdirectory(external/libuv)

add_subdirectory(include)

add_subdirectory(examples)

enable_testing()
add_subdirectory(test)