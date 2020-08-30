
PROJECT(graphtinker) 
CMAKE_MINIMUM_REQUIRED(VERSION 3.6)

add_executable(config_test config_test.cpp)

target_link_libraries(config_test config glog gflags)

file(COPY ../../config.ini DESTINATION .) 
