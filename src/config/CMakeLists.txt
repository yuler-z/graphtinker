project(graphtinker)
cmake_minimum_required(VERSION 3.6)

aux_source_directory(. config_source_file)

add_library(config ${config_source_file})

target_link_libraries(config util)

