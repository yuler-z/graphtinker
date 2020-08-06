project(graphtinker)
cmake_minimum_required(VERSION 3.6)

include_directories(${PROJECT_SOURCE_DIR}/include/graphtinker)

aux_source_directory(. graphtinker_source_files)

add_library(graphtinker ${graphtinker_source_files})

target_link_libraries(graphtinker config util)