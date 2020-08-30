project(graphtinker)
cmake_minimum_required(VERSION 3.6)


include_directories(${PROJECT_SOURCE_DIR}/include/util)

aux_source_directory(. util_source_files)

add_library(util ${util_source_files})

