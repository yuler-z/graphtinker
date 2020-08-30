project(graphtinker)
cmake_minimum_required(VERSION 3.6)


# 编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3 -fopenmp")

# Release模式
SET(CMAKE_BUILD_TYPE "Release")
if (NOT CMAKE_BUILD_TYPE STREQUAL Debug)
    add_definitions(
        -DNDEBUG
    )
endif ()


list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# glog
find_package(glog)
include_directories(${GLOG_INCLUDE_PATH})


# 头文件目录
include_directories("${PROJECT_SOURCE_DIR}/include")

# 添加子目录
add_subdirectory("src")
add_subdirectory("test")



