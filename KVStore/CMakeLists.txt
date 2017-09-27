cmake_minimum_required(VERSION 3.2)
project(KVStore)

set(CMAKE_CXX_STANDARD 14)
#if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  # using regular Clang or AppleClang
#endif()

add_subdirectory(lib/glog-0.3.5)

set(SOURCE_FILES include/KVStore.h db/KVStoreImpl.cpp db/KVStoreImpl.h)
add_library(KVStore ${SOURCE_FILES})
set_target_properties(KVStore PROPERTIES PUBLIC_HEADER include/KVStore.h)
target_link_libraries(KVStore glog)
#target_link_libraries(KVStore pthread)
