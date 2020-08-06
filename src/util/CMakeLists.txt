project(graphtinker)
cmake_minimum_required(VERSION 3.6)


include_directories(${PROJECT_SOURCE_DIR}/include/util)

set(util_source_files
        util.cpp)

add_library(util ${util_source_files})

