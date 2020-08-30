
project(graphtinker)
cmake_minimum_required(VERSION 3.6)

aux_source_directory(. unit_source_files)

add_library(unit ${unit_source_files})

target_link_libraries(unit)