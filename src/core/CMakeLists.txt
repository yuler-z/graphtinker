project(graphtinker)
cmake_minimum_required(VERSION 3.6)

include_directories(.)

aux_source_directory(. core_source_files)

add_library(core ${core_source_files})

target_link_libraries(core config unit)