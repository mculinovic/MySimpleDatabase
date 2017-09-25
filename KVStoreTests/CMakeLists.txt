cmake_minimum_required(VERSION 3.8)
project(KVStoreTests)

add_subdirectory(lib/googletest)
add_subdirectory(lib/benchmark)
add_subdirectory(validationTests)
add_subdirectory(performanceTests)