PROJECT(graphtinker)

CMAKE_MINIMUM_REQUIRED(VERSION 3.6)

add_executable(util_test util_test.cpp)

target_link_libraries(util_test util glog gflags)
