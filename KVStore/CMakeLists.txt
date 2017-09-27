cmake_minimum_required(VERSION 3.2)
project(KVStore)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

add_subdirectory(vendor/glog-0.3.5)

set(SOURCE_FILES include/KVStore.h db/KVStoreImpl.cpp db/KVStoreImpl.h)
add_library(KVStore STATIC ${SOURCE_FILES})
set_target_properties(KVStore PROPERTIES PUBLIC_HEADER include/KVStore.h)
target_link_libraries(KVStore glog)
