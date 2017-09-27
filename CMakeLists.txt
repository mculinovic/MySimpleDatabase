cmake_minimum_required(VERSION 3.2)
project(KVStore)

#set(CMAKE_CXX_STANDARD 14)

add_subdirectory(KVStore)
add_subdirectory(KVStoreTests)

#add_executable(run main.cpp)
#target_link_libraries(run KVStore)