PROJECT(graphtinker)

CMAKE_MINIMUM_REQUIRED(VERSION 3.6)

include_directories(${PROJECT_SOURCE_DIR}/include/graphtinker)

add_executable(updates_test updates_test.cpp)

target_link_libraries(updates_test graphtinker)

file(COPY ../../config.ini DESTINATION .)
file(COPY ../../RMAT_500K_8M DESTINATION .)
