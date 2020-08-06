project(graphtinker)
cmake_minimum_required(VERSION 3.6)

include_directories(${PROJECT_SOURCE_DIR}/include/config)

set(config_source_file
        config.cpp)

add_library(config ${config_source_file})

target_link_libraries(config util)

