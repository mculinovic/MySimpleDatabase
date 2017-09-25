include_directories($(gtest_SOURCE_DIR)/include $(gtest_SOURCE_DIR))

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x")

add_executable(runTests KVStoreImplSetTest.cpp KVStoreImplGetTest.cpp KVStoreImpleDeleteTest.cpp Utility.cpp Utility.h)

target_link_libraries(runTests gtest gtest_main)
target_link_libraries(runTests KVStore)
#target_link_libraries(runTests cmake-build-debug/KVStore/libKVStore.a)