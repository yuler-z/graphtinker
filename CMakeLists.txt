
project(graphtinker)
cmake_minimum_required(VERSION 3.6)


# 编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3 -fopenmp")

# 头文件目录
include_directories("${PROJECT_SOURCE_DIR}/include")

# 添加子目录
add_subdirectory("src")
add_subdirectory("test")

#


