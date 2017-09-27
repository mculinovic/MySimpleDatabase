cmake_minimum_required(VERSION 3.2)
project(KVStoreTests)

add_subdirectory(vendor/googletest)
add_subdirectory(vendor/benchmark)
add_subdirectory(validationTests)
add_subdirectory(performanceTests)