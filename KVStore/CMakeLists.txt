cmake_minimum_required(VERSION 3.2)
project(KVStore)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(lib/glog)

set(SOURCE_FILES include/KVStore.h db/KVStoreImpl.cpp db/KVStoreImpl.h)
add_library(KVStore ${SOURCE_FILES})
set_target_properties(KVStore PROPERTIES PUBLIC_HEADER include/KVStore.h)
target_link_libraries(KVStore glog)