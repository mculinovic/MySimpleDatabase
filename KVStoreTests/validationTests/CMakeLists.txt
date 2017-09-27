include_directories($(gtest_SOURCE_DIR)/include $(gtest_SOURCE_DIR))

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

add_executable(runTests KVStoreImplSetTest.cpp KVStoreImplGetTest.cpp KVStoreImpleDeleteTest.cpp Utility.cpp Utility.h)

target_link_libraries(runTests gtest gtest_main)
target_link_libraries(runTests KVStore)