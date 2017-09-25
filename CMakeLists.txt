cmake_minimum_required(VERSION 3.8)
project(KVStore)

add_subdirectory(KVStore)
add_subdirectory(KVStoreTests)

add_executable(run main.cpp)
target_link_libraries(run KVStore)